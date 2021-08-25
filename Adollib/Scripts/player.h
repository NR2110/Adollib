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

		//�����Ƃ��̑��̗͂̋���
		float leg_rot_max_speed = 1000;
		float leg_rot_max_pow = 1000;
		float leg_rot_pow = 100;

		//���L�΂��Ƃ��̗͂̋���
		float hand_rot_max_speed = 1000;
		float hand_rot_max_pow = 1000;
		float hand_rot_pow = 100;

		//�J������]�ɉ����Ď���グ�������鋭��
		float hand_updown_max_rot = ToRadian(45);
		float hand_updown_rot_pow = 2;
		float hand_updown_rot_center = ToRadian(30);



		float jump_power = 150;

		float turn_speed = 2;

	private:

		Quaternion rotate; //rotate��buffer
		Vector3 dir; //������buffer

		std::shared_ptr<Transfome> camera; //camera�ւ̃|�C���^

		bool is_jumping = false; //���W�����v���Ă��邩
		float coyote = 0.3f; //jump�̋��e����
		float move_timer = 0;

		Joint_base* catch_right_joint = nullptr; //�E�肪����ł���joint
		Joint_base* catch_left_joint = nullptr;	 //���肪����ł���joint

	private:
		//::: GO :::
		const int Human_gameobject_size = 11;
		union {
			Gameobject* Human_gameobjects[11] = { nullptr };
			struct {
				Gameobject* Head    ;
				Gameobject* Rsholder;
				Gameobject* Relbow  ;
				Gameobject* Lsholder;
				Gameobject* Lelbow  ;
				Gameobject* Body    ;
				Gameobject* Waist   ;
				Gameobject* Rleg    ;
				Gameobject* Rfoot   ;
				Gameobject* Lleg    ;
				Gameobject* Lfoot   ;
			};
		};

		//::: collider :::
		const int Human_collider_size = 11;
		union {
			Collider* Human_colliders[11] = { nullptr };
			struct {
				Collider* Head_collider;
				Collider* Rsholder_collider;
				Collider* Relbow_collider;
				Collider* Lsholder_collider;
				Collider* Lelbow_collider;
				Collider* Body_collider;
				Collider* Waist_collider;
				Collider* Rleg_collider;
				Collider* Rfoot_collider;
				Collider* Lleg_collider;
				Collider* Lfoot_collider;
			};
		};
		Collider* onground_collider = nullptr;




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
			onground_collider = l_Waist_capsule_collider;

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
		void awake() override;

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start()override;

		// ���t���[���Ă΂��X�V����
		void update()override;

		void Update_hierarchy() override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable()override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable()override;

	};

}