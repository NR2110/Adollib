#include "collider_croth.h"

#include "ALP__physics_manager.h"

#include "ALP_contact.h"

#include "ALP_collider.h"
#include "ALP_physics.h"
#include "ALP_joint.h"

#include "shape_meshcoll.h"
#include "shape_croth.h"

#include "joint.h"


using namespace Adollib;
using namespace Physics_function;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Rigitbody
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

const bool Collider_Croth::concoll_enter(const Collider_tagbit tag_name) {
	bool is_oncoll = false;
	for (auto& map : colliders) for (auto ptr : map.second) is_oncoll = is_oncoll || ptr->concoll_enter(tag_name);
	return is_oncoll;
}

void Collider_Croth::add_force(const Vector3& force, const float& delta_time, const bool& is_force_local) {
	if (delta_time > inv60)return;
	for (auto& map : colliders) for (auto ptr : map.second) ptr->add_force(force * delta_time, is_force_local);
}
void Collider_Croth::add_force(const Vector3& force, const Vector3& position, const float& delta_time, const bool& is_position_local, const bool& is_force_local) {
	if (delta_time > inv60)return;
	for (auto& map : colliders) for (auto ptr : map.second) ptr->add_force(force * delta_time, position, is_position_local, is_force_local);
}
void Collider_Croth::add_torque(const Vector3& force, const float& delta_time, const bool& is_local) {
	if (delta_time > inv60)return;
	for (auto& map : colliders) for (auto ptr : map.second) ptr->add_torque(force * delta_time, is_local);
}
void Collider_Croth::add_linear_acc(const Vector3& acc, const float& delta_time) {
	if (delta_time > inv60)return;
	for (auto& map : colliders) for (auto ptr : map.second) ptr->add_linear_acc(acc, delta_time);
}
void Collider_Croth::add_angula_acc(const Vector3& acc, const float& delta_time) {
	if (delta_time > inv60)return;
	for (auto& map : colliders) for (auto ptr : map.second) ptr->add_angula_acc(acc, delta_time);
}


#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// アタッチされたjointの数
const int Collider_Croth::get_joint_count() {
	int joint_count = 0;
	for (auto& map : colliders) for (auto ptr : map.second)joint_count += ptr->get_joint_count();
	return joint_count;
}

// 指定した番号にアタッチされているjointの情報を得る
Joint_base* Collider_Croth::get_joint(const int num) {

	int joint_count = 0;
	for (auto& map : colliders) for (auto ptr : map.second) {

		const int& get_joint_count = ptr->get_joint_count();
		if (joint_count + get_joint_count > num) {
			return ptr->get_joint(num - joint_count);
		}
		joint_count += get_joint_count;
	}

	return nullptr;
}

void Collider_Croth::set_vertex_data(const int& mesh_num, const int& vertex_num, const Physics_data& physics_data) {
	colliders.at(&meshcoll_data->at(mesh_num)).at(vertex_num)->physics_data = physics_data;
}
const Physics_data& Collider_Croth::get_vertex_data(const int& mesh_num, const int& vertex_num) const {
	return colliders.at(&meshcoll_data->at(mesh_num)).at(vertex_num)->physics_data;
}

void Collider_Croth::load_file(const std::string& filename, bool is_right_rtiangle, const void* unique_ptr, const DirectX::XMFLOAT3& Wpos, const DirectX::XMFLOAT4& Worient, const DirectX::XMFLOAT3& Wscale, const DirectX::XMFLOAT4& pearent_Worient_inv, const bool is_permit_edge_have_many_facet) {
	//FBXのLoadを行う
	//std::vector<Physics_function::Meshcollider_data>* meshcoll_data = nullptr;
	meshcoll_data = nullptr;
	Physics_function::Collider_ResourceManager::CreateMCFromFBX(filename.c_str(), &meshcoll_data, is_right_rtiangle, is_permit_edge_have_many_facet);

	vertex_offset = std::make_shared<std::vector<std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>>>>();
	vertex_offset->resize(meshcoll_data->size());

	// すべての頂点情報から個別にColliderを生成
	int mesh_id = 0;
	for (auto& data : *meshcoll_data) {

		int vertex_size = data.vertices.size();
		auto& colliders_per_mesh = colliders[&data];
		colliders_per_mesh.resize(vertex_size);

		for (int vertex_num = 0; vertex_num < vertex_size; ++vertex_num) {

			vertex_offset->at(mesh_id).emplace_back(std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>(DirectX::XMFLOAT3(0,0,0), DirectX::XMFLOAT3(0,0,0)));

			// colliderを情報として扱う (気持ち悪いけれど仕方ない)
			auto collider = std::make_shared<Collider>();
			collider->tag = tag;
			collider->ignore_tags = ignore_tags;
			collider->awake(unique_ptr, Wpos, Worient, Wscale, pearent_Worient_inv);

			collider->physics_data = default_physics_data;

			auto croth = collider->add_shape<Croth_vertex>();

			croth->mesh_data = &data;
			croth->mesh_id = mesh_id;
			croth->vertex_id = vertex_num;
			croth->vertex_offset = vertex_offset;
			croth->size = default_sphere_r;


			colliders_per_mesh.at(vertex_num) = collider;
		}

		++mesh_id;
	}


	// vertexの情報からばねを作成
	for (auto& data : *meshcoll_data) {
		std::unordered_map<int, int> edgeID_Table;

		int vertex_size = data.vertices.size();
		for (int vertexID = 0; vertexID < vertex_size; ++vertexID) {

			// ばねの設定
			for (int edge_num = 0; edge_num < data.vertex_involvements[vertexID].get_edge_involvment_size(); ++edge_num) {
				// 面の中で一番長いエッジは無視
				if (data.vertex_involvements[vertexID].edge_involvements[edge_num].is_edge_longer == true)continue;

				int pair_vertexID = 0;
				// 構成ばねの設定
				{
					const auto& edge = data.edges[data.vertex_involvements[vertexID].edge_involvements[edge_num].edge_id];

					const int vertId0 = ALmin(edge.vertexID[0], edge.vertexID[1]);
					const int vertId1 = ALmax(edge.vertexID[0], edge.vertexID[1]);

					const u_int b = vertId1 * vertId1 - vertId1;

					// vertId0, vertId1から作られるユニークな数字
					int tableId = (int)(b + vertId0 * 2);

					// 登録していなかったら追加
					if (edgeID_Table.count(tableId) == 0) {
						Croth_constraint constraint;
						constraint.vertId0 = vertId0;
						constraint.vertId1 = vertId1;
						constraint.mesh = &data;
						constraint.constraint_type = croth_constraint_type::sructural_spring;
						constraint.natural_length = vector3_distance_sqr(data.vertices[vertId0] * Wscale, data.vertices[vertId1] * Wscale);

						edgeID_Table[tableId] = croth_constraints.size();
						croth_constraints.emplace_back(constraint);
					}

					croth_constraints.at(edgeID_Table[tableId]).constraint_type = croth_constraint_type::sructural_spring;

					// vertex_numとのpairIDを保存
					pair_vertexID = (vertexID == vertId0 ? vertId1 : vertId0);
				}


				// 構成ばねからつながるせん断ばね、曲げばねの設定
				for (int pair_edge_num = 0; pair_edge_num < data.vertex_involvements[pair_vertexID].get_edge_involvment_size(); ++pair_edge_num) {
					// 面の中で一番長いエッジは無視
					if (data.vertex_involvements[pair_vertexID].edge_involvements[pair_edge_num].is_edge_longer == true)continue;

					const auto& edge = data.edges[data.vertex_involvements[pair_vertexID].edge_involvements[pair_edge_num].edge_id];

					// pair_vertexから延びるvertexとvertex_numをつなげるため IDがpair_edge_numのところをvertex_numに置き換える
					const int edgeID_buf0 = (edge.vertexID[0] == pair_vertexID ? vertexID : edge.vertexID[0]);
					const int edgeID_buf1 = (edge.vertexID[1] == pair_vertexID ? vertexID : edge.vertexID[1]);
					const int vertId0 = ALmin(edgeID_buf0, edgeID_buf1);
					const int vertId1 = ALmax(edgeID_buf0, edgeID_buf1);

					// IDの小さいから大きいほうにつないでいく vertId0がvertexIDでなければ大きいほうからつなごうとしているためcontinue
					if (vertId0 != vertexID)continue;

					// 自身と自信はつなげない
					if (vertId1 == vertId0)continue;

					const u_int b = vertId1 * vertId1 - vertId1;

					// vertId0, vertId1から作られるユニークな数字
					int tableId = (int)(b + vertId0 * 2);

					// 登録していなかったら追加
					if (edgeID_Table.count(tableId) == 0) {
						Croth_constraint constraint;
						constraint.vertId0 = vertId0;
						constraint.vertId1 = vertId1;
						constraint.mesh = &data;
						constraint.constraint_type = croth_constraint_type::bending_spring; //初めは曲げばね
						constraint.natural_length = vector3_distance_sqr(data.vertices[vertId0] * Wscale, data.vertices[vertId1] * Wscale);

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

	constexpr float sructural_stretch = 0.50f;
	constexpr float sructural_shrink  = 0.50f;
	constexpr float shear_stretch     = 0.20f;
	constexpr float shear_shrink      = 0.001f;
	constexpr float bending_stretch   = 0.10f;
	constexpr float bending_shrink    = 0.50f;
	{
		for (auto& spring : croth_constraints) {

			if (spring.constraint_type == croth_constraint_type::sructural_spring) {

				auto joint = Joint::add_balljoint(
					colliders[spring.mesh].at(spring.vertId0), colliders[spring.mesh].at(spring.vertId1),
					Vector3(0), Vector3(0)
				);

				joint->offset = spring.natural_length;
				//joint->slop = spring.natural_length;
				joint->stretch_bias = sructural_stretch;
				joint->shrink_bias  = sructural_shrink;

			}
			else if (spring.constraint_type == croth_constraint_type::shear_spring) {

				auto joint = Joint::add_balljoint(
					colliders[spring.mesh].at(spring.vertId0), colliders[spring.mesh].at(spring.vertId1),
					Vector3(0), Vector3(0)
				);

				joint->offset = spring.natural_length;
				//joint->slop = spring.natural_length;
				joint->stretch_bias = shear_stretch;
				joint->shrink_bias  = shear_shrink;
			}
			else if (spring.constraint_type == croth_constraint_type::bending_spring) {

				auto joint = Joint::add_balljoint(
					colliders[spring.mesh].at(spring.vertId0), colliders[spring.mesh].at(spring.vertId1),
					Vector3(0), Vector3(0)
				);

				joint->offset = spring.natural_length;
				//joint->slop = spring.natural_length;
				joint->stretch_bias = bending_stretch;
				joint->shrink_bias  = bending_shrink;
			}

		}


	}

}

void Collider_Croth::update() {

	// 各facetのnormalを更新する
	std::vector<std::vector<Vector3>> mesh_facet_normals;
	mesh_facet_normals.resize(meshcoll_data->size());
	{
		int mesh_num = 0;
		for (auto& data : *meshcoll_data) {

			auto& facet_normals = mesh_facet_normals[mesh_num];
			facet_normals.resize(data.facets.size());

			int facet_mum = 0;
			for (auto& facet : data.facets) {

				const Vector3& vertex_0 = data.vertices[facet.vertexID[0]] + Vector3(vertex_offset->at(mesh_num).at(facet.vertexID[0]).first);
				const Vector3& vertex_1 = data.vertices[facet.vertexID[1]] + Vector3(vertex_offset->at(mesh_num).at(facet.vertexID[1]).first);
				const Vector3& vertex_2 = data.vertices[facet.vertexID[2]] + Vector3(vertex_offset->at(mesh_num).at(facet.vertexID[2]).first);

				const Vector3& normal = vector3_cross(vertex_1 - vertex_0, vertex_2 - vertex_0).unit_vect();

				facet_normals[facet_mum] = normal;
				++facet_mum;
			}

			++mesh_num;
		}
	}

	//各頂点の属する面から平均をとってoffsetを計算
	{
		int mesh_num = 0;
		for (auto& data : *meshcoll_data) {

			int vertex_num = 0;
			for (auto& vertex : data.vertices) {

				//vertex_offset->at(mesh_num).at(vertex_num).second;

				Vector3 facet_normal;
				for (int facet_num = 0; facet_num < data.vertex_involvements[vertex_num].get_facet_involvment_size(); ++facet_num) {

					facet_normal += mesh_facet_normals[mesh_num][data.vertex_involvements[vertex_num].facet_involvements[facet_num]];
				}
				facet_normal /= (float)data.vertex_involvements[vertex_num].get_facet_involvment_size();

				vertex_offset->at(mesh_num).at(vertex_num).second = facet_normal;


				++vertex_num;
			}

			++mesh_num;
		}
	}

	for (auto& map_s : colliders) {
		for (auto& coll : map_s.second) {
			coll->update();
		}
	}

	//Physics_manager::mutex_lock();

	//Vector3 position_amount_of_change;
	//Quaternion orientation_amount_of_change;
	//ALPcollider_ptr->adapt_to_gameobject_transform(position_amount_of_change, orientation_amount_of_change, pearent_Worientation_inverse);

	//Wposition = Vector3(Wposition) + position_amount_of_change;
	//Worientation = Quaternion(Worientation) * orientation_amount_of_change;

	//ALPcollider_ptr->copy_transform_gameobject(Wposition, Worientation, Wscale, pearent_Worientation_inverse);

	//Physics_manager::mutex_unlock();


}

void Collider_Croth::awake(){
	// 初期値の調整
	Physics_manager::physicsParams.set_default_physics_data(default_physics_data);

	default_physics_data.anglar_drag = 0.9f;
	default_physics_data.inertial_mass = 1;

	default_sphere_r = 0.01f;

}

void Collider_Croth::finalize() {

	for (auto& map : colliders) for (auto ptr : map.second) ptr->finalize();

	//for (auto& map : colliders) for (auto ptr : map.second) delete ptr;
	int adsfdg = 0;
}

//void Collider_Croth::Update_hierarchy() {
//
//	for (auto& map : colliders) for (auto ptr : map.second) ptr->Update_hierarchy();
//
//}


// 重心のlocal座標を返す
const Vector3 Collider_Croth::get_barycenter() const {

	Physics_function::Physics_manager::mutex_lock();

	Vector3 num_barycenter;
	for (auto& map : colliders) for (auto ptr : map.second) num_barycenter += ptr->get_barycenter();
	num_barycenter /= (float)colliders.size();

	Physics_function::Physics_manager::mutex_unlock();

	return num_barycenter;
};