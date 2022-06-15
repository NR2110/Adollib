
#include "audio_listener.h"
#include "audio_manager.h"
#include "../../DirectXTK-master/Inc/Audio.h"

using namespace Adollib;

void Audio_listener::awake() {
	this_itr = Audio_manager::add_listener_component(this);
}

void Audio_listener::finalize() {
	Audio_manager::remove_listener_component(this_itr);
}

void Audio_listener::set_listener() {
    Audio_manager::move_listener_end(this_itr);
}


void Audio_listener::convert_to_DirectXAudioListener(DirectX::AudioListener* out) {
    out->Position = vector3_quatrotate(Position_off, transform->orientation) + transform->position;
    out->Velocity = vector3_quatrotate(Velocity_off, transform->orientation);
    out->OrientFront = vector3_quatrotate(OrientFront_off, transform->orientation);
    out->OrientTop = vector3_quatrotate(OrientTop_off, transform->orientation);
    //listener.pCone = pCone;
}

