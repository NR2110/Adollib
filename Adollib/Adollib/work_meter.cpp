#include "work_meter.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx11.h"

#define UseRelese TRUE;

using namespace Adollib;

std::unordered_map<std::string, Work_meter::meter> Work_meter::start_stop;
std::unordered_map<std::string, float[Work_meter::max_ * 2]> Work_meter::meters;
std::vector<std::string> Work_meter::names;
std::vector<bool> Work_meter::name_flags;


bool Work_meter::render() {
#if UseRelese || _DEBUG

	static float max_num = 0.016;
	static int start_num = 0;
	static float before = 0;

	static LARGE_INTEGER now, bef, freq;
	static ImGuiWindowFlags flag = 0;
	static bool draw_Back_ground = true;
	static bool adjust_max = false;
	static bool draw_work_meter_manager = false;
	static bool is_true = true;
	static bool stop = false;

	//使われてなかったら何もせずにreturn
	if (names.size() == 0)return true;


	// delta_timeの所得(multisledに対応するため)
	QueryPerformanceFrequency(&freq);
	float F = (float)((double)1000 / freq.QuadPart * 0.001f);

	QueryPerformanceCounter(&now);
	if(stop == false)before -= (float)((now.QuadPart - bef.QuadPart)) * F;
	QueryPerformanceCounter(&bef);

	// deltatimeを用いてフレームに依存しない更新
	if (before < 0) {

		start_num++;

		if (start_num >= max_) {
			start_num = 0;

			for (int name = 0; name < names.size(); name++) {
				for (int i = 0; i < max_; i++) {
					meters[names[name]][i] = meters[names[name]][i + max_];
				}
			}
		}

		for (int name = 0; name < names.size(); name++) {
			meters[names[name]][start_num + max_] = (float)(start_stop[names[name]].stop.QuadPart - start_stop[names[name]].start.QuadPart) * F;

			//自動調節
			if (adjust_max && max_num < meters[names[name]][start_num + max_] * 1.2f) max_num = meters[names[name]][start_num + max_] * 1.2f;

			start_stop[names[name]].stop.QuadPart = 0;
			start_stop[names[name]].start.QuadPart = 0;
		}

		// 0.016秒ごとに更新
		before = 0.016;
	}
 


	flag = 0;
	//if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoTitleBar;			// タイトルバーを非表示にします。
	//if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoBackground;		// ウィンドウ内の背景を非表示にします。
	flag |= ImGuiWindowFlags_AlwaysAutoResize;		// 自動でウィンドウ内のコンテンツに合わせてリサ

	if (draw_work_meter_manager) {
		ImGui::Begin("WM_manager", &draw_work_meter_manager, flag);

		bool V;
		for (int i = 0; i < name_flags.size(); i++) {
			V = name_flags[i];
			ImGui::Checkbox(names[i].c_str(), &V);
			name_flags[i] = V;
		}

		ImGui::End();
	}

	flag = 0;
	//if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoTitleBar;			// タイトルバーを非表示にします。
	//if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoBackground;		// ウィンドウ内の背景を非表示にします。
	flag |= ImGuiWindowFlags_AlwaysAutoResize;		// 自動でウィンドウ内のコンテンツに合わせてリサ

	ImGui::Begin("work_meter", 0, flag);

	//更新
	ImGui::Checkbox("stop", &stop);
	////背景描画
	//ImGui::Checkbox("draw_Back_ground", &draw_Back_ground); 
	//グラフの最大値を自動調節するか
	ImGui::Checkbox("adjust_max", &adjust_max);

	//グラフの最大値の入出力
	ImGui::InputFloat("max", &max_num, 0.001f, 1.0f, "%.3f");

	for (int i = 0; i < names.size(); i++) {
		if (name_flags[i] == false)continue;
		ImGui::PlotLines("", &meters[names[i]][start_num], 120, 0, names[i].c_str(), 0.0f, max_num, ImVec2(500, 50));
	}

	// work_meter_managerの表示の切り替え
	ImGui::Checkbox("draw_work_meter_manager", &draw_work_meter_manager);

	ImGui::End();


#endif
	return true;
}

bool Work_meter::start(std::string name) {

#if UseRelese || _DEBUG
	if (meters.count(name) == 0) {
		names.push_back(name);
		name_flags.push_back(1);
		memset(meters[name], 0, max_ * 2 * sizeof(float));
	}

	QueryPerformanceCounter(&start_stop[name].start);
#endif
	return true;
}

bool Work_meter::stop(std::string name) {
#if UseRelese || _DEBUG
	if (meters.count(name) == 0) {
		names.push_back(name);
		name_flags.push_back(1);
		memset(meters[name], 0, max_ * 2 * sizeof(float));
	}

	QueryPerformanceCounter(&start_stop[name].stop);
#endif
	return true;
}