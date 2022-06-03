#pragma once

#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Object/component.h"
#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>
#include <stdio.h>
#include "Audio.h"

namespace DirectX {
	class AudioEngine;
	class SoundEffect;
	class SoundEffectInstance;
}

namespace Adollib {

	class Demo_music : public Component {

	private:
		std::unique_ptr<DirectX::AudioEngine> audioEngine = nullptr;
		DirectX::SoundEffect* soundeffect;
		std::unique_ptr<DirectX::SoundEffectInstance> audioInstance;

		DirectX::AudioListener listener; //���g�̍��W
		DirectX::AudioEmitter emitter;   //���̍��W
	private:


	public:
		void awake() override;

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start() override {};

		// ���t���[���Ă΂��X�V����
		void update() override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable() override {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable() override {};
	};


}