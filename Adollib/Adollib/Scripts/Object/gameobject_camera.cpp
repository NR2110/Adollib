#include "gameobject_camera.h"
#include "../Main/Adollib.h"

#include "../Imgui/imgui_all.h"
namespace Adollib {

	void Camera::initialize() {

		aspect = (float)Al_Global::SCREEN_WIDTH / Al_Global::SCREEN_HEIGHT;

		std::list <std::shared_ptr<Component_camera>>::iterator itr = components.begin();
		std::list <std::shared_ptr<Component_camera>>::iterator itr_end = components.end();

		for (; itr != itr_end; itr++) {
			itr->get()->start();
		}

	}

	void Camera::update_imgui_toChildren() {
		if (ImGui::CollapsingHeader(name.c_str())) {
			ImGui::Checkbox("active", &active);
			//transforme‚Ì•\Ž¦
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("camera_Transforme")) {
				//position
				{
					float vec3[3] = { transform->local_pos.x, transform->local_pos.y, transform->local_pos.z };
					ImGui::DragFloat3("position", vec3, 1 * 0.01f, 0, 0, "%.2f");
					transform->local_pos = Vector3(vec3[0], vec3[1], vec3[2]);
				}
				//rotate
				{
					Vector3 rotate = transform->local_orient.euler();
					float vec3[3] = { rotate.x, rotate.y, rotate.z };
					ImGui::DragFloat3("rotate", vec3, 0.1f, 0, 0, "%.2f");
					rotate = Vector3(vec3[0], vec3[1], vec3[2]);
					transform->local_orient = quaternion_from_euler(rotate);
				}
				{
					ImGui::DragFloat("nearZ", &nearZ, 0.01f, 0, 0, "%.2f");
					ImGui::DragFloat("farZ", &farZ, 0.01f, 0, 0, "%.2f");
					ImGui::DragFloat("fov", &fov, 0.01f, 0, 0, "%.2f");
					ImGui::DragFloat("aspect", &aspect, 0.01f, 0.01f, 10000000000000000000, "%.2f");
				}

				ImGui::TreePop();
			}

			int script_count = 0;

			//Žq‚Ìgo
			for (auto& itr : *children()) {
				itr->update_imgui_toChildren();
			}

		}

	}

	void Camera::update() {

		std::list <std::shared_ptr<Component_camera>>::iterator itr = components.begin();
		std::list <std::shared_ptr<Component_camera>>::iterator itr_end = components.end();

		for (; itr != itr_end; itr++) {
			itr->get()->update();
		}

	}


}