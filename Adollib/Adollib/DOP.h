#pragma once
#include "vector.h"

namespace Adollib {
	namespace DOP {
		static const int DOP_size = 14 / 2;

		//::::::::::::::::::
		//Boardphase�p��DOP_6
		// x : (1,0,0)
		// y : (0,1,0)
		// z : (0,0,1)
		// a : (1,1,1)
		// b : (-1,1,1)
		// c : (1,1,-1)
		// d : (-1,1,-1)
		//:::::::::::::::::::
		static const vector3 DOP_14_axis[DOP_size] = {
		vector3(1,0,0),
		vector3(0,1,0),
		vector3(0,0,1),
		vector3(+0.3333333,0.3333333,+0.3333333),
		vector3(-0.3333333,0.3333333,+0.3333333),
		vector3(+0.3333333,0.3333333,-0.3333333),
		vector3(-0.3333333,0.3333333,-0.3333333)
		};
		struct DOP_14 {
			//���S���W
			vector3 pos;

			//::::::::::::::::::
			//Boardphase�p��DOP_6
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
			//Boardphase�p��DOP_6
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
			//���S���W
			vector3 pos;

			vector3 max;
			vector3 min;
		};
	}
}