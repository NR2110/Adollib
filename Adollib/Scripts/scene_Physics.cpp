
#include "Scene_Physics.h"

#include "../../Scripts/Object/gameobject_manager.h"
#include "../../Scripts/Audio/audio_listener.h"
#include "debug_camera_controller.h"

#include "physics_demo_stage.h"
#include "physics_demo_obj.h"


namespace Adollib {


	void Scene_Physics::initialize() {

		{
			Gameobject* camera = Gameobject_manager::create("camera");
			camera->addComponent<Camera_component>();
			auto camera_cont = camera->addComponent<Debug_camera_controller>();
			camera->addComponent<Audio_listener>();

			camera->transform->local_pos = Vector3(0, 2, -10);
			camera->transform->local_orient = quaternion_from_euler(20, 0, 0);
			camera_cont->linear_speed = 1;

		}

		{
			auto go = Gameobject_manager::create("stage");
			go->addComponent<Physics_demo_stage>();
			go->is_hierarchy = false;
		}

		{
			auto go = Gameobject_manager::create("obj");
			go->addComponent<Physics_demo_obj>();
			go->is_hierarchy = false;
		}

	}



}