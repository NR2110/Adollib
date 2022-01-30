#pragma once
#include <DirectXMath.h>
#include "../Math/math.h"

namespace Adollib {
	struct Frustum_data {
		Vector3 normals[6] = { Vector3(0) }; //äeïΩñ ÇÃñ@ê¸
		float distances[6] = { 0 }; //äeïΩñ ÇÃãóó£

		static Frustum_data create_frustum_data(
			const Vector3& position,
			const Quaternion& orientation,
			const float& nearZ,
			const float& farZ,
			const float& fov,
			const float& aspect
			) {

			Frustum_data data;

			const float x_rad = ToRadian(fov * 0.5f);
			const float y_rad = PI_inv2 - atanf(1 / (aspect * tanf(x_rad)));
			const Quaternion rot_y_p = quaternion_axis_radian(Vector3(0, 1, 0), +y_rad);
			const Quaternion rot_y_d = quaternion_axis_radian(Vector3(0, 1, 0), -y_rad);
			const Quaternion rot_x_p = quaternion_axis_radian(Vector3(1, 0, 0), +x_rad);
			const Quaternion rot_x_d = quaternion_axis_radian(Vector3(1, 0, 0), -x_rad);

			// near
			data.normals[0] = Vector3(0, 0, +1);
			data.normals[0] = vector3_quatrotate(data.normals[0], orientation);
			data.distances[0] = vector3_dot(data.normals[0], position) + nearZ;

			// far
			data.normals[1] = Vector3(0, 0, -1);
			data.normals[1] = vector3_quatrotate(data.normals[1], orientation);
			data.distances[1] = vector3_dot(data.normals[1], position) - farZ;

			// left
			data.normals[2] = Vector3(+1, 0, 0);
			data.normals[2] = vector3_quatrotate(data.normals[2], rot_y_d * orientation);
			data.distances[2] = vector3_dot(data.normals[2], position);

			// right
			data.normals[3] = Vector3(-1, 0, 0);
			data.normals[3] = vector3_quatrotate(data.normals[3], rot_y_p * orientation);
			data.distances[3] = vector3_dot(data.normals[3], position);

			// bottom
			data.normals[4] = Vector3(0, +1, 0);
			data.normals[4] = vector3_quatrotate(data.normals[4], rot_x_p * orientation);
			data.distances[4] = vector3_dot(data.normals[4], position);

			// top
			data.normals[5] = Vector3(0, -1, 0);
			data.normals[5] = vector3_quatrotate(data.normals[5], rot_x_d * orientation);
			data.distances[5] = vector3_dot(data.normals[5], position);

			return data;
		};

		static Frustum_data create_frustum_data(
			const Vector3& position,
			const Vector3& direction,
			const float& nearZ,
			const float& farZ,
			const float& fov,
			const float& aspect
		) {

			Frustum_data data;

			Vector3 right_vec = vector3_cross(Vector3(0, 1, 0), direction);
			if (right_vec.norm() < FLT_EPSILON)right_vec = Vector3(0, 1, 0);
			right_vec = right_vec.unit_vect();
			Vector3 up_vec = vector3_cross(direction, right_vec).unit_vect();

			const float x_rad = ToRadian(fov * 0.5f);
			const float y_rad = PI_inv2 - atanf(1 / (aspect * tanf(x_rad)));
			const Quaternion rot_y_p = quaternion_axis_radian(up_vec, +y_rad);
			const Quaternion rot_y_d = quaternion_axis_radian(up_vec, -y_rad);
			const Quaternion rot_x_p = quaternion_axis_radian(right_vec, +x_rad);
			const Quaternion rot_x_d = quaternion_axis_radian(right_vec, -x_rad);

			// near
			data.normals[0] = +direction;
			data.distances[0] = vector3_dot(data.normals[0], position) + nearZ;

			// far
			data.normals[1] = -direction;
			data.distances[1] = vector3_dot(data.normals[1], position) - farZ;

			// left
			data.normals[2] = +right_vec;
			data.normals[2] = vector3_quatrotate(data.normals[2], rot_y_d);
			data.distances[2] = vector3_dot(data.normals[2], position);

			// right
			data.normals[3] = -right_vec;
			data.normals[3] = vector3_quatrotate(data.normals[3], rot_y_p);
			data.distances[3] = vector3_dot(data.normals[3], position);

			// bottom
			data.normals[4] = +up_vec;
			data.normals[4] = vector3_quatrotate(data.normals[4], rot_x_p);
			data.distances[4] = vector3_dot(data.normals[4], position);

			// top
			data.normals[5] = -up_vec;
			data.normals[5] = vector3_quatrotate(data.normals[5], rot_x_d);
			data.distances[5] = vector3_dot(data.normals[5], position);

			return data;
		};
	};
}