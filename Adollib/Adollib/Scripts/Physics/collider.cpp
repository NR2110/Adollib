#include "ALP__physics_manager.h"

//#include "ALP__manager.h"
#include "../Object/gameobject.h"
#include "../Main/Adollib.h"
#include "contact.h"

#include "ALP_joint.h"

using namespace Adollib;
using namespace Physics_function;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Rigitbody
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

const bool Collider::concoll_enter(const unsigned int tag_name) const {
	return ALPcollider_ptr->concoll_enter(tag_name);
}

void Collider::add_force(const Vector3& force) {
	ALPphysics_ptr->add_force(force);
}
void Collider::add_torque(const Vector3& force) {
	ALPphysics_ptr->add_torque(force);
}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Collider::awake() {
	Phyisics_manager::ColliderPhysics_ptrs data;

	data = Phyisics_manager::add_collider(this);

	ALPcollider_ptr = data.ALPcollider_ptr;
	ALPphysics_ptr = data.ALPphysics_ptr;

	//data.ALPcollider_ptr->shapes.resize(1);
	//data.ALPcollider_itr->shapes.at(0).ALPcollider = data.ALPcollider_itr;
}

void Collider::finalize() {

	//アタッチされたALP_Collider,ALP_Physicsの終了処理
	ALPcollider_ptr->destroy();
	ALPphysics_ptr->destroy();

	//削除
	delete ALPcollider_ptr;
	delete ALPphysics_ptr;
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

		ImGui::DragFloat("linear_drag", &physics_data.drag, 0.01f, 0, 0);
		ImGui::DragFloat("anglar_drag", &physics_data.anglar_drag, 0.01f, 0, 0);


		ImGui::EndTabBar();
	}

}

////規定のshapeをアタッチする
//template<typename T>
//T* Collider::add_shape() {
//	return ALPcollider_itr->add_shape<T>();
//}

//meshcolliderのアタッチ
void Collider::add_shape(const char* filepass, bool is_right_rtiangle) {

	//FBXのLoadを行う
	std::vector<Physics_function::Meshcollider_data>* meshcoll_data = nullptr;
	Physics_function::Collider_ResourceManager::CreateMCFromFBX(filepass, &meshcoll_data, is_right_rtiangle);

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
