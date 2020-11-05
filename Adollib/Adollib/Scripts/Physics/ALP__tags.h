#pragma once

namespace Adollib {
	struct ALP_tags {
		const unsigned int none = (1 << 0);

		const unsigned int player = (1 << 1);
		const unsigned int stage = (1 << 2);

		const unsigned int tags_max = (1 << 32);
	};



}