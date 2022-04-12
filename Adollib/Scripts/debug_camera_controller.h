#pragma once

#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Physics/collider.h"

namespace Adollib
{
	class Input_changer;


	class Debug_camera_controller : public Component
	{
	public:

		Vector2 max_rotate = Vector2(-50, 60); //�J�����̏㉺�̍ő�p�x

		Vector3 rotate_vec_save;
		Vector2 cursol_pos_save;

		float rotate_speed = 0.5f;
		float linear_speed = 30;

	public:
		Quaternion camera_rot; //camera�̌���

	public:
		void awake() override;

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start() override;

		// ���t���[���Ă΂��X�V����
		void update() override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable() override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable() override;

	};

}