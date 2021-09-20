#pragma once

namespace Adollib {
	typedef unsigned int Collider_tagbit;

	struct Collider_tags {
		static const Collider_tagbit None = (1 << 0);

		static const Collider_tagbit Sphere =		(1 << 1);
		static const Collider_tagbit Box =			(1 << 2);
		static const Collider_tagbit Plane =		(1 << 3);
		static const Collider_tagbit Mesh =		(1 << 4);
		static const Collider_tagbit Capsule =	(1 << 5);
		static const Collider_tagbit Cylinder =	(1 << 6);
		static const Collider_tagbit FBX =			(1 << 7);


		static const Collider_tagbit croth = (1 << 10);

		static const Collider_tagbit Human = (1 << 11);

		static const Collider_tagbit Stage = (1 << 12);
		static const Collider_tagbit Jumpable_Stage = (1 << 13);
		static const Collider_tagbit Having_Stage = (1 << 14);
		static const Collider_tagbit Static_Stage = (1 << 15);
		static const Collider_tagbit Kinematic_Stage = (1 << 16);
		static const Collider_tagbit Caera_not_sunk_Stage = (1 << 17);



		static const Collider_tagbit Tags_max = (1 << 31);
	};



}