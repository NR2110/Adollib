#include "work_meter.h"

#include "systems.h"
#include "imgui_all.h"

#define UseRelese TRUE;


int Work_meter::start_num  = 0;
float Work_meter::max_num = 0.016;
float Work_meter::before = 0;
std::unordered_map<std::string, Work_meter::meter> Work_meter::s_g;
std::unordered_map<std::string, float[Work_meter::max_ * 2]> Work_meter::meters;
std::vector<std::string> Work_meter::names;


bool Work_meter::render() {
#if UseRelese || _DEBUG
	if (names.size() == 0)return false;

	before -= Al_Global::second_per_frame;

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

		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		float F = (float)((double)1000 / freq.QuadPart * 0.001f);

		for (int name = 0; name < names.size(); name++) {
			meters[names[name]][start_num + max_] = (float)(s_g[names[name]].stop.QuadPart - s_g[names[name]].start.QuadPart) * F;
			s_g[names[name]].stop.QuadPart = 0;
			s_g[names[name]].start.QuadPart = 0;
		}

		before += 0.016;
	}
 
	static ImGuiWindowFlags flag;
	static bool use_second_per_frame = false;
	static bool draw_Back_ground = true;

	flag = 0;
	if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoTitleBar;			// タイトルバーを非表示にします。
	flag |= ImGuiWindowFlags_NoResize;				// ウィンドウをリサイズ不可にします。
	if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoMove;				// ウィンドウを移動不可にします。
	flag |= ImGuiWindowFlags_NoScrollbar;			// スクロールバーを無効にします。
	flag |= ImGuiWindowFlags_NoScrollWithMouse;	// マウスホイールでのスクロール操作を無効にしま
	//flag |= ImGuiWindowFlags_NoCollapse;			// タイトルバーをダブルクリックして閉じる挙動を
	if(!draw_Back_ground) flag |= ImGuiWindowFlags_NoBackground;			// ウィンドウ内の背景を非表示にします。
	flag |= ImGuiWindowFlags_NoBringToFrontOnFocus;// ウィンドウをクリックしてフォーカスした際
	flag |= ImGuiWindowFlags_NoNav;					// ゲームパッドやキーボードでのUIの操作を無効
	//flag |= ImGuiWindowFlags_NoSavedSettings;		// imgui.iniでウィンドウの位置などを自動保存/
	flag |= ImGuiWindowFlags_AlwaysAutoResize;		// 自動でウィンドウ内のコンテンツに合わせてリサ
	flag |= ImGuiWindowFlags_NoFocusOnAppearing;	// 表示/非表示の際のトランジションアニメーショ
	static bool is_true = true;
	ImGui::Begin("work_meter", &is_true, flag);

	//std::vector<float> arr;
	//LARGE_INTEGER freq;
	//QueryPerformanceFrequency(&freq);
	//float F = (float)((double)1000 / freq.QuadPart * 0.001f);

	//arr.push_back(0);
	ImGui::Checkbox("draw_Back_ground", &draw_Back_ground);
	ImGui::Checkbox("Use_deltatime", &use_second_per_frame);

	ImGui::InputFloat("max", &max_num, 0.001f, 1.0f, "%.3f");
	if (use_second_per_frame) {
		max_num = Al_Global::second_per_frame;
	}

	for (int i = 0; i < names.size(); i++) {
		ImGui::PlotLines("", &meters[names[i]][start_num], 120, 0, names[i].c_str(), 0.0f, max_num, ImVec2(500, 50));
	}

	//static float progress = 0.22f;
	static float before_spf = 0;
	ImGui::ProgressBar((before_spf + Al_Global::second_per_frame) * 30, ImVec2(0.0f, 0.0f));
	before_spf = Al_Global::second_per_frame;

	ImGui::End();

	//names.clear();
	//meters.clear();

#endif
	return true;
}

bool Work_meter::start(std::string name) {

#if UseRelese || _DEBUG
	if (meters.count(name) == 0) {
		names.push_back(name);
		memset(meters[name], 0, max_ * 2 * sizeof(float));
	}

	QueryPerformanceCounter(&s_g[name].start);
#endif
	return true;
}

bool Work_meter::stop(std::string name) {
#if UseRelese || _DEBUG
	if (meters.count(name) == 0) {
		names.push_back(name);
		memset(meters[name], 0, max_ * 2 * sizeof(float));
	}

	QueryPerformanceCounter(&s_g[name].stop);
#endif
	return true;
}