#include "ALP__physics_function.h"
#include "ALP__physics_manager.h"

#include <thread>
#include <chrono>

#include "collider.h"
#include "ALP_joint.h"

#include "ALP_raycast.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

//ContactPoint�̕\��
#define Update60fps
//#define Draw_Contact //�g�p����ۂ�thread��`��̂Ɠ����ɂ��邱��
//#define Draw_JointContact //�g�p����ۂ�thread��`��̂Ɠ����ɂ��邱��

//::: static�����o�̏����� :::::
#pragma region static_initialize

namespace Adollib
{
	volatile bool Physics_manager::is_added_ALPcollider = true; //physics���X�V����frame����true�ɂȂ�

	std::thread Physics_manager::physics_thread; //physics��thread�p
	bool Physics_manager::is_stop_physics_thread = false; //true�ɂȂ����Ƃ�thread_update���~�߂�

	LARGE_INTEGER Physics_manager::frame_count;
	LARGE_INTEGER Physics_manager::frame_count_stop; //;
	LARGE_INTEGER Physics_manager::update_start_time; //;
	LARGE_INTEGER Physics_manager::update_end_time; //;
	float Physics_manager::update_time; //;

	std::mutex Physics_manager::mtx;

	// �edata�̎��Ԕz��
	std::list<Physics_function::ALP_Collider*> Physics_manager::ALP_colliders;
	std::list<Physics_function::ALP_Physics*>  Physics_manager::ALP_physicses;
	std::list<Physics_function::ALP_Collider*> Physics_manager::added_buffer_ALP_colliders;
	std::list<Physics_function::ALP_Physics*>  Physics_manager::added_buffer_ALP_physicses;
	std::list<Physics_function::ALP_Collider*> Physics_manager::deleted_buffer_ALP_colliders; //�}���`�X���b�h�p �����̓r����GO���폜���ꂽ�v�f
	std::list<Physics_function::ALP_Physics*>  Physics_manager::deleted_buffer_ALP_physicses; //�}���`�X���b�h�p �����̓r����GO���폜���ꂽ�v�f

	std::list<Physics_function::ALP_Joint*> Physics_manager::ALP_joints;
	std::list<Physics_function::ALP_Joint*> Physics_manager::added_buffer_ALP_joints;
	std::list<Physics_function::ALP_Joint*> Physics_manager::deleted_buffer_ALP_joints;

	std::vector<Physics_function::Contacts::Contact_pair*> Physics_manager::pairs[2];
	u_int Physics_manager::pairs_new_num = 0; //pairs�̂ǂ������V�����Փ˂Ȃ̂�
	// std::vector<Physics_function::Contacts::Collider_2> Phyisics_manager::broad_mid_pair;

	std::vector<Physics_function::ALP_Collider*> Physics_manager::moved_collider_for_insertsort;   //�}���\�[�g�p ������
	std::vector<Physics_function::ALP_Collider*> Physics_manager::added_collider_for_insertsort;   //�}���\�[�g�p �ǉ����ꂽ

	u_int Physics_manager::collider_index_count;

	Physics_function::PhysicsParams Physics_manager::physicsParams;

	bool Physics_manager::is_draw_collider = false;
	bool Physics_manager::is_draw_dop = false;
	bool Physics_manager::is_draw_joint = false;
}

#pragma endregion

bool Physics_manager::update()
{
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&update_start_time));

	//if (Al_Global::second_per_game < 1) {

	//	adapt_added_data();
	//	copy_transform(ALP_colliders, ALP_physicses);

	//	resetforce(ALP_physicses);
	//	is_updated_physicsthread = true;
	//	is_updated_mainthread = false;
	//	return true;
	//}


	// �f�[�^�̎󂯓n������
	{
		std::lock_guard<std::mutex>lock(mtx);

		// �폜�������̂�z�񂩂����
		dadapt_delete_data(false);

		//
		if (is_added_ALPcollider == false) {
			// �ǉ��������̂�z��ɉ����� �ǉ����ꂽ�u�Ԃ�is_added_ALPcollider��true worldtransform���X�V���ꂽ�疈�t���[��false�ɂȂ�
			adapt_added_data(false);

		}

		// gameobject���X�V����world_trnsform�̍X�V
		copy_transform(ALP_colliders, ALP_physicses);

		// Collider��World���̍X�V
		adapt_component_data(ALP_colliders, ALP_physicses, ALP_joints);

	}


	LARGE_INTEGER time;
	{
		std::lock_guard<std::mutex> lock(mtx);

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));
		LONGLONG counts_per_sec;
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));
		float seconds_per_count = 1.0f / static_cast<double>(counts_per_sec);

		if (frame_count_stop.QuadPart != 0) {
			frame_count.QuadPart += time.QuadPart - frame_count_stop.QuadPart;
			frame_count_stop.QuadPart = time.QuadPart;
		}

		physicsParams.timeStep += (float)(time.QuadPart - frame_count.QuadPart) * seconds_per_count;
		frame_count.QuadPart = time.QuadPart;
	}

	if (physicsParams.timeStep > physicsParams.caluculate_time)
	{
		//physicsParams.timeStep = physicsParams.caluculate_time;

		const float inv60_per_timeStep = physicsParams.caluculate_time / physicsParams.timeStep;
		physicsParams.timeStep = ALmin(physicsParams.timeStep, physicsParams.max_timeStep);

		physicsParams.timeStep /= physicsParams.calculate_iteration;

		// ���̂܂܎g����mainthread��0������ꂽ��ˑR���ʉ\�������邽�ߕۑ�����
		const float time_scale = physicsParams.timescale;

		// timescale�̉e���͍��W�̍X�V�݂̂Ȃ̂�
		physicsParams.timeStep *= time_scale;

		update_per_calculate(ALP_colliders);

		if (physicsParams.timeStep != 0)
			for (int i = 0; i < physicsParams.calculate_iteration; i++) {

				// shape��world���, physics��tensor�Ȃǂ̍X�V
				update_world_trans(ALP_colliders);

				// �O�͂̍X�V
				//applyexternalforce(ALP_physicses[Sce], 1);
				applyexternalforce(ALP_physicses, inv60_per_timeStep, time_scale);

				// Joint�ɂ�鑬�x�ւ̉e��
				apply_joint_velocityeffect(ALP_joints);

				// pair��new/old�����ւ���
				pairs_new_num = 1 - pairs_new_num;

				// ��G�c�ȓ����蔻��
				BroadMidphase(
					ALP_colliders, pairs[pairs_new_num],
					moved_collider_for_insertsort, added_collider_for_insertsort, mtx
				);

				Midphase(pairs[1 - pairs_new_num], pairs[pairs_new_num]);

				// �Փː���
				generate_contact(pairs[pairs_new_num]);

				// �Փˉ���
				resolve_contact(ALP_colliders, pairs[pairs_new_num], ALP_joints, time_scale);

				{

					std::lock_guard <std::mutex> lock(mtx);

					//Collider��frame���ɕۑ�����data��reset
					copy_transform(ALP_colliders, ALP_physicses);

					// �ʒu�̍X�V
					integrate(ALP_physicses);

					dadapt_delete_data(false);

				}
			}

		physicsParams.timeStep = 0;

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&update_end_time));
		update_time = static_cast<float>(update_end_time.QuadPart - update_start_time.QuadPart) * 0.0000001f;;
	}
	else
	{
		float sleep_time = physicsParams.caluculate_time - physicsParams.timeStep - 0.0005f;

		if (sleep_time > 0)
			//std::this_thread::sleep_for(std::chrono::minutes((long)(sleep_time)));
			std::this_thread::sleep_for(std::chrono::milliseconds((long)(sleep_time * 1000)));

	}

	return true;

}

/*
bool Physics_manager::update_Gui() {
	ImGuiWindowFlags flag = 0;
	// flag |= ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;


	if (ImGui::Begin("physics_global", 0, flag)) {

		//Collider�̕\��
		ImGui::Checkbox("draw_collider", &is_draw_collider);
		ImGui::Checkbox("draw_DOP", &is_draw_dop);
		ImGui::Checkbox("draw_joint", &is_draw_joint);

		// �d�͂̒���
		ImGui::InputFloat("gravity", &physicsParams.gravity, 0.1f, 1.0f, "%.2f");

		// ���m���̒���
		ImGui::InputInt("solver_iteration", &physicsParams.solver_iteration, 1, 200);
		ImGui::InputInt("calculate_iteration", &physicsParams.calculate_iteration, 1, 200);

		// SweepAndPrune��X�������̕���
		ImGui::InputInt("sweep_and_prune_divide_value", &physicsParams.sweep_and_prune_divide_value, 1, 200);

		// �ђʎ��̂΂˂̋���
		ImGui::InputFloat("bias", &physicsParams.bias, 0.01f, 0.1f, "%.3f");
		// �ђʋ��e�덷
		ImGui::InputFloat("slop", &physicsParams.slop, 0.0001f, 0.001f, "%.4f");

		// �ő��timestep
		ImGui::DragFloat("max_timeStep", &physicsParams.max_timeStep, 0.001f, 0.001f, 100000000);
		ImGui::DragFloat("caluculate_time", &physicsParams.caluculate_time, 0.001f, 0.001f, 100000000,"%.5f");
		ImGui::DragFloat("timescale", &physicsParams.timescale, 0.01f, 0, 100000000);
		ImGui::Text("timeStep : %f", physicsParams.timeStep);

		// physics_default�̕\��
		if (ImGui::CollapsingHeader("physics_default")) {

			ImGui::DragFloat("mass", &physicsParams.inertial_mass, 0.1f);
			ImGui::DragFloat("linear_drag", &physicsParams.linear_drag, 0.01f);
			ImGui::DragFloat("anglar_drag", &physicsParams.anglar_drag, 0.01f);
			ImGui::DragFloat("dynamic_friction", &physicsParams.dynamic_friction, 0.01f);
			ImGui::DragFloat("static_friction", &physicsParams.static_friction, 0.01f);
			ImGui::DragFloat("restitution", &physicsParams.restitution, 0.01f);
			ImGui::InputFloat("linear_sleep_threrhold", &physicsParams.linear_sleep_threrhold, 0.01f);
			ImGui::InputFloat("angular_sleep_threrhold", &physicsParams.angula_sleep_threrhold, 0.01f);
			ImGui::Checkbox("is_fallable", &physicsParams.is_fallable);
			ImGui::Checkbox("is_kinmatic_anglar", &physicsParams.is_kinmatic_anglar);
			ImGui::Checkbox("is_kinmatic_linear", &physicsParams.is_kinmatic_linear);
			ImGui::Checkbox("is_moveable", &physicsParams.is_moveable);
			ImGui::Checkbox("is_hitable", &physicsParams.is_hitable);
		}

	}
	ImGui::End();

	return true;
}
*/

bool Physics_manager::ray_cast(
	const Vector3& Ray_pos, const Vector3& Ray_dir,
	u_int tag,
	const float& ray_min,
	float& tmin, float& tmax,
	Vector3& normal,
	Collider*& ret_coll
) {

	std::lock_guard <std::mutex> lock(mtx);

	//tmin����tmax�ɂ�����ray���������Ă���
	tmin = +FLT_MAX;
	tmax = -FLT_MAX;
	float min = -FLT_MAX;
	float max = +FLT_MAX;
	Vector3 norm;

	bool ret = false;

	for (const auto coll : ALP_colliders) {
		if (!(coll->tag & tag))continue;

		if (Physics_function::ray_cast(
			Ray_pos, Ray_dir,
			ray_min,
			coll,
			min, max, norm
		) == false) continue;

		ret = true;

		if (tmin > min) {
			tmin = min;
			normal = norm;
			ret_coll = coll->get_collptr();
		}
		//tmin = ALmin(tmin, min);
		tmax = ALmax(tmax, max);
	}

	return ret;
}

bool Physics_manager::sphere_cast(
	const Vector3& Ray_pos, const Vector3& Ray_dir,
	const float& radius,
	Vector3& contact_point,
	u_int tag,
	const float& ray_min,
	float& tmin, float& tmax,
	Vector3& normal,
	Collider*& ret_coll
) {

	std::lock_guard <std::mutex> lock(mtx);

	//tmin����tmax�ɂ�����ray���������Ă���
	tmin = +FLT_MAX;
	tmax = -FLT_MAX;
	float min = -FLT_MAX;
	float max = +FLT_MAX;
	Vector3 norm;
	Vector3 contactp;

	bool ret = false;

	for (const auto coll : ALP_colliders) {
		if (!(coll->tag & tag))continue;

		if (Physics_function::sphere_cast(
			Ray_pos, Ray_dir, radius, contactp,
			ray_min,
			coll,
			min, max, norm
		) == false) continue;

		ret = true;

		if (tmin > min) {
			tmin = min;
			normal = norm;
			ret_coll = coll->get_collptr();
			contact_point = contactp;
		}
		//tmin = ALmin(tmin, min);
		tmax = ALmax(tmax, max);
	}

	return ret;
}


void Physics_manager::thread_update() {
	while (is_stop_physics_thread == false)
	{
		update();
	}
}

void Physics_manager::adapt_added_data(bool is_mutex_lock) {
	if (is_mutex_lock) mtx.lock(); //�Ăяo���̂Ƃ����lock���Ă��� if�����Ȃ̂�std::lock_guard <std::mutex> lock(mtx)�͎g���Ȃ�

	{
		for (auto coll : added_buffer_ALP_colliders) {
			// �ǉ����ꂽcollider�̏�����&Broadphase�ɓo�^
			coll->adapt_added_data();
			//coll->copy_transform_gameobject();
			coll->copy_transform();
			added_collider_for_insertsort.emplace_back(coll);
		}

		// �����z��
		int save_size = ALP_colliders.size();
		ALP_colliders.splice(ALP_colliders.end(), std::move(added_buffer_ALP_colliders));

		// list�������z��������itr���ω����Ă��� ���g��itr���X�V
		auto save_itr = ALP_colliders.begin();
		for (save_itr; save_itr != ALP_colliders.end(); ++save_itr) {
			save_size--;
			if (save_size >= 0)continue;
			(*save_itr)->set_this_itr(save_itr);

			// ���C���z��Ɉ����z��������is_added��true��
			(*save_itr)->is_added = true;
		}
	}

	{
		// �ǉ����ꂽphysics�̏�����
		for (auto& phys : added_buffer_ALP_physicses) {
			phys->copy_transform_ptr();
		}

		int save_size = ALP_physicses.size();
		ALP_physicses.splice(ALP_physicses.end(), std::move(added_buffer_ALP_physicses));

		// list�������z��������itr���ω����Ă��� ���g��itr���X�V
		auto save_itr = ALP_physicses.begin();
		for (save_itr; save_itr != ALP_physicses.end(); ++save_itr) {
			save_size--;
			if (save_size >= 0)continue;
			(*save_itr)->set_this_itr(save_itr);

			// ���C���z��Ɉ����z��������is_added��true��
			(*save_itr)->is_added = true;
		}
	}

	added_buffer_ALP_colliders.clear();
	added_buffer_ALP_physicses.clear();


	for (auto added_coll : ALP_colliders) {
		// �ecollider��shape��added_data������
		added_coll->adapt_added_data();
	}

	if (added_buffer_ALP_joints.size() != 0)
	{
		// �����z��
		auto save_itr = added_buffer_ALP_joints.begin();
		ALP_joints.splice(ALP_joints.end(), std::move(added_buffer_ALP_joints));

		// list�������z��������itr���ω����Ă��� ���g��itr���X�V
		for (save_itr; save_itr != ALP_joints.end(); ++save_itr) {
			// added_buffer_ALP_joints�ɐV�KJoint��ǉ����������userjoint�Ƀ|�C���^�����Ă��邪�A
			// ���̌��Ԃł������Ă΂���null�Q�ƂŎ���ł��܂��̂�
			// �C������������ nullptr�łȂ��Ȃ�܂őҋ@����
			while ((*save_itr)->userjoint == nullptr) {}

			(*save_itr)->set_this_itr(save_itr);

			//(*save_itr)->ALPcollider colliderA->set_ptr_to_joint(joint);
			//colliderB->set_ptr_to_joint(joint);

			// ���C���z��Ɉ����z��������is_added��true��
			(*save_itr)->is_added = true;
		}
	}

	if (is_mutex_lock) mtx.unlock();
}

void Physics_manager::dadapt_delete_data(bool is_mutex_lock) {
	if (is_mutex_lock) mtx.lock(); //�Ăяo���̂Ƃ����lock���Ă��� if�����Ȃ̂�std::lock_guard <std::mutex> lock(mtx)�͎g���Ȃ�

	// �폜���ꂽcollider
	for (auto& coll : deleted_buffer_ALP_colliders) {
		coll->destroy();
		delete coll;

	}

	// �폜���ꂽphysics
	for (auto& phys : deleted_buffer_ALP_physicses) {
		phys->destroy();
		delete phys;
	}

	deleted_buffer_ALP_colliders.clear();
	deleted_buffer_ALP_physicses.clear();

	// deleted_joint->destroy��deleted_buffer_ALP_joints�̑Ή�����deleted_joint��nullptr�ɂ��邽�߃R�s�[�n���ɂ���
	for (auto deleted_joint : deleted_buffer_ALP_joints) {
		if (deleted_joint == nullptr)continue;
		deleted_joint->destroy(nullptr);
		delete deleted_joint;
		deleted_joint = nullptr;
	}
	deleted_buffer_ALP_joints.clear();

	if (is_mutex_lock) mtx.unlock();
}


// ��thread��update����
void Physics_manager::thread_start() {
	is_stop_physics_thread = false;
	physics_thread = std::thread(thread_update);
}

// ��thread�ł�update���~�߂āAjoin���s��
void Physics_manager::thread_stop_and_join() {
	is_stop_physics_thread = true;
	if (physics_thread.joinable())physics_thread.join();
}

void Physics_manager::timer_stop() {
	std::lock_guard<std::mutex> lock(mtx);
	if (frame_count_stop.QuadPart == 0)
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&frame_count_stop));
}
void Physics_manager::timer_start() {
	std::lock_guard<std::mutex> lock(mtx);
	frame_count_stop.QuadPart = 0;
}


void Physics_manager::destroy() {

	dadapt_delete_data();

	for (auto& joint : ALP_joints) {
		auto& base = joint->userjoint;
		base->destroy();
		delete base;
	}


	for (auto& p : pairs[0]) {
		delete p;
	}
	for (auto& p : pairs[1]) {
		delete p;
	}
	pairs[0].clear();
	pairs[1].clear();
}

