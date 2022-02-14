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

// アタッチされたjointの数
const int Collider_Rope::get_joint_count(Rope_constraint_type type) {
	return joints[type].size();
}
Joint_base* Collider_Rope::get_joint_ptr(Rope_constraint_type type, int joint_num) {
	if (joint_num < 0)return nullptr;

	const int size = joints[type].size();

	if (size <= joint_num)return nullptr;

	else  return joints[type][joint_num];
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

	// sphere_sizeからcolliderを生成
	for (int sphere_num = 0; sphere_num < sphere_num_size; ++sphere_num) {

		// colliderを情報として扱う (GOに直接アタッチするわけではないから 気持ち悪いけれど仕方ない)
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
	//constexpr float sructural_shrink = 0.40f;
	constexpr float sructural_shrink = 0.9f;
	constexpr float bending_stretch = 0.10f;
	constexpr float bending_shrink = 0.1f;

	// sphereをつなぐjointの生成
	for (int collider_num = 0; collider_num < sphere_num_size; ++collider_num) {

		// 構成ばねの追加
		if (collider_num + 1 < sphere_num_size) {
			auto joint = Joint::add_balljoint(
				colliders[collider_num], colliders[collider_num + 1],
				Vector3(0), Vector3(0)
			);

			joint->offset = sphree_offset_size * 1.01f;
			joint->stretch_bias = sructural_stretch;
			joint->shrink_bias = sructural_shrink;

			joints[Rope_constraint_type::sructural_spring].emplace_back(joint);
		}

		// 曲げばねの追加
		if (collider_num + 2 < sphere_num_size) {
			auto joint = Joint::add_balljoint(
				colliders[collider_num], colliders[collider_num + 2],
				Vector3(0), Vector3(0)
			);

			joint->offset = sphree_offset_size * 1.5f;
			joint->slop = sphree_offset_size * 0.5f;
			joint->stretch_bias = bending_stretch;
			joint->shrink_bias = bending_shrink;

			joints[Rope_constraint_type::bending_spring].emplace_back(joint);
		}

	}

	// 初期座標の設定
	for (int collider_num = 0; collider_num < sphere_num_size; ++collider_num) {
		vertex_offset->at(collider_num).first = start_rope_dir * sphree_offset_size * collider_num;
	}





}

void Collider_Rope::update() {

	// 構成ばねの長さが0ならけす
	int structural_spring_size = joints[Rope_constraint_type::sructural_spring].size();

	std::vector<Collider*> ignore_colliders;


	for (int i = 0; i < structural_spring_size; ++i) {
		auto& joint = joints[Rope_constraint_type::sructural_spring][i];
		if (joint == nullptr) continue;
		if (joint->offset != 0)continue;

		ignore_colliders.emplace_back(joint->get_colliderB());

		// bending_springの削除、再設定を行う
		{
			// 自身を挟んで存在するbending_springの削除
			if (i < structural_spring_size - 2) {
				Joint::delete_joint(joints[Rope_constraint_type::bending_spring][i]);
				joints[Rope_constraint_type::bending_spring][i] = nullptr;
			}

			if (i > 0) {
				// 後ろ
				auto collider_A = joints[Rope_constraint_type::bending_spring][i - 1]->get_colliderA();
				const float stretch_bias = joints[Rope_constraint_type::bending_spring][i - 1]->stretch_bias;
				const float shrink_bias = joints[Rope_constraint_type::bending_spring][i - 1]->shrink_bias;
				const float offset = joints[Rope_constraint_type::bending_spring][i - 1]->offset;

				Joint::delete_joint(joints[Rope_constraint_type::bending_spring][i - 1]);
				joints[Rope_constraint_type::bending_spring][i - 1] = nullptr;

				if (i > structural_spring_size - 1) {
					auto collider_B = joints[Rope_constraint_type::sructural_spring][i + 1]->get_colliderB();

					joints[Rope_constraint_type::bending_spring][i - 1] = Joint::add_balljoint(
						collider_A, collider_B,
						Vector3(0), Vector3(0)
					);
					joints[Rope_constraint_type::bending_spring][i - 1]->stretch_bias = stretch_bias;
					joints[Rope_constraint_type::bending_spring][i - 1]->shrink_bias = shrink_bias;
					joints[Rope_constraint_type::bending_spring][i - 1]->offset = offset;

				}
			}

			if (i < structural_spring_size - 2) {
				// 前
				auto collider_B = joints[Rope_constraint_type::bending_spring][i + 1]->get_colliderB();
				const float stretch_bias = joints[Rope_constraint_type::bending_spring][i + 1]->stretch_bias;
				const float shrink_bias = joints[Rope_constraint_type::bending_spring][i + 1]->shrink_bias;
				const float offset = joints[Rope_constraint_type::bending_spring][i + 1]->offset;

				Joint::delete_joint(joints[Rope_constraint_type::bending_spring][i + 1]);
				joints[Rope_constraint_type::bending_spring][i + 1] = nullptr;

				if (1) {
					auto collider_A = joints[Rope_constraint_type::sructural_spring][i + 0]->get_colliderA();

					joints[Rope_constraint_type::bending_spring][i + 1] = Joint::add_balljoint(
						collider_A, collider_B,
						Vector3(0), Vector3(0)
					);
					joints[Rope_constraint_type::bending_spring][i + 1]->stretch_bias = stretch_bias;
					joints[Rope_constraint_type::bending_spring][i + 1]->shrink_bias = shrink_bias;
					joints[Rope_constraint_type::bending_spring][i + 1]->offset = offset;

				}
			}
		}

		// structural_springの削除、再設定
		{
			// offsetが0のjointを削除
			auto collider_A = joint->get_colliderA();

			Joint::delete_joint(joint);
			joint = nullptr;

			if (i < structural_spring_size - 1) {
				// 削除したjointの内 indexの大きいcolliderを、無視するため、
				// そのcolliderの接続しているsructuralなjointをつなぎなおす

				auto collider_B = joints[Rope_constraint_type::sructural_spring][i + 1]->get_colliderB();
				const float stretch_bias = joints[Rope_constraint_type::sructural_spring][i + 1]->stretch_bias;
				const float shrink_bias = joints[Rope_constraint_type::sructural_spring][i + 1]->shrink_bias;
				const float offset = joints[Rope_constraint_type::sructural_spring][i + 1]->offset;

				Joint::delete_joint(joints[Rope_constraint_type::sructural_spring][i + 1]);
				joints[Rope_constraint_type::sructural_spring][i + 1] = nullptr;

				joints[Rope_constraint_type::sructural_spring][i + 1] = Joint::add_balljoint(
					collider_A, collider_B,
					Vector3(0), Vector3(0)
				);

				joints[Rope_constraint_type::sructural_spring][i + 1]->stretch_bias = stretch_bias;
				joints[Rope_constraint_type::sructural_spring][i + 1]->shrink_bias = shrink_bias;
				joints[Rope_constraint_type::sructural_spring][i + 1]->offset = offset;

			}
		}


		std::vector<Joint_base*> sructural_buf;
		std::vector<Joint_base*> bending_buf;

		for (auto joint : joints[Rope_constraint_type::sructural_spring]) if (joint != nullptr)sructural_buf.emplace_back(joint);
		for (auto joint : joints[Rope_constraint_type::bending_spring]) if (joint != nullptr)bending_buf.emplace_back(joint);

		//joints[Rope_constraint_type::sructural_spring].swap(sructural_buf);
		//joints[Rope_constraint_type::bending_spring].swap(bending_buf);
		joints[Rope_constraint_type::sructural_spring] = (sructural_buf);
		joints[Rope_constraint_type::bending_spring] = (bending_buf);

		structural_spring_size = joints[Rope_constraint_type::sructural_spring].size();
		--i;
	}

	{}

	{
		int collider_size = colliders.size();
		for (auto& coll : ignore_colliders) {

			for (int i = 0; i < collider_size; ++i) {
				if (colliders[i] == coll) ignore_coll_num.emplace_back(i);
			}

		}

		for (auto& num : ignore_coll_num) {
			vertex_offset->at(num).first = vertex_offset->at(0).first;
		}
	}

}

void Collider_Rope::awake() {
	// 初期値の調整
	Physics_manager::physicsParams.set_default_physics_data(default_physics_data);

	default_physics_data.anglar_drag = 0.9f;
	default_physics_data.dynamic_friction = 0;
	default_physics_data.inertial_mass = 2;

	//::: ropeのsphereの個数 ::::
	sphere_num_size = 5;

	//::: ropeのsphereの半径 ::::
	sphere_size_r = 0.5f;

	//::: ropeのsphreeの距離 ::::
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
