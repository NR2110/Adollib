#pragma once

//

#include <unordered_map>
#include "audio_data_struct.h"
#include "../../DirectXTK-master/Inc/Audio.h"


namespace Adollib {
	class Audio_listener;

	static class Audio_resource_manager {


	private:
		// load����audio�̃��\�[�X
		static std::unordered_map<std::string, Audio_data> audio_datas;
		// audio engin
		static std::unique_ptr<DirectX::AudioEngine> audioEngine;
		// listener�ւ̃|�C���^ --end �̍��p��listener�Ƃ���
		static std::list<Audio_listener*> audio_listener_comps;
		// listener�̏�� update�ōX�V���Ă���
		static DirectX::AudioListener listener;

	public:
		// file��load
		static Audio_data* load(const std::string& file_path);
		// ���\�[�X�̉��
		static void release(const std::string& file_path);
		// soundeffect instance�̐��� (sorce voice�̐���)
		static std::unique_ptr<DirectX::SoundEffectInstance>& create_instance(std::unique_ptr<DirectX::SoundEffect>& soundefect, const bool is_use3D = true, const bool is_useReverb = true, const bool is_usePitchchenge = false);

		//:::: listener :::
		// listener��ς�
		static std::list<Audio_listener*>::iterator add_listener_component(Audio_listener* comp);
		// listener�̍폜
		static void remove_listener_component(std::list<Audio_listener*>::iterator itr);
		// �w�肵��listener���Ō����(���̍��W���特���o��悤��)
		static void move_listener_end(std::list<Audio_listener*>::iterator itr);
		// listener�̏���Ԃ�
		static const DirectX::AudioListener& get_listener_data();




	public:
		static void start();
		static void update();
		static void destroy();


	};
}