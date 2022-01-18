#include "ALP__physics_function.h"
#include "ALP__physics_manager.h"
#include "../Scene/scene.h"
#include "../Main/Adollib.h"

#include "../Object/gameobject_manager.h"
#include "../Imgui/imgui_all.h"
#include "../Imgui/work_meter.h"
#include "../Imgui/debug.h"

#include "../Renderer/renderer_base.h"

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
	volatile bool Physics_manager::is_updated_mainthread = true; //physics���X�V����frame����true�ɂȂ�
	volatile bool Physics_manager::is_updated_physicsthread = true; //physics���X�V����frame����true�ɂȂ�
	volatile bool Physics_manager::is_added_ALPcollider = true; //physics���X�V����frame����true�ɂȂ�

	std::thread Physics_manager::physics_thread; //physics��thread�p
	bool Physics_manager::is_stop_physics_thread = false; //true�ɂȂ����Ƃ�thread_update���~�߂�

	LARGE_INTEGER Physics_manager::frame_count;
	LARGE_INTEGER Physics_manager::frame_count_stop; //;

	std::mutex Physics_manager::mtx;

	// �edata�̎��Ԕz��
	std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>> Physics_manager::ALP_colliders;
	std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>  Physics_manager::ALP_physicses;
	std::map<Scenelist, std::list<Physics_function::ALP_Collider*>> Physics_manager::added_buffer_ALP_colliders;
	std::map<Scenelist, std::list<Physics_function::ALP_Physics*>>  Physics_manager::added_buffer_ALP_physicses;
	std::map<Scenelist, std::list<Physics_function::ALP_Collider*>> Physics_manager::deleted_buffer_ALP_colliders; //�}���`�X���b�h�p �����̓r����GO���폜���ꂽ�v�f
	std::map<Scenelist, std::list<Physics_function::ALP_Physics*>>  Physics_manager::deleted_buffer_ALP_physicses; //�}���`�X���b�h�p �����̓r����GO���폜���ꂽ�v�f

	std::list<Physics_function::ALP_Joint*> Physics_manager::ALP_joints;
	std::list<Physics_function::ALP_Joint*> Physics_manager::added_buffer_ALP_joints;
	std::list<Physics_function::ALP_Joint*> Physics_manager::deleted_buffer_ALP_joints;

	std::vector<Physics_function::Contacts::Contact_pair*> Physics_manager::pairs[2];
	u_int Physics_manager::pairs_new_num = 0; //pairs�̂ǂ������V�����Փ˂Ȃ̂�
	// std::vector<Physics_function::Contacts::Collider_2> Phyisics_manager::broad_mid_pair;

	Scenelist Physics_manager::old_sce;

	std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> Physics_manager::moved_collider_for_insertsort;   //�}���\�[�g�p ������
	std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> Physics_manager::added_collider_for_insertsort;   //�}���\�[�g�p �ǉ����ꂽ

	std::unordered_map<Scenelist, u_int> Physics_manager::collider_index_count;

	Physics_function::PhysicsParams Physics_manager::physicsParams;

	bool Physics_manager::is_draw_collider = false;
	bool Physics_manager::is_draw_dop = false;
	bool Physics_manager::is_draw_joint = false;
}

#pragma endregion

bool Physics_manager::update(Scenelist Sce)
{
	Work_meter::start("Phyisics_manager", 1);
	if (Sce != old_sce) {
		pairs[0].clear();
		pairs[1].clear();
		old_sce = Sce;
	}

	if (Al_Global::second_per_game < 1) {

		adapt_added_data(Sce);
		copy_transform(ALP_colliders[Sce], ALP_physicses[Sce]);

		resetforce(ALP_physicses[Sce]);
		is_updated_physicsthread = true;
		is_updated_mainthread = false;
		return true;
	}


	// �f�[�^�̎󂯓n������
	{
		std::lock_guard<std::mutex>lock(mtx);

		// �폜�������̂�z�񂩂����
		dadapt_delete_data(false);

		//
		if (is_added_ALPcollider == false) {
			// �ǉ��������̂�z��ɉ����� �ǉ����ꂽ�u�Ԃ�is_added_ALPcollider��true worldtransform���X�V���ꂽ�疈�t���[��false�ɂȂ�
			adapt_added_data(Sce, false);

		}

		// mainthread���X�V������
		if (is_updated_mainthread) {
			is_updated_mainthread = false; // mainthread��gameobject_transform���Ă񂾂��� gameobject��transform��physics�ɃR�s�[����

			// gameobject���X�V����world_trnsform�̍X�V
			copy_transform(ALP_colliders[Sce], ALP_physicses[Sce]);

		}

		// Collider��World���̍X�V
		adapt_component_data(ALP_colliders[Sce], ALP_physicses[Sce], ALP_joints);

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

	Work_meter::set("physicsParams.timeStep", physicsParams.timeStep, 1);
	if (physicsParams.timeStep > physicsParams.caluculate_time)
	{

		const float inv60_per_timeStep = physicsParams.caluculate_time / physicsParams.timeStep;
		physicsParams.timeStep = ALmin(physicsParams.timeStep, physicsParams.max_timeStep);

		physicsParams.timeStep /= physicsParams.calculate_iteration;

		// ���̂܂܎g����mainthread��0������ꂽ��ˑR���ʉ\�������邽�ߕۑ�����
		const float time_scale = physicsParams.timescale;

		// timescale�̉e���͍��W�̍X�V�݂̂Ȃ̂�
		physicsParams.timeStep *= time_scale;

		update_per_calculate(ALP_colliders[Sce]);

		if (physicsParams.timeStep != 0)
		for (int i = 0; i < physicsParams.calculate_iteration; i++) {

			// shape��world���, physics��tensor�Ȃǂ̍X�V
			update_world_trans(ALP_colliders[Sce]);

			// �O�͂̍X�V
			//applyexternalforce(ALP_physicses[Sce], 1);
			applyexternalforce(ALP_physicses[Sce], inv60_per_timeStep, time_scale);

			// pair��new/old�����ւ���
			pairs_new_num = 1 - pairs_new_num;

			// ��G�c�ȓ����蔻��
			Work_meter::start("Broad,Mid,Narrow", 1);

			Work_meter::start("Broadphase", 1);
			Work_meter::tag_start("Broadphase", 1);
			BroadMidphase(Sce,
				ALP_colliders[Sce], pairs[pairs_new_num],
				moved_collider_for_insertsort[Sce], added_collider_for_insertsort[Sce], mtx
			);
			Work_meter::tag_stop(1);
			Work_meter::stop("Broadphase", 1);

			Work_meter::start("Midphase", 1);
			Work_meter::tag_start("Midphase", 1);
			Midphase(pairs[1 - pairs_new_num], pairs[pairs_new_num]);
			Work_meter::tag_stop(1);
			Work_meter::stop("Midphase", 1);

			// �Փː���
			Work_meter::start("Narrowphase", 1);
			Work_meter::tag_start("Narrowphase", 1);
			generate_contact(pairs[pairs_new_num]);
			Work_meter::tag_stop(1);
			Work_meter::stop("Narrowphase", 1);

			Work_meter::stop("Broad,Mid,Narrow", 1);

			// �Փˉ���
			Work_meter::start("Resolve", 1);
			Work_meter::tag_start("Resolve", 1);
			resolve_contact(ALP_colliders[Sce], pairs[pairs_new_num], ALP_joints, time_scale);
			Work_meter::tag_stop(1);
			Work_meter::stop("Resolve", 1);

			{
				std::lock_guard <std::mutex> lock(mtx);
				// �v�Z����gameobject��transform���K������Ă����ꍇ ������transform���X�V���Ă���integrate���s��
				if (is_updated_mainthread) {
					is_updated_mainthread = false; // mainthread��gameobject_transform���Ă񂾂��� gameobject��transform��physics�ɃR�s�[����

					//Collider��frame���ɕۑ�����data��reset
					copy_transform(ALP_colliders[Sce], ALP_physicses[Sce]);
				}
				// �ʒu�̍X�V
				integrate(ALP_physicses[Sce]);

				is_updated_physicsthread = true;
			}
		}

		physicsParams.timeStep = 0;
	}

	Work_meter::stop("Phyisics_manager", 1);

#ifdef Draw_JointContact
	static bool init = true;
	static const int size = 100;
	static Gameobject* debug_go[size];
	if (init) {
		for (int i = 0; i < size; i++) {
			debug_go[i] = Gameobject_manager::createSphere();
			debug_go[i]->transform->local_scale = Vector3(1) * 0.3f;
			if (i % 2 == 0)debug_go[i]->renderer->color = Vector4(1, 0.5f, 0.5f, 1);
			else debug_go[i]->renderer->color = Vector4(0.5f, 1, 0.5f, 1);

			debug_go[i]->is_hierarchy = false;
		}
		init = false;
	}

	for (int i = 0; i < size; i++) {
		debug_go[i]->transform->local_pos = Vector3(1000000, 1000000, 1000000);
	}

	int count = 0;
	for (const auto& p : ALP_joints) {

		count += 2;
		if (count > size)break;
		const auto coll0 = p->userjoint->get_colliderA();
		const auto coll1 = p->userjoint->get_colliderB();

		//debug_go[count - 1]->transform->local_pos = vector3_quatrotate((p.contacts.contactpoints[i].point[0]), coll0.get_gameobject()->world_orientate()) + coll0.get_gameobject()->world_position();
		//debug_go[count - 2]->transform->local_pos = vector3_quatrotate((p.contacts.contactpoints[i].point[1]), coll1.get_gameobject()->world_orientate()) + coll1.get_gameobject()->world_position();

		debug_go[count - 1]->transform->local_pos = coll0->gameobject->transform->position + vector3_quatrotate(p->limit_constraint_pos[0], coll0->gameobject->transform->orientation);
		debug_go[count - 2]->transform->local_pos = coll1->gameobject->transform->position + vector3_quatrotate(p->limit_constraint_pos[1], coll1->gameobject->transform->orientation);
		int adfsdg = 0;


	}

#endif // DEBUG

#ifdef Draw_Contact
	static bool init = true;
	static const int size = 100;
	static Gameobject* debug_go[size];
	if (init) {
		for (int i = 0; i < size; i++) {
			debug_go[i] = Gameobject_manager::createSphere();
			debug_go[i]->transform->local_scale = Vector3(1) * 0.3f;
			if (i % 2 == 0)debug_go[i]->material->color = Vector4(1, 0.5f, 0.5f, 1);
			else debug_go[i]->material->color = Vector4(0.5f, 1, 0.5f, 1);

			debug_go[i]->is_hierarchy = false;
		}
		init = false;
	}

	for (int i = 0; i < size; i++) {
		debug_go[i]->transform->local_pos = Vector3(1000000, 1000000, 1000000);
	}

	int count = 0;
	for (const auto& p : pairs[pairs_new_num]) {
		for (int i = 0; i < p->contacts.contact_num; i++) {
			count += 2;
			if (count > size)break;
			const auto coll0 = *p->body[0]->get_ALPcollider();
			const auto coll1 = *p->body[1]->get_ALPcollider();

			//debug_go[count - 1]->transform->local_pos = vector3_quatrotate((p.contacts.contactpoints[i].point[0]), coll0.get_gameobject()->world_orientate()) + coll0.get_gameobject()->world_position();
			//debug_go[count - 2]->transform->local_pos = vector3_quatrotate((p.contacts.contactpoints[i].point[1]), coll1.get_gameobject()->world_orientate()) + coll1.get_gameobject()->world_position();

			debug_go[count - 1]->transform->local_pos = p->body[0]->world_position() + vector3_quatrotate(p->contacts.contactpoints[i].point[0], p->body[0]->world_orientation());
			debug_go[count - 2]->transform->local_pos = p->body[1]->world_position() + vector3_quatrotate(p->contacts.contactpoints[i].point[1], p->body[1]->world_orientation());
			int adfsdg = 0;
			}
		}

#endif // DEBUG

		return true;

	}

bool Physics_manager::update_Gui() {
	ImGuiWindowFlags flag = 0;
	//flag |= ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;

	if (ImGui::Begin("physics_global", 0, flag)) {

		//Collider�̕\��
		ImGui::Checkbox("draw_collider", &is_draw_collider);
		ImGui::Checkbox("draw_DOP", &is_draw_dop);
		ImGui::Checkbox("draw_joint", &is_draw_joint);

		//�d�͂̒���
		ImGui::InputFloat("gravity", &physicsParams.gravity, 0.1f, 1.0f, "%.2f");

		//���m���̒���
		ImGui::InputInt("solver_iteration", &physicsParams.solver_iteration, 1, 200);
		ImGui::InputInt("calculate_iteration", &physicsParams.calculate_iteration, 1, 200);

		//sleep��臒l
		ImGui::InputFloat("linear_sleep_threrhold", &physicsParams.linear_sleep_threrhold, 0.01f);
		ImGui::InputFloat("angular_sleep_threrhold", &physicsParams.angula_sleep_threrhold, 0.01f);

		//�ђʎ��̂΂˂̋���
		ImGui::InputFloat("bias", &physicsParams.bias, 0.01f, 0.1f, "%.3f");
		//�ђʋ��e�덷
		ImGui::InputFloat("slop", &physicsParams.slop, 0.0001f, 0.001f, "%.4f");

		//�ő��timestep
		ImGui::DragFloat("max_timeStep", &physicsParams.max_timeStep, 0.001f, 0.001f, 100000000);
		ImGui::DragFloat("timescale", &physicsParams.timescale, 0.01f, 0, 100000000);
		ImGui::Text("timeStep : %f", physicsParams.timeStep);

		//physics_default�̕\��
		if (ImGui::CollapsingHeader("physics_default")) {

			ImGui::DragFloat("mass", &physicsParams.inertial_mass, 0.1f);
			ImGui::DragFloat("linear_drag", &physicsParams.linear_drag, 0.01f);
			ImGui::DragFloat("anglar_drag", &physicsParams.anglar_drag, 0.01f);
			ImGui::DragFloat("dynamic_friction", &physicsParams.dynamic_friction, 0.01f);
			ImGui::DragFloat("static_friction", &physicsParams.static_friction, 0.01f);
			ImGui::DragFloat("restitution", &physicsParams.restitution, 0.01f);
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

bool Physics_manager::ray_cast(
	const Vector3& Ray_pos, const Vector3& Ray_dir,
	u_int tag,
	const float& ray_min,
	float& tmin, float& tmax,
	Vector3& normal,
	Collider*& ret_coll,
	Scenelist Sce) {

	std::lock_guard <std::mutex> lock(mtx);

	//tmin����tmax�ɂ�����ray���������Ă���
	tmin = +FLT_MAX;
	tmax = -FLT_MAX;
	float min = -FLT_MAX;
	float max = +FLT_MAX;
	Vector3 norm;

	bool ret = false;

	for (const auto coll : ALP_colliders[Sce]) {
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
	Collider*& ret_coll,
	Scenelist Sce) {

	std::lock_guard <std::mutex> lock(mtx);

	//tmin����tmax�ɂ�����ray���������Ă���
	tmin = +FLT_MAX;
	tmax = -FLT_MAX;
	float min = -FLT_MAX;
	float max = +FLT_MAX;
	Vector3 norm;
	Vector3 contactp;

	bool ret = false;

	for (const auto coll : ALP_colliders[Sce]) {
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

void Physics_manager::adapt_added_data(Scenelist Sce, bool is_mutex_lock) {
	if (is_mutex_lock) mtx.lock(); //�Ăяo���̂Ƃ����lock���Ă��� if�����Ȃ̂�std::lock_guard <std::mutex> lock(mtx)�͎g���Ȃ�

	for (auto added_coll : added_buffer_ALP_colliders) {
		// �ǉ����ꂽcollider�̏�����&Broadphase�ɓo�^
		for (auto& coll : added_coll.second) {
			coll->adapt_added_data();
			coll->copy_transform_gameobject();
			coll->copy_transform();
			added_collider_for_insertsort[added_coll.first].emplace_back(coll);
		}

		// �����z��
		int save_size = ALP_colliders[added_coll.first].size();
		ALP_colliders[added_coll.first].splice(ALP_colliders[added_coll.first].end(), std::move(added_coll.second));

		// list�������z��������itr���ω����Ă��� ���g��itr���X�V
		auto save_itr = ALP_colliders[added_coll.first].begin();
		for (save_itr; save_itr != ALP_colliders[added_coll.first].end(); ++save_itr) {
			save_size--;
			if (save_size >= 0)continue;
			(*save_itr)->set_this_itr(save_itr);

			// ���C���z��Ɉ����z��������is_added��true��
			(*save_itr)->is_added = true;
		}
	}
	for (auto& added_phys : added_buffer_ALP_physicses) {

		// �ǉ����ꂽphysics�̏�����
		for (auto& phys : added_phys.second) {
			phys->copy_transform_ptr();
		}

		int save_size = ALP_physicses[added_phys.first].size();
		ALP_physicses[added_phys.first].splice(ALP_physicses[added_phys.first].end(), std::move(added_phys.second));

		// list�������z��������itr���ω����Ă��� ���g��itr���X�V
		auto save_itr = ALP_physicses[added_phys.first].begin();
		for (save_itr; save_itr != ALP_physicses[added_phys.first].end(); ++save_itr) {
			save_size--;
			if (save_size >= 0)continue;
			(*save_itr)->set_this_itr(save_itr);

			// ���C���z��Ɉ����z��������is_added��true��
			(*save_itr)->is_added = true;
		}
	}

	added_buffer_ALP_colliders.clear();
	added_buffer_ALP_physicses.clear();

	for (auto added_coll : ALP_colliders[Sce]) {
		// �ecollider��shape��added_data������
		added_coll->adapt_added_data();
	}

	if(added_buffer_ALP_joints.size() != 0)
	{
		// �����z��
		auto save_itr = added_buffer_ALP_joints.begin();
		ALP_joints.splice(ALP_joints.end(), std::move(added_buffer_ALP_joints));

		// list�������z��������itr���ω����Ă��� ���g��itr���X�V
		for (save_itr; save_itr != ALP_joints.end(); ++save_itr) {
			// added_buffer_ALP_joints�ɐV�KJoint��ǉ����������userjoint�Ƀ|�C���^�����Ă��邪�A
			// ���̌��Ԃł������Ă΂���null�Q�ƂŎ���ł��܂��̂�
			// �C������������ nullptr�Ȃ�continue����
			if ((*save_itr)->userjoint == nullptr) {
				--save_itr;
				continue;
			}

			(*save_itr)->set_this_itr(save_itr);
			(*save_itr)->set_ptr_to_joint(*save_itr);
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

	for (auto deleted_coll : deleted_buffer_ALP_colliders) {
		// �폜���ꂽcollider
		for (auto& coll : deleted_coll.second) {
			coll->destroy();
			delete coll;
		}
	}


	for (auto& deleted_phys : deleted_buffer_ALP_physicses) {
		// �폜���ꂽphysics
		for (auto& phys : deleted_phys.second) {
			phys->destroy();
			delete phys;
		}
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

bool Physics_manager::adapt_transform_to_gameobject(Scenelist Sce) {
	std::lock_guard <std::mutex> lock(mtx);

	if (is_updated_physicsthread == false) return false;

	for (auto coll : ALP_colliders[Sce]) {
		coll->adapt_to_gameobject_transform();
	}

	is_updated_physicsthread = false;

	return true;
}

void Physics_manager::copy_gameobject_transform(Scenelist Sce) {
	std::lock_guard <std::mutex> lock(mtx);

	for (auto& coll : added_buffer_ALP_colliders[Sce]) {
		coll->copy_transform_gameobject();
	}
	for (auto& coll : ALP_colliders[Sce]) {
		coll->copy_transform_gameobject();
	}

	Physics_manager::is_updated_mainthread = true;
	is_added_ALPcollider = false;
}





// ��thread��update����
void Physics_manager::thread_start() {
	is_stop_physics_thread = false;
	is_updated_mainthread = false;
	is_updated_physicsthread = false;
	physics_thread = std::thread(thread_update);
}

// ��thread�ł�update���~�߂āAjoin���s��
void Physics_manager::thread_stop_and_join() {
	is_stop_physics_thread = true;
	is_updated_mainthread = true;
	if (physics_thread.joinable())physics_thread.join();
}

void Physics_manager::timer_stop() {
	std::lock_guard<std::mutex> lock(mtx);
	if(frame_count_stop.QuadPart == 0)
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&frame_count_stop));
}
void Physics_manager::timer_start() {
	std::lock_guard<std::mutex> lock(mtx);
	frame_count_stop.QuadPart = 0;
}


#include "../Main/systems.h"
#include "../Renderer/material_for_collider.h"

bool Physics_manager::render_collider(Scenelist Sce) {
	std::lock_guard<std::mutex> lock(mtx);

	render_dop(Sce);
	render_joint(Sce);

	if (is_draw_collider == false)return false;

	for (auto& itr : ALP_colliders[Sce]) {
		Collider_renderer::render_collider(itr);
	}

	return true;
}

bool Physics_manager::render_dop(Scenelist Sce) {
	if (is_draw_dop == false)return false;

	for (auto& itr : ALP_colliders[Sce]) {
		Collider_renderer::render_AABB(itr);
	}

	return true;

}

bool Physics_manager::render_joint(Scenelist Sce) {

	if (is_draw_joint == false)return false;

	for (auto& itr : ALP_joints) {
		Collider_renderer::render_joint(itr);
	}

	return true;
}

void Physics_manager::destroy(Scenelist Sce) {

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

