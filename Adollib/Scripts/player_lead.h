#pragma once
#include "player_base.h"
#include "stage_base.h"

#include "camera_s.h"

namespace Adollib
{
	const int lay_max_up = 10;
	const int lay_max_tan = 30;

	//const int lay_max_up = 50;
	//const int lay_max_tan = 50;

	const int lays_size = 64;

	class Playerlead_s : public Player_base
	{
	private:

		vector3 lay_dirs[64];
		Gameobject* Debobb[64];
		Gameobject* Debsph[64];


	public:
		Camera* camera;
		Sphere* sphere;
		std::list<Stage_base*>* stages;

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
