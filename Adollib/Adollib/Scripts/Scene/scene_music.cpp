#include "scene_music.h"
#include "../Object/gameobject_manager.h"

#include "../../Scripts/Main/mono_audio.h"


#include "../../../Scripts/demo_music.h"

#include "../../../Scripts/debug_camera_controller.h"

#include "../Audio/audio_source.h"
#include "../Audio/audio_listener.h"


namespace Adollib {


	void Scene_music::initialize() {

		{
			Gameobject* go = Gameobject_manager::create("demo_music_1");
			auto sorce = go->addComponent<Audio_source>();
			auto demo = go->addComponent<Demo_music>();
			demo->audio = sorce;
		}

		{
			Gameobject* camera = Gameobject_manager::create("camera");
			camera->addComponent<Camera_component>();
			auto camera_cont = camera->addComponent<Debug_camera_controller>();
			camera->addComponent<Audio_listener>();

			camera->transform->local_pos = Vector3(0, 0, 0);
			camera_cont->linear_speed = 1;

		}
	}



}