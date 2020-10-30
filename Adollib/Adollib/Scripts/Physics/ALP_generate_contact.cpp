#include "ALP_generate_contact.h"

#include "../Math/closest_func.h"

#include "../Imgui/work_meter.h"

using namespace Adollib;
using namespace Contacts;

using namespace Closest_func;

//:::::::::::::::::::::::::::
#pragma region generate_contact
//:::::::::::::::::::::::::::

void physics_function::generate_contact(std::vector<Contacts::Contact_pair>& pairs) {

	//::: dynamic_cast 多用地帯 危険! 危険!
	for (int i = 0; i < pairs.size(); i++) {

		if (pairs[i].body[0]->shape == Collider_shape::shape_sphere) {
			Sphere* shape0 = dynamic_cast<Sphere*>(pairs[i].body[0]);

			if (pairs[i].body[1]->shape == Collider_shape::shape_sphere) {
				Sphere* shape1 = dynamic_cast<Sphere*>(pairs[i].body[1]);
				assert(shape0 != nullptr && shape1 != nullptr);
				generate_contact_sphere_sphere(*shape0, *shape1, pairs[i]);
			}
			if (pairs[i].body[1]->shape == Collider_shape::shape_box) {
				Box* shape1 = dynamic_cast<Box*>(pairs[i].body[1]);
				assert(shape0 != nullptr && shape1 != nullptr);
				generate_contact_sphere_box(*shape0, *shape1, pairs[i]);
			}
			if (pairs[i].body[1]->shape == Collider_shape::shape_plane) {
				Plane* shape1 = dynamic_cast<Plane*>(pairs[i].body[1]);
				assert(shape0 != nullptr && shape1 != nullptr);
				generate_contact_sphere_plane(*shape0, *shape1, pairs[i]);
			}
			if (pairs[i].body[1]->shape == Collider_shape::shape_mesh) {
				Meshcoll* shape1 = dynamic_cast<Meshcoll*>(pairs[i].body[1]);
				assert(shape0 != nullptr && shape1 != nullptr);
				generate_contact_sphere_mesh(*shape0, *shape1, pairs[i]);
			}
		}
		if (pairs[i].body[0]->shape == Collider_shape::shape_box) {
			Box* shape0 = dynamic_cast<Box*>(pairs[i].body[0]);

			if (pairs[i].body[1]->shape == Collider_shape::shape_sphere) {
				Sphere* shape1 = dynamic_cast<Sphere*>(pairs[i].body[1]);
				assert(shape0 != nullptr && shape1 != nullptr);
				generate_contact_sphere_box(*shape1, *shape0, pairs[i]);
			}
			if (pairs[i].body[1]->shape == Collider_shape::shape_box) {
				Box* shape1 = dynamic_cast<Box*>(pairs[i].body[1]);
				assert(shape0 != nullptr && shape1 != nullptr);
				generate_contact_box_box(*shape0, *shape1, pairs[i]);
			}
			if (pairs[i].body[1]->shape == Collider_shape::shape_plane) {
				Plane* shape1 = dynamic_cast<Plane*>(pairs[i].body[1]);
				assert(shape0 != nullptr && shape1 != nullptr);
				generate_contact_box_plane(*shape0, *shape1, pairs[i]);
			}
		}
		if (pairs[i].body[0]->shape == Collider_shape::shape_plane) {
			Plane* shape0 = dynamic_cast<Plane*>(pairs[i].body[0]);

			if (pairs[i].body[1]->shape == Collider_shape::shape_sphere) {
				Sphere* shape1 = dynamic_cast<Sphere*>(pairs[i].body[1]);
				assert(shape0 != nullptr && shape1 != nullptr);
				generate_contact_sphere_plane(*shape1, *shape0, pairs[i]);
			}

			if (pairs[i].body[1]->shape == Collider_shape::shape_box) {
				Box* shape1 = dynamic_cast<Box*>(pairs[i].body[1]);
				assert(shape0 != nullptr && shape1 != nullptr);
				generate_contact_box_plane(*shape1, *shape0, pairs[i]);
			}
			if (pairs[i].body[0]->shape == Collider_shape::shape_plane) {

			}
		}

		if (pairs[i].body[0]->shape == Collider_shape::shape_mesh) {
			Meshcoll* shape0 = dynamic_cast<Meshcoll*>(pairs[i].body[0]);

			if (pairs[i].body[1]->shape == Collider_shape::shape_sphere) {
				Sphere* shape1 = dynamic_cast<Sphere*>(pairs[i].body[1]);
				assert(shape0 != nullptr && shape1 != nullptr);
				generate_contact_sphere_mesh(*shape1, *shape0, pairs[i]);
			}
			if (pairs[i].body[1]->shape == Collider_shape::shape_plane) {
				Plane* shape1 = dynamic_cast<Plane*>(pairs[i].body[1]);
				assert(shape0 != nullptr && shape1 != nullptr);
				generate_contact_mesh_plane(*shape0 , *shape1, pairs[i]);
			}
			if (pairs[i].body[1]->shape == Collider_shape::shape_mesh) {
				Meshcoll* shape1 = dynamic_cast<Meshcoll*>(pairs[i].body[1]);
				assert(shape0 != nullptr && shape1 != nullptr);
				generate_contact_mesh_mesh(*shape0, *shape1, pairs[i]);

			}
		}


	}
}


#pragma region data

//分離軸判定を用いる際に使用する
enum SAT_TYPE {
	POINTA_FACETB,
	POINTB_FACETA,
	EDGE_EDGE
};
struct Ret_S {
	float penetrate;
	int smallest_axis[2];
	SAT_TYPE smallest_case;
	bool hit_point_to_face;
};

struct OBB {
	Vector3 world_position; //中心座標
	Vector3 u_axes[3]; //軸の向き
	Quaternion world_orientation;
	Vector3 half_width; //軸ごとの辺の長さ
};

//軸に投影した長さ
float sum_of_projected_radii(const OBB& obb, const Vector3& vec) {
	return
		fabsf(vector3_dot(vec, obb.half_width.x * obb.u_axes[0])) +
		fabsf(vector3_dot(vec, obb.half_width.y * obb.u_axes[1])) +
		fabsf(vector3_dot(vec, obb.half_width.z * obb.u_axes[2]));
}

float sum_of_projected_radii(float& max, float& min, const Meshcoll& meshcoll, const Vector3& nor) {
	float value;
	max = -FLT_MAX;
	min = +FLT_MAX;
	for (Vector3 vertex : *meshcoll.vertices) {
		value = vector3_dot(vertex * meshcoll.world_scale, nor);
		if (max < value)max = value;
		if (min > value)min = value;
	}

	return max - min;
}

//OBBとOBBの分離軸判定 衝突していればtrueを返す ついでに色々引数に入れる
bool sat_obb_obb(
	const OBB& obbA, const OBB& obbB,
	float& smallest_penetration, //最小の貫通量
	int smallest_axis[2], //どの軸で最近になったか(edge×edge用に2つ分用意)
	SAT_TYPE& smallest_case //どのような形で最近になっているか
) {
	smallest_penetration = FLT_MAX;

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;


	float penetration = 0; //貫通量
	float ra, rb; //obbA,obbBのLに投影された長さ
	Vector3 axis; //投影する軸
	Vector3 distBtoA = obbA.world_position - obbB.world_position; //2obbの中心座標の距離

	//::: obbAの軸にobbBを投影 :::::::::::::::::::
	for (int i = 0; i < 3; i++)
	{
		axis = obbA.u_axes[i];
		ra = fabsf(obbA.half_width[i]);
		rb = fabsf(sum_of_projected_radii(obbB, axis));

		assert(ra > 0 && rb > 0);

		penetration = ra + rb - abs(vector3_dot(axis, distBtoA));
		if (penetration < 0) return false;

		if (fabsf(vector3_dot(axis, distBtoA)) + rb < ra) {
			PA_FB.hit_point_to_face = false;
		}

		if (PB_FA.penetrate > penetration) {
			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = i;
			PB_FA.smallest_axis[1] = -1;
			PB_FA.smallest_case = POINTB_FACETA;
		}
	}

	//::: obbBの軸にobbAを投影 ::::::::::::::::::::
	if (PA_FB.hit_point_to_face == true)
		for (int i = 0; i < 3; i++)
		{
			axis = obbB.u_axes[i];
			ra = fabsf(sum_of_projected_radii(obbA, axis));
			rb = fabsf(obbB.half_width[i]);

			penetration = ra + rb - abs(vector3_dot(axis, distBtoA));
			if (penetration < 0) return false;

			if (fabsf(vector3_dot(axis, distBtoA)) + ra < rb) {
				PB_FA.hit_point_to_face = false;
			}

			if (PA_FB.penetrate > penetration) {

				PA_FB.penetrate = penetration;
				PA_FB.smallest_axis[0] = -1;
				PA_FB.smallest_axis[1] = i;
				PA_FB.smallest_case = POINTA_FACETB;
			}
		}

	if (PB_FA.hit_point_to_face && PB_FA.penetrate < smallest_penetration) {
		smallest_penetration = PB_FA.penetrate;
		smallest_axis[0] = PB_FA.smallest_axis[0];
		smallest_axis[1] = PB_FA.smallest_axis[1];
		smallest_case = PB_FA.smallest_case;
	}
	if (PA_FB.hit_point_to_face && PA_FB.penetrate < smallest_penetration) {
		smallest_penetration = PA_FB.penetrate;
		smallest_axis[0] = PA_FB.smallest_axis[0];
		smallest_axis[1] = PA_FB.smallest_axis[1];
		smallest_case = PA_FB.smallest_case;

	}

	//::: 外積の軸に投影
	for (int OB1 = 0; OB1 < 3; OB1++) {
		for (int OB2 = 0; OB2 < 3; OB2++) {

			axis = vector3_cross(obbA.u_axes[OB1], obbB.u_axes[OB2]);
			if (axis.norm() <= FLT_EPSILON * FLT_EPSILON)continue;//外積が 0 = 平行

			axis = axis.unit_vect();

			ra = fabsf(sum_of_projected_radii(obbA, axis));
			rb = fabsf(sum_of_projected_radii(obbB, axis));
			//if (vector3_dot(axis, distBtoA) + rb < ra)continue;

			penetration = ra + rb - fabsf(vector3_dot(axis, distBtoA));
			if (penetration < 0) return false;
			if (smallest_penetration > penetration) {

				smallest_penetration = penetration;
				smallest_axis[0] = OB1;
				smallest_axis[1] = OB2;
				smallest_case = EDGE_EDGE;
			}
		}
	}

	// 分離軸が見当たらない場合OBBは交差しているはず
	return (smallest_penetration < FLT_MAX && smallest_penetration > FLT_EPSILON) ? true : false;
}

//どちらも凸包の場合
bool sat_convex_mesh_mesh(const Meshcoll& collA, const Meshcoll& collB,
	float& smallest_penetration, //最小の貫通量
	int smallest_axis[2], //どの軸で最近になったか(edge×edge用に2つ分用意)
	SAT_TYPE& smallest_case //どのような形で最近になっているか
) {
	smallest_penetration = FLT_MAX;

	// Meshcoll_1の各面法線を分離軸とする
	Quaternion offset_quatBA = collB.world_orientation * collA.world_orientation.conjugate();
	Quaternion offset_quatAB = collA.world_orientation * collB.world_orientation.conjugate();
	Vector3 offset_posBA = collB.world_position - collA.world_position;
	Vector3 offset_posAB = collA.world_position - collB.world_position;

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;
	float penetration = FLT_MAX;

	////::: obbAの軸にobbBを投影 :::::::::::::::::::
		//collAの座標系で計算を行う
	float maxA, minA, maxB, minB;
	for (u_int f = 0; f < collA.facet_num; f++) {
		const Facet& facet = collA.facets[f];
		const Vector3& axis = facet.normal;

		// collAを分離軸に投影
		sum_of_projected_radii(maxA, minA, collA, axis);
		assert(maxA >= minA);

		// collBを分離軸に投影
		sum_of_projected_radii(maxB, minB, collB, vector3_quatrotate(axis, offset_quatAB).unit_vect());
		assert(maxB >= minB);
		float off = vector3_dot(offset_posBA, vector3_quatrotate(axis, collA.world_orientation).unit_vect());
		maxB += off;
		minB += off;

		//貫通の計算
		float d1 = minA - maxB;
		float d2 = minB - maxA;
		if (d1 >= 0.0f || d2 >= 0.0f)
			return false;


		penetration = -1 * ALmax(d1, d2);

		//if (fabsf(vector3_dot(axis, distBtoA)) + rb < ra) {
		//	PA_FB.hit_point_to_face = false;
		//}

		if (PB_FA.penetrate > penetration) {
			sum_of_projected_radii(maxB, minB, collB, vector3_quatrotate(axis, offset_quatAB).unit_vect());

			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = f;
			PB_FA.smallest_axis[1] = -1;
			PB_FA.smallest_case = POINTB_FACETA;
		}
	}

	////::: obbBの軸にobbAを投影 :::::::::::::::::::
		//collBの座標系で計算を行う
	if (PA_FB.hit_point_to_face == true)
		for (u_int f = 0; f < collB.facet_num; f++) {
			const Facet& facet = collB.facets[f];
			const Vector3& axis = facet.normal;

			// collBを分離軸に投影
			sum_of_projected_radii(maxB, minB, collB, axis);
			assert(maxB >= minB);

			// collAを分離軸に投影
			sum_of_projected_radii(maxA, minA, collA, vector3_quatrotate(axis, offset_quatBA));
			assert(maxA >= minA);
			float off = vector3_dot(offset_posAB, vector3_quatrotate(axis, collB.world_orientation).unit_vect());
			maxA += off;
			minA += off;

			//貫通の計算
			float d1 = minA - maxB;
			float d2 = minB - maxA;
			if (d1 >= 0.0f || d2 >= 0.0f)
				return false;


			penetration = -1 * ALmax(d1, d2);

			if (PA_FB.penetrate > penetration) {
				sum_of_projected_radii(maxA, minA, collA, vector3_quatrotate(axis, offset_quatBA));

				PA_FB.penetrate = penetration;
				PA_FB.smallest_axis[0] = -1;
				PA_FB.smallest_axis[1] = f;
				PA_FB.smallest_case = POINTA_FACETB;
			}
		}

	if (PB_FA.hit_point_to_face && PB_FA.penetrate < smallest_penetration) {
		smallest_penetration = PB_FA.penetrate;
		smallest_axis[0] = PB_FA.smallest_axis[0];
		smallest_axis[1] = PB_FA.smallest_axis[1];
		smallest_case = PB_FA.smallest_case;
	}
	if (PA_FB.hit_point_to_face && PA_FB.penetrate < smallest_penetration) {
		smallest_penetration = PA_FB.penetrate;
		smallest_axis[0] = PA_FB.smallest_axis[0];
		smallest_axis[1] = PA_FB.smallest_axis[1];
		smallest_case = PA_FB.smallest_case;

	}

	Work_meter::start("generate_edge_edge");
	//::: 外積の軸に投影(最近距離を求めるため)
		//collAの座標系で計算を行う
	Vector3 save_axisW;
	float save_crossnorm;
	Vector3 axisA, axisB, axisW;
	for (u_int eA = 0; eA < collA.edge_num; eA++) {
		const Edge& edgeA = collA.edges[eA];
		if (edgeA.type != Edgetype::EdgeConvex) continue;

		const Vector3& edgeVecA = (*collA.vertices)[edgeA.vertexID[1]] - (*collA.vertices)[edgeA.vertexID[0]];
		for (u_int eB = 0; eB < collB.edge_num; eB++) {
			const Edge& edgeB = collB.edges[eB];
			if (edgeB.type != Edgetype::EdgeConvex) continue;

			const Vector3 edgeVecB = vector3_quatrotate((*collB.vertices)[edgeB.vertexID[1]] - (*collB.vertices)[edgeB.vertexID[0]], offset_quatBA);

			axisA = vector3_cross(edgeVecA, edgeVecB);
			if (axisA.norm() <= FLT_EPSILON * FLT_EPSILON) continue;
			axisA = axisA.unit_vect();
			axisW = vector3_quatrotate(axisA, collA.world_orientation);
			//axisの向きをA->Bにする
			if (vector3_dot(axisW, collB.world_position - collA.world_position) < 0) {
				axisA *= -1;
				axisW *= -1;
			}
			axisB = vector3_quatrotate(axisA, offset_quatAB);

			sum_of_projected_radii(maxA, minA, collA, axisA);		assert(maxA >= minA);
			sum_of_projected_radii(maxB, minB, collB, axisB);		assert(maxB >= minB);
			float off = vector3_dot(offset_posBA, axisW);

			maxB += off;
			minB += off;

			//辺と辺の距離
			float CN = fabsf(off) - (vector3_dot(axisA, (*collA.vertices)[edgeA.vertexID[0]]) + -(vector3_dot(axisB, (*collB.vertices)[edgeB.vertexID[0]])));

			//貫通の計算
			float d1 = minA - maxB;
			float d2 = minB - maxA;
			if (d1 >= 0.0f || d2 >= 0.0f)
				return false;

			penetration = -1 * ALmax(d1, d2) * 1.0001f;

			if (smallest_penetration > penetration) {
				smallest_penetration = penetration;
				smallest_axis[0] = eA;
				smallest_axis[1] = eB;
				smallest_case = EDGE_EDGE;
				save_axisW = axisW;
				save_crossnorm = CN;
			}
			else if (fabsf(vector3_dot(axisW, save_axisW)) > 0.999 && CN < save_crossnorm) {
				//分離軸は同じだがより近い軸を見つけた時の処理
				smallest_axis[0] = eA;
				smallest_axis[1] = eB;
				save_axisW = axisW;
				save_crossnorm = CN;
			}
		}
	}
	Work_meter::stop("generate_edge_edge");
	// 分離軸が見当たらない場合OBBは交差しているはず
	return (smallest_penetration < FLT_MAX && smallest_penetration > FLT_EPSILON) ? true : false;
}

bool sat_obb_convex_mesh(const OBB& obb, const Meshcoll& mesh,
	float& smallest_penetration, //最小の貫通量
	int smallest_axis[2], //どの軸で最近になったか(edge×edge用に2つ分用意)
	SAT_TYPE& smallest_case //どのような形で最近になっているか
) {
	smallest_penetration = FLT_MAX;

	// Meshcoll_1の各面法線を分離軸とする
	Quaternion offset_quatBA = mesh.world_orientation * obb.world_orientation.conjugate();
	Quaternion offset_quatAB = obb.world_orientation * mesh.world_orientation.conjugate();
	Vector3 offset_posBA = mesh.world_position - obb.world_position;
	Vector3 offset_posAB = obb.world_position - mesh.world_position;

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;
	float penetration = FLT_MAX;

	////::: obbの軸にmeshを投影 :::::::::::::::::::
		//obbの座標系で計算を行う
	float ra, maxA, minA;
	for (u_int f = 0; f < 3; f++) {
		const Vector3& axis = obb.u_axes[f]; //worldcoord

		// obbを分離軸に投影
		float ra = obb.half_width[f];

		// meshを分離軸に投影
		sum_of_projected_radii(maxA, minA, mesh, vector3_quatrotate(axis, mesh.world_orientation.conjugate()).unit_vect());
		assert(maxA >= minA);
		float off = vector3_dot(offset_posBA, axis);
		maxA += off;
		minA += off;

		//貫通の計算
		float d1 = -ra - maxA;
		float d2 = minA - ra;
		if (d1 >= 0.0f || d2 >= 0.0f)
			return false;

		penetration = -1 * ALmax(d1, d2);

		if (PB_FA.penetrate > penetration) {

			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = f;
			PB_FA.smallest_axis[1] = -1;
			PB_FA.smallest_case = POINTB_FACETA;
		}
	}

	////::: obbBの軸にobbAを投影 :::::::::::::::::::
		//meshの座標系で計算を行う
	if (PA_FB.hit_point_to_face == true)
		for (u_int f = 0; f < mesh.facet_num; f++) {
			const Facet& facet = mesh.facets[f];
			const Vector3& axis = facet.normal; //meshcoord

			// meshを分離軸に投影
			sum_of_projected_radii(maxA, minA, mesh, axis);
			assert(maxA >= minA);

			// obbを分離軸に投影
			float ra = sum_of_projected_radii(obb, vector3_quatrotate(axis, mesh.world_orientation));

			float off = vector3_dot(offset_posAB, vector3_quatrotate(axis, mesh.world_orientation).unit_vect());
			maxA += off;
			minA += off;

			//貫通の計算
			float d1 = minA - ra;
			float d2 = -ra - maxA;
			if (d1 >= 0.0f || d2 >= 0.0f)
				return false;


			penetration = -1 * ALmax(d1, d2);

			if (PA_FB.penetrate > penetration) {

				PA_FB.penetrate = penetration;
				PA_FB.smallest_axis[0] = -1;
				PA_FB.smallest_axis[1] = f;
				PA_FB.smallest_case = POINTA_FACETB;
			}
		}

	if (PB_FA.hit_point_to_face && PB_FA.penetrate < smallest_penetration) {
		smallest_penetration = PB_FA.penetrate;
		smallest_axis[0] = PB_FA.smallest_axis[0];
		smallest_axis[1] = PB_FA.smallest_axis[1];
		smallest_case = PB_FA.smallest_case;
	}
	if (PA_FB.hit_point_to_face && PA_FB.penetrate < smallest_penetration) {
		smallest_penetration = PA_FB.penetrate;
		smallest_axis[0] = PA_FB.smallest_axis[0];
		smallest_axis[1] = PA_FB.smallest_axis[1];
		smallest_case = PA_FB.smallest_case;

	}

	//Work_meter::start("generate_edge_edge");
	////::: 外積の軸に投影(最近距離を求めるため)
	//	//obbの座標系で計算を行う
	//Vector3 save_axisW;
	//float save_crossnorm;
	//Vector3 axisM, axisO, axisW;
	//for (u_int eO = 0; eO < 3; eO++) {
	//	const Vector3& edgeVecOw = obb.u_axes[eO]; //worldcoord

	//	for (int eM = 0; eM < mesh.edge_num; eM++) {
	//		const Edge& edgeB = mesh.edges[eM]; //meshcoord
	//		if (edgeB.type != Edgetype::EdgeConvex) continue;

	//		//meshcoord
	//		const Vector3 edgeVecMl = vector3_quatrotate((*mesh.vertices)[edgeB.vertexID[1]] - (*mesh.vertices)[edgeB.vertexID[0]], offset_quatBA);
	//		//meshcoord
	//		const Vector3& edgeVecOm = vector3_quatrotate(edgeVecOw, mesh.world_orientation.conjugate());

	//		axisM = vector3_cross(edgeVecMl, edgeVecOm); //meshcoord
	//		if (axisM.norm() <= FLT_EPSILON * FLT_EPSILON) continue;
	//		axisM = axisM.unit_vect();
	//		axisW = vector3_quatrotate(axisW, obb.world_orientation); //worldcoord
	//		//axisの向きをA->Bにする
	//		if (vector3_dot(axisW, mesh.world_position - obb.world_position) < 0) {
	//			axisM *= -1;
	//			axisW *= -1;
	//		}
	//		ra = sum_of_projected_radii(obb, axisW);
	//		sum_of_projected_radii(maxA, minA, mesh, axisM);		assert(maxA >= minA);
	//		float off = vector3_dot(offset_posBA, axisW);

	//		//辺と辺の距離
	//		float CN = fabsf(off) - (obb.half_width[eO]) + -(vector3_dot(axisM, (*mesh.vertices)[edgeB.vertexID[0]]));

	//		//貫通の計算
	//		float d1 = minA - ra;
	//		float d2 = -ra - maxA;
	//		if (d1 >= 0.0f || d2 >= 0.0f)
	//			return false;

	//		penetration = -1 * ALmax(d1, d2) * 1.0001f;


	//		if (smallest_penetration > penetration) {
	//			smallest_penetration = penetration;
	//			smallest_axis[0] = eA;
	//			smallest_axis[1] = eB;
	//			smallest_case = EDGE_EDGE;
	//			save_axisW = axisW;
	//			save_crossnorm = CN;
	//		}
	//		else if (fabsf(vector3_dot(axisW, save_axisW)) > 0.999 && CN < save_crossnorm) {
	//			//分離軸は同じだがより近い軸を見つけた時の処理
	//			smallest_axis[0] = eA;
	//			smallest_axis[1] = eB;
	//			save_axisW = axisW;
	//			save_crossnorm = CN;
	//		}
	//	}

		//ra = sum_of_projected_radii(obb, axisW);
		//sum_of_projected_radii(maxA, minA, mesh, axisM);		assert(maxA >= minA);
		//float off = vector3_dot(offset_posBA, axisW);

		//maxA += off;
		//minA += off;

		////辺と辺の距離
		//float CN = fabsf(off) - (obb.half_width[eO]) + -(vector3_dot(axisM, (*mesh.vertices)[edgeB.vertexID[0]])));

		////貫通の計算
		//float d1 = minA - maxB;
		//float d2 = minB - maxA;
		//if (d1 >= 0.0f || d2 >= 0.0f)
		//	return false;

		//penetration = -1 * ALmax(d1, d2) * 1.0001f;

		//if (smallest_penetration > penetration) {
		//	smallest_penetration = penetration;
		//	smallest_axis[0] = eA;
		//	smallest_axis[1] = eB;
		//	smallest_case = EDGE_EDGE;
		//	save_axisW = axisW;
		//	save_crossnorm = CN;
		//}
		//else if (fabsf(vector3_dot(axisW, save_axisW)) > 0.999 && CN < save_crossnorm) {
		//	//分離軸は同じだがより近い軸を見つけた時の処理
		//	smallest_axis[0] = eA;
		//	smallest_axis[1] = eB;
		//	save_axisW = axisW;
		//	save_crossnorm = CN;
	//	}

	//}
	Work_meter::stop("generate_edge_edge");
	// 分離軸が見当たらない場合OBBは交差しているはず
	return (smallest_penetration < FLT_MAX && smallest_penetration > FLT_EPSILON) ? true : false;
}
//
#pragma endregion

//衝突生成
#pragma region SPHERE-SPHERE
bool physics_function::generate_contact_sphere_sphere(const Sphere& S0, const Sphere& S1, Contacts::Contact_pair& pair) {
	Vector3 p0 = S0.world_position;
	Vector3 p1 = S1.world_position;

	//p1 から p0　方向への法線
	Vector3 n = p0 - p1;
	float length = n.norm_sqr();
	n = n.unit_vect();

	if (length < S0.world_scale.x + S1.world_scale.x) {
		//衝突していたらContactオブジェクトを生成する
		pair.contacts.addcontact(
			S0.world_scale.x + S1.world_scale.x - length,
			n,
			S0.world_scale.x * vector3_quatrotate(-n, S0.world_orientation.conjugate()),
			S1.world_scale.x * vector3_quatrotate(n, S1.world_orientation.conjugate())
		);
		return true;
	}

	return false;
}
#pragma endregion

#pragma region SPHERE-PLANE
bool physics_function::generate_contact_sphere_plane(const Sphere& sphere, const Plane& plane, Contacts::Contact_pair& pair) {
	//球面と平面の衝突判定を行う
	matrix rotate, inverse_rotate;
	rotate = plane.world_orientation.get_rotate_matrix();
	rotate._41 = plane.world_position.x; //transpseの入力
	rotate._42 = plane.world_position.y;
	rotate._43 = plane.world_position.z;
	rotate._44 = 1;
	Vector3 n(rotate._21, rotate._22, rotate._23);
	inverse_rotate = matrix_inverse(rotate);

	Vector3 p;
	p = vector3_trans(sphere.world_position, inverse_rotate); //平面が"原点を通り"法線が(0,1,0)"の状態の時の球の中心座標

	////平面の裏からの衝突判定
	//if (half_space && p.y < 0)return 0;

	if (abs(p.y) < sphere.world_scale.x) {
		n = p.y > 0 ? n : -n;

		if (pair.body[0]->shape == plane.shape) {
			//body[0]　が　plane
			//body[1]　が　sphere
			pair.contacts.addcontact(
				sphere.world_scale.x - abs(p.y),
				n,
				Vector3(p.x, 0, p.z),
				sphere.world_scale.x * vector3_quatrotate(-n, sphere.world_orientation.conjugate())
			);
		}
		else {

			//body[0]　が　sphere
			//body[1]　が　plane
			pair.contacts.addcontact(
				sphere.world_scale.x - abs(p.y),
				n,
				sphere.world_scale.x * vector3_quatrotate(-n, sphere.world_orientation.conjugate()),
				Vector3(p.x, 0, p.z)
			);
		}
		return true;
	}

	return false;
}
#pragma endregion

#pragma region SPHERE-BOX
bool physics_function::generate_contact_sphere_box(const Sphere& sphere, const  Box& box, Contacts::Contact_pair& pair) {
	//球とboxの衝突判定を行う
	matrix rotate, inverse_rotate;
	//rotate = box.world_orientation.get_rotate_matrix();
	//rotate._41 = box.world_position.x; //transpseの入力
	//rotate._42 = box.world_position.y;
	//rotate._43 = box.world_position.z;
	//rotate._44 = 1;
	rotate = matrix_world(Vector3(1, 1, 1), box.world_orientation.get_rotate_matrix(), box.world_position);
	inverse_rotate = matrix_inverse(rotate);

	Vector3 center;
	center = vector3_trans(sphere.world_position, inverse_rotate); //boxのlocal座標系での球の中心座標

	Vector3 box_halfsize = box.half_size * box.world_scale;

	if (
		abs(center.x) - sphere.world_scale.x > box_halfsize.x ||
		abs(center.y) - sphere.world_scale.x > box_halfsize.y ||
		abs(center.z) - sphere.world_scale.x > box_halfsize.z
		) return 0;

	//box上の最近点
	Vector3 closest_point;

	closest_point = center;
	if (center.x > +box_halfsize.x)closest_point.x = +box_halfsize.x;
	if (center.x < -box_halfsize.x)closest_point.x = -box_halfsize.x;

	if (center.y > +box_halfsize.y)closest_point.y = +box_halfsize.y;
	if (center.y < -box_halfsize.y)closest_point.y = -box_halfsize.y;

	if (center.z > +box_halfsize.z)closest_point.z = +box_halfsize.z;
	if (center.z < -box_halfsize.z)closest_point.z = -box_halfsize.z;

	float distance = (closest_point - center).norm_sqr(); //最近点と球中心の距離
	if (sphere.world_scale.x - distance > FLT_EPSILON) { //float誤差も調整
		Vector3 n = (sphere.world_position - vector3_trans(closest_point, rotate)).unit_vect(); //boxからsphereへのベクトル
		if (vector3_dot(n, sphere.world_position - box.world_position) < 0)n *= -1;

		if (pair.body[0]->shape == box.shape) {
			//body[0]　が　box
			//body[1]　が　sphere
			pair.contacts.addcontact(
				sphere.world_scale.x - distance,
				-n,
				closest_point,
				sphere.world_scale.x * vector3_quatrotate(n, sphere.world_orientation.conjugate())
			);
		}
		else {
			//body[0]　が　sphere
			//body[1]　が　box
			pair.contacts.addcontact(
				sphere.world_scale.x - distance,
				n,
				sphere.world_scale.x * vector3_quatrotate(n, sphere.world_orientation.conjugate()),
				closest_point
			);
		}
		return true;
	}

	return false;
}
#pragma endregion

#pragma region SPHERE-MESH
bool physics_function::generate_contact_sphere_mesh(const Sphere& sphere, const Meshcoll& mesh, Contacts::Contact_pair& pair) {
	if (mesh.is_Convex == true) {
		//球とmeshの衝突判定を行う
		matrix rotate, inverse_rotate;
		rotate = matrix_world(Vector3(1, 1, 1), mesh.world_orientation.get_rotate_matrix(), mesh.world_position);
		inverse_rotate = matrix_inverse(rotate);

		Vector3 center;
		center = vector3_trans(sphere.world_position, inverse_rotate); //meshのlocal座標系での球の中心座標

		Vector3 box_halfsize = mesh.world_scale;

		//mesh上の最近点
		Vector3 closest_point;

		closest_point = center;
		for (int i = 0; i < mesh.facet_num; i++) {
			const Vector3& nor = mesh.facets[i].normal.unit_vect();
			const Vector3& pos = (*mesh.vertices)[mesh.facets[i].vertexID[0]] * mesh.world_scale;
			float d = vector3_dot(nor, pos) - vector3_dot(nor, closest_point);
			if (d < 0) 
				closest_point += d * nor;		
		}

		float distance = (closest_point - center).norm_sqr(); //最近点と球中心の距離
		if (sphere.world_scale.x - distance > FLT_EPSILON) { //float誤差も調整
			Vector3 n = (sphere.world_position - vector3_trans(closest_point, rotate)).unit_vect(); //meshからsphereへのベクトル

			if (pair.body[0]->shape == mesh.shape) {
				//body[0]　が　mesh
				//body[1]　が　sphere
				pair.contacts.addcontact(
					sphere.world_scale.x - distance,
					-n,
					closest_point,
					sphere.world_scale.x * vector3_quatrotate(n, sphere.world_orientation.conjugate())
				);
			}
			else {
				//body[0]　が　sphere
				//body[1]　が　mesh
				pair.contacts.addcontact(
					sphere.world_scale.x - distance,
					n,
					sphere.world_scale.x * vector3_quatrotate(n, sphere.world_orientation.conjugate()),
					closest_point
				);
			}
			return true;
		}
	}
	else {
		return false;

	}

	return false;
}
#pragma endregion


#pragma region BOX-PLANE
bool physics_function::generate_contact_box_plane(const Box& box, const Plane& plane, Contacts::Contact_pair& pair) {

	Vector3 vertices[8] = {
		// obb座標系での各頂点のローカル座標
		Vector3(-box.world_scale.x, -box.world_scale.y, -box.world_scale.z),
		Vector3(-box.world_scale.x, -box.world_scale.y, +box.world_scale.z),
		Vector3(-box.world_scale.x, +box.world_scale.y, -box.world_scale.z),
		Vector3(-box.world_scale.x, +box.world_scale.y, +box.world_scale.z),
		Vector3(+box.world_scale.x, -box.world_scale.y, -box.world_scale.z),
		Vector3(+box.world_scale.x, -box.world_scale.y, +box.world_scale.z),
		Vector3(+box.world_scale.x, +box.world_scale.y, -box.world_scale.z),
		Vector3(+box.world_scale.x, +box.world_scale.y, +box.world_scale.z)
	};
	for (int i = 0; i < 8; i++) {
		vertices[i] *= box.half_size;
	}

	//Boxと平面の衝突判定を行う
	matrix rotate, inverse_rotate;
	rotate = plane.world_orientation.get_rotate_matrix();
	rotate._41 = plane.world_position.x; //transpseの入力
	rotate._42 = plane.world_position.y;
	rotate._43 = plane.world_position.z;
	rotate._44 = 1;
	Vector3 n(rotate._21, rotate._22, rotate._23);
	inverse_rotate = matrix_inverse(rotate);

	Vector3 p;
	p = vector3_trans(box.world_position, inverse_rotate); //平面が"原点を通り"法線が(0,1,0)"の状態の時のBoxの中心座標(p.yが距離になる)
	//n = p.y > 0 ? n : -n; //ボックスが平面下にあったら法線を反対にする

	float max_penetrate = 0.0f;
	Vector3 pointbox, pointplane;
	Vector3 vs;

	for (int i = 0; i < 8; i++) {
		// 頂点のワールド座標
		vs = vector3_quatrotate(vertices[i], box.world_orientation);

		// -面法線の内積をとる(面方向への長さ)
		float rA = vector3_dot(-n, vs);

		if (max_penetrate < rA - p.y) {
			max_penetrate = rA - p.y;
			pointbox = vertices[i];

			// 頂点をplane空間に
			vs = vector3_trans(vs + box.world_position, inverse_rotate);
			pointplane = vs;
			pointplane.y = 0;
		}
	}

	if (max_penetrate != 0) {
		if (pair.body[0]->shape == plane.shape) {
			//body[0]　が　plane
			//body[1]　が　box
			pair.contacts.addcontact(
				max_penetrate,
				-n,
				pointplane,
				pointbox
			);
		}
		else {
			//body[0]　が　box
			//body[1]　が　plane
			pair.contacts.addcontact(
				max_penetrate,
				n,
				pointbox,
				pointplane
			);
		}

		return true;
	}
	return false;
}
#pragma endregion

#pragma region BOX-BOX

bool physics_function::generate_contact_box_box(const Box& b0, const Box& b1, Contacts::Contact_pair& pair) {
	matrix m;
	m = b0.world_orientation.get_rotate_matrix();
	OBB obbA;
	obbA.world_position = b0.world_position;
	obbA.u_axes[0].x = m._11; obbA.u_axes[0].y = m._12; obbA.u_axes[0].z = m._13;
	obbA.u_axes[1].x = m._21; obbA.u_axes[1].y = m._22; obbA.u_axes[1].z = m._23;
	obbA.u_axes[2].x = m._31; obbA.u_axes[2].y = m._32; obbA.u_axes[2].z = m._33;
	obbA.half_width = b0.half_size * b0.world_scale;
	obbA.world_orientation = b0.world_orientation;
	assert(!isnan(obbA.world_orientation.norm()));

	m = b1.world_orientation.get_rotate_matrix();
	OBB obbB;
	obbB.world_position = b1.world_position;
	obbB.u_axes[0].x = m._11; obbB.u_axes[0].y = m._12; obbB.u_axes[0].z = m._13;
	obbB.u_axes[1].x = m._21; obbB.u_axes[1].y = m._22; obbB.u_axes[1].z = m._23;
	obbB.u_axes[2].x = m._31; obbB.u_axes[2].y = m._32; obbB.u_axes[2].z = m._33;
	obbB.half_width = b1.half_size * b1.world_scale;
	obbB.world_orientation = b1.world_orientation;
	assert(!isnan(obbB.world_orientation.norm()));

	float smallest_penetration = FLT_MAX;	//最小めり込み量
	int smallest_axis[2];	//最小めり込み量を得た分離軸の作成に使用した各OBBのローカル軸番号 辺×辺用に2つ
	SAT_TYPE smallest_case;	//衝突の種類 
	if (!sat_obb_obb(obbA, obbB, smallest_penetration, smallest_axis, smallest_case)) return false;

	//obb1の頂点がobb0の面と衝突した場合
	if (smallest_case == POINTB_FACETA)
	{
		Vector3 d = obbB.world_position - obbA.world_position;	//obb0からobb1への相対位置
		Vector3 n = obbA.u_axes[smallest_axis[0]];	//obb0の衝突面の法線と平行のobb0のローカル軸ベクトル
		if (vector3_dot(n, d) < 0)	//obb0とobb1の位置関係より衝突面の法線ベクトルを決定する
		{
			n = n * -1.0f;
		}
		n = n.unit_vect();

		//box1のローカル座標系での最近点(p1) obb1の8頂点のうちのどれか
		Vector3 p1 = obbB.half_width;	//obb1の各辺の長さは、obb1の重心から接触点(p)への相対位置の手がかりになる
		//obb0とobb1の位置関係(d)より接触点(p)を求める
		if (vector3_dot(obbB.u_axes[0], n) > 0) p1.x = -p1.x;
		if (vector3_dot(obbB.u_axes[1], n) > 0) p1.y = -p1.y;
		if (vector3_dot(obbB.u_axes[2], n) > 0) p1.z = -p1.z;

		//box0の逆行列の作成
		matrix rotate, inverse_rotate;
		rotate = b0.world_orientation.get_rotate_matrix();
		rotate._41 = b0.world_position.x; //transpseの入力
		rotate._42 = b0.world_position.y;
		rotate._43 = b0.world_position.z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p1をobb0のローカル座標系へ
		Vector3 P = vector3_quatrotate(p1, b1.world_orientation) + b1.world_position;
		Vector3 c = vector3_trans(P, inverse_rotate);

		//obb0の最近点を求める
		Vector3 p0 = c;
		Vector3 box_halfsize = b0.half_size * b0.world_scale;
		if (c.x > +box_halfsize.x)p0.x = +box_halfsize.x;
		if (c.x < -box_halfsize.x)p0.x = -box_halfsize.x;

		if (c.y > +box_halfsize.y)p0.y = +box_halfsize.y;
		if (c.y < -box_halfsize.y)p0.y = -box_halfsize.y;

		if (c.z > +box_halfsize.z)p0.z = +box_halfsize.z;
		if (c.z < -box_halfsize.z)p0.z = -box_halfsize.z;

		pair.contacts.addcontact(
			smallest_penetration,
			-n,
			p0,
			p1
		);
	}
	//②obb0の頂点がobb1の面と衝突した場合
	else if (smallest_case == POINTA_FACETB)
	{
		Vector3 d = obbB.world_position - obbA.world_position;
		Vector3 n = obbB.u_axes[smallest_axis[1]];
		if (vector3_dot(n, d) < 0)
		{
			n = n * -1.0f;
		}
		n = n.unit_vect();

		Vector3 p0 = obbA.half_width;
		if (vector3_dot(obbA.u_axes[0], -n) > 0) p0.x = -p0.x;
		if (vector3_dot(obbA.u_axes[1], -n) > 0) p0.y = -p0.y;
		if (vector3_dot(obbA.u_axes[2], -n) > 0) p0.z = -p0.z;

		//box0の逆行列の作成
		matrix rotate, inverse_rotate;
		rotate = b1.world_orientation.get_rotate_matrix();
		rotate._41 = b1.world_position.x; //transpseの入力
		rotate._42 = b1.world_position.y;
		rotate._43 = b1.world_position.z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p0をobb0のローカル座標系へ
		Vector3 P = vector3_quatrotate(p0, b0.world_orientation) + b0.world_position;
		Vector3 c = vector3_trans(P, inverse_rotate);

		//obb0の最近点を求める
		Vector3 p1 = c;
		Vector3 box_halfsize = b1.half_size * b1.world_scale;
		if (c.x > +box_halfsize.x)p1.x = +box_halfsize.x;
		if (c.x < -box_halfsize.x)p1.x = -box_halfsize.x;

		if (c.y > +box_halfsize.y)p1.y = +box_halfsize.y;
		if (c.y < -box_halfsize.y)p1.y = -box_halfsize.y;

		if (c.z > +box_halfsize.z)p1.z = +box_halfsize.z;
		if (c.z < -box_halfsize.z)p1.z = -box_halfsize.z;

		if (p0.y > 1) {
			int dafsgdf = 0;
		}
		pair.contacts.addcontact(
			smallest_penetration,
			-n,
			p0,
			p1
		);
	}
	//③obb0の辺とobb1の辺と衝突した場合
	else if (smallest_case == EDGE_EDGE)
	{

		Vector3 d = obbB.world_position - obbA.world_position;
		Vector3 n;
		n = vector3_cross(obbA.u_axes[smallest_axis[0]], obbB.u_axes[smallest_axis[1]]);
		n = n.unit_vect();
		if (vector3_dot(n, d) < 0)
		{
			n = n * -1.0f;
		}

		Vector3 p[2] = { obbA.half_width, obbB.half_width };
		{
			if (vector3_dot(obbA.u_axes[0], -n) > 0) p[0].x = -p[0].x;
			if (vector3_dot(obbA.u_axes[1], -n) > 0) p[0].y = -p[0].y;
			if (vector3_dot(obbA.u_axes[2], -n) > 0) p[0].z = -p[0].z;

			if (vector3_dot(obbB.u_axes[0], +n) > 0) p[1].x = -p[1].x;
			if (vector3_dot(obbB.u_axes[1], +n) > 0) p[1].y = -p[1].y;
			if (vector3_dot(obbB.u_axes[2], +n) > 0) p[1].z = -p[1].z;
		}

		Vector3 P0a = vector3_quatrotate(p[0], obbA.world_orientation) + obbA.world_position;
		Vector3 P1a = vector3_quatrotate(p[1], obbB.world_orientation) + obbB.world_position;

		float s, t;
		Closest_func::get_closestP_two_line(
			P0a, obbA.u_axes[smallest_axis[0]],
			P1a, obbB.u_axes[smallest_axis[1]],
			s, t
		);
		Vector3 b_axis[3]{
			Vector3(1,0,0),
			Vector3(0,1,0),
			Vector3(0,0,1)
		};
		pair.contacts.addcontact(
			smallest_penetration,
			-n,
			p[0] + s * b_axis[smallest_axis[0]],
			p[1] + t * b_axis[smallest_axis[1]]
		);
	}
	else assert(0);

	return true;
}
#pragma endregion

#pragma region BOX-MESH
bool physics_function::generate_contact_box_mesh(const Box& S1, const Meshcoll& S2, Contacts::Contact_pair& pair) {
	return true;
}
#pragma endregion


#pragma region MESH-PLANE
bool physics_function::generate_contact_mesh_plane(const Meshcoll& S1, const Plane& S2, Contacts::Contact_pair& pair) {
	return true;
}
#pragma endregion

#pragma region MESH-MESH

bool GC_concave_mesh_mesh(const Meshcoll& S1, const Meshcoll& S2, int concave_num, Contacts::Contact_pair& pair) {
	return false;
}

bool physics_function::generate_contact_mesh_mesh(const Meshcoll& SA, const Meshcoll& SB, Contacts::Contact_pair& pair) {
	float smallest_penetration = FLT_MAX;	//最小めり込み量
	int smallest_facetID[2];	//最小めり込み量を得た分離軸の作成に使用した各OBBのローカル軸番号 辺×辺用に2つ
	SAT_TYPE smallest_case;		//衝突の種類 

	if (SA.is_Convex == true && SB.is_Convex == true) {
		if (!sat_convex_mesh_mesh(SA, SB, smallest_penetration, smallest_facetID, smallest_case))return false;

		Quaternion offset_quatBA = SB.world_orientation * SA.world_orientation.conjugate();
		Quaternion offset_quatAB = SA.world_orientation * SB.world_orientation.conjugate();
		Vector3 offset_posBA = SB.world_position - SA.world_position;
		Vector3 offset_posAB = SA.world_position - SB.world_position;

		//SBの頂点がSAの面と衝突した場合
		if (smallest_case == POINTB_FACETA)
		{
			const Meshcoll& facet_coll = SA;
			const Meshcoll& vertex_coll = SB;

			Quaternion offset_quatVF = vertex_coll.world_orientation * facet_coll.world_orientation.conjugate();
			Quaternion offset_quatFV = facet_coll.world_orientation * vertex_coll.world_orientation.conjugate();
			Vector3 offset_posVF = vertex_coll.world_position - facet_coll.world_position;
			Vector3 offset_posFV = facet_coll.world_position - vertex_coll.world_position;

			assert(smallest_facetID[1] == -1);
			const Facet& nerest_facet = facet_coll.facets[smallest_facetID[0]];

			Vector3 axisF = nerest_facet.normal.unit_vect();
			Vector3 axisW = vector3_quatrotate(axisF, facet_coll.world_orientation).unit_vect();
			if (vector3_dot(axisW, offset_posAB) < 0) {
				axisF = -axisF;
				axisW = -axisW;
			}

			//vertex_collのどの頂点が最近点か求める
			u_int nearest_pointID;
			Vector3 pB;
			{
				float max_len = -FLT_MAX;
				Vector3 axisV = vector3_quatrotate(axisF, offset_quatFV);

				for (u_int v_num = 0; v_num < vertex_coll.vertex_num; v_num++) {

					if (vector3_dot((*vertex_coll.vertices)[v_num], axisV) > max_len) {
						max_len = vector3_dot((*vertex_coll.vertices)[v_num], axisV);
						nearest_pointID = v_num;
					}
				}
			}
			pB = (*vertex_coll.vertices)[nearest_pointID];
			pB *= vertex_coll.world_scale;

			//上記のp0がfacet_collの最近面上のどこにあるのか
			Vector3 pA;
			{
				
				Vector3 p = vector3_quatrotate(vector3_quatrotate(pB, vertex_coll.world_orientation) + offset_posVF,facet_coll.world_orientation.conjugate());
				p /= facet_coll.world_scale;
				float min_len = FLT_MAX;
				Vector3 n_pos;


				for (u_int f_num = 0; f_num < facet_coll.facet_num; f_num++) {

					if (vector3_dot(facet_coll.facets[f_num].normal, -axisF) < 0.5)continue; //衝突法線と比べて

					//メッシュと点の最近点を求める
					get_closestP_point_triangle(p,
						(*facet_coll.vertices)[facet_coll.facets[f_num].vertexID[0]],
						(*facet_coll.vertices)[facet_coll.facets[f_num].vertexID[1]],
						(*facet_coll.vertices)[facet_coll.facets[f_num].vertexID[2]],
						facet_coll.facets[f_num].normal,
						n_pos
					);

					if ((p - n_pos).norm() < min_len) {
						pA = n_pos;
						min_len = (p - n_pos).norm();
					}
				}

				pA*= facet_coll.world_scale;
			}

			pair.contacts.addcontact(
				smallest_penetration,
				axisW,
				pA,
				pB
			);
		}

		//SAの頂点がSBの面と衝突した場合
		if (smallest_case == POINTA_FACETB)
		{
			const Meshcoll& facet_coll = SB;
			const Meshcoll& vertex_coll = SA;

			Quaternion offset_quatVF = vertex_coll.world_orientation * facet_coll.world_orientation.conjugate();
			Quaternion offset_quatFV = facet_coll.world_orientation * vertex_coll.world_orientation.conjugate();
			Vector3 offset_posVF = vertex_coll.world_position - facet_coll.world_position;
			Vector3 offset_posFV = facet_coll.world_position - vertex_coll.world_position;

			assert(smallest_facetID[0] == -1);
			const Facet& nerest_facet = facet_coll.facets[smallest_facetID[1]];

			Vector3 axisF = nerest_facet.normal.unit_vect();
			Vector3 axisW = vector3_quatrotate(axisF, facet_coll.world_orientation).unit_vect();
			if (vector3_dot(axisW, offset_posBA) < 0) {
				axisF = -axisF;
				axisW = -axisW;
			}

			//vertex_collのどの頂点が最近点か求める
			u_int nearest_pointID;
			Vector3 pB;
			{
				float max_len = -FLT_MAX;
				Vector3 axisV = vector3_quatrotate(axisF, offset_quatFV);

				for (u_int v_num = 0; v_num < vertex_coll.vertex_num; v_num++) {

					if (vector3_dot((*vertex_coll.vertices)[v_num], axisV) > max_len) {
						max_len = vector3_dot((*vertex_coll.vertices)[v_num], axisV);
						nearest_pointID = v_num;
					}
				}
			}
			pB = (*vertex_coll.vertices)[nearest_pointID];
			pB *= vertex_coll.world_scale;

			//上記のpBがfacet_collの最近面上のどこにあるのか
			Vector3 pA;
			{

				Vector3 p = vector3_quatrotate(vector3_quatrotate(pB, vertex_coll.world_orientation) + offset_posVF, facet_coll.world_orientation.conjugate());
				p /= facet_coll.world_scale;
				float min_len = FLT_MAX;
				Vector3 n_pos;


				for (u_int f_num = 0; f_num < facet_coll.facet_num; f_num++) {

					if (vector3_dot(facet_coll.facets[f_num].normal, -axisF) < 0.5)continue; //衝突法線と比べて

					//メッシュと点の最近点を求める
					get_closestP_point_triangle(p,
						(*facet_coll.vertices)[facet_coll.facets[f_num].vertexID[0]],
						(*facet_coll.vertices)[facet_coll.facets[f_num].vertexID[1]],
						(*facet_coll.vertices)[facet_coll.facets[f_num].vertexID[2]],
						facet_coll.facets[f_num].normal,
						n_pos
					);

					if ((p - n_pos).norm() < min_len) {
						pA = n_pos;
						min_len = (p - n_pos).norm();
					}
				}

				pA *= facet_coll.world_scale;
			}

			pair.contacts.addcontact(
				smallest_penetration,
				-axisW,
				pB,
				pA
			);
		}

		
		//SAとSBの辺同士が衝突した場合
		else if (smallest_case == EDGE_EDGE)
		{
			Quaternion offset_quatAB = SA.world_orientation * SB.world_orientation.conjugate();
			Quaternion offset_quatBA = SB.world_orientation * SA.world_orientation.conjugate();
			Vector3 offset_posAB = SA.world_position - SB.world_position;
			Vector3 offset_posBA = SB.world_position - SA.world_position;

			const Edge& edgeA = SA.edges[smallest_facetID[0]];
			const Edge& edgeB = SB.edges[smallest_facetID[1]];

			Vector3 edgeA_p[2] = {
				{(*SA.vertices)[edgeA.vertexID[0]] * SA.world_scale},
				{(*SA.vertices)[edgeA.vertexID[1]] * SA.world_scale}
			};
			Vector3 edgeB_p[2] = {
				{(*SB.vertices)[edgeB.vertexID[0]] * SB.world_scale},
				{(*SB.vertices)[edgeB.vertexID[1]] * SB.world_scale}
			};

			Vector3 edgeA_vec = (edgeA_p[0] - edgeA_p[1]).unit_vect();
			Vector3 edgeB_vec = (edgeB_p[0] - edgeB_p[1]).unit_vect();

			//SBの情報をSAの座標系に持ってきた
			//Vector3 edgeB_p_A = vector3_quatrotate(edgeB_p[0], offset_quatBA) + offset_posBA;
			Vector3 edgeB_p_A = vector3_quatrotate(vector3_quatrotate(edgeB_p[0], SB.world_orientation) + offset_posBA,SA.world_orientation.conjugate());
			Vector3 edgeB_v_A = vector3_quatrotate(edgeB_vec, offset_quatBA);

			//SAの座標系でaxisの生成
			Vector3 axisA = vector3_cross(edgeA_vec, edgeB_v_A);
			axisA = axisA.unit_vect();

			//axisをworld座標系へ
			Vector3 axisW = vector3_quatrotate(axisA, SA.world_orientation);

			//axisの向きをSB->SAの向きへ
			if (vector3_dot(axisW, offset_posAB) < 0)
			{
				axisA = axisA * -1.0f;
				axisW = axisW * -1.0f;
			}

			//SAの座標系に持ってきて直線と直線の最近点を獲得
			float s, t;
			Closest_func::get_closestP_two_line(
				edgeA_p[0], edgeA_vec,
				edgeB_p_A, edgeB_v_A,
				s, t
			);
			pair.contacts.addcontact(
				smallest_penetration,
				axisW,
				edgeA_p[0] + s * edgeA_vec,
				edgeB_p[0] + t * edgeB_vec
			);
		}

	}
	else {
	return false;
	}




	return true;
}

#pragma endregion

#pragma endregion
//:::::::::::::::::::::::::::