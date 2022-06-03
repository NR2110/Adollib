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

		DirectX::AudioListener listener; //自身の座標
		DirectX::AudioEmitter emitter;   //音の座標
	private:


	public:
		void awake() override;

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start() override {};

		// 毎フレーム呼ばれる更新処理
		void update() override;

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable() override {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable() override {};
	};


}