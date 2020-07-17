#pragma once

#include "vector.h"
#include "matrix.h"
#include "quaternion.h"

namespace Adollib {

	static float ToRadian(float angle) {
		return (float)angle * (3.141592654f / 180.0f);
	}

	static float ToAngle(float radian) {
		return (float)radian * 180 / 3.1415926535f;
	}


	static const vector3 DOP_7_axis[7] = {
		vector3(1,0,0),
		vector3(0,1,0),
		vector3(0,0,1),
		vector3(+0.3333333,0.3333333,+0.3333333),
		vector3(-0.3333333,0.3333333,+0.3333333),
		vector3(+0.3333333,0.3333333,-0.3333333),
		vector3(-0.3333333,0.3333333,-0.3333333)
	};

	static const int DOP_size = 7;
	struct DOP_7 {
		//中心座標
		vector3 pos;

		//::::::::::::::::::
		//Boardphase用のDOP_6
		// x : (1,0,0)
		// y : (0,1,0)
		// z : (0,0,1)
		// a : (1,1,1)
		// b : (-1,1,1)
		// c : (1,1,-1)
		// d : (-1,1,-1)
		//:::::::::::::::::::
		float max[7];

		//::::::::::::::::::
		//Boardphase用のDOP_6
		// x : (1,0,0)
		// y : (0,1,0)
		// z : (0,0,1)
		// a : (1,1,1)
		// b : (-1,1,1)
		// c : (1,1,-1)
		// d : (-1,1,-1)
		//:::::::::::::::::::
		float min[7];
	};

	struct AABB {
		//中心座標
		vector3 pos;

		vector3 max;
		vector3 min;
	};






}