
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"

#include "player.h"

namespace Adollib
{
	// ��������V�[���̏��������Ɉ�x�����Ă΂��
	void player::awake() {

	}

	void player::start()
	{

	}

	// ���t���[���Ă΂��X�V����
	void player::update()
	{
		vector3 move_vec = vector3(0, 0, 0);
		float move_force = 0.1;
		if (input->getKeyState(Key::Down))move_vec += vector3(0, 0, -1);
		if (input->getKeyState(Key::Up))move_vec += vector3(0, 0, +1);
		collier->add_force(vector3_be_rotated_by_quaternion(move_vec * move_force,gameobject->get_world_orientate()));

		vector3 rot_vec = vector3(0, 0, 0);
		float rot_force = 0.2;
		if (input->getKeyState(Key::Left))rot_vec += vector3(0, -1, 0);
		if (input->getKeyState(Key::Right))rot_vec += vector3(0, +1, 0);
		collier->add_torque(rot_vec * rot_force);
		//	collier->orientation * quaternion_angle_axis(1, vector3(-1, 1, 0).unit_vect());

		//framework::debug->setString("angular_velocity : %f,%f,%f", collier->angular_velocity.x, collier->angular_velocity.y, collier->angular_velocity.z);
	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
	void player::onEnable()
	{

	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
	void player::onDisable()
	{

	}

}
