#include "gameobject.h"

#include "gameobject_manager.h"

#include "../Main/systems.h"

#include "../Renderer/material_for_collider.h"
#include "../Renderer/frustum_culling.h"
#include "../Main/Adollib.h"

#include "../Renderer/Shader/constant_buffer.h"

#include "../Imgui/imgui_all.h"

using namespace Adollib;
using namespace ConstantBuffer;

void Gameobject::initialize() {

	for (auto& comp : components) {
		comp->start();
	}

}

void Gameobject::update_imgui_toChildren() {
	if (ImGui::CollapsingHeader((name + std::string("##") + std::to_string((int)this)).c_str())) {
		ImGui::Checkbox("active", &active);
		bool de = false;
		ImGui::Checkbox("destroy", &de);
		if (de) {
			Gameobject_manager::deleteGameobject(this);
			return;
		}

		ImGuiTreeNodeFlags f = 0;
		//transforme‚Ì•\¦
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNodeEx((void*)(int)this, f, "Transforme")) {
			float ave = (transform->local_scale.x + transform->local_scale.y + transform->local_scale.z) * 0.333333333f * 10;
			//position
			{
				float vec3[3] = { transform->local_pos.x, transform->local_pos.y, transform->local_pos.z };
				if (ImGui::DragFloat3("position", vec3, ave * 0.01f, 0, 0, "%.2f", 3)) {
					transform->local_pos = Vector3(vec3[0], vec3[1], vec3[2]);
				}
			}
			//rotate
			{
				Vector3 rotate = transform->local_orient.euler();
				float vec3[3] = { rotate.x, rotate.y, rotate.z };
				if (ImGui::DragFloat3("rotate", vec3, 0.1f, 0, 0, "%.2f", 3)) {
					rotate = Vector3(vec3[0], vec3[1], vec3[2]);
					if (ImGui::IsItemActive())
						transform->local_orient = quaternion_from_euler(rotate);
				}
			}
			//scale
			{
				float vec3[3] = { transform->local_scale.x, transform->local_scale.y, transform->local_scale.z };
				if (ImGui::DragFloat3("scale", vec3, ave * 0.01f, 0, 0, "%.2f", 3)) {
					transform->local_scale = Vector3(vec3[0], vec3[1], vec3[2]);
				}
			}
			ImGui::Text("Quaternion : %f,%f,%f,%f", transform->local_orient.x, transform->local_orient.y, transform->local_orient.z, transform->local_orient.w);

			ImGui::TreePop();
		}

		int script_count = 0;
		//Šecomponent‚Å•`‰æ‚·‚éŠÖ”
		for (auto& comp : components) {
			if (ImGui::TreeNode(comp->name.c_str())) {
				comp->Update_hierarchy();
				ImGui::TreePop();
			}
		}

		//q‚Ìgo
		for (auto& itr : *children()) {
			itr->update_imgui_toChildren();
		}

	}

}


void Gameobject::update() {
	std::for_each(components.begin(), components.end(), [](Component* com) {com->update(); });
}


void Gameobject::destroy() {
	clearComponent();
	Gameobject_manager::removeGameobject(this_scene, this_itr);

	if (parent() != nullptr) parent()->remove_child(this);

	for (const auto child : *children()) {
		child->set_parent(nullptr);
	}

}