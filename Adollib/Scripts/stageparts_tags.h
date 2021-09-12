#pragma once

namespace Adollib {
	namespace Stage_parts {
		typedef unsigned int Stageparts_tagbit;

		struct Stageparts_tags {
			static const Stageparts_tagbit None = 0;
			static const Stageparts_tagbit Flag_0 = (1 << 0);
			static const Stageparts_tagbit Flag_1 = (1 << 1);
			static const Stageparts_tagbit Flag_2 = (1 << 2);
			static const Stageparts_tagbit Flag_3 = (1 << 3);
			static const Stageparts_tagbit Flag_4 = (1 << 4);
			static const Stageparts_tagbit Flag_5 = (1 << 5);
			static const Stageparts_tagbit Flag_6 = (1 << 6);
		};


	}
}