#pragma once
#include "../Math/math.h"
namespace Adollib {
	struct GO_tag {
		const static u_int None = (1 << 0);

		const static u_int Player = (1 << 1);

		const static u_int GO_tag_max = (1 << 32);
	};
}
