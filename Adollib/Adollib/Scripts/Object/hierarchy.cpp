#include "hierarchy.h"

#include "../Imgui/imgui_all.h"

using namespace Adollib;
using namespace Hierarchy;

void Hierarchy::update_hierarchy(std::vector<object*>& masters) {
	ImGuiWindowFlags flag = 0;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;

	if (ImGui::Begin("Hierarchy", 0, flag)) {
		//�e����q��Hierarchy��update���Ă�
		int ID = 0;
		for (auto& obj : masters) {
			ImGui::PushID(ID);
			obj->update_imgui_P_to_C();
			ImGui::PopID();
			++ID;
		}
	}
	ImGui::End();
}