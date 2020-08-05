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

	//�g���ĂȂ������牽��������return
	if (names.size() == 0)return true;


	// delta_time�̏���(multisled�ɑΉ����邽��)
	QueryPerformanceFrequency(&freq);
	float F = (float)((double)1000 / freq.QuadPart * 0.001f);

	QueryPerformanceCounter(&now);
	if(stop == false)before -= (float)((now.QuadPart - bef.QuadPart)) * F;
	QueryPerformanceCounter(&bef);

	// deltatime��p���ăt���[���Ɉˑ����Ȃ��X�V
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

			//��������
			if (adjust_max && max_num < meters[names[name]][start_num + max_] * 1.2f) max_num = meters[names[name]][start_num + max_] * 1.2f;

			start_stop[names[name]].stop.QuadPart = 0;
			start_stop[names[name]].start.QuadPart = 0;
		}

		// 0.016�b���ƂɍX�V
		before = 0.016;
	}
 


	flag = 0;
	//if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoTitleBar;			// �^�C�g���o�[���\���ɂ��܂��B
	//if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoBackground;		// �E�B���h�E���̔w�i���\���ɂ��܂��B
	flag |= ImGuiWindowFlags_AlwaysAutoResize;		// �����ŃE�B���h�E���̃R���e���c�ɍ��킹�ă��T

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
	//if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoTitleBar;			// �^�C�g���o�[���\���ɂ��܂��B
	//if (!draw_Back_ground) flag |= ImGuiWindowFlags_NoBackground;		// �E�B���h�E���̔w�i���\���ɂ��܂��B
	flag |= ImGuiWindowFlags_AlwaysAutoResize;		// �����ŃE�B���h�E���̃R���e���c�ɍ��킹�ă��T

	ImGui::Begin("work_meter", 0, flag);

	//�X�V
	ImGui::Checkbox("stop", &stop);
	////�w�i�`��
	//ImGui::Checkbox("draw_Back_ground", &draw_Back_ground); 
	//�O���t�̍ő�l���������߂��邩
	ImGui::Checkbox("adjust_max", &adjust_max);

	//�O���t�̍ő�l�̓��o��
	ImGui::InputFloat("max", &max_num, 0.001f, 1.0f, "%.3f");

	for (int i = 0; i < names.size(); i++) {
		if (name_flags[i] == false)continue;
		ImGui::PlotLines("", &meters[names[i]][start_num], 120, 0, names[i].c_str(), 0.0f, max_num, ImVec2(500, 50));
	}

	// work_meter_manager�̕\���̐؂�ւ�
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