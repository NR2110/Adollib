#pragma once


namespace Adollib {

	static float ToRadian(float angle) {
		return (float)angle * (3.141592654f / 180.0f);
	}

	static float ToAngle(float radian) {
		return (float)radian * 180 / 3.1415926535f;
	}

}
