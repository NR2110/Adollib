#pragma once

#include "../Object/component.h"

namespace DirectX {
	class SoundEffectInstance;
}

namespace Adollib {
	class Audio_emitter;

	class Audio_source : public Component {

	public:
		float volume = 0.3f;
		bool is_mute = false;
		bool is_loop = false;

		float pitch = 0;


	private:
		std::unique_ptr<DirectX::SoundEffectInstance> sound_instance;
		std::shared_ptr<Audio_emitter> emitter_data;

		bool is_use3D = true;

		std::string filepath;

	private:
		// ファイルを開く
		bool openFileName(char* filepath, int size, const char* filter);


	public:
		std::shared_ptr<Audio_emitter> get_emitter_ptr() { return emitter_data; };

		//static void set_reverb(value);

		//Gameobject* go = Gameobject::create();
	public:
		// fileの読み込み, sorcevoiceの作成
		std::shared_ptr<Audio_emitter> load(const std::string& file_path, const bool is_use3D = true);

		void play(const bool is_loop);

		void stop();

		void pause();






	public:
		void awake() override;

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start() override {};

		// 毎フレーム呼ばれる更新処理
		void update() override;

		// Hierarchyの表示(Imguiの関数 Imgui::begin,Imgui::endはいらない)
		void Update_hierarchy() override;

		//
		void finalize() override;

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable() override {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable() override {};

	};
}