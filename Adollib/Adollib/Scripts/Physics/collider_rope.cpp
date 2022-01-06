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
const int Collider_Rope::get_joint_count() {

	return joints[Rope_constraint_type::bending_spring].size() + joints[Rope_constraint_type::sructural_spring].size();
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
		collider->physics_data = default_physics_data;

		auto croth = collider->add_shape<Rope_vertex>();

		croth->vertex_id = sphere_num;
		croth->size = sphere_size_r;
		croth->vertex_offset = vertex_offset;

		vertex_offset->at(sphere_num) = std::pair<Vector3, Vector3>(Vector3(0), Vector3(0));

		colliders.at(sphere_num) = collider;
	}

	constexpr float sructural_stretch = 0.10f;
	constexpr float sructural_shrink = 0.10f;
	constexpr float bending_stretch = 0.10f;
	constexpr float bending_shrink = 0.f;

	// sphereをつなぐjointの生成
	for (int collider_num = 0; collider_num < sphere_num_size; ++collider_num) {

		// 構成ばねの追加
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

		// 曲げばねの追加
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
		vertex_offset->at(collider_num).first = Vector3(0, 0, 1) * sphree_offset_size * collider_num;
	}





}

void Collider_Rope::update() {

	//// 各facetのnormalを更新する
	//std::vector<std::vector<Vector3>> mesh_facet_normals;
	//mesh_facet_normals.resize(meshcoll_data->size());
	//{
	//	int mesh_num = 0;
	//	for (auto& data : *meshcoll_data) {

	//		auto& facet_normals = mesh_facet_normals[mesh_num];
	//		facet_normals.resize(data.facets.size());

	//		int facet_mum = 0;
	//		for (auto& facet : data.facets) {

	//			const Vector3& vertex_0 = data.vertices[facet.vertexID[0]] + vertex_offset->at(mesh_num).at(facet.vertexID[0]).first;
	//			const Vector3& vertex_1 = data.vertices[facet.vertexID[1]] + vertex_offset->at(mesh_num).at(facet.vertexID[1]).first;
	//			const Vector3& vertex_2 = data.vertices[facet.vertexID[2]] + vertex_offset->at(mesh_num).at(facet.vertexID[2]).first;

	//			const Vector3& normal = -vector3_cross(vertex_1 - vertex_0, vertex_2 - vertex_0).unit_vect();

	//			facet_normals[facet_mum] = normal;
	//			++facet_mum;
	//		}

	//		++mesh_num;
	//	}
	//}

	////各頂点の属する面から平均をとってoffsetを計算
	//{
	//	int mesh_num = 0;
	//	for (auto& data : *meshcoll_data) {

	//		int vertex_num = 0;
	//		for (auto& vertex : data.vertices) {

	//			//vertex_offset->at(mesh_num).at(vertex_num).second;

	//			Vector3 facet_normal;
	//			for (int facet_num = 0; facet_num < data.vertex_involvements[vertex_num].get_facet_involvment_size(); ++facet_num) {

	//				facet_normal += mesh_facet_normals[mesh_num][data.vertex_involvements[vertex_num].facet_involvements[facet_num]];
	//			}
	//			facet_normal /= data.vertex_involvements[vertex_num].get_facet_involvment_size();

	//			vertex_offset->at(mesh_num).at(vertex_num).second = facet_normal;


	//			++vertex_num;
	//		}

	//		++mesh_num;
	//	}
	//}




}

void Collider_Rope::awake() {
	// 初期値の調整
	Physics_manager::physicsParams.set_default_physics_data(default_physics_data);

	default_physics_data.anglar_drag = 0.9f;
	default_physics_data.inertial_mass = 0.1f;

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


// 重心のlocal座標を返す
const Vector3 Collider_Rope::get_barycenter() const {

	Physics_function::Physics_manager::mutex_lock();

	Vector3 num_barycenter;
	for (auto& ptr : colliders)  num_barycenter += ptr->get_barycenter();
	num_barycenter /= colliders.size();

	Physics_function::Physics_manager::mutex_unlock();

	return num_barycenter;
};