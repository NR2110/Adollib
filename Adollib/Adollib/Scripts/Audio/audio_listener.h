#pragma once

#include "../Object/component.h"

namespace DirectX {
	class AudioListener;
};

namespace Adollib {
	class Audio_listener : public Component {

	public:
		Vector3 OrientFront; // orientation of front direction, used only for matrix and delay calculations or listeners with cones for matrix, LPF (both direct and reverb paths), and reverb calculations, must be normalized when used
		Vector3 OrientTop;   // orientation of top direction, used only for matrix and delay calculations, must be orthonormal with OrientFront when used

		Vector3 Position; // position in user-defined world units, does not affect Velocity
		Vector3 Velocity; // velocity vector in user-defined world units/second, used only for doppler calculations, does not affect Position
		//X3DAUDIO_CONE* pCone; // sound cone, used only for matrix, LPF (both direct and reverb paths), and reverb calculations, NULL specifies omnidirectionality

	private:
		std::list<Audio_listener*>::iterator this_itr;

	public:
		// listenerはmanagerにlistで管理されており、最後尾のlistenerの座標から音を出している 自身を最後尾に持ってくる
		void set_listener();

		void convert_to_DirectXAudioListener(DirectX::AudioListener* out);

	public:
		void awake() override;

		void finalize() override;

	};
}