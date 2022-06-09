
#include "audio_emitter.h"

#include "../../DirectXTK-master/Inc/Audio.h"

using namespace Adollib;
using namespace DirectX;

void Audio_emitter::convert_to_DirectXAudioEmitter(DirectX::AudioEmitter* out, const Vector3& pos, const Quaternion& orient) {

	//out->pCone = pCone;
	out->OrientFront = vector3_quatrotate(OrientFront_local, orient);
	out->OrientTop = vector3_quatrotate(OrientTop_local, orient);

	out->Position = vector3_quatrotate(Position_local, orient) + pos;
	out->Velocity = vector3_quatrotate(Velocity_local, orient);

	out->InnerRadius = InnerRadius;
	out->InnerRadiusAngle = InnerRadiusAngle;

	out->ChannelCount = ChannelCount;
	out->ChannelRadius = ChannelRadius;
	out->pChannelAzimuths = pChannelAzimuths;

	//out->pVolumeCurve = pVolumeCurve;
	//out->pLFECurve = pLFECurve;
	//out->pLPFDirectCurve = pLPFDirectCurve;
	//out->pLPFReverbCurve = pLPFReverbCurve;
	//out->pReverbCurve = pReverbCurve;

	//out->CurveDistanceScaler = CurveDistanceScaler;
	out->DopplerScaler = DopplerScaler;


}