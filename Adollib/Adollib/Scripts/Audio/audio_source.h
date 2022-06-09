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
		// file�̓ǂݍ���, sorcevoice�̍쐬
		void load(const std::string& file_path, const bool is_use3D = true, const bool is_useReverb = true, const bool is_usePitchchenge = false);

		void play(const bool is_loop);






	public:
		void awake() override;

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start() override {};

		// ���t���[���Ă΂��X�V����
		void update() override;

		void finalize() override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable() override {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable() override {};

	};
}