#pragma once

#include "vector.h"
namespace Adollib {

	//絶賛いい感じの名前募集中
	class Al_Global {
	public:
		static int SCREEN_WIDTH;
		static int SCREEN_HEIGHT;

		static bool render_collider_flag;

		static float elapsed_time;

		static float Coefficient_of_friction;
		static float resituation;

		//::::::::::::::::::::
		// time : 
		// change_time : 色が変化する間隔
		// min : 色の最小値(0 ～ 1)
		// max : 色の最大値(0 ～ 1)
		//::::::::::::::::::::
		static vector4 get_gaming(int time, float change_time = 600, float min = 0.0, float max = 1.0);
	};






}


