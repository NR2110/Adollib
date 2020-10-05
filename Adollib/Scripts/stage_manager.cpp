
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"
#include "../Adollib/imgui_all.h"

#include "stage_manager.h"

#include "stage_part.h"

using namespace Adollib;

// 所属するシーンの初期化時に一度だけ呼ばれる
void Stage_manager_s::awake() {

}

void Stage_manager_s::start() {

	Camera* C = Gameobject_manager::find_camera("camera");
	camera_pos = &C->transform->local_pos;
	camera_orient = &C->transform->local_orient;



	//set_cube(vector3(0, -50 - 60, 0), vector3(120, 60, 120), vector3(0, 0, 0)); //下
	//set_cube(vector3(0, +50 + 60, 0), vector3(120, 60, 120), vector3(0, 0, 0)); //上


	set_cube(vector3(0, -80, 170), vector3(5, 60, 120), vector3(0, 0, 0));
	//set_cube(vector3(0, 0, 0), vector3(10, 10, 120), vector3(0, 0, 0));

	set_cube(vector3(0, -60 - 40, 120), vector3(120, 60, 120), vector3(0, 0, 0)); //下
	set_cube(vector3(0, +60 + 40, 120), vector3(120, 60, 120), vector3(0, 0, 0)); //上


	set_cube(vector3(+90 + 60, 0, 120), vector3(60, 120, 120), vector3(0, 0, 0));
	set_cube(vector3(-90 - 60, 0, 120), vector3(60, 120, 120), vector3(0, 0, 0));


	//set_cube(vector3(0, 0, 0), vector3(5, 5, 120), vector3(0, 0, 45));





	//set_cube(vector3(0, 0, -30), vector3(220, 220, 60), vector3(0, 0, 0), vector4(1, 1, 1, 0.5));
}

// 毎フレーム呼ばれる更新処理
void Stage_manager_s::update() {


}

// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
void Stage_manager_s::onEnable() {

}

// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
void Stage_manager_s::onDisable() {

}


void Stage_manager_s::set_cube(vector3 pos, vector3 size, vector3 rotate, vector4 color) {
	Gameobject* GO = Gameobject_manager::createCube("");
	GO->material->color = color;
	GO->transform->local_pos = pos;
	GO->transform->local_scale = size;
	GO->transform->local_orient = quaternion_from_euler(rotate);

	Stage_part_s* s = GO->addComponent<Stage_part_s>();
	s->camera_orient = camera_orient;
	s->camera_pos = camera_pos;

	stages.emplace_back(s);

}
