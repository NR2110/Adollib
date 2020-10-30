#pragma once
#include "../Math/math.h"

#include "mesh.h"
#include "../Object/gameobject_camera.h"
#include "../Object/gameobject.h"

namespace Adollib {
	class FrustumCulling {

		static Vector3 line[4];
		static float nearZ;
		static float farZ;

		static Vector3 camera_pos;
		static Quaternion camera_orient;

		static Vector3 obj_pos;
		static Quaternion obj_orient;
		static Vector3 obj_scale;

		static Vector3 offset_pos; //obj‚ğcamera‚ÌÀ•WŒn‚É
		static Quaternion offset_orient; //obj‚ğcamera‚ÌÀ•WŒn‚É

	public:
		static void update_frustum(const Camera* camera);
		static void update_obj(const Gameobject* GO);

		static void frustum_culling_init();

		static bool frustum_culling(const Mesh::mesh* mesh);
	};
}