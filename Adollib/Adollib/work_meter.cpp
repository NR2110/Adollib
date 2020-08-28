#include "work_meter.h"
#include "debug.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx11.h"

#define UseRelese TRUE;

using namespace Adollib;

std::unordered_map<std::string, std::unordered_map<std::string, Work_meter::meter>> Work_meter::start_stop;
std::unordered_map<std::string, std::unordered_map<std::string, float[Work_meter::max_ * 2]>> Work_meter::meters;

std::unordered_map<std::string, std::vector<std::string>> Work_meter::names;
std::unordered_map < std::string, std::vector<bool>> Work_meter::name_flags;
std::vector<std::string> Work_meter::tags;

std::string Work_meter::now_tag = std::string("");


bool Work_meter::render() {
#if UseRelese || _DEBUG

	static float max_num = 0.016;
	static int start_num = 0;
	static float before = 0;

	static LARGE_INTEGER now, bef, freq;
	static ImGuiWindowFlags flag = 0;
	static bool draw_Back_ground = true;
	static bool update_always = true;
	static bool draw_work_meter_manager = false;
	static bool is_true = true;
	static bool stop = false;

	//使われてなかったら何もせずにreturn
	if (names.size() == 0)return true;


	// delta_timeの所得(multisledに対応するため)
	QueryPerformanceFrequency(&freq);
	float F = (float)((double)1000 / freq.QuadPart * 0.001f);

	QueryPerformanceCounter(&now);
	if (stop == false) before -= ((float)((now.QuadPart - bef.QuadPart)) * F);
	QueryPerformanceCounter(&bef);

	flag = 0;
	//if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoTitleBar;			// タイトルバーを非表示にします。
	//if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoBackground;		// ウィンドウ内の背景を非表示にします。
	flag |= ImGuiWindowFlags_AlwaysAutoResize;		// 自動でウィンドウ内のコンテンツに合わせてリサ

	if (draw_work_meter_manager) {
		ImGui::Begin("WM_manager", &draw_work_meter_manager, flag);


		bool V;
		for (int t = 0; t < tags.size(); t++) {
			for (int i = 0; i < name_flags[tags[t]].size(); i++) {
				V = name_flags[tags[t]][i];
				ImGui::Checkbox(names[tags[t]][i].c_str(), &V);
				name_flags[tags[t]][i] = V;
			}
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
	//タブを閉じても更新するか
	ImGui::Checkbox("update_always", &update_always);

	//グラフの最大値の入出力
	ImGui::InputFloat("max", &max_num, 0.001f, 1.0f, "%.3f");

	{
		if (before < 0) start_num++;

		bool retern_list = false;
		if (start_num >= max_) {
			start_num = 0;
			retern_list = true;
		}

		std::string tag, name;

		tag = std::string("");
		for (int n_num = 0; n_num < names[tag].size(); n_num++) {
			if (name_flags[tag][n_num] == false)continue;
			name = names[tag][n_num];


			if (retern_list) {
				//折り返し処理
				for (int i = 0; i < max_; i++) {
					meters[tag][name][i] = meters[tag][name][i + max_];
				}
			}

			// 値の記入
			if (before < 0)
				meters[tag][name][start_num + max_] = (float)(start_stop[tag][name].stop.QuadPart - start_stop[tag][name].start.QuadPart) * F;
			

				// 描画
				ImGui::PlotLines("", &meters[tag][name][start_num], max_, 0, name.c_str(), 0.0f, max_num, ImVec2(500, 50));		

			start_stop[tag][name].stop.QuadPart = 0;
			start_stop[tag][name].start.QuadPart = 0;
		}


		for (int t_num = 0; t_num < tags.size(); t_num++) {
			if (tags[t_num] == std::string(""))continue;
			tag = tags[t_num];

			if (ImGui::CollapsingHeader(tags[t_num].c_str())) {

				for (int n_num = 0; n_num < names[tag].size(); n_num++) {
					if (name_flags[tag][n_num] == false)continue;
					name = names[tag][n_num];


					if (retern_list) {
						//折り返し処理
						for (int i = 0; i < max_; i++) {
							meters[tag][name][i] = meters[tag][name][i + max_];
						}
					}

					// 値の記入
					if (before < 0)
						meters[tag][name][start_num + max_] = (float)(start_stop[tag][name].stop.QuadPart - start_stop[tag][name].start.QuadPart) * F;
					

						// 描画
						ImGui::PlotLines("", &meters[tag][name][start_num], 120, 0, name.c_str(), 0.0f, max_num, ImVec2(500, 50));
					
					start_stop[tag][name].stop.QuadPart = 0;
					start_stop[tag][name].start.QuadPart = 0;
				}
			}
			else if(update_always) {
				for (int n_num = 0; n_num < names[tag].size(); n_num++) {
					if (name_flags[tag][n_num] == false)continue;
					name = names[tag][n_num];


					if (retern_list) {
						//折り返し処理
						for (int i = 0; i < max_; i++) {
							meters[tag][name][i] = meters[tag][name][i + max_];
						}
					}

					// 値の記入
					if (before < 0)
						meters[tag][name][start_num + max_] = (float)(start_stop[tag][name].stop.QuadPart - start_stop[tag][name].start.QuadPart) * F;

					start_stop[tag][name].stop.QuadPart = 0;
					start_stop[tag][name].start.QuadPart = 0;
				}
			}
		}

		if (before < 0)
		before = 0.016;
	}

	

	// work_meter_managerの表示の切り替え
	ImGui::Checkbox("draw_work_meter_manager", &draw_work_meter_manager);

	ImGui::End();


#endif
	return true;
}

bool Work_meter::start(std::string name) {

#if UseRelese || _DEBUG
	if (meters[now_tag].count(name) == 0) {
		names[now_tag].push_back(name);
		name_flags[now_tag].push_back(1);
		memset(meters[now_tag][name], 0, max_ * 2 * sizeof(float));
	}

	QueryPerformanceCounter(&start_stop[now_tag][name].start);
#endif
	return true;
}

bool Work_meter::stop(std::string name) {
#if UseRelese || _DEBUG
	if (meters[now_tag].count(name) == 0) {
		names[now_tag].push_back(name);
		name_flags[now_tag].push_back(1);
		memset(meters[now_tag][name], 0, max_ * 2 * sizeof(float));
	}

	QueryPerformanceCounter(&start_stop[now_tag][name].stop);
#endif
	return true;
}

bool Work_meter::tag_start(std::string tag) {
	if (meters.count(tag) == 0) {
		tags.push_back(tag);
		meters[tag];
	}
	now_tag = tag;

	return 1;
}
bool Work_meter::tag_stop() {
	now_tag = std::string("");

	return 1;
}