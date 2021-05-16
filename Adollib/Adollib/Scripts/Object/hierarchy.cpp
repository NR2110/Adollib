#include "hierarchy.h"

#include "../Imgui/imgui_all.h"

using namespace Adollib;
using namespace Hierarchy;

void Hierarchy::update_hierarchy(std::vector<object*>& masters) {
	ImGuiWindowFlags flag = 0;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;

	if (ImGui::Begin("Hierarchy", 0, flag)) {
		//e‚©‚çŽq‚ÖHierarchy‚Ìupdate‚ðŒÄ‚Ô
		int ID = 0;
		for (auto& obj : masters) {
			if (obj->is_hierarchy == false)continue;

			ImGui::PushID(ID);
			obj->update_imgui_toChildren();
			ImGui::PopID();
			++ID;
		}
	}
	ImGui::End();
}