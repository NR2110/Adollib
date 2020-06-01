#include "gameobject_manager.h"

#include "object_fall.h"
#include "Adollib.h"

using namespace Adollib;

namespace Adollib
{
	// ��������V�[���̏��������Ɉ�x�����Ă΂��
	void object_fall::awake() {

	}

	void object_fall::start()
	{

	}

	// ���t���[���Ă΂��X�V����
	void object_fall::update()
	{
		vector3 move_vec = vector3(0, 0, 0);
		float force = 10;
		if (input->getKeyState(Key::Up))move_vec += vector3(0, -1, 0);
		if (input->getKeyState(Key::Down))move_vec += vector3(0, +1, 0);
		if (input->getKeyState(Key::Left))move_vec += vector3(-1, 0, 0);
		if (input->getKeyState(Key::Right))move_vec += vector3(+1, 0, 0);

		float fall_pow = 0.89 * 3 * Al_Global::elapsed_time;
		//fall_pow = 0;
		vector3 fall = vector3(0, -1, 0) * fall_pow;

		collier->add_force(move_vec * force + fall);

		//collier->orientation = collier->orientation * quaternion_angle_axis(1, vector3(-1, 1, 0).unit_vect());

		//framework::debug->setString("angular_velocity : %f,%f,%f", collier->angular_velocity.x, collier->angular_velocity.y, collier->angular_velocity.z);
	}
	// ���t���[���Aupdate()��ɌĂ΂��X�V����
	void object_fall::lateUpdate()
	{


	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
	void object_fall::onEnable()
	{

	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
	void object_fall::onDisable()
	{

	}

}
