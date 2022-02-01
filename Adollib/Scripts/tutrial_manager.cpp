
#include "tutrial_manager.h"

#include "tutrial_stage01_move_camera_catch.h"
#include "input_changer.h"
#include "player.h"

using namespace Adollib;

void Tutrial_manager::awake() {


}

// 所属するシーンの初期化時に一度だけ呼ばれる
void Tutrial_manager::start() {
	{
		auto ptr = std::make_shared<Tutrial_stage01_move_camera_catch>();
		ptr->time = time;
		ptr->input_changer = gameobject->findComponent<Input_changer>();
		ptr->player = gameobject->findComponent<Player>();
		ptr->awake();
		tutrials.emplace_back(ptr);
	}
}

// 毎フレーム呼ばれる更新処理
void Tutrial_manager::update() {
	for (auto& tutrial : tutrials)tutrial->update();
}
