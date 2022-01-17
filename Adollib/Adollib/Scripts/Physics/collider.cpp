
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

const bool Collider::concoll_enter(const Collider_tagbit tag_name) {
	return ALPcollider_ptr->concoll_enter(tag_name);
}

void Collider::add_force(const Vector3& force, const bool& is_force_local) {
	if (Al_Global::second_per_frame > inv60)return;
	ALPphysics_ptr->add_force(force * Al_Global::second_per_frame, is_force_local);
}
void Collider::add_force(const Vector3& force, const Vector3& position, const bool& is_position_local, const bool& is_force_local) {
	if (Al_Global::second_per_frame > inv60)return;
	ALPphysics_ptr->add_force(force * Al_Global::second_per_frame, position, is_position_local, is_force_local);
}
void Collider::add_torque(const Vector3& force, const bool& is_local) {
	if (Al_Global::second_per_frame > inv60)return;
	ALPphysics_ptr->add_torque(force * Al_Global::second_per_frame, is_local);
}
void Collider::add_linear_acc(const Vector3& acc) {
	if (Al_Global::second_per_frame > inv60)return;
	ALPphysics_ptr->add_linear_acc(acc * Al_Global::second_per_frame);
}
void Collider::add_angula_acc(const Vector3& acc) {
	if (Al_Global::second_per_frame > inv60)return;
	ALPphysics_ptr->add_angula_acc(acc * Al_Global::second_per_frame);
}


#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// アタッチされたjointの数
const int Collider::get_joint_count() {
	return ALPcollider_ptr->get_joints().size();
}
// 指定した番号にアタッチされているjointの情報を得る
Joint_base* Collider::get_joint(const int num) {
	int size =  ALPcollider_ptr->get_joints().size();
	if (size < num + 1)return nullptr;

	auto itr = ALPcollider_ptr->get_joints().begin();
	for (int i = 0; i < num; ++i) ++itr;

	return (*itr)->userjoint;
}


void Collider::awake() {
	Physics_manager::ColliderPhysics_ptrs data;

	data = Physics_manager::add_collider(this);

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

	if (ImGui::BeginTabBar("Physics")) {
		ImGui::Checkbox("moveable", &physics_data.is_moveable);
		ImGui::Checkbox("fallable", &physics_data.is_fallable);
		ImGui::Checkbox("kinematic", &physics_data.is_kinematic);
		ImGui::Checkbox("is_kinmatic_anglar", &physics_data.is_kinmatic_anglar);
		ImGui::Checkbox("is_kinmatic_linear", &physics_data.is_kinmatic_linear);
		ImGui::Checkbox("hitable", &physics_data.is_hitable);

		ImGui::DragFloat("mass", &physics_data.inertial_mass, 0.001f, 0, 0);

		ImGui::DragFloat("restitution", &physics_data.restitution, 0.001f, 0, 0);

		ImGui::DragFloat("friction", &physics_data.dynamic_friction, 0.001f, 0, 100000000.f);

		ImGui::DragFloat("linear_drag", &physics_data.drag, 0.01f, 0, 1);
		ImGui::DragFloat("anglar_drag", &physics_data.anglar_drag, 0.01f, 0, 1);

		ImGui::Text("linear_velocity : %f", linear_velocity().norm_sqr());
		ImGui::Text("angula_velocity : %f", angula_velocity().norm_sqr());


		ImGui::EndTabBar();
	}

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
const Vector3 Collider::get_barycenter() const {

	Physics_function::Physics_manager::mutex_lock();
	auto ret = ALPphysics_ptr->get_barycenter_contain_added();

	Physics_function::Physics_manager::mutex_unlock();

	return ret;
};