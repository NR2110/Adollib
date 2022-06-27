
#include "physics_demo_stage.h"

#include "../../Scripts/Object/gameobject_manager.h"
#include "../../Scripts/Renderer/renderer_base.h"

#include "collobject_creater.h"

#include "../../Imgui/imgui.h"

#include <Adollibphysics.h>

using namespace Adollib;

void Physics_demo_stage::awake() {

	auto coll = Collider_creater::create_box("stage", Vector3(0, -60, 0), Vector3(0), Vector3(60, 60, 60), Vector3(30, 87, 124));
	stage = coll->gameobject;

	coll->get_collider()->physics_data.is_moveable = false;
}

void Physics_demo_stage::update() {


}

void Physics_demo_stage::Update_hierarchy() {

}