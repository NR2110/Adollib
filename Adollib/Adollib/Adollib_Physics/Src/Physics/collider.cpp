
#include "ALP__physics_manager.h"

#include "../Math/math.h"
#include "ALP_contact.h"

#include "ALP_collider.h"
#include "ALP_physics.h"
#include "ALP_joint.h"

#include "shape_meshcoll.h"
#include "shape_croth.h"


using namespace Adollib;
using namespace Physics_function;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Rigitbody
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

const bool Collider::concoll_enter(const Collider_tagbit tag_name) {
	return ALPcollider_ptr->concoll_enter(tag_name);
}

void Collider::add_force(const DirectX::XMFLOAT3& force, const float& delta_time, const bool& is_force_local) {
	if (delta_time > inv60)return;
	const Vector3& force_casted = force;
	ALPphysics_ptr->add_force(force_casted * delta_time, is_force_local);
}
void Collider::add_force(const DirectX::XMFLOAT3& force, const DirectX::XMFLOAT3& position, const float& delta_time, const bool& is_position_local, const bool& is_force_local) {
	if (delta_time > inv60)return;
	const Vector3& force_casted = force;
	ALPphysics_ptr->add_force(force_casted * delta_time, position, is_position_local, is_force_local);
}
void Collider::add_torque(const DirectX::XMFLOAT3& force, const float& delta_time, const bool& is_local) {
	if (delta_time > inv60)return;
	const Vector3& force_casted = force;
	ALPphysics_ptr->add_torque(force_casted * delta_time, is_local);
}
void Collider::add_linear_acc(const DirectX::XMFLOAT3& acc, const float& delta_time) {
	if (delta_time > inv60)return;
	const Vector3& acc_casted = acc;
	ALPphysics_ptr->add_linear_acc(acc_casted * delta_time);
}
void Collider::add_angula_acc(const DirectX::XMFLOAT3& acc, const float& delta_time) {
	if (delta_time > inv60)return;
	const Vector3& acc_casted = acc;
	ALPphysics_ptr->add_angula_acc(acc_casted * delta_time);
}


#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::::::::::::::::::::::::::::
#pragma region ALPphysics
//::::::::::::::::::::::::::::::::::::::::::::

const DirectX::XMFLOAT3 Collider::linear_velocity() const { return ALPphysics_ptr->linear_velocity(); }
const DirectX::XMFLOAT3 Collider::angula_velocity() const { return ALPphysics_ptr->angula_velocity(); }
const void Collider::linear_velocity(DirectX::XMFLOAT3 v) {
	ALPphysics_ptr->set_linear_velocity(v);
	ALPphysics_ptr->set_old_linear_velocity(v);
}
const void Collider::angula_velocity(DirectX::XMFLOAT3 v) {
	ALPphysics_ptr->set_angula_velocity(v);
	ALPphysics_ptr->set_old_angula_velocity(v);
}
void Collider::reset_force() { ALPphysics_ptr->reset_force(); };

// 速度制限を行う
void Collider::set_max_linear_velocity(const float& max_scalar) { ALPphysics_ptr->set_max_linear_velocity(max_scalar); };
void Collider::set_max_angula_velocity(const float& max_scalar) { ALPphysics_ptr->set_max_angula_velocity(max_scalar); };

// 慣性モーメントをユーザー定義で設定する
void Collider::set_tensor(const Matrix33& tensor) { ALPphysics_ptr->set_tensor(tensor); };

// 重心をユーザー定義で設定する
void Collider::set_barycenter(const DirectX::XMFLOAT3& cent) { ALPphysics_ptr->set_barycenter(cent); };
#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::


// 指定した一点での速度
const DirectX::XMFLOAT3 Collider::get_point_velocity(const DirectX::XMFLOAT3& pos, bool is_local) {
	Vector3 local_pos = pos;
	if (is_local == false) {
		local_pos = Vector3(pos) - Vector3(Wposition);
	}
	const Vector3& angula_vec = angula_velocity();
	const Vector3& linear_vec = linear_velocity();
	const Vector3& axis = angula_vec.unit_vect();

	const Vector3& r_vec = local_pos - axis * vector3_dot(local_pos, axis);
	float angle_pow = sinf(vector3_radian(angula_vec.unit_vect(), local_pos.unit_vect())) * angula_vec.norm_sqr() * r_vec.norm_sqr();
	if (angle_pow == 0)return linear_velocity();

	const Vector3& angle_dir = vector3_cross(angula_vec.unit_vect(), r_vec.unit_vect());

	return linear_vec + angle_dir * angle_pow;
}

// アタッチされたjointの数
const int Collider::get_joint_count() {
	return ALPcollider_ptr->get_joints().size();
}
// 指定した番号にアタッチされているjointの情報を得る
Joint_base* Collider::get_joint(const int num) {
	int size = ALPcollider_ptr->get_joints().size();
	if (size < num + 1)return nullptr;

	auto itr = ALPcollider_ptr->get_joints().begin();
	for (int i = 0; i < num; ++i) ++itr;

	return (*itr)->userjoint;
}


void Collider::awake(void* Goptr){
	Physics_manager::ColliderPhysics_ptrs data;

	data = Physics_manager::add_collider(this, Goptr);

	ALPcollider_ptr = data.ALPcollider_ptr;
	ALPphysics_ptr = data.ALPphysics_ptr;

	//data.ALPcollider_ptr->shapes.resize(1);
	//data.ALPcollider_itr->shapes.at(0).ALPcollider = data.ALPcollider_itr;
}

void Collider::finalize() {

	//Phyisics_manager::add_delete_buffer_ALPCollider(ALPcollider_ptr);
	//Phyisics_manager::add_delete_buffer_ALPPhysics(ALPphysics_ptr);
	Physics_manager::add_delete_buffer_ALPCollider_and_ALPPhsics(ALPcollider_ptr, ALPphysics_ptr);
	////アタッチされたALP_Collider,ALP_Physicsの終了処理
	//ALPcollider_ptr->destroy();
	//ALPphysics_ptr->destroy();

	////削除
	//delete ALPcollider_ptr;
	//delete ALPphysics_ptr;
}


void Collider::Update_hierarchy() {

	ALPcollider_ptr->Update_hierarchy();

	//if (ImGui::BeginTabBar("Physics")) {
	//	ImGui::Checkbox("moveable", &physics_data.is_moveable);
	//	ImGui::Checkbox("fallable", &physics_data.is_fallable);
	//	ImGui::Checkbox("kinematic", &physics_data.is_kinematic);
	//	ImGui::Checkbox("is_kinmatic_anglar", &physics_data.is_kinmatic_anglar);
	//	ImGui::Checkbox("is_kinmatic_linear", &physics_data.is_kinmatic_linear);
	//	ImGui::Checkbox("hitable", &physics_data.is_hitable);

	//	ImGui::DragFloat("mass", &physics_data.inertial_mass, 0.001f, 0, 0);

	//	ImGui::DragFloat("restitution", &physics_data.restitution, 0.001f, 0, 0);

	//	ImGui::DragFloat("friction", &physics_data.dynamic_friction, 0.001f, 0, 100000000.f);

	//	ImGui::DragFloat("linear_drag", &physics_data.drag, 0.01f, 0, 1);
	//	ImGui::DragFloat("anglar_drag", &physics_data.anglar_drag, 0.01f, 0, 1);

	//	ImGui::DragFloat("linear_sleep_threrhold", &physics_data.linear_sleep_threrhold, 0.01f, 0, 1);
	//	ImGui::DragFloat("angula_sleep_threrhold", &physics_data.angula_sleep_threrhold, 0.01f, 0, 1);

	//	ImGui::Text("linear_velocity : %f", linear_velocity().norm_sqr());
	//	ImGui::Text("angula_velocity : %f", angula_velocity().norm_sqr());


	//	ImGui::EndTabBar();
	//}

}

//meshcolliderのアタッチ
void Collider::add_mesh_shape(const char* filepass, bool is_right_rtiangle, bool is_permit_edge_have_many_facet) {

	//FBXのLoadを行う
	std::vector<Physics_function::Meshcollider_data>* meshcoll_data = nullptr;
	Physics_function::Collider_ResourceManager::CreateMCFromFBX(filepass, &meshcoll_data, is_right_rtiangle, is_permit_edge_have_many_facet);

	for (auto& data : *meshcoll_data) {
		Physics_function::Meshcoll_part* shape = ALPcollider_ptr->add_mesh_shape(filepass, &data);
	}

	//Physics_function::ALP_shape coll_mesh;
	//coll_mesh.ALPcollider = ALPcollider_itr;
}

void Collider::set_ptr_to_joint(Physics_function::ALP_Joint* joint) {
	if (joint->ALPcollider[0] == nullptr)joint->ALPcollider[0] = ALPcollider_ptr;
	else if (joint->ALPcollider[1] == nullptr)joint->ALPcollider[1] = ALPcollider_ptr;
	else assert(0 && "too many ALP_Collider to attach joint");


	ALPcollider_ptr->add_joint(joint);
};

std::vector<Contacted_data> Collider::get_Contacted_data() const {
	return ALPcollider_ptr->get_contacted_collider();
}

// 現在の慣性モーメントの値
const Matrix33 Collider::get_tensor() {

	// shapeの情報などを変更するためmutexをlockする
	Physics_function::Physics_manager::mutex_lock();
	auto ret = ALPphysics_ptr->get_tensor_contain_added();

	Physics_function::Physics_manager::mutex_unlock();

	return ret;
};

// 重心のlocal座標を返す
const DirectX::XMFLOAT3 Collider::get_barycenter() const {

	Physics_function::Physics_manager::mutex_lock();
	auto ret = ALPphysics_ptr->get_barycenter_contain_added();

	Physics_function::Physics_manager::mutex_unlock();

	return ret;
};

void Collider::update() {

	Physics_manager::mutex_lock();

	Vector3 position_amount_of_change;
	Quaternion orientation_amount_of_change;
	ALPcollider_ptr->adapt_to_gameobject_transform(position_amount_of_change, orientation_amount_of_change, pearent_Worientation_inverse);

	Wposition = Vector3(Wposition) + position_amount_of_change;
	Worientation = Quaternion(Worientation) * orientation_amount_of_change;

	ALPcollider_ptr->copy_transform_gameobject(Wposition, Worientation, Wscale, pearent_Worientation_inverse);

	Physics_manager::mutex_unlock();
}