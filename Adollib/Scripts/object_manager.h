#pragma once
#include "../Adollib/component.h"
#include "../Adollib/rigit_body.h"

namespace Adollib
{

	class object_manager : public Component
	{
	private:


	public:
		void set_sphere(vector3 pos, float r, vector3 color = vector3(1, 1, 1));
		void set_box(vector3 pos, vector3 size, vector3 rotate, vector3 color = vector3(1, 1, 1));
		void set_plane(vector3 pos, vector3 normal, vector3 color = vector3(1, 1, 1));

		void set_fall_sphere(vector3 pos, float r, vector3 color = vector3(1, 1, 1));
		void set_fall_box(vector3 pos, vector3 size, vector3 rotate, vector3 color = vector3(1, 1, 1));
		void set_fall_plane(vector3 pos, vector3 normal, vector3 color = vector3(1, 1, 1));

		void set_nohit_plane(vector3 pos, vector3 normal, vector3 color = vector3(1, 1, 1));

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