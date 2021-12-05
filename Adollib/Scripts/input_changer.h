#pragma once

#include "../Adollib/Scripts/Object/component.h"

namespace Adollib
{
	class Input_changer : public Component
	{
	public:
		int pad_num = 0;

		Vector3 dir; //��������

		bool is_Rarm_state = false;
		bool is_Larm_state = false;
		bool is_jump_trigger = false;
		bool is_gunyatto_state = false;
		bool is_respown_trigger = false;

		// cursol�֌W
		Vector2 cursol_move; //cursol�̈ړ���
		Vector2 cursol_pos_save; //cursol�̑O�̍��W
		float rotate_speed = 0.5f;


	private:
		//const Key move_front = Key::W;
		//const Key move_left =  Key::A;
		//const Key move_back =  Key::S;
		//const Key move_right = Key::D;

		//const Mouse Rarm_up = Mouse::RBUTTON;
		//const Mouse Larm_up = Mouse::LBUTTON;

		//const GamePad Rarm_up_pad = GamePad::RSHOULDER;
		//const GamePad Larm_up_pad = GamePad::RSHOULDER;

	private:
		bool is_lock_cursol = false;
		bool is_show_cursol = false;

	public:
		void set_showcursol(bool is_show);
		bool get_showcursol() { return is_show_cursol; };

	public:
		void awake() override;

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start() override;

		// ���t���[���Ă΂��X�V����
		void update() override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable() override {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable() override {};

	};

}