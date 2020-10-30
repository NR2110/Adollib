#pragma once

#include "../Math/vector.h"
namespace Adollib {

	//��^���������̖��O��W��
	class Al_Global {
	public:
		static int SCREEN_WIDTH;
		static int SCREEN_HEIGHT;

		static bool render_collider_flag; //collider��\�����邩�̃t���O

		static float second_per_frame;
		static float second_per_game;

		static float base_Coefficient_of_friction; //���̖̂��C�͂̏����l
		static float base_resituation; //
		static float gravity;

		//::::::::::::::::::::
		// time : 
		// change_time : �F���ω�����Ԋu
		// min : �F�̍ŏ��l(0 �` 1)
		// max : �F�̍ő�l(0 �` 1)
		//::::::::::::::::::::
		static Vector4 get_gaming(int time, float change_time = 600, float min = 0.0, float max = 1.0);
	};






}


