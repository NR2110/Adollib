#include "ALP__physics_function.h"
#include "ALP__physics_manager.h"
#include "../Scene/scene.h"
#include "../Main/Adollib.h"

#include "../Object/gameobject_manager.h"
#include "../Imgui/imgui_all.h"
#include "../Imgui/work_meter.h"

#include "collider.h"

#include "ALP_raycast.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

//ContactPoint�̕\��
//#define Draw_Contact

//Addforce�̉e����1frame�x��邪Velocity�ɑ��x���������邱�Ƃ��ł���
#define Allow_delay

//::: static�����o�̏����� :::::
#pragma region static_initialize

namespace Adollib
{
	float Phyisics_manager::gravity = Physics_manager_default::gravity; //�d��

	Physics_function::ALP_Physics Phyisics_manager::default_physics = ALP_Physics(
		Physics_manager_default::inertial_mass, //����
		Physics_manager_default::linear_drag, //��C��R
		Physics_manager_default::anglar_drag, //��C��R
		Physics_manager_default::dynamic_friction,//�����C
		Physics_manager_default::static_friction, //�Ö��C
		Physics_manager_default::restitution,	 //�����W��

		Physics_manager_default::is_fallable, //�����Ȃ�
		Physics_manager_default::is_kinematic, //�e�������Ȃ�(fall�͂���)
		Physics_manager_default::is_kinmatic_anglar, //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
		Physics_manager_default::is_kinmatic_linear, //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
		Physics_manager_default::is_moveable, //�����Ȃ�
		Physics_manager_default::is_hitable //�Փ˂��Ȃ�
	); //�����ݒ肵�Ă��Ȃ��Ƃ���pyisics�̒l

	float Phyisics_manager::bounce_threshold = Physics_manager_default::bounce_threshold; //���˕Ԃ��臒l
	float Phyisics_manager::sleep_threshold = Physics_manager_default::sleep_threshold; //sleep��臒l

	float Phyisics_manager::contact_allowable_error = Physics_manager_default::contact_allowable_error; //����ȏ�߂��Ɠ���Փ˓_�Ƃ���
	float Phyisics_manager::contact_threrhold_normal = Physics_manager_default::contact_threrhold_normal; //�Փ˓_��臒l
	float Phyisics_manager::contact_threrhold_tangent = Physics_manager_default::contact_threrhold_tangent;//�Փ˓_��臒l

	float Phyisics_manager::bias = Physics_manager_default::bias;//�߂荞�݂𒼂���
	float Phyisics_manager::slop = Physics_manager_default::slop;//�Փ˂̋��e�l

	int Phyisics_manager::solver_iterations = Physics_manager_default::solver_iterations; //�Փ˂̐��x
	bool Phyisics_manager::hit_backfaces_flag = Physics_manager_default::hit_backfaces_flag;//mesh�̌�납��Փ˂��邩

	float Phyisics_manager::timeStep = 0.016f;

	bool Phyisics_manager::is_draw_collider = false;
	bool Phyisics_manager::is_draw_dop = false;
}

namespace Adollib
{
	int Phyisics_manager::frame_count;

	//collider_component�̃|�C���^�z��
	std::unordered_map<Scenelist, std::list<Collider*>> Phyisics_manager::colliders;

	//�edata�̎��Ԕz��
	std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider>> Phyisics_manager::ALP_colliders;
	std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics>> Phyisics_manager::ALP_physicses;

	std::vector<Physics_function::Contacts::Contact_pair> Phyisics_manager::pairs;
	std::vector<Physics_function::Contacts::Collider_2> Phyisics_manager::broad_mid_pair;

	std::unordered_map<Scenelist, std::vector<std::list<Physics_function::ALP_Collider>::iterator>> Phyisics_manager::moved_collider;   //������
	std::unordered_map<Scenelist, std::vector<std::list<Physics_function::ALP_Collider>::iterator>> Phyisics_manager::added_collider;   //�ǉ����ꂽ

	std::unordered_map<Scenelist, u_int> Phyisics_manager::collider_index_count;
}

#pragma endregion

bool Phyisics_manager::update(Scenelist Sce)
{
	update_Gui();

	if (frame_count < 10) {
		resetforce(ALP_physicses[Sce]);
		frame_count++;
		return true;
	}

	Work_meter::start("refresh_ALP_from_data");
	// get_data��data��Collider�ɓ���
	refresh_ALP_from_data(ALP_colliders[Sce]);
	Work_meter::stop("refresh_ALP_from_data");

	// Collider��oncoll_bit��reset
	reset_oncoll_bits(ALP_colliders[Sce]);

	// Collider��World���̍X�V
	update_world_trans(ALP_colliders[Sce]);

#ifdef Allow_delay
#else
	// �O�͂̍X�V
	applyexternalforce(ALP_physicses[Sce]);
	timeStep = Al_Global::second_per_frame;
#endif

	//TODO : resolve_contact�̌�Ɏ����Ă���
	// �ʒu�̍X�V
	integrate(ALP_physicses[Sce]);


	// ��G�c�ȓ����蔻��
	Work_meter::start("Broad,Mid,Narrow");

	Work_meter::start("Broadphase");
	Work_meter::tag_start("Broadphase");
	Broadphase(Sce,
		ALP_colliders[Sce], broad_mid_pair,
		moved_collider[Sce], added_collider[Sce]
		);
	Work_meter::tag_stop();
	Work_meter::stop("Broadphase");

	Work_meter::start("Midphase");
	Work_meter::tag_start("Midphase");
	Midphase(broad_mid_pair, pairs);
	Work_meter::tag_stop();
	Work_meter::stop("Midphase");

	// �Փː���
	Work_meter::start("Narrowphase");
	Work_meter::tag_start("Narrowphase");
	generate_contact(pairs);
	Work_meter::tag_stop();
	Work_meter::stop("Narrowphase");

	Work_meter::stop("Broad,Mid,Narrow");

	// �Փˉ���
	Work_meter::start("Resolve");
	Work_meter::tag_start("Resolve");
	resolve_contact(ALP_colliders[Sce], pairs);
	Work_meter::tag_stop();
	Work_meter::stop("Resolve");


#ifdef Allow_delay
	timeStep = Al_Global::second_per_frame;
	// �O�͂̍X�V
	applyexternalforce(ALP_physicses[Sce]);
#endif

	// GO��Collider�̉e����n��
	solv_resolve(ALP_colliders[Sce]);
	resolve_gameobject(ALP_colliders[Sce]);

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
		}
		init = false;
	}

	for (int i = 0; i < size; i++) {
		debug_go[i]->transform->local_pos = Vector3(1000000, 1000000, 1000000);
	}

	int count = 0;
	for (const auto& p : pairs) {
		for (int i = 0; i < p.contacts.contact_num; i++) {
			count += 2;
			if (count > size)break;
			const auto* coll0 = (*p.body[0]->ALPcollider->coll_itr)->transform;
			const auto* coll1 = (*p.body[1]->ALPcollider->coll_itr)->transform;

			debug_go[count - 1]->transform->local_pos = vector3_quatrotate((p.contacts.contactpoints[i].point[0]/* * coll0->scale*/), coll0->orientation) + coll0->position;
			debug_go[count - 2]->transform->local_pos = vector3_quatrotate((p.contacts.contactpoints[i].point[1]/* * coll1->scale*/), coll1->orientation) + coll1->position;
			int adfsdg = 0;
		}
	}

#endif // DEBUG
	return true;

}

bool Phyisics_manager::update_Gui() {
	ImGuiWindowFlags flag = 0;
	//flag |= ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;

	if (ImGui::Begin("physics_global", 0, flag)) {

		//Collider�̕\��
		ImGui::Checkbox("draw_collider", &is_draw_collider);
		//Collider�̕\��
		ImGui::Checkbox("draw_DOP", &is_draw_dop);

		//�d�͂̒���
		ImGui::InputFloat("gravity", &gravity, 0.1f, 1.0f, "%.2f");

		//���m���̒���
		ImGui::InputInt("accuracy", &solver_iterations, 1, 200);

		//�ђʎ��̂΂˂̋���
		ImGui::InputFloat("bias", &bias, 0.01f, 0.1f, "%.3f");
		//�ђʋ��e�덷
		ImGui::InputFloat("slop", &slop, 0.0001f, 0.001f, "%.4f");

		//physics_default�̕\��
		if (ImGui::CollapsingHeader("physics_default")) {

			ImGui::DragFloat("mass", &Phyisics_manager::default_physics.inertial_mass, 0.1f);
			ImGui::DragFloat("linear_drag", &Phyisics_manager::default_physics.linear_drag, 0.01f);
			ImGui::DragFloat("anglar_drag", &Phyisics_manager::default_physics.anglar_drag, 0.01f);
			ImGui::DragFloat("dynamic_friction", &Phyisics_manager::default_physics.dynamic_friction, 0.01f);
			ImGui::DragFloat("static_friction", &Phyisics_manager::default_physics.static_friction, 0.01f);
			ImGui::DragFloat("restitution", &Phyisics_manager::default_physics.restitution, 0.01f);
			ImGui::Checkbox("is_fallable", &Phyisics_manager::default_physics.is_fallable);
			ImGui::Checkbox("is_kinematic", &Phyisics_manager::default_physics.is_kinematic);
			ImGui::Checkbox("is_kinmatic_anglar", &Phyisics_manager::default_physics.is_kinmatic_anglar);
			ImGui::Checkbox("is_kinmatic_linear", &Phyisics_manager::default_physics.is_kinmatic_linear);
			ImGui::Checkbox("is_moveable", &Phyisics_manager::default_physics.is_moveable);
			ImGui::Checkbox("is_hitable", &Phyisics_manager::default_physics.is_hitable);
		}

	}
	ImGui::End();

	return true;
}

bool Phyisics_manager::ray_cast(
	const Vector3& Ray_pos, const Vector3& Ray_dir,
	u_int tag,
	const float ray_min,
	float& tmin, float& tmax,
	Vector3& normal,
	Collider* ret_coll,
	Scenelist Sce) {

	//tmin����tmax�ɂ�����ray���������Ă���
	tmin = +FLT_MAX;
	tmax = -FLT_MAX;
	float min = -FLT_MAX;
	float max = +FLT_MAX;
	Vector3 norm;

	bool ret = false;

	for (const auto& coll : ALP_colliders[Sce]) {
		if (!(coll.tag | tag))continue;

		if (Physics_function::ray_cast(
			Ray_pos, Ray_dir,
			coll,
			ray_min,
			min, max, norm
		) == false) continue;

		ret = true;
		if (tmin > min) {
			tmin = min;
			normal = norm;
		}
		//tmin = ALmin(tmin, min);
		tmax = ALmax(tmax, max);
	}

	return ret;
}

void Phyisics_manager::remove_collider(
	std::list<Collider*>::iterator coll_itr,
	std::list<Physics_function::ALP_Collider>::iterator ALPcoll_itr,
	std::list<Physics_function::ALP_Physics>::iterator ALPphys_itr,
	Scenelist Sce) {

	//�폜���ꂽcolider
	remove_collider_broad_phase(ALPcoll_itr);

	ALP_colliders[Sce].erase(ALPcoll_itr);
	ALP_physicses[Sce].erase(ALPphys_itr);

	colliders[Sce].erase(coll_itr);

}

void Phyisics_manager::remove_collider(
	std::list<Collider*>::iterator coll_itr,
	std::list<Physics_function::ALP_Collider>::iterator ALPcoll_itr,
	Scenelist Sce) {

	//�폜���ꂽcolider
	remove_collider_broad_phase(ALPcoll_itr);

	ALP_colliders[Sce].erase(ALPcoll_itr);

	colliders[Sce].erase(coll_itr);
}

#include "../Main/systems.h"
#include "../Mesh/material_for_collider.h"

bool Phyisics_manager::render_collider(Scenelist Sce) {
	render_dop(Sce);

	if (is_draw_collider == false)return false;

	for (auto& itr : ALP_colliders[Sce]) {
		Collider_renderer::render_collider(itr);
	}

	return true;
}

bool Phyisics_manager::render_dop(Scenelist Sce) {
	if (is_draw_dop == false)return false;

	for (auto& itr : ALP_colliders[Sce]) {
		Collider_renderer::render_AABB(itr);
	}

	return true;

}

void Phyisics_manager::destroy(Scenelist Sce) {

	colliders[Sce].clear();
}

