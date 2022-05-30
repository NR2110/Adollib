#include "scene_music.h"
#include "../Object/gameobject_manager.h"

#include "../../Scripts/Main/mono_audio.h"


#include "../../../Scripts/demo_music.h"

#include "../../../Scripts/debug_camera_controller.h"


namespace Adollib {


	void Scene_music::initialize() {

		{
			Gameobject* go = Gameobject_manager::create("demo_music");
			go->addComponent<Demo_music>();
		}

		{
			Gameobject* camera = Gameobject_manager::create("camera");
			camera->addComponent<Camera_component>();
			auto camera_cont = camera->addComponent<Debug_camera_controller>();

			camera->transform->local_pos = Vector3(0, 0, -10);
			camera_cont->linear_speed = 1;

		}
	}



}