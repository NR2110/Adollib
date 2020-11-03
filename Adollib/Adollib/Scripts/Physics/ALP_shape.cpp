#include "ALP_shape.h"

//#include "collider__base.h"
//#include "../Object/gameobject.h"

using namespace Adollib;
using namespace physics_function;

void ALP_Shape::update_dop14(){

	switch (ALP_coll->shape)
	{
	case ALP_Collider_shape::shape_sphere:
		ALP_Shape::update_dop14_as_sphere();

		break;
	case ALP_Collider_shape::shape_box:
		ALP_Shape::update_dop14_as_box();

		break;
	case ALP_Collider_shape::shape_plane:
		ALP_Shape::update_dop14_as_plane();

		break;
	case ALP_Collider_shape::shape_mesh:
		ALP_Shape::update_dop14_as_mesh();

		break;
	default:
		break;
	}
}

void ALP_Shape::update_dop14_as_sphere() {

}

void ALP_Shape::update_dop14_as_box() {
	//dop14.pos = ALP_coll + local_position;

	////各頂点のローカル座標
	//Vector3 half[4]{
	//{+half_size.x, -half_size.y, -half_size.z},
	//{+half_size.x, -half_size.y, +half_size.z},
	//{+half_size.x, +half_size.y, -half_size.z},
	//{+half_size.x, +half_size.y, +half_size.z}
	//};

	//for (int i = 0; i < 4; i++) {
	//	half[i] = half[i] * world_scale;
	//}

	//Quaternion WO = gameobject->get_world_orientate();
	//for (int i = 0; i < 4; i++) {
	//	half[i] = vector3_quatrotate(half[i], WO);
	//}

	////DOPの更新
	//float max_len = 0;
	//for (int i = 0; i < DOP_size; i++) {
	//	max_len = 0;
	//	for (int o = 0; o < 4; o++) {
	//		float dis = fabsf(vector3_dot(DOP_14_axis[i], half[o]));
	//		if (max_len < dis) {
	//			dop14.max[i] = +dis * 1.00000001f;//確実にするためちょっと大きめにとる
	//			dop14.min[i] = -dis * 1.00000001f;//確実にするためちょっと大きめにとる
	//			max_len = dis;
	//		}

	//	}
	//}
}

void ALP_Shape::update_dop14_as_plane() {

}

void ALP_Shape::update_dop14_as_mesh() {

}

