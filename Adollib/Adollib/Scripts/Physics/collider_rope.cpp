#include "collider_rope.h"

#include "ALP__physics_manager.h"

#include "../Object/gameobject.h"
#include "../Main/Adollib.h"
#include "ALP_contact.h"

#include "ALP_collider.h"
#include "ALP_physics.h"
#include "ALP_joint.h"

#include "shape_rope.h"

#include "../Main/Adollib.h"

#include "joint.h"


using namespace Adollib;
using namespace Physics_function;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Rigitbody
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

const bool Collider_Rope::concoll_enter(const Collider_tagbit tag_name) {
	bool is_oncoll = false;
	for (auto& ptr : colliders)  is_oncoll = is_oncoll || ptr->concoll_enter(tag_name);
	return is_oncoll;
}

void Collider_Rope::add_force(const Vector3& force, const bool& is_force_local) {
	if (Al_Global::second_per_frame > inv60)return;
	for (auto& ptr : colliders)  ptr->add_force(force * Al_Global::second_per_frame, is_force_local);
}
void Collider_Rope::add_force(const Vector3& force, const Vector3& position, const bool& is_position_local, const bool& is_force_local) {
	if (Al_Global::second_per_frame > inv60)return;
	for (auto& ptr : colliders)  ptr->add_force(force * Al_Global::second_per_frame, position, is_position_local, is_force_local);
}
void Collider_Rope::add_torque(const Vector3& force, const bool& is_local) {
	if (Al_Global::second_per_frame > inv60)return;
	for (auto& ptr : colliders)  ptr->add_torque(force * Al_Global::second_per_frame, is_local);
}
void Collider_Rope::add_linear_acc(const Vector3& acc) {
	if (Al_Global::second_per_frame > inv60)return;
	for (auto& ptr : colliders)  ptr->add_linear_acc(acc * Al_Global::second_per_frame);
}
void Collider_Rope::add_angula_acc(const Vector3& acc) {
	if (Al_Global::second_per_frame > inv60)return;
	for (auto& ptr : colliders)  ptr->add_angula_acc(acc * Al_Global::second_per_frame);
}


#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// �A�^�b�`���ꂽjoint�̐�
const int Collider_Rope::get_joint_count() {
	return joints[Rope_constraint_type::sructural_spring].size() + joints[Rope_constraint_type::bending_spring].size();
}
Joint_base* Collider_Rope::get_joint_ptr(int joint_num) {
	if (joint_num < 0)return nullptr;

	const int bending_spring_size = joints[Rope_constraint_type::bending_spring].size();
	const int sructural_spring_size = joints[Rope_constraint_type::sructural_spring].size();

	if (sructural_spring_size + bending_spring_size <= joint_num)return nullptr;

	if (sructural_spring_size > joint_num) return joints[Rope_constraint_type::sructural_spring][joint_num];
	else  return joints[Rope_constraint_type::bending_spring][joint_num - sructural_spring_size];
}

void Collider_Rope::set_vertex_data(const int& vertex_num, const Physics_data& physics_data) {
	colliders.at(vertex_num)->physics_data = physics_data;
}
const Physics_data& Collider_Rope::get_vertex_data(const int& vertex_num) const {
	return colliders.at(vertex_num)->physics_data;
}

void Collider_Rope::create_rope() {

	vertex_offset = std::make_shared<std::vector<std::pair<Vector3, Vector3>>>();
	vertex_offset->resize(1);
	vertex_offset->resize(sphere_num_size);

	colliders.resize(sphere_num_size);

	// sphere_size����collider�𐶐�
	for (int sphere_num = 0; sphere_num < sphere_num_size; ++sphere_num) {

		// collider�����Ƃ��Ĉ��� (GO�ɒ��ڃA�^�b�`����킯�ł͂Ȃ����� �C������������ǎd���Ȃ�)
		auto collider = new Collider;
		collider->gameobject = gameobject;
		collider->awake();
		collider->transform = std::make_shared<Transform>();
		collider->physics_data = default_physics_data;
		collider->tag = tag;
		collider->ignore_tags = ignore_tags;

		auto croth = collider->add_shape<Rope_vertex>();

		croth->vertex_id = sphere_num;
		croth->size = sphere_size_r;
		croth->vertex_offset = vertex_offset;

		vertex_offset->at(sphere_num) = std::pair<Vector3, Vector3>(Vector3(0), Vector3(0));

		colliders.at(sphere_num) = collider;
	}

	constexpr float sructural_stretch = 0.10f;
	constexpr float sructural_shrink = 0.40f;
	constexpr float bending_stretch = 0.10f;
	constexpr float bending_shrink = 0.f;

	// sphere���Ȃ�joint�̐���
	for (int collider_num = 0; collider_num < sphere_num_size; ++collider_num) {

		// �\���΂˂̒ǉ�
		if (collider_num + 1 < sphere_num_size) {
			auto joint = Joint::add_balljoint(
				colliders[collider_num], colliders[collider_num + 1],
				Vector3(0), Vector3(0)
			);

			joint->offset = sphree_offset_size;
			joint->stretch_bias = sructural_stretch;
			joint->shrink_bias = sructural_shrink;

			joints[Rope_constraint_type::bending_spring].emplace_back(joint);
		}

		// �Ȃ��΂˂̒ǉ�
		if (collider_num + 2 < sphere_num_size) {
			auto joint = Joint::add_balljoint(
				colliders[collider_num], colliders[collider_num + 2],
				Vector3(0), Vector3(0)
			);

			joint->offset = sphree_offset_size;
			joint->stretch_bias = bending_stretch;
			joint->shrink_bias = bending_shrink;

			joints[Rope_constraint_type::sructural_spring].emplace_back(joint);
		}

	}

	for (int collider_num = 0; collider_num < sphere_num_size; ++collider_num) {
		vertex_offset->at(collider_num).first = start_rope_dir * sphree_offset_size * collider_num;
	}





}

void Collider_Rope::update() {
	int vertex_offset_size = vertex_offset->size();
	if (vertex_offset_size <= 1)return;

	float offset = joints[Rope_constraint_type::sructural_spring][0]->offset;

	for (int i = 0; i < vertex_offset_size; ++i) {

		if (i == vertex_offset_size - 1)continue;

		float dis = vector3_distance(vertex_offset->at(i).first, vertex_offset->at(i + 1).first);

		if (offset + 0.1f < dis) offset = dis - 0.1f;
	}

	if (joints[Rope_constraint_type::sructural_spring][0]->offset != offset) {
		float n = offset - joints[Rope_constraint_type::sructural_spring][0]->offset;
		n /= get_joint_count();
		for (auto& joint : joints[Rope_constraint_type::sructural_spring])joint->offset += n;
		for (auto& joint : joints[Rope_constraint_type::bending_spring])joint->offset += n;
	}

}

void Collider_Rope::awake() {
	// �����l�̒���
	Physics_manager::physicsParams.set_default_physics_data(default_physics_data);

	default_physics_data.anglar_drag = 0.9f;
	default_physics_data.inertial_mass = 2;

	//::: rope��sphere�̌� ::::
	sphere_num_size = 5;

	//::: rope��sphere�̔��a ::::
	sphere_size_r = 0.5f;

	//::: rope��sphree�̋��� ::::
	sphree_offset_size = 0.5f;
}

void Collider_Rope::finalize() {

	for (auto& ptr : colliders)  ptr->finalize();

	for (auto& ptr : colliders)  delete ptr;
	int adsfdg = 0;
}

void Collider_Rope::Update_hierarchy() {

	for (auto& ptr : colliders)  ptr->Update_hierarchy();

}
