#include "collider_croth.h"

#include "ALP__physics_manager.h"

#include "../Object/gameobject.h"
#include "../Main/Adollib.h"
#include "ALP_contact.h"

#include "ALP_collider.h"
#include "ALP_physics.h"
#include "ALP_joint.h"

#include "shape_meshcoll.h"
#include "shape_croth.h"

#include "../Main/Adollib.h"


using namespace Adollib;
using namespace Physics_function;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Rigitbody
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

const bool Collider_Croth::concoll_enter(const Collider_tagbit tag_name) {
	bool is_oncoll = false;
	for (auto ptr : colliders) is_oncoll = is_oncoll || ptr->concoll_enter(tag_name);
	return is_oncoll;
}

void Collider_Croth::add_force(const Vector3& force, const bool& is_force_local) {
	if (Al_Global::second_per_frame > inv60)return;
	for (auto ptr : colliders) ptr->add_force(force * Al_Global::second_per_frame, is_force_local);
}
void Collider_Croth::add_force(const Vector3& force, const Vector3& position, const bool& is_position_local, const bool& is_force_local) {
	if (Al_Global::second_per_frame > inv60)return;
	for (auto ptr : colliders) ptr->add_force(force * Al_Global::second_per_frame, position, is_position_local, is_force_local);
}
void Collider_Croth::add_torque(const Vector3& force, const bool& is_local) {
	if (Al_Global::second_per_frame > inv60)return;
	for (auto ptr : colliders) ptr->add_torque(force * Al_Global::second_per_frame, is_local);
}
void Collider_Croth::add_linear_acc(const Vector3& acc) {
	if (Al_Global::second_per_frame > inv60)return;
	for (auto ptr : colliders) ptr->add_linear_acc(acc * Al_Global::second_per_frame);
}
void Collider_Croth::add_angula_acc(const Vector3& acc) {
	if (Al_Global::second_per_frame > inv60)return;
	for (auto ptr : colliders) ptr->add_angula_acc(acc * Al_Global::second_per_frame);
}


#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// アタッチされたjointの数
const int Collider_Croth::get_joint_count() {
	int joint_count = 0;
	for (auto ptr : colliders)joint_count += ptr->get_joint_count();
	return joint_count;
}

// 指定した番号にアタッチされているjointの情報を得る
Joint_base* Collider_Croth::get_joint(const int num) {

	int joint_count = 0;
	for (auto ptr : colliders) {

		const int& get_joint_count = ptr->get_joint_count();
		if (joint_count + get_joint_count > num) {
			return ptr->get_joint(num - joint_count);
		}
		joint_count += get_joint_count;
	}

	return nullptr;
}


void Collider_Croth::load_file(const std::string& filename, bool is_right_rtiangle, bool is_permit_edge_have_many_facet) {

	//FBXのLoadを行う
	std::vector<Physics_function::Meshcollider_data>* meshcoll_data = nullptr;
	Physics_function::Collider_ResourceManager::CreateMCFromFBX(filename.c_str(), &meshcoll_data, is_right_rtiangle, is_permit_edge_have_many_facet);

	vertex_offset = std::make_shared<Vector3>();

	// すべての頂点情報から個別にColliderを生成
	for (auto& data : *meshcoll_data) {

		int vertex_size = data.vertices.size();
		for (int vertex_num = 0; vertex_num < vertex_size; ++vertex_num) {

			// colliderを情報として扱う (気持ち悪いけれど仕方ない)
			auto collider = new Collider;
			collider->gameobject = gameobject;
			collider->awake();

			auto croth = collider->add_shape<Croth_vertex>();

			croth->center = Vector3(0);
			croth->mesh_data = &data;
			croth->vertex_id = vertex_num;

			vertex_offset->emplace_back(Vector3(0));

			colliders.emplace_back(collider);
		}
	}


	// vertexの情報からばねを作成
	for (auto& data : *meshcoll_data) {
		std::unordered_map<int, int> edgeID_Table;

		int vertex_size = data.vertices.size();
		for (int vertex_num = 0; vertex_num < vertex_size; ++vertex_num) {

			// 構成ばねの設定
			for (int edge_num = 0; edge_num < data.vertex_involvements[vertex_num].get_edge_involvment_size(); ++edge_num) {
				// 面の中で一番長いエッジは無視
				if (data.vertex_involvements[vertex_num].edge_involvements[edge_num].is_edge_longer == true)continue;

				const auto& edge = data.edges[data.vertex_involvements[vertex_num].edge_involvements[edge_num].edge_id];

				const int vertId0 = ALmax(edge.vertexID[0], edge.vertexID[1]);
				const int vertId1 = ALmin(edge.vertexID[0], edge.vertexID[1]);

				const u_int b = vertId1 * vertId1 - vertId1;

				// vertId0, vertId1から作られるユニークな数字
				int tableId = (int)(b + vertId0 * 2);

				// 登録していなかったら追加
				if (edgeID_Table.count(tableId) == 0) {
					Croth_constraint constraint;
					constraint.vertId0 = vertId0;
					constraint.vertId1 = vertId1;
					constraint.constraint_type = croth_constraint_type::sructural_spring;
					constraint.natural_length = vector3_distance_sqr(data.vertices[vertId0], data.vertices[vertId1]);

					edgeID_Table[tableId] = croth_constraints.size();
					croth_constraints.emplace_back(constraint);
				}
				croth_constraints.at(edgeID_Table[tableId]).constraint_type = croth_constraint_type::sructural_spring;


				const int pair_vertex_num = vertex_num == vertId0 ? vertId1 : vertId0;


				// 構成ばねからつながるせん断ばね、曲げばねの設定
				for (int pair_edge_num = 0; pair_edge_num < data.vertex_involvements[pair_vertex_num].get_edge_involvment_size(); ++pair_edge_num) {
					// 面の中で一番長いエッジは無視
					if (data.vertex_involvements[pair_vertex_num].edge_involvements[pair_edge_num].is_edge_longer == true)continue;

					const auto& edge = data.edges[data.vertex_involvements[pair_vertex_num].edge_involvements[pair_edge_num].edge_id];

					const int vertId0 = ALmax(edge.vertexID[0], edge.vertexID[1]);
					const int vertId1 = ALmin(edge.vertexID[0], edge.vertexID[1]);

					const u_int b = vertId1 * vertId1 - vertId1;

					// vertId0, vertId1から作られるユニークな数字
					int tableId = (int)(b + vertId0 * 2);

					// 登録していなかったら追加
					if (edgeID_Table.count(tableId) == 0) {
						Croth_constraint constraint;
						constraint.vertId0 = vertId0;
						constraint.vertId1 = vertId1;
						constraint.constraint_type = croth_constraint_type::bending_spring; //初めは曲げばね
						constraint.natural_length = vector3_distance_sqr(data.vertices[vertId0], data.vertices[vertId1]);

						edgeID_Table[tableId] = croth_constraints.size();
						croth_constraints.emplace_back(constraint);
					}
					else {
						// すでに登録されて、曲げばねだった場合せん断ばねにする
						if (croth_constraints.at(edgeID_Table[tableId]).constraint_type == croth_constraint_type::bending_spring) {
							croth_constraints.at(edgeID_Table[tableId]).constraint_type = croth_constraint_type::shear_spring;
						};
					}

				}


			}

		}
	}

}

void Collider_Croth::update() {

	for (auto& constraint : croth_constraints) {





	}
}

void Collider_Croth::finalize() {

	for (auto ptr : colliders) ptr->finalize();

	for (auto ptr : colliders)delete ptr;
}

void Collider_Croth::Update_hierarchy() {

	for (auto ptr : colliders) ptr->Update_hierarchy();

}


// 重心のlocal座標を返す
const Vector3 Collider_Croth::get_barycenter() const {

	Physics_function::Physics_manager::mutex_lock();

	Vector3 num_barycenter;
	for (auto ptr : colliders) num_barycenter += ptr->get_barycenter();
	num_barycenter /= colliders.size();

	Physics_function::Physics_manager::mutex_unlock();

	return num_barycenter;
};