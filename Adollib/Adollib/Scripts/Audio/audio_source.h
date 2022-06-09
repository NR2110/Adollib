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
		float pitch = 1;
		float pan = 0;


	private:
		std::unique_ptr<DirectX::SoundEffectInstance> sound_instance;
		std::shared_ptr<Audio_emitter> emitter_data;

		bool is_use3D = true;
		bool is_useReverb = true;
		bool is_usePitchchenge = false;

	public:
		std::shared_ptr<Audio_emitter> get_emitter_ptr() { return emitter_data; };

	public:
		// fileの読み込み, sorcevoiceの作成
		void load(const std::string& file_path, const bool is_use3D = true, const bool is_useReverb = true, const bool is_usePitchchenge = false);

		void play(const bool is_loop);






	public:
		void awake() override;

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start() override {};

		// 毎フレーム呼ばれる更新処理
		void update() override;

		void finalize() override;

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable() override {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable() override {};

	};
}