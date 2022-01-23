#pragma once

#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Physics/collider.h"

namespace Adollib
{

	class Fall_object_manager : public Component
	{
	private:
		Gameobject* fall_object_pearent = nullptr;

		float timer = 0;
		float fall_start_time = 1;
		float fall_cool_time = 0.3f;

		float start_pos_y; //�������镨�̂̏���y���W
		float start_pos_x[4]; //�������镨�̂̏���x���W
		float start_pos_y_randam_size = 2;
		float start_pos_x_randam_size = 2;


		void set_box(const char* name, Vector3 position, Vector3 rotate, Vector3 scale, Vector3 color, Gameobject* pearent = nullptr);
		void set_sphere(const char* name, Vector3 position, float r, Vector3 color, Gameobject* pearent = nullptr);

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