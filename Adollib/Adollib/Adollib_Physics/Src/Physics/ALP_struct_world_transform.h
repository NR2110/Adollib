#pragma once

#include "../Math/vector.h"
#include "../Math/quaternion.h"

#include <list>
#include <memory>

namespace Adollib {
	namespace Physics_function {

		// �}���`�X���b�h�p transform��world����ۑ����Ďg�p����
		struct world_trans {
			Vector3 position = Vector3(0, 0, 0);
			Quaternion orientation = Quaternion(1, 0, 0, 0);
			Vector3 scale = Vector3(1, 1, 1);

			Quaternion parent_orientate_inv = Quaternion(1, 0, 0, 0);
		};

	}
}