
#include "tutorial_manager.h"

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"

#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Physics/joint.h"
#include "../Adollib/Scripts/Physics/collider_croth.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "../Adollib/Scripts/Renderer/mesh_renderer.h"
#include "../Adollib/Scripts/Renderer/material_manager.h"
#include "../Adollib/Scripts/Renderer/material.h"
#include "../Adollib/Scripts/Renderer/texture.h"
#include "../Adollib/Scripts/Renderer/sprite_renderer.h"
#include "../Adollib/Scripts/Renderer/UI_renderer.h"
#include "../Adollib/Scripts/Object/component_camera.h"

#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Renderer/croth_renderer.h"


using namespace Adollib;

void Tutrial_manager::awake() {

	mat_tutrial_move = Material_manager::create_material("mat_tutrial_move");
	mat_tutrial_move->get_texture()->Load(L"./DefaultTexture/tutrial/tuterial_stick.png");
	mat_tutrial_move->BS_state = State_manager::BStypes::BS_ALPHA;
	mat_tutrial_move->Load_VS("./DefaultShader/ui_vs.cso");
	mat_tutrial_move->Load_PS("./DefaultShader/ui_ps.cso");

	mat_tutrial_camera =Material_manager::create_material("mat_tutrial_camera");
	mat_tutrial_camera->get_texture()->Load(L"./DefaultTexture/tutrial/tuterial_stick.png");
	mat_tutrial_camera->BS_state = State_manager::BStypes::BS_ALPHA;
	mat_tutrial_camera->Load_VS("./DefaultShader/ui_vs.cso");
	mat_tutrial_camera->Load_PS("./DefaultShader/ui_ps.cso");

	mat_tutrial_hand   =Material_manager::create_material("mat_tutrial_hand");
	mat_tutrial_hand->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_chatch_and_hold_Turn_your_hand.png");
	mat_tutrial_hand->BS_state = State_manager::BStypes::BS_ALPHA;
	mat_tutrial_hand->Load_VS("./DefaultShader/ui_vs.cso");
	mat_tutrial_hand->Load_PS("./DefaultShader/ui_ps.cso");


}

static Gameobject* go_tutrial_move;
void Tutrial_manager::start() {
	go_tutrial_move = Gameobject_manager::createPlane("go_tutrial_move");
	auto renderer_tutrial_move = go_tutrial_move->addComponent<UI_renderer>();
	renderer_tutrial_move->set_material(mat_tutrial_move);

	//renderer_tutrial_move->ui_data. = 1;

}


void Tutrial_manager::update() {

	//go_tutrial_move->transform->local_pos =



}