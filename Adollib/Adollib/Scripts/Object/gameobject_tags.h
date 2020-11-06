#pragma once
#include "../Math/math.h"
namespace Adollib {
	struct GO_tag {
		static const unsigned int None = (1 << 0);

		static const unsigned int Sphere = (1 << 1);
		static const unsigned int Box = (1 << 2);
		static const unsigned int Plane = (1 << 3);
		static const unsigned int Mesh = (1 << 4);
		static const unsigned int Cylinder = (1 << 5);
		static const unsigned int FBX = (1 << 6);
		static const unsigned int Sprite = (1 << 7);

		
		static const unsigned int Player = (1 << 10);
		static const unsigned int Sage = (1 << 11);

		static const unsigned int Tags_max = (1 << 31);
	};
}
