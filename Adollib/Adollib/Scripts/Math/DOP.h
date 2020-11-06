#pragma once
#include "vector.h"

namespace Adollib {
	namespace DOP {
		static const int DOP_size = 14 / 2;

		//::::::::::::::::::
		//Boardphase用のDOP_6
		// x : (1,0,0)
		// y : (0,1,0)
		// z : (0,0,1)
		// a : (+0.3,0.3,+0.3)
		// b : (-0.3,0.3,+0.3)
		// c : (+0.3,0.3,-0.3)
		// d : (-0.3,0.3,-0.3)
		//:::::::::::::::::::
		static const Vector3 DOP_14_axis[DOP_size] = {
		Vector3(1,0,0),
		Vector3(0,1,0),
		Vector3(0,0,1),
		Vector3(+0.577350259,0.577350259,+0.577350259),
		Vector3(-0.577350259,0.577350259,+0.577350259),
		Vector3(+0.577350259,0.577350259,-0.577350259),
		Vector3(-0.577350259,0.577350259,-0.577350259)
		};
		struct DOP_14 {
			//中心座標
			Vector3 pos;

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
			float max[DOP_size] = { 0 };

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
			float min[DOP_size] = { 0 };
		};

		struct AABB {
			//中心座標
			Vector3 pos;

			Vector3 max;
			Vector3 min;
		};
	}
}