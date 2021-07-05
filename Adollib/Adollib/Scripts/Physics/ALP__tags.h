#pragma once

namespace Adollib {
	typedef unsigned int Collider_tagbit;

	struct Collider_tags {
		static const Collider_tagbit None = (1 << 0);

		static const Collider_tagbit Sphere =		(1 << 1);
		static const Collider_tagbit Box =			(1 << 2);
		static const Collider_tagbit Plane =		(1 << 3);
		static const Collider_tagbit Mesh =		(1 << 4);
		static const Collider_tagbit Cylinder =	(1 << 5);
		static const Collider_tagbit FBX =			(1 << 6);

		//static const unsigned int Cylinder = (1 << 5);
		//static const unsigned int Cylinder = (1 << 5);
		static const Collider_tagbit croth = (1 << 7);
		static const Collider_tagbit Tags_max = (1 << 31);
	};



}