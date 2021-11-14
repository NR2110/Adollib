#include "debug.h"

#include "../../Imgui/imgui.h"
#include "../../Imgui/imgui_impl_win32.h"
#include "../../Imgui/imgui_impl_dx11.h"

using namespace Adollib;

std::unordered_map<std::string, std::vector<Debug::base>> Debug::debug_params;
std::vector<std::string> Debug::tags;

std::vector<Debug::base> Debug::notag_debug_param;
bool Debug::stop_flag = false;

void Debug::render() {

	ImGuiTabBarFlags flag = ImGuiTabBarFlags_Reorderable; // タブをドラッグして並び替えができる
	flag |= ImGuiTabBarFlags_TabListPopupButton; // タブの一番左端にドロップダウンリストが表示
	//flag |= ImGuiTabBarFlags_AutoSelectNewTabs; // タブを新しく作成した時に自動でそのタブを選択
	// flag |= ImGuiTabBarFlags_NoCloseWithMiddleMouseButton; // タブの中でマウス中央ボタンクリ
	// flag |= ImGuiTabBarFlags_NoTooltip; // タブ上にマウスオーバーした場合に表示されるタブ名
	// flag |= ImGuiTabBarFlags_FittingPolicyResizeDown; // タブがウィンドウ幅を超えてたくさん
	//flag |= ImGuiTabBarFlags_FittingPolicyScroll; // タブの幅を自動でリサイズさせずに左右の矢印

	ImGui::Begin("Debug");

	ImGui::Checkbox("stop", &stop_flag);

	if (notag_debug_param.size() != 0)
	{
		ImGui::Columns(5, "columnListIDs");
		ImGui::Separator();
		ImGui::Text("Name"); ImGui::NextColumn();
		ImGui::Text("x"); ImGui::NextColumn();
		ImGui::Text("y"); ImGui::NextColumn();
		ImGui::Text("z"); ImGui::NextColumn();
		ImGui::Text("w"); ImGui::NextColumn();
		ImGui::Separator();

		for (u_int param_num = 0; param_num < notag_debug_param.size(); param_num++) {

			ImGui::Text(notag_debug_param[param_num].param_name.c_str()); ImGui::NextColumn();

			ImGui::Text("%f", notag_debug_param[param_num].param.x); ImGui::NextColumn();
			ImGui::Text("%f", notag_debug_param[param_num].param.y); ImGui::NextColumn();
			ImGui::Text("%f", notag_debug_param[param_num].param.z); ImGui::NextColumn();
			ImGui::Text("%f", notag_debug_param[param_num].param.w); ImGui::NextColumn();
		}
	}


	for (u_int tab_num = 0; tab_num < tags.size(); tab_num++) {

		if (ImGui::CollapsingHeader(tags[tab_num].c_str())) {

			ImGui::Columns(5, "columnListID");
				ImGui::Separator();
				ImGui::Text("Name"); ImGui::NextColumn();
				ImGui::Text("x"); ImGui::NextColumn();
				ImGui::Text("y"); ImGui::NextColumn();
				ImGui::Text("z"); ImGui::NextColumn();
				ImGui::Text("w"); ImGui::NextColumn();
				ImGui::Separator();

				for (u_int param_num = 0; param_num < debug_params[tags[tab_num]].size(); param_num++) {

					ImGui::Text(debug_params[tags[tab_num]][param_num].param_name.c_str()); ImGui::NextColumn();

					ImGui::Text("%f", debug_params[tags[tab_num]][param_num].param.x); ImGui::NextColumn();
					ImGui::Text("%f", debug_params[tags[tab_num]][param_num].param.y); ImGui::NextColumn();
					ImGui::Text("%f", debug_params[tags[tab_num]][param_num].param.z); ImGui::NextColumn();
					ImGui::Text("%f", debug_params[tags[tab_num]][param_num].param.w); ImGui::NextColumn();

				}

		}

		ImGui::Columns(1);

	}
	ImGui::End();
}

void Debug::reset() {
	//debug_params.clear();
	//notag_debug_param.clear();
	tags.clear();
}


void Debug::set(std::string name, float x, float y, float z, float w) {
	if (stop_flag == true)return;
	for (auto& param : notag_debug_param) {
		if (param.param_name == name) {
			param.param = DirectX::XMFLOAT4(x, y, z, w);
			param.param_num = 3;
			return;
		}
	}

	base b;
	b.param_name = name;
	b.param = DirectX::XMFLOAT4(x, y, z, w);
	b.param_num = 4;

	notag_debug_param.emplace_back(b);
}
void Debug::set(std::string name, DirectX::XMFLOAT4 xyzw) {
	if (stop_flag == true)return;
	for (auto& param : notag_debug_param) {
		if (param.param_name == name) {
			param.param = xyzw;
			param.param_num = 3;
			return;
		}
	}

	base b;
	b.param_name = name;
	b.param = xyzw;
	b.param_num = 4;

	notag_debug_param.emplace_back(b);
}
void Debug::set(std::string name, DirectX::XMFLOAT3 xyz) {
	if (stop_flag == true)return;
	for (auto& param : notag_debug_param) {
		if (param.param_name == name) {
			param.param = DirectX::XMFLOAT4(xyz.x, xyz.y, xyz.z, 0);
			param.param_num = 3;
			return;
		}
	}

	base b;
	b.param_name = name;
	b.param = DirectX::XMFLOAT4(xyz.x, xyz.y, xyz.z, 0);
	b.param_num = 3;

	notag_debug_param.emplace_back(b);
}

void Debug::set(std::string tabname, std::string name, float x, float y, float z, float w) {
	if (stop_flag == true)return;
	for (auto& param : notag_debug_param) {
		if (param.param_name == name) {
			param.param = DirectX::XMFLOAT4(x, y, z, w);
			param.param_num = 3;
			return;
		}
	}

	base b;
	b.param_name = name;
	b.param = DirectX::XMFLOAT4(x, y, z, w);
	b.param_num = 4;

	if (debug_params.count(tabname) == 0) {
		tags.emplace_back(tabname);
	}

	debug_params[tabname].emplace_back(b);
}
void Debug::set(std::string tabname, std::string name, DirectX::XMFLOAT4 xyzw) {
	if (stop_flag == true)return;
	for (auto& param : notag_debug_param) {
		if (param.param_name == name) {
			param.param = xyzw;
			param.param_num = 3;
			return;
		}
	}

	base b;
	b.param_name = name;
	b.param = xyzw;
	b.param_num = 4;

	if (debug_params.count(tabname) == 0) {
		tags.emplace_back(tabname);
	}

	debug_params[tabname].emplace_back(b);
}
void Debug::set(std::string tabname, std::string name, DirectX::XMFLOAT3 xyz) {
	if (stop_flag == true)return;
	for (auto& param : notag_debug_param) {
		if (param.param_name == name) {
			param.param = DirectX::XMFLOAT4(xyz.x, xyz.y, xyz.z, 0);
			param.param_num = 3;
			return;
		}
	}

	base b;
	b.param_name = name;
	b.param = DirectX::XMFLOAT4(xyz.x, xyz.y, xyz.z, 0);
	b.param_num = 3;

	if (debug_params.count(tabname) == 0) {
		tags.emplace_back(tabname);
	}

	debug_params[tabname].emplace_back(b);
}