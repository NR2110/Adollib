#include "ALP__physics_manager.h"

using namespace Adollib;

//#include "ALP__manager.h"
#include "../Object/gameobject.h"
#include "../Main/Adollib.h"
#include "contact.h"

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
	Physics_function::ColliderPhysics_ptrs data;

	data = Phyisics_manager::add_collider(this);

	ALPcollider_ptr = data.ALPcollider_ptr;
	ALPphysics_ptr = data.ALPphysics_ptr;
	coll_itr = data.coll_itr;

	data.ALPcollider_ptr->coll_itr = coll_itr;

	data.ALPcollider_ptr->shapes.resize(1);
	//data.ALPcollider_itr->shapes.at(0).ALPcollider = data.ALPcollider_itr;
}

void Collider::finalize() {

	ALPcollider_ptr->destroy();
	ALPphysics_ptr->destroy();
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