#include "scene_title.h"

#include "../Object/gameobject_manager.h"
#include "../Object/component_camera.h"
#include "../Renderer/directional_shadow.h"
#include "../Renderer/renderer_base.h"

#include "../../../Scripts/scene_title_all.h"

namespace Adollib {

	void Scene_title::initialize() {

		{
			Gameobject* camera = Gameobject_manager::create("camera");
			camera->addComponent<Camera_component>();
			auto comp = camera->addComponent<Title_camera>();
			comp->Set_start_transform(Vector3(0, 0, -60), quaternion_from_euler(0, 0, 0));
		}

		{
			Gameobject* stage = Gameobject_manager::create("stage_pearent");
			stage->addComponent<Title_stage>();
			stage->addComponent<Fall_object_manager>();
		}

		{
			Gameobject* state_manager = Gameobject_manager::create("title_state_manager");
			state_manager->addComponent<Title_state_manager>();
		}



	}





}
