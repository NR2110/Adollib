#pragma once
#include "vector.h"

namespace Adollib {

	static const int DOP_size = 7;

	static const vector3 DOP_7_axis[DOP_size] = {
	vector3(1,0,0),
	vector3(0,1,0),
	vector3(0,0,1),
	vector3(+0.3333333,0.3333333,+0.3333333),
	vector3(-0.3333333,0.3333333,+0.3333333),
	vector3(+0.3333333,0.3333333,-0.3333333),
	vector3(-0.3333333,0.3333333,-0.3333333)
	};
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
		float max[DOP_size];

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
		float min[DOP_size];
	};

	struct AABB {
		//中心座標
		vector3 pos;

		vector3 max;
		vector3 min;
	};
}