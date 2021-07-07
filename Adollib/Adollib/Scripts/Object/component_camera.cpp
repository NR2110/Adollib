#include "component_camera.h"

#include "../Imgui/imgui_all.h"
#include "../Main/Adollib.h"
#include "gameobject_manager.h"

using namespace Adollib;

void Camera_component::awake() {
	this_itr = Gameobject_manager::add_camera_component(gameobject->get_scene(), this);
	aspect = (float)Al_Global::SCREEN_WIDTH / Al_Global::SCREEN_HEIGHT;
}

void Camera_component::finalize() {
	Gameobject_manager::remove_camera_component(gameobject->get_scene(), this_itr);
}


void Camera_component::Update_hierarchy() {

	ImGui::DragFloat("fov", &fov, 0.1f, 0, 0, "%.2f");
	ImGui::DragFloat("aspect", &aspect, 0.1f, 0, 0, "%.2f");
	ImGui::DragFloat("nearZ", &nearZ, 0.1f, 0, 0, "%.2f");
	ImGui::DragFloat("farZ", &farZ, 0.1f, 0, 0, "%.2f");
}
