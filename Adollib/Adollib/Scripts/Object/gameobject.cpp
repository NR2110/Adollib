#include "gameobject.h"
#include "../Main/systems.h"

#include "../Mesh/material_for_collider.h"
#include "../Main/Adollib.h"
#include "../Mesh/frustum_culling.h"

#include "../Shader/constant_buffer.h"

#include "../Imgui/imgui_all.h"

using namespace Adollib;
using namespace ConstantBuffer;

void Gameobject::initialize() {

	std::list <std::shared_ptr<Component>>::iterator itr = components.begin();
	std::list <std::shared_ptr<Component>>::iterator itr_end = components.end();

	Systems::CreateConstantBuffer(&world_cb, sizeof(ConstantBufferPerGO));

	for (; itr != itr_end; itr++) {
		itr->get()->start();
	}

}

void Gameobject::update_imgui_P_to_C() {
	if (ImGui::CollapsingHeader(name.c_str())) {
		//transformeの表示
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Transforme")) {
			float ave = (transform->local_scale.x + transform->local_scale.y + transform->local_scale.z) * 0.333333333f;
			//position
			{
				float vec3[3] = { transform->local_pos.x, transform->local_pos.y, transform->local_pos.z };
				ImGui::DragFloat3("position", vec3, ave * 0.01f, 0, 0, "%.2f");
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
			//scale
			{
				float vec3[3] = { transform->local_scale.x, transform->local_scale.y, transform->local_scale.z };
				ImGui::DragFloat3("scale", vec3, ave * 0.01f, 0, 0, "%.2f");
				transform->local_scale = Vector3(vec3[0], vec3[1], vec3[2]);
			}

			ImGui::TreePop();
		}

		int script_count = 0;
		//各componentで描画する関数
		for (auto& comp : components) {
			if (ImGui::TreeNode(comp->name.c_str())) {
				comp->Update_hierarchy();
				ImGui::TreePop();
			}
		}

		//子のgo
		for (auto& itr : children) {
			itr->update_imgui_P_to_C();
		}

	}

}

void Gameobject::update() {
	std::for_each(components.begin(), components.end(), [](std::shared_ptr<Component> com) {com->update(); });
}

void Gameobject::render() {
	if (no_material == false) {

		//CB : ConstantBufferPerOBJ
		ConstantBufferPerGO g_cb;
		g_cb.world = matrix_world(transform->scale, transform->orientation.get_rotate_matrix(), transform->position).get_XMFLOAT4X4();
		Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

		//視錐台カリング用
		FrustumCulling::update_obj(this);
		FrustumCulling::frustum_culling_init();

		material->render();
	}

}

