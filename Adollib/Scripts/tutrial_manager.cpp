
#include "tutrial_manager.h"

#include "tutrial_stage01_move_camera_catch.h"
#include "tutrial_stage02_rope.h"
#include "input_changer.h"
#include "player.h"

using namespace Adollib;

void Tutrial_manager::awake() {


}

// ��������V�[���̏��������Ɉ�x�����Ă΂��
void Tutrial_manager::start() {
	{
		auto ptr = std::make_shared<Tutrial_stage01_move_camera_catch>();
		ptr->time = time;
		ptr->input_changer = gameobject->findComponent<Input_changer>();
		ptr->player = gameobject->findComponent<Player>();
		ptr->tutrial_ui_x = tutrial_ui_x;
		ptr->awake();
		tutrials.emplace_back(ptr);
	}
	{
		auto ptr = std::make_shared<Tutrial_stage02_rope>();
		ptr->time = time;
		ptr->input_changer = gameobject->findComponent<Input_changer>();
		ptr->player = gameobject->findComponent<Player>();
		ptr->tutrial_ui_x = tutrial_ui_x;
		ptr->awake();
		tutrials.emplace_back(ptr);
	}
}

// ���t���[���Ă΂��X�V����
void Tutrial_manager::update() {
	for (auto& tutrial : tutrials)tutrial->update();
}
