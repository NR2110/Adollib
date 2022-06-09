
#include "audio_listener.h"
#include "audio_resource_manager.h"
#include "../../DirectXTK-master/Inc/Audio.h"

using namespace Adollib;

void Audio_listener::awake() {
	this_itr = Audio_resource_manager::add_listener_component(this);
}

void Audio_listener::finalize() {
	Audio_resource_manager::remove_listener_component(this_itr);
}

void Audio_listener::set_listener() {
	Audio_resource_manager::move_listener_end(this_itr);
}


void Audio_listener::convert_to_DirectXAudioListener(DirectX::AudioListener* out) {
    out->Position = Position;
    out->Velocity = Velocity;
    out->OrientFront = OrientFront;
    out->OrientTop = OrientTop;
    //listener.pCone = pCone;
}

