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
		static const Collider_tagbit Human_rope = (1 << 12);
		static const Collider_tagbit Player00 = (1 << 15);
		static const Collider_tagbit Player01 = (1 << 16);
		static const Collider_tagbit Player02 = (1 << 17);
		static const Collider_tagbit Player03 = (1 << 18);

		static const Collider_tagbit Stage = (1 << 20);
		//static const Collider_tagbit Jumpable_Stage = (1 << 13);
		//static const Collider_tagbit Having_Stage = (1 << 14);
		static const Collider_tagbit Static_Stage = (1 << 21);
		static const Collider_tagbit Kinematic_Stage = (1 << 22);
		static const Collider_tagbit Caera_not_sunk_Stage = (1 << 23);
		static const Collider_tagbit Goal_stage = (1 << 24);

		static const Collider_tagbit Rope_event = (1 << 25);
		static const Collider_tagbit Catch_able_easy = (1 << 26);

		static const Collider_tagbit Title = (1 << 28);


		static const Collider_tagbit Tags_max = UINT_MAX;
	};



}