#include "Adollib.h"
#include "time.h"
namespace Adollib {

	int Al_Global::SCREEN_WIDTH = 1280;
	int Al_Global::SCREEN_HEIGHT = 720;

	bool Al_Global::render_collider_flag = false;

	float Al_Global::elapsed_time = 0;

	float get_P_by_f(float M, float F) {
		int i = M / F;
		return M - i * F;
	};

	vector4 Al_Global::get_gaming(int time, float change_time, float min, float max ) {
		vector4 color;
		float T = time;
		color.w = 1;

		color.x = fabsf(get_P_by_f(T, change_time * 2) - change_time) / change_time;

		T += change_time / 3.0f;
		color.y = fabsf(get_P_by_f(T, change_time * 2) - change_time) / change_time;

		T += change_time / 3.0f;
		color.z = fabsf(get_P_by_f(T, change_time * 2) - change_time) / change_time;

		color = color * 3 - vector4(1, 1, 1, 0);
		color.y = 1 - color.y;

		color *= (max - min);
		color += vector4(min, min, min, 0);

		return color;

	}
}