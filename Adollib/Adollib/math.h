#pragma once
#include "vector.h"
#include "matrix.h"
#include "quaternion.h"

namespace Adollib {

	static vector3 DOP_6_axis[6] = {
		vector3(1,0,0),
		vector3(0,1,0),
		vector3(0,0,1),
		vector3(1.41421356,1.41421356,0),
		vector3(0,1.41421356,1.41421356),
		vector3(1.41421356,0,1.41421356),
	};

	struct DOP_6 {
		//���S���W
		vector3 pos;

		//::::::::::::::::::
		//Boardphase�p��DOP_6
		// x : (1,0,0)
		// y : (0,1,0)
		// z : (0,0,1)
		// xy : (1,1,0)
		// yz : (0,1,1)
		// zx : (1,0,1)
		//:::::::::::::::::::
		float max[6];

		//::::::::::::::::::
		//Boardphase�p��DOP_6
		// x : (1,0,0)
		// y : (0,1,0)
		// z : (0,0,1)
		// xy : (1,1,0)
		// yz : (0,1,1)
		// zx : (1,0,1)
		//:::::::::::::::::::
		float min[6];
	};

	struct AABB {
		//���S���W
		vector3 pos;

		vector3 max;
		vector3 min;
	};






}