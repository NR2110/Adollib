#pragma once
#include <memory>

#include "../Math/math.h"

namespace DirectX {
	class AudioEmitter;
}

namespace Adollib {
	class Audio_emitter {
	public:
		Vector3 OrientFront_local = Vector3(0, 0, -1); // orientation of front direction, used only for emitter angle calculations or with multi-channel emitters for matrix calculations or single-channel emitters with cones for matrix, LPF (both direct and reverb paths), and reverb calculations, must be normalized when used
		Vector3 OrientTop_local = Vector3(0, 1, 0);   // orientation of top direction, used only with multi-channel emitters for matrix calculations, must be orthonormal with OrientFront when used

		Vector3 Position_local= Vector3(0); // position in user-defined world units, does not affect Velocity
		Vector3 Velocity_local= Vector3(0); // velocity vector in user-defined world units/second, used only for doppler calculations, does not affect Position

		float InnerRadius = 0;      // inner radius, must be within [0.0f, FLT_MAX]
		float InnerRadiusAngle = ToRadian(90); // inner radius angle, must be within [0.0f, X3DAUDIO_PI/4.0)

		int ChannelCount= 1;       // number of sound channels, must be > 0
		float ChannelRadius = 1;     // channel radius, used only with multi-channel emitters for matrix calculations, must be >= 0.0f when used
		float* pChannelAzimuths = nullptr; // channel azimuth array, used only with multi-channel emitters for matrix calculations, contains positions of each channel expressed in radians along the channel radius with respect to the front orientation vector in the plane orthogonal to the top orientation vector, or X3DAUDIO_2PI to specify an LFE channel, must have at least ChannelCount elements, all within [0.0f, X3DAUDIO_2PI] when used

		//X3DAUDIO_DISTANCE_CURVE* pVolumeCurve;    // volume level distance curve, used only for matrix calculations, NULL specifies a default curve that conforms to the inverse square law, calculated in user-defined world units with distances <= CurveDistanceScaler clamped to no attenuation
		//X3DAUDIO_DISTANCE_CURVE* pLFECurve;       // LFE level distance curve, used only for matrix calculations, NULL specifies a default curve that conforms to the inverse square law, calculated in user-defined world units with distances <= CurveDistanceScaler clamped to no attenuation
		//X3DAUDIO_DISTANCE_CURVE* pLPFDirectCurve; // LPF direct-path coefficient distance curve, used only for LPF direct-path calculations, NULL specifies the default curve: [0.0f,1.0f], [1.0f,0.75f]
		//X3DAUDIO_DISTANCE_CURVE* pLPFReverbCurve; // LPF reverb-path coefficient distance curve, used only for LPF reverb-path calculations, NULL specifies the default curve: [0.0f,0.75f], [1.0f,0.75f]
		//X3DAUDIO_DISTANCE_CURVE* pReverbCurve;    // reverb send level distance curve, used only for reverb calculations, NULL specifies the default curve: [0.0f,1.0f], [1.0f,0.0f]

		float CurveDistanceScaler = 1; // curve distance scaler, used to scale normalized distance curves to user-defined world units and/or exaggerate their effect, used only for matrix, LPF (both direct and reverb paths), and reverb calculations, must be within [FLT_MIN, FLT_MAX] when used
		float DopplerScaler = 1;       // doppler shift scaler, used to exaggerate doppler shift effect, used only for doppler calculations, must be within [0.0f, FLT_MAX] when used

	public:
		void convert_to_DirectXAudioEmitter(DirectX::AudioEmitter* out, const Vector3& pos, const Quaternion& orient);


	};
}