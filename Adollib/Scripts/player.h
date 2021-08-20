#pragma once
#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"
#include "../Adollib/Scripts/Physics/joint.h"

namespace Adollib
{

	class Player : public Component
	{
	private:
		//player�̊�{�̒l ���܂�����const�ɂ���
		float waist_move_max_speed = 1000;
		float waist_move_max_pow = 1000;
		float waist_move_pow = 1000;

		float head_rot_max_speed = 1000;
		float head_rot_max_pow = 1000;
		float head_rot_pow = 1000;

		float waist_rot_max_speed = 1000;
		float waist_rot_max_pow = 1000;
		float waist_rot_pow = 1000;

		float body_rot_max_speed = 1000;
		float body_rot_max_pow = 1000;
		float body_rot_pow = 1000;

		float jump_power = 150;

		float move_speed = 12000;
		float turn_speed = 2;

	private:

		Quaternion rotate; //rotate��buffer
		Vector3 dir; //������buffer
		Vector3 pos; //���W��buffer

		std::shared_ptr<Transfome> camera; //camera�ւ̃|�C���^

		bool is_jumping = false; //���W�����v���Ă��邩
		float coyote = 0.3f; //jump�̋��e����
		float move_timer = 0;

		Joint_base* catch_right_joint = nullptr; //�E�肪����ł���joint
		Joint_base* catch_left_joint = nullptr;	 //���肪����ł���joint

	private:
		//::: GO :::
		Gameobject* Head = nullptr;

		Gameobject* Rsholder = nullptr;
		Gameobject* Relbow = nullptr;
		Gameobject* Lsholder = nullptr;
		Gameobject* Lelbow = nullptr;

		Gameobject* Body = nullptr;

		Gameobject* Waist = nullptr;

		Gameobject* Rleg = nullptr;
		Gameobject* Rfoot = nullptr;
		Gameobject* Lleg = nullptr;
		Gameobject* Lfoot = nullptr;

		//::: collider :::
		Collider* Head_collider = nullptr;
		Collider* Rsholder_collider = nullptr;
		Collider* Relbow_collider = nullptr;
		Collider* Lsholder_collider = nullptr;
		Collider* Lelbow_collider = nullptr;
		Collider* Body_collider = nullptr;
		Collider* Waist_collider = nullptr;
		Collider* Rleg_collider = nullptr;
		Collider* Rfoot_collider = nullptr;
		Collider* Lleg_collider = nullptr;
		Collider* Lfoot_collider = nullptr;

		Collider* hanger_collider = nullptr;




	public:
		void set_player_colliders(
		Collider* l_Head_collider		,
		Collider* l_Rsholder_collider	,
		Collider* l_Relbow_collider		,
		Collider* l_Lsholder_collider	,
		Collider* l_Lelbow_collider		,
		Collider* l_Body_collider		,
		Collider* l_Waist_collider		,
		Collider* l_Rleg_collider		,
		Collider* l_Rfoot_collider		,
		Collider* l_Lleg_collider		,
		Collider* l_Lfoot_collider		,
		Collider* l_Waist_capsule_collider
		) {
			Head_collider		= l_Head_collider;
			Rsholder_collider	= l_Rsholder_collider;
			Relbow_collider		= l_Relbow_collider;
			Lsholder_collider	= l_Lsholder_collider;
			Lelbow_collider		= l_Lelbow_collider;
			Body_collider		= l_Body_collider;
			Waist_collider		= l_Waist_collider;
			Rleg_collider		= l_Rleg_collider;
			Rfoot_collider		= l_Rfoot_collider;
			Lleg_collider		= l_Lleg_collider;
			Lfoot_collider		= l_Lfoot_collider;
			hanger_collider = l_Waist_capsule_collider;

			Head		=Head_collider		->gameobject;
			Rsholder	=Rsholder_collider	->gameobject;
			Relbow		=Relbow_collider	->gameobject;
			Lsholder	=Lsholder_collider	->gameobject;
			Lelbow		=Lelbow_collider	->gameobject;
			Body		=Body_collider		->gameobject;
			Waist		=Waist_collider		->gameobject;
			Rleg		=Rleg_collider		->gameobject;
			Rfoot		=Rfoot_collider		->gameobject;
			Lleg		=Lleg_collider		->gameobject;
			Lfoot		=Lfoot_collider		->gameobject;
		}

	public:
		void awake();

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start();

		// ���t���[���Ă΂��X�V����
		void update();

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable();

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable();

	};

}