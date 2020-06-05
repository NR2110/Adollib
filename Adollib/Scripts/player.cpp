
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
		float force = 10;
		if (input->getKeyState(Key::Up))move_vec += vector3(0, -1, 0);
		if (input->getKeyState(Key::Down))move_vec += vector3(0, +1, 0);
		if (input->getKeyState(Key::Left))move_vec += vector3(-1, 0, 0);
		if (input->getKeyState(Key::Right))move_vec += vector3(+1, 0, 0);

		collier->add_force(move_vec * force);

		//collier->orientation = collier->orientation * quaternion_angle_axis(1, vector3(-1, 1, 0).unit_vect());

		//framework::debug->setString("angular_velocity : %f,%f,%f", collier->angular_velocity.x, collier->angular_velocity.y, collier->angular_velocity.z);
	}
	// ���t���[���Aupdate()��ɌĂ΂��X�V����
	void player::lateUpdate()
	{


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
