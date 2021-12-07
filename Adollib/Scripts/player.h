#pragma once
#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"
#include "../Adollib/Scripts/Physics/joint.h"
#include "../Adollib/Scripts/Physics/ray.h"

namespace Adollib
{

	class Stage_manager;
	class Camera;
	class Input_changer;

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

		float jump_y_power = 150;
		float jump_front_power = 150;

		float turn_speed = 200;

	private:
		Quaternion rotate; //rotate��buffer
		Vector3 dir; //������buffer

		// �͊֌W
		bool is_gunyatto = false; //true�̎��A�̂����ɂ���Ƃ���
		float gunyatto_pow = 1; //�̂��N�����͂Ɋ|����l
		float hand_gunyatto_pow = 1; //�r�̂��݂Ȃǂ̏�񂩂狁��gunyatto_pow�ւ�buffer

		// ���݊֌W
		Joint_base* catch_right_joint = nullptr; //�E�肪����ł���joint
		Joint_base* catch_left_joint = nullptr;	 //���肪����ł���joint
		bool is_maked_right_joint = false; //����ł������̂������ŗ��ꂽ�Ƃ��Astate�݂̂��Ƒ����Ɏ��̂��̂�����ł��܂� ���Ȃ̂�bool�ŊǗ�
		bool is_maked_left_joint = false;
		BallJoint* Rhand_joint = nullptr; //elbow��hand���Ȃ�joint
		BallJoint* Lhand_joint = nullptr; //elbow��hand���Ȃ�joint
		float Rhand_joint_ylength_default = 0; //����ł���Ƃ�hand��joint��anchor��L�΂����� �����l��ۑ�
		float Lhand_joint_ylength_default = 0;

		// �ڒn������s��(sphere_cast)
		Collider* onground_collider = nullptr; //�����Ă���collider���
		Ray::Raycast_struct onground_ray_data; //onground�Ŏg����data raymin�ȂǓ����Ă���
		Vector3 onground_contactpoint; //onground�̏Փ˓_
		const float onground_dis = 2.0f; //raymin������ȉ��Ȃ痧���Ă���
		const float onground_radius = 0.6f; //sphere_cast�̔��a

		// jump
		bool is_jumping = false; //���W�����v���Ă��邩
		float coyote = 0.3f; //jump�̋��e����

		// ��
		float move_timer = 0; //���̉�]�����߂邽�߂� ���͎��Ԃ�ۑ�

		// respown�֌W
		Collider* check_onplayer_coll = nullptr; //respown�̐ڒn������s��Collider
		float respown_timer = 0; //respown�����p��timer >0�̎� check_respown()��Player::update��return (���͂��󂯕t���Ȃ��Agunyatto����)

	private:
		Stage_manager* stage_manager = nullptr; //stage_manager�ւ̃|�C���^
		Camera* camera = nullptr; //camera�ւ̃|�C���^
		Input_changer* input_changer = nullptr; //���g�̕ێ�����input_changer�ւ̃|�C���^

	private:
		//::: ���t���[���Ăяo��
		bool check_respown(); //respown����
		void update_onground(); //onground_collider���X�V
		void update_gnyat_pow(); //�̂��N�����͂̍X�V
		void reach_out_hands(); //���L�΂�
		void catch_things(); //��������
		void push_waist_for_stand(); //ray���΂��č��𗧂�����
		void linear_move(); //�ړ�
		void angula_move(); //��]
		void accume_move_dir(); //�ړ��������v�Z
		void add_pow_for_stand(); //���悤�ɗ͂�������
		void move_legs(); //���𓮂���
		void make_jump(); //jump������
		//:::

		void turn_gunyatto_dir();
		void set_default_drag();

	public:
		void delete_catchjoint(); //"��������"joint���폜����
		void respown(); //respown����


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

		//::: dafault_drag :::
		union {
			float Human_default_drags[Human_collider_size] = { 0 };
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
		Collider* l_Lfoot_collider
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

			Head		= Head_collider		->gameobject;
			Lsholder	= Lsholder_collider	->gameobject;
			Lelbow		= Lelbow_collider	->gameobject;
			Lhand		= Lhand_collider	->gameobject;
			Rsholder	= Rsholder_collider	->gameobject;
			Relbow		= Relbow_collider	->gameobject;
			Rhand		= Rhand_collider	->gameobject;
			Body		= Body_collider		->gameobject;
			Waist		= Waist_collider	->gameobject;
			Rleg		= Rleg_collider		->gameobject;
			Rfoot		= Rfoot_collider	->gameobject;
			Lleg		= Lleg_collider		->gameobject;
			Lfoot		= Lfoot_collider	->gameobject;

			for (int i = 0; i < Human_collider_size; i++) {
				Human_default_drags[i] = Human_colliders[i]->physics_data.drag;
			}
		}

		void set_player_joints(
			BallJoint* l_Lhand_joint,
			BallJoint* l_Rhand_joint
		) {
			Lhand_joint = l_Lhand_joint;
			Rhand_joint = l_Rhand_joint;
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