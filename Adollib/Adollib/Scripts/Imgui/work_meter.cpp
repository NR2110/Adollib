#include "work_meter.h"
#include "debug.h"
#include "../../Imgui/imgui.h"
#include "../../Imgui/imgui_impl_win32.h"
#include "../../Imgui/imgui_impl_dx11.h"

using namespace Adollib;

std::mutex Work_meter::mtx;

std::unordered_map<std::string, std::unordered_map<std::string, Work_meter::meter>> Work_meter::start_stop;
std::unordered_map<std::string, std::unordered_map<std::string, float[Work_meter::max_ * 2]>> Work_meter::meters;

std::unordered_map<std::string, std::vector<std::string>> Work_meter::names;
std::unordered_map < std::string, std::vector<bool>> Work_meter::name_flags;
std::vector<std::string> Work_meter::tags;

std::string  Work_meter::now_tag[10] = { std::string("") };


bool Work_meter::render() {
	std::lock_guard <std::mutex> lock(mtx);

#ifdef UseImgui

	static float max_num = 0.016f;
	static int start_num = 0;
	static float before = 0;

	static LARGE_INTEGER now, bef, freq;
	static ImGuiWindowFlags flag = 0;
	static bool draw_Back_ground = true;
	static bool update_always = true;
	static bool draw_work_meter_manager = false;
	static bool is_true = true;
	static bool stop = false;

	int name_tags_size = 0;

	//使われてなかったら何もせずにreturn
	if (names.size() == 0)return true;


	// delta_timeの所得(multisledに対応するため)
	QueryPerformanceFrequency(&freq);
	const float F = (float)((double)1000 / freq.QuadPart * 0.001f);

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
		for(const auto& tag: tags){
			name_tags_size = name_flags[tag].size();
			for (int i = 0; i < name_tags_size; i++) {
				V = name_flags[tag][i];
				ImGui::Checkbox(names[tag][i].c_str(), &V);
				name_flags[tag][i] = V;
			}
		}


		ImGui::End();
	}

	flag = 0;
	//if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoTitleBar;			// タイトルバーを非表示にします。
	//if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoBackground;		// ウィンドウ内の背景を非表示にします。
	//flag |= ImGuiWindowFlags_AlwaysAutoResize;		// 自動でウィンドウ内のコンテンツに合わせてリサ
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;

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
		name_tags_size = names[tag].size();
		for (int n_num = 0; n_num < name_tags_size; n_num++) {
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
				meters[tag][name][start_num + max_] = (float)(start_stop[tag][name].duration_time) * F;

			// 描画
			ImGui::PlotLines("", &meters[tag][name][start_num], max_, 0, name.c_str(), 0.0f, max_num, ImVec2(500, 50));

			if (start_stop[tag][name].is_reset_duration_time)
				start_stop[tag][name].duration_time = 0;

		}


		for(auto& tag : tags){
			if (tag == std::string(""))continue;

			if (ImGui::CollapsingHeader(tag.c_str())) {
				name_tags_size = names[tag].size();
				for (int n_num = 0; n_num < name_tags_size; n_num++) {
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
						meters[tag][name][start_num + max_] = (float)(start_stop[tag][name].duration_time) * F;


						// 描画
						ImGui::PlotLines("", &meters[tag][name][start_num], 120, 0, name.c_str(), 0.0f, max_num, ImVec2(500, 50));

						if(start_stop[tag][name].is_reset_duration_time)
						start_stop[tag][name].duration_time = 0;
				}
			}
			else if(update_always) {
				name_tags_size = names[tag].size();
				for (int n_num = 0; n_num < name_tags_size; n_num++) {
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
						meters[tag][name][start_num + max_] = (float)(start_stop[tag][name].duration_time) * F;

					if (start_stop[tag][name].is_reset_duration_time)
						start_stop[tag][name].duration_time = 0;
				}
			}
		}

		if (before < 0)
		before = 0.016f;
	}



	// work_meter_managerの表示の切り替え
	ImGui::Checkbox("draw_work_meter_manager", &draw_work_meter_manager);

	ImGui::End();


#endif
	return true;
}

bool Work_meter::start(const std::string& name, int thread_num) {
	std::lock_guard <std::mutex> lock(mtx);
	const auto& tag = now_tag[thread_num];

	start_stop[tag][name].stop.QuadPart = 0;
	start_stop[tag][name].start.QuadPart = 0;

#ifdef UseImgui
	if (meters.count(tag) == 0) {
		tags.push_back(tag);
		meters[tag];
	}

	if (meters[tag].count(name) == 0) {
		names[tag].push_back(name);
		name_flags[tag].push_back(1);
		memset(meters[tag][name], 0, max_ * 2 * sizeof(float));
	}

	QueryPerformanceCounter(&start_stop[tag][name].start);

#endif
	return true;
}

bool Work_meter::stop(const std::string& name, int thread_num) {
	std::lock_guard <std::mutex> lock(mtx);
	const auto& tag = now_tag[thread_num];

#ifdef UseImgui
	if (meters[tag].count(name) == 0) {
		names[tag].push_back(name);
		name_flags[tag].push_back(1);
		memset(meters[tag][name], 0, max_ * 2 * sizeof(float));
	}

	QueryPerformanceCounter(&start_stop[tag][name].stop);

	start_stop[tag][name].duration_time += start_stop[tag][name].stop.QuadPart - start_stop[tag][name].start.QuadPart;

#endif
	return true;
}

void Work_meter::set(const std::string& name, const float& value, int thread_num) {
	std::lock_guard <std::mutex> lock(mtx);
	const auto& tag = now_tag[thread_num];

	if (meters[tag].count(name) == 0) {
		names[tag].push_back(name);
		name_flags[tag].push_back(1);
		memset(meters[tag][name], 0, max_ * 2 * sizeof(float));
	}

	start_stop[tag][name].start.QuadPart = (LONGLONG)0;
	start_stop[tag][name].stop.QuadPart = (LONGLONG)(value * 10000000);

	start_stop[tag][name].duration_time = start_stop[tag][name].stop.QuadPart - start_stop[tag][name].start.QuadPart;
	start_stop[tag][name].is_reset_duration_time = false;
}

bool Work_meter::tag_start(const std::string& tag, int thread_num) {
	std::lock_guard <std::mutex> lock(mtx);

	if (meters.count(tag) == 0) {
		tags.push_back(tag);
		meters[tag];
	}
	now_tag[thread_num] = tag;

	return 1;
}
bool Work_meter::tag_stop(int thread_num) {
	std::lock_guard <std::mutex> lock(mtx);

	now_tag[thread_num] = std::string("");

	return 1;
}