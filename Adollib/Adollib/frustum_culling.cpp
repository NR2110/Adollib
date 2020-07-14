#include "frustum_culling.h"

using namespace Adollib;

vector3 FrustumCulling::line[4];
float FrustumCulling::nearZ;
float FrustumCulling::farZ;

vector3 FrustumCulling::camera_pos;
quaternion FrustumCulling::camera_orient;

vector3 FrustumCulling::obj_pos;
quaternion FrustumCulling::obj_orient;
vector3 FrustumCulling::obj_scale;

vector3 FrustumCulling::offset_pos;
quaternion FrustumCulling::offset_orient;

void FrustumCulling::update_frustum(const Camera* camera) {

	vector3 L;
	//L.x = sinf(ToRadian(camera->fov));
	//L.y = L.x * (1.0f / camera->aspect);
	L.z = 1;

	L.y = sinf(ToRadian(camera->fov / 2.0f) * 1.2f);
	L.x = L.y * camera->aspect;

	line[0] = vector3(+L.x, +L.y, L.z);
	line[1] = vector3(+L.x, -L.y, L.z);
	line[2] = vector3(-L.x, -L.y, L.z);
	line[3] = vector3(-L.x, +L.y, L.z);

	nearZ = camera->nearZ;
	farZ = camera->farZ;

	camera_pos = camera->transform->position;
	camera_orient = camera->transform->orientation;
}

void FrustumCulling::update_obj(const Gameobject* GO) {
	obj_pos = GO->transform->position;
	obj_orient = GO->transform->orientation;
	obj_scale = GO->transform->scale;
}

void FrustumCulling::frustum_culling_init() {
	//offset_pos = camera_pos - obj_pos; //objからcamera座標系へのoffset
	//offset_orient = camera_orient * obj_orient.conjugate();

	offset_pos =  obj_pos - camera_pos ; //objからcamera座標系へのoffset
	offset_orient = obj_orient * camera_orient.conjugate();
}


bool FrustumCulling::frustum_culling(const Mesh::mesh* mesh) {

	//TODO : 視錐台カリング未完成
	return true;

	vector3 P_obj_cn;
	vector3 vertex[12];
	vector3 V_L[4], V_P[4];

	for (int i = 0; i < 4; i++) {
		V_L[i] = line[(4 + i - 1) % 4] - line[i];
	}
	//if (obj_scale.x > 10) return true;

	//max
	for (int axi = 0; axi < 6; axi++) {

		//objのローカル座標系での各軸の長さ
		P_obj_cn = DOP_6_axis[axi] * mesh->dop6.max[axi];

		//scaleの適応
		P_obj_cn = P_obj_cn * obj_scale;

		//座標と向きをcameraの座標系へ
		vertex[axi] = P_obj_cn = vector3_be_rotated_by_quaternion(P_obj_cn + offset_pos, offset_orient);

		//nearとfarの確認
		if (P_obj_cn.z < nearZ || P_obj_cn.z > farZ)
			continue;

		for (int i = 0; i < 4; i++) {
			V_P[i] = P_obj_cn - line[i] * P_obj_cn.z;
		}

		if (
			vector3_cross(V_L[0], V_P[0]).z >= 0 &&
			vector3_cross(V_L[1], V_P[1]).z >= 0 &&
			vector3_cross(V_L[2], V_P[2]).z >= 0 &&
			vector3_cross(V_L[3], V_P[3]).z >= 0 
			)
			return true;
	}

	//min
	for (int axi = 0; axi < 6; axi++) {

		//objのローカル座標系での各軸の長さ
		P_obj_cn = DOP_6_axis[axi] * mesh->dop6.min[axi];

		//scaleの適応
		P_obj_cn = P_obj_cn * obj_scale;

		//座標と向きをcameraの座標系へ
		vertex[6+axi] = P_obj_cn = vector3_be_rotated_by_quaternion(P_obj_cn + offset_pos, offset_orient);

		//nearとfarの確認
		if (P_obj_cn.z < nearZ || P_obj_cn.z > farZ)
			continue;

		for (int i = 0; i < 4; i++) {
			V_P[i] = P_obj_cn - line[i] * P_obj_cn.z;
		}

		if (
			vector3_cross(V_L[0], V_P[0]).z >= 0 &&
			vector3_cross(V_L[1], V_P[1]).z >= 0 &&
			vector3_cross(V_L[2], V_P[2]).z >= 0 &&
			vector3_cross(V_L[3], V_P[3]).z >= 0
			)
			return true;
	}

	{
		vector3 p[4];
		float x_l[2], y_l[2], z_l[2], Len;

		x_l[0] = y_l[0] = z_l[0] = +FLT_MAX;
		x_l[1] = y_l[1] = z_l[1] = -FLT_MAX;

		for (int i = 0; i < 12; i++) {
			Len = vector3_dot(vector3(1, 0, 0), vertex[i]);
			if (Len < x_l[0]) x_l[0] = Len;
			if (Len > x_l[1]) x_l[1] = Len;

			Len = vector3_dot(vector3(0, 1, 0), vertex[i]);
			if (Len < y_l[0]) y_l[0] = Len;
			if (Len > y_l[1]) y_l[1] = Len;

			Len = vector3_dot(vector3(0, 1, 0), vertex[i]);
			if (Len < z_l[0]) z_l[0] = Len;
			if (Len > z_l[1]) z_l[1] = Len;
		}

		if (z_l[0] > farZ || z_l[1] < nearZ)return false;

		p[0] = vector3(x_l[0], y_l[0], 0);
		p[1] = vector3(x_l[1], y_l[0], 0);
		p[2] = vector3(x_l[1], y_l[1], 0);
		p[3] = vector3(x_l[0], y_l[1], 0);

		for (int i = 0; i < 4; i++) {
			V_L[i] = p[(4 + i - 1) % 4] - p[i];
		}

		for (int i = 0; i < 4; i++) {
			V_P[i] = -p[i];
		}

		if (
			vector3_cross(V_L[0], V_P[0]).z >= 0 &&
			vector3_cross(V_L[1], V_P[1]).z >= 0 &&
			vector3_cross(V_L[2], V_P[2]).z >= 0 &&
			vector3_cross(V_L[3], V_P[3]).z >= 0
			)
			return true;
	}


 	return false;
}