#pragma once

#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Physics/collider.h"

namespace Adollib
{
	class Camera : public Component
	{
	private:
		bool follow_player = false;
		float dis = 20; //����
		float dis_buffer = 20; //������buffer easing�p
		float ray_timer = 0; //�J������Ray�̉e���ł����������Ȃ��悤�� Ray�̉e�����󂯂Ă��炵�΂炭�͎����I�ɖ߂�Ȃ��悤��
		Vector3 pos_buffer; //���W��easing�p
		float pos_slop = 1; //�J�����̍��W�̋��e�덷

		bool is_lock_cursol = false;
		Vector2 max_rotate = Vector2(-50, 60); //�J�����̏㉺�̍ő�p�x
		//Vector2 max_rotate_moving = Vector2(-15, 15); //�ړ����̃J�����̏㉺�̍ő�p�x
		float rotate_min_pow_bufer = 1;
		float rotate_min_pow = 0.3f;
		//Vector2 max_rotate_buffer = Vector2(-50, 60); //�J�����̍ő�p�x�̃C�[�W���O��buffer

		std::shared_ptr<Transform> player;
		Vector3 cursol_pos_save;
		Vector3 rotate_vec_save;

		Quaternion camera_rot_goal; //Easing�p

	public:
		Quaternion camera_rot; //camera�̌���

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