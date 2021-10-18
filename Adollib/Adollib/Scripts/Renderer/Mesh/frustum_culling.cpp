//#include "frustum_culling.h"
//
//using namespace Adollib;
//using namespace DOP;
//
//Vector3 FrustumCulling::line[4];
//float FrustumCulling::nearZ;
//float FrustumCulling::farZ;
//
//Vector3 FrustumCulling::camera_pos;
//Quaternion FrustumCulling::camera_orient;
//
//Vector3 FrustumCulling::obj_pos;
//Quaternion FrustumCulling::obj_orient;
//Vector3 FrustumCulling::obj_scale;
//
//Vector3 FrustumCulling::offset_pos;
//Quaternion FrustumCulling::offset_orient;
//
//void FrustumCulling::update_frustum(const Camera* camera) {
//
//	Vector3 L;
//	//L.x = sinf(ToRadian(camera->fov));
//	//L.y = L.x * (1.0f / camera->aspect);
//	L.z = 1;
//
//	L.y = sinf(ToRadian(camera->fov / 2.0f) * 1.2f);
//	L.x = L.y * camera->aspect;
//
//	line[0] = Vector3(+L.x, +L.y, L.z);
//	line[1] = Vector3(+L.x, -L.y, L.z);
//	line[2] = Vector3(-L.x, -L.y, L.z);
//	line[3] = Vector3(-L.x, +L.y, L.z);
//
//	nearZ = camera->nearZ;
//	farZ = camera->farZ;
//
//	camera_pos = camera->transform->position;
//	camera_orient = camera->transform->orientation;
//}
//
//void FrustumCulling::update_obj(const Gameobject* GO) {
//	obj_pos = GO->transform->position;
//	obj_orient = GO->transform->orientation;
//	obj_scale = GO->transform->scale;
//}
//
//void FrustumCulling::frustum_culling_init() {
//	//offset_pos = camera_pos - obj_pos; //objからcamera座標系へのoffset
//	//offset_orient = camera_orient * obj_orient.conjugate();
//
//	offset_pos =  obj_pos - camera_pos ; //objからcamera座標系へのoffset
//	offset_orient = obj_orient * camera_orient.inverse();
//}
//
//
//bool FrustumCulling::frustum_culling(const Mesh::mesh* mesh) {
//
//	//TODO : 視錐台カリング未完成
//	return true;
//
//	Vector3 P_obj_cn;
//	Vector3 vertex[12];
//	Vector3 V_L[4], V_P[4];
//
//	for (int i = 0; i < 4; i++) {
//		V_L[i] = line[(4 + i - 1) % 4] - line[i];
//	}
//	//if (obj_scale.x > 10) return true;
//
//	//max
//	for (int axi = 0; axi < DOP14_size; axi++) {
//
//		//objのローカル座標系での各軸の長さ
//		P_obj_cn = DOP_14_axis[axi] * mesh->dop7.max[axi];
//
//		//scaleの適応
//		P_obj_cn = P_obj_cn * obj_scale;
//
//		//座標と向きをcameraの座標系へ
//		vertex[axi] = P_obj_cn = vector3_quatrotate(P_obj_cn + offset_pos, offset_orient);
//
//		//nearとfarの確認
//		if (P_obj_cn.z < nearZ || P_obj_cn.z > farZ)
//			continue;
//
//		for (int i = 0; i < 4; i++) {
//			V_P[i] = P_obj_cn - line[i] * P_obj_cn.z;
//		}
//
//		if (
//			vector3_cross(V_L[0], V_P[0]).z >= 0 &&
//			vector3_cross(V_L[1], V_P[1]).z >= 0 &&
//			vector3_cross(V_L[2], V_P[2]).z >= 0 &&
//			vector3_cross(V_L[3], V_P[3]).z >= 0
//			)
//			return true;
//	}
//
//	//min
//	for (int axi = 0; axi < 6; axi++) {
//
//		//objのローカル座標系での各軸の長さ
//		P_obj_cn = DOP_14_axis[axi] * mesh->dop7.min[axi];
//
//		//scaleの適応
//		P_obj_cn = P_obj_cn * obj_scale;
//
//		//座標と向きをcameraの座標系へ
//		vertex[6+axi] = P_obj_cn = vector3_quatrotate(P_obj_cn + offset_pos, offset_orient);
//
//		//nearとfarの確認
//		if (P_obj_cn.z < nearZ || P_obj_cn.z > farZ)
//			continue;
//
//		for (int i = 0; i < 4; i++) {
//			V_P[i] = P_obj_cn - line[i] * P_obj_cn.z;
//		}
//
//		if (
//			vector3_cross(V_L[0], V_P[0]).z >= 0 &&
//			vector3_cross(V_L[1], V_P[1]).z >= 0 &&
//			vector3_cross(V_L[2], V_P[2]).z >= 0 &&
//			vector3_cross(V_L[3], V_P[3]).z >= 0
//			)
//			return true;
//	}
//
//	{
//		Vector3 p[4];
//		float x_l[2], y_l[2], z_l[2], Len;
//
//		x_l[0] = y_l[0] = z_l[0] = +FLT_MAX;
//		x_l[1] = y_l[1] = z_l[1] = -FLT_MAX;
//
//		for (int i = 0; i < 12; i++) {
//			Len = vector3_dot(Vector3(1, 0, 0), vertex[i]);
//			if (Len < x_l[0]) x_l[0] = Len;
//			if (Len > x_l[1]) x_l[1] = Len;
//
//			Len = vector3_dot(Vector3(0, 1, 0), vertex[i]);
//			if (Len < y_l[0]) y_l[0] = Len;
//			if (Len > y_l[1]) y_l[1] = Len;
//
//			Len = vector3_dot(Vector3(0, 1, 0), vertex[i]);
//			if (Len < z_l[0]) z_l[0] = Len;
//			if (Len > z_l[1]) z_l[1] = Len;
//		}
//
//		if (z_l[0] > farZ || z_l[1] < nearZ)return false;
//
//		p[0] = Vector3(x_l[0], y_l[0], 0);
//		p[1] = Vector3(x_l[1], y_l[0], 0);
//		p[2] = Vector3(x_l[1], y_l[1], 0);
//		p[3] = Vector3(x_l[0], y_l[1], 0);
//
//		for (int i = 0; i < 4; i++) {
//			V_L[i] = p[(4 + i - 1) % 4] - p[i];
//		}
//
//		for (int i = 0; i < 4; i++) {
//			V_P[i] = -p[i];
//		}
//
//		if (
//			vector3_cross(V_L[0], V_P[0]).z >= 0 &&
//			vector3_cross(V_L[1], V_P[1]).z >= 0 &&
//			vector3_cross(V_L[2], V_P[2]).z >= 0 &&
//			vector3_cross(V_L[3], V_P[3]).z >= 0
//			)
//			return true;
//	}
//
//
// 	return false;
//}