#pragma once

//

#include <unordered_map>
#include "audio_data_struct.h"
#include "../../DirectXTK-master/Inc/Audio.h"


namespace Adollib {
	class Audio_listener;

	static class Audio_manager {


	private:
		// audio engin
		static std::unique_ptr<DirectX::AudioEngine> audioEngine;
		// listenerへのポインタ --end の座用をlistenerとする
		static std::list<Audio_listener*> audio_listener_comps;
		// listenerの情報 updateで更新している
		static DirectX::AudioListener listener;

	public:
		// fileのload
		static Audio_data* load(const std::string& file_path);
		// リソースの解放
		static void release(const std::string& file_path);
		// soundeffect instanceの生成 (sorce voiceの生成)
		static void create_instance(std::unique_ptr<DirectX::SoundEffectInstance>& out, std::unique_ptr<DirectX::SoundEffect>& soundefect, const bool is_use3D, const bool is_useReverb, const bool is_usePitchchenge);

		//:::: listener :::
		// listenerを積む
		static std::list<Audio_listener*>::iterator add_listener_component(Audio_listener* comp);
		// listenerの削除
		static void remove_listener_component(std::list<Audio_listener*>::iterator itr);
		// 指定したlistenerを最後尾に(その座標から音が出るように)
		static void move_listener_end(std::list<Audio_listener*>::iterator itr);
		// listenerの情報を返す
		static const DirectX::AudioListener& get_listener_data();

		//:::: reverb :::
		// reverbのセット
		static void set_reverb(const XAUDIO2FX_REVERB_PARAMETERS* native);



	public:
		static void initialize();
		static void update();
		static void destroy();


	};
}