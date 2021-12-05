#pragma once

#include "../Adollib/Scripts/Object/component.h"

namespace Adollib
{
	class Input_changer : public Component
	{
	public:
		int pad_num = 0;

		Vector3 dir; //��������
		Vector2 cursol_move; //cursol�̈ړ���

		bool is_Rarm_state = false;
		bool is_Larm_state = false;

		bool is_respown_trigger = false;

		Quaternion camera_rot; //camera�̌���


	private:
		//const Key move_front = Key::W;
		//const Key move_left =  Key::A;
		//const Key move_back =  Key::S;
		//const Key move_right = Key::D;

		//const Mouse Rarm_up = Mouse::RBUTTON;
		//const Mouse Larm_up = Mouse::LBUTTON;

		//const GamePad Rarm_up_pad = GamePad::RSHOULDER;
		//const GamePad Larm_up_pad = GamePad::RSHOULDER;



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