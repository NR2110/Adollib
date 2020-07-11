#pragma once
#include "math.h"

#include "mesh.h"
#include "gameobject_camera.h"
#include "gameobject.h"

namespace Adollib {
	class FrustumCulling {

		static vector3 line[4];
		static float nearZ;
		static float farZ;

		static vector3 camera_pos;
		static quaternion camera_orient;

		static vector3 obj_pos;
		static quaternion obj_orient;
		static vector3 obj_scale;

		static vector3 offset_pos; //obj‚ğcamera‚ÌÀ•WŒn‚É
		static quaternion offset_orient; //obj‚ğcamera‚ÌÀ•WŒn‚É

	public:
		static void update_frustum(const Camera* camera);
		static void update_obj(const Gameobject* GO);

		static void frustum_culling_init();

		static bool frustum_culling(const Mesh::mesh* mesh);
	};
}