
#include "audio_source.h"
#include "audio_manager.h"

#include "audio_data_struct.h"
#include "audio_emitter.h"

#include "../../DirectXTK-master/Inc/Audio.h"
#include "../../Imgui/imgui.h"


using namespace Adollib;

void Audio_source::awake() {
	emitter_data = std::make_shared<Audio_emitter>();
}

void Audio_source::update() {

	if (sound_instance == nullptr) {
		return;
	}

	// 3D座標の更新、など(Apply3DはReverbの更新もしてくれる)
	{
		if (is_use3D) {
			const auto& listener = Audio_manager::get_listener_data();
			DirectX::AudioEmitter emitter;
			emitter_data->convert_to_DirectXAudioEmitter(&emitter, transform->position, transform->orientation);
			sound_instance->Apply3D(listener, emitter);
		}

		if (is_mute) sound_instance->SetVolume(0);
		else sound_instance->SetVolume(volume);

		sound_instance->SetPitch(pitch);
	}

}

void Audio_source::finalize() {
	if (sound_instance == nullptr) {
		return;
	}
	sound_instance->Stop();
}

std::shared_ptr<Audio_emitter> Audio_source::load(const std::string& file_path, const bool is_use3D_) {
	auto audio_data = Audio_manager::load(file_path);

	emitter_data = std::make_shared<Audio_emitter>();

	is_use3D = is_use3D_;

	sound_instance.reset();
	Audio_manager::create_instance(sound_instance, audio_data->soundEffect, is_use3D);
	filepath = file_path;

	return emitter_data;
}

void Audio_source::play(const bool l_is_loop) {
	if (sound_instance == nullptr) {
		assert(0 && "this AudioSorce doesnt load files,");
		return;
	}
	sound_instance->Play(l_is_loop);
	is_loop = l_is_loop;
}
void Audio_source::stop() {
	if (sound_instance == nullptr) {
		assert(0 && "this AudioSorce didnt load the file,");
		return;
	}
	sound_instance->Stop();
}
void Audio_source::pause() {
	if (sound_instance == nullptr) {
		assert(0 && "this AudioSorce didnt load the file,");
		return;
	}
	sound_instance->Pause();
}


void Audio_source::Update_hierarchy() {

	// fileのload
	ImGui::Text("filepath: %s", filepath.c_str());
	if (ImGui::Button("load audio"))
	{
		char path[MAX_PATH] = { "\0" };
		if (openFileName(path, MAX_PATH, ".wav"))
		{
			load(std::string(path), true);
		}
	}

	// play stop pause
	{
		const DirectX::SoundState state = sound_instance->GetState();

		ImVec4& color = ImGui::GetStyle().Colors[ImGuiCol_Text];
		const ImVec4 base_color = color;
		const ImVec4 changed_color = ImVec4(0, 0, 0, 1);


		color = changed_color;
		if (state == DirectX::SoundState::PLAYING) color = base_color;
		if (ImGui::Button("play"))  sound_instance->Play(is_loop);

		color = changed_color;
		if (state == DirectX::SoundState::STOPPED) color = base_color;
		ImGui::SameLine();
		if (ImGui::Button("stop"))  sound_instance->Stop();

		color = changed_color;
		if (state == DirectX::SoundState::PAUSED) color = base_color;
		ImGui::SameLine();
		if (ImGui::Button("pause")) sound_instance->Pause();

		color = base_color;
	}

	ImGui::DragFloat("volume", &volume, 0.01f, 0, 1);
	ImGui::DragFloat("pitch", &pitch, 0.01f, -1, 1);	pitch = ALClamp(pitch, -1, 1);
	ImGui::DragFloat("InnerRadius", &emitter_data->InnerRadius, 0.1f, 0, FLT_MAX);


	// loop
	if (ImGui::Checkbox("is_loop", &is_loop)) {
		const DirectX::SoundState state = sound_instance->GetState();
		if (state == DirectX::PLAYING) { sound_instance->Stop();  sound_instance->Play(is_loop); }
	}


	ImGui::Checkbox("is_mute",  &is_mute);


	//ImGui::Checkbox("is_loop", &is_loop);
}

bool Audio_source::openFileName(char* filepath, int size, const char* filter)
{
	static char path_buffer[MAX_PATH];

	// 初期パス設定
	char dirname[MAX_PATH];
	if (filepath[0] != '0')
	{
		// ディレクトリパス取得
		_splitpath_s(filepath, nullptr, 0, dirname, MAX_PATH, nullptr, 0, nullptr, 0);
	}
	else
	{
		filepath[0] = dirname[0] = '\0';
	}
	if ((dirname[0] == '\0'))
	{
		strcpy_s(dirname, MAX_PATH, path_buffer);
	}

	for (char* p = dirname; *p != '\0'; p++)
	{
		if (*p == '/')
			*p = '\\';
	}

	if (filter == nullptr)
	{
		filter = "All Files\0*.*\0\0";
	}

	// 構造体セット
	OPENFILENAMEA	ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = 0;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filepath;
	ofn.nMaxFile = size;
	ofn.lpstrTitle = nullptr;
	ofn.lpstrInitialDir = (dirname[0] != '\0') ? dirname : nullptr;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	//if (multi_select) ofn.Flags |= OFN_ALLOWMULTISELECT;

	// カレントディレクトリ取得
	char current_dir[MAX_PATH];
	if (!GetCurrentDirectoryA(MAX_PATH, current_dir))
	{
		current_dir[0] = '\0';
	}

	// ダイアログオープン
	if (GetOpenFileNameA(&ofn) == FALSE)
	{
		return false;
	}

	// カレントディレクトリ復帰
	if (current_dir[0] != '\0')
	{
		SetCurrentDirectoryA(current_dir);
	}

	// 最終パスを記憶
	strcpy_s(path_buffer, MAX_PATH, filepath);

	return true;
}