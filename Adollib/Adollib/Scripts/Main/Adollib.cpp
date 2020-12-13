#include "Adollib.h"
#include "time.h"
namespace Adollib {

	int Al_Global::SCREEN_WIDTH = 1280;
	int Al_Global::SCREEN_HEIGHT = 720;

	bool Al_Global::render_collider_flag = true;

	float Al_Global::second_per_frame = 0;
	float Al_Global::second_per_game = 0;

	float Al_Global::base_Coefficient_of_friction = 0.6f;
	float Al_Global::base_resituation = 0.2f;

	float get_P_by_f(float M, float F) {
		int i = (int)(M / F);
		return M - i * F;
	};

	Vector3 Al_Global::get_gaming(int time, float change_time, float min, float max ) {
		Vector3 color;
		float T = time;

		color.x = fabsf(get_P_by_f(T, change_time * 2) - change_time) / change_time;

		T += change_time * 0.33333333f;
		color.y = fabsf(get_P_by_f(T, change_time * 2) - change_time) / change_time;

		T += change_time * 0.3333333f;
		color.z = fabsf(get_P_by_f(T, change_time * 2) - change_time) / change_time;

		color = color * 3 - Vector3(1, 1, 1);
		color.y = 1 - color.y;

		color *= (max - min);
		color += Vector3(min, min, min);

		return color;

	}
}