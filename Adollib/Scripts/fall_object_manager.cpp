
#include "fall_object_manager.h"

#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Object/transform.h"
#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Renderer/renderer_base.h"
#include "../Adollib/Scripts/Renderer/material_manager.h"
#include "../Adollib/Scripts/Renderer/texture.h"
#include "../Adollib/Scripts/Renderer/croth_renderer.h"
#include "../Adollib/Scripts/Physics/collider_croth.h"

#include "../Adollib/Imgui/imgui.h"

#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "fall_object_comp.h"

using namespace Adollib;

void Fall_object_manager::set_box(const char* name, Vector3 position, Vector3 rotate, Vector3 scale, Vector3 color, Gameobject* pearent) {
	auto go = Gameobject_manager::createCube(name);
	go->transform->local_scale = scale;
	go->transform->local_orient = quaternion_from_euler(rotate);
	go->transform->local_pos = position;
	go->renderer->color = Vector4(color,1);
	auto comp = go->addComponent<Fall_object_comp>();
	comp->check_transform = go->transform;

	if (pearent != nullptr)pearent->add_child(go);

	auto coll = go->addComponent<Collider>();
	coll->add_shape<Box>();
	coll->physics_data.inertial_mass = scale.x * scale.y * scale.z;
}
void Fall_object_manager::set_sphere(const char* name, Vector3 position, float r, Vector3 color, Gameobject* pearent) {
	auto go = Gameobject_manager::createSphere(name);
	go->transform->local_scale = Vector3(r);
	//go->transform->local_orient = quaternion_from_euler(rotate);
	go->transform->local_pos = position;
	go->renderer->color = Vector4(color, 1);
	auto comp = go->addComponent<Fall_object_comp>();
	comp->check_transform = go->transform;

	if (pearent != nullptr)pearent->add_child(go);

	auto coll = go->addComponent<Collider>();
	coll->add_shape<Sphere>();
	coll->physics_data.inertial_mass = 4 / 3 * PI * r * r * r;
}

// 所属するシーンの初期化時に一度だけ呼ばれる
void Fall_object_manager::awake()
{
	fall_object_pearent = Gameobject_manager::create("fall_object_pearent");
	start_pos_y = 60;
	start_pos_x[0] = 63;
	start_pos_x[1] = 46;
	start_pos_x[2] = -43;
	start_pos_x[3] = -70;

	timer -= fall_start_time;
}

void Fall_object_manager::start()
{

}

// 毎フレーム呼ばれる更新処理
void Fall_object_manager::update()
{
	timer += time->deltaTime();

	while (timer > fall_cool_time) {

		int type_num = rand() % 3;
		Vector3 rotate_num = Vector3(rand() % 360, rand() % 360, rand() % 360);
		int scale_num = (rand() % 100) * 0.01f * 3 + 2.2;
		Vector3 pos = Vector3(start_pos_x[rand() % 4] + rand() % 100 * 0.01f * start_pos_x_randam_size, start_pos_y + rand() % 100 * 0.01f * start_pos_x_randam_size, -(1 + scale_num));

		if (type_num == 0)set_box("fall_box",    pos, rotate_num, Vector3(scale_num), Al_Global::get_gaming(rand(), 2, 196 / 255.0f, 1), fall_object_pearent);
		if (type_num == 1)set_sphere("fall_box", pos, scale_num, Al_Global::get_gaming(rand(), 2, 196 / 255.0f, 1), fall_object_pearent);
		if (type_num == 2)set_sphere("fall_box", pos, scale_num, Al_Global::get_gaming(rand(), 2, 196 / 255.0f, 1), fall_object_pearent);

		timer = 0;
	}

	// 適当な乱数消費
	rand(); rand(); rand();
}

// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
void Fall_object_manager::onEnable()
{

}

// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
void Fall_object_manager::onDisable()
{

}


