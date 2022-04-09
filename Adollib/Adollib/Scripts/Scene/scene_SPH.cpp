#include "Scene_SPH.h"
#include "../Object/gameobject_manager.h"

#include "../../Scripts/Main/mono_audio.h"


#include "../../../Scripts/simulation_SPH.h"

#include "../../../Scripts/debug_camera_controller.h"


namespace Adollib {


	void Scene_SPH::initialize() {

		//ALKLib::MonoAudio::PlayMusic(0, true);
		//ALKLib::MonoAudio::PlayMusic(1, true);

		{
			Gameobject* simulation_SPH = Gameobject_manager::create("simulation_SPH");
			simulation_SPH->addComponent<Simulation_SPH>();
		}

		{
			Gameobject* camera = Gameobject_manager::create("camera");
			camera->addComponent<Camera_component>();
			camera->addComponent<Debug_camera_controller>();

			camera->transform->local_pos = Vector3(0.6f, 0.3f, -1.4f);

		}
	}



}