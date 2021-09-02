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
		float hand_camera_max_rot = ToRadian(45);
		float hand_camera_rot_pow = 2;
		float hand_camera_rot_center = ToRadian(30);



		float jump_power = 150;

		float turn_speed = 2;

	private:

		Quaternion rotate; //rotate��buffer
		Vector3 dir; //������buffer

		// static�Ȃ��̂����ނƂ��A���ʂ�ς��邽�ߕۑ�����
		float shlder_mass = 0;
		float elbow_mass = 0;
		float hand_mass = 0;

		// ����collider���ڒn���Ă����ꍇ �����Ƃ��ł��� ���΂炭����collider���ڒn���Ă��Ȃ��� ���ɂႮ�ɂ�ɂȂ�
		Collider* check_standable_collider = nullptr;
		float check_standable_collider_timer = 0;

		// �ڒn������s��
		Collider* onground_collider = nullptr;
		Gameobject* onground_collider_GO = nullptr; //�������邽�߂�sphere�̍��W�Ɏ����Ă���K�v�����邽�� �ϐ��Ŏ����ĊǗ�

		// waist_collider�ɃA�^�b�`����Ă��� waist���������邽�߂�sphere �ꍇ�ɂ����anchor�̍��W��ύX������
		Sphere* waist_sphere = nullptr;
		float Waist_sphere_length = 0;

		bool is_jumping = false; //���W�����v���Ă��邩
		float coyote = 0.3f; //jump�̋��e����

		float move_timer = 0; //���̉�]�����߂邽�߂� ���͎��Ԃ�ۑ�

		Joint_base* catch_right_joint = nullptr; //�E�肪����ł���joint
		Joint_base* catch_left_joint = nullptr;	 //���肪����ł���joint


	private:
		std::shared_ptr<Transfome> camera; //camera�ւ̃|�C���^

	private:
		//::: GO :::
		static constexpr int Human_gameobject_size = 13;
		union {
			Gameobject* Human_gameobjects[Human_gameobject_size] = { nullptr };
			struct {
				Gameobject* Head    ;
				Gameobject* Lsholder;
				Gameobject* Lelbow  ;
				Gameobject* Lhand   ;
				Gameobject* Rsholder;
				Gameobject* Relbow  ;
				Gameobject* Rhand	;
				Gameobject* Body    ;
				Gameobject* Waist   ;
				Gameobject* Rleg    ;
				Gameobject* Rfoot   ;
				Gameobject* Lleg    ;
				Gameobject* Lfoot   ;
			};
		};

		//::: collider :::
		static constexpr int Human_collider_size = 13;
		union {
			Collider* Human_colliders[Human_collider_size] = { nullptr };
			struct {
				Collider* Head_collider;
				Collider* Lsholder_collider;
				Collider* Lelbow_collider;
				Collider* Lhand_collider;
				Collider* Rsholder_collider;
				Collider* Relbow_collider;
				Collider* Rhand_collider;
				Collider* Body_collider;
				Collider* Waist_collider;
				Collider* Rleg_collider;
				Collider* Rfoot_collider;
				Collider* Lleg_collider;
				Collider* Lfoot_collider;
			};
		};


	public:
		void set_player_colliders(
		Collider* l_Head_collider		,
		Collider* l_Lsholder_collider	,
		Collider* l_Lelbow_collider		,
		Collider* l_Lhand_collider		,
		Collider* l_Rsholder_collider	,
		Collider* l_Relbow_collider		,
		Collider* l_Rhand_collider		,
		Collider* l_Body_collider		,
		Collider* l_Waist_collider		,
		Collider* l_Rleg_collider		,
		Collider* l_Rfoot_collider		,
		Collider* l_Lleg_collider		,
		Collider* l_Lfoot_collider		,
		  Sphere* l_waist_sphere
		) {
			Head_collider		= l_Head_collider;
			Lsholder_collider	= l_Lsholder_collider;
			Lelbow_collider		= l_Lelbow_collider;
			Lhand_collider		= l_Lhand_collider;
			Rsholder_collider	= l_Rsholder_collider;
			Relbow_collider		= l_Relbow_collider;
			Rhand_collider		= l_Rhand_collider;
			Body_collider		= l_Body_collider;
			Waist_collider		= l_Waist_collider;
			Rleg_collider		= l_Rleg_collider;
			Rfoot_collider		= l_Rfoot_collider;
			Lleg_collider		= l_Lleg_collider;
			Lfoot_collider		= l_Lfoot_collider;
			waist_sphere		= l_waist_sphere;

			Head		=Head_collider		->gameobject;
			Lsholder	=Lsholder_collider	->gameobject;
			Lelbow		=Lelbow_collider	->gameobject;
			Lhand		=Lhand_collider	    ->gameobject;
			Rsholder	=Rsholder_collider	->gameobject;
			Relbow		=Relbow_collider	->gameobject;
			Rhand		=Rhand_collider	    ->gameobject;
			Body		=Body_collider		->gameobject;
			Waist		=Waist_collider		->gameobject;
			Rleg		=Rleg_collider		->gameobject;
			Rfoot		=Rfoot_collider		->gameobject;
			Lleg		=Lleg_collider		->gameobject;
			Lfoot		=Lfoot_collider		->gameobject;

			shlder_mass = Lsholder_collider->physics_data.inertial_mass;
			elbow_mass = Lelbow_collider->physics_data.inertial_mass;
			hand_mass =  Lhand_collider->physics_data.inertial_mass;

			Waist_sphere_length = l_waist_sphere->local_position.y;
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