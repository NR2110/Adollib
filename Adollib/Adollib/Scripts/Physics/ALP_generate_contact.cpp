#include "ALP_generate_contact.h"

#include "../Imgui/work_meter.h"

#include "../Imgui/imgui_all.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

using namespace Closest_func;

//:::::::::::::::::::::::::::
#pragma region generate_contact
//:::::::::::::::::::::::::::

void Physics_function::generate_contact(std::vector<Contacts::Contact_pair>& pairs) {

	for (auto& pair : pairs) {

		const ALP_Collider_mesh* meshA = pair.body[0];
		const ALP_Collider_mesh* meshB = pair.body[1];
		const std::list<ALP_Collider>::iterator& shapeA = pair.body[0]->ALPcollider;
		const std::list<ALP_Collider>::iterator& shapeB = pair.body[1]->ALPcollider;

		if (shapeA->shape == ALP_Collider_shape::Sphere) {

			if (shapeB->shape == ALP_Collider_shape::Sphere)generate_contact_sphere_sphere(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::BOX)	generate_contact_sphere_box(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Capsule)generate_contact_sphere_capsule(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Plane)	generate_contact_sphere_plane(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Mesh)	generate_contact_sphere_mesh(meshA, meshB, pair);
		}
		if (shapeA->shape == ALP_Collider_shape::BOX) {

			if (shapeB->shape == ALP_Collider_shape::Sphere)generate_contact_sphere_box(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::BOX)	generate_contact_box_box(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Capsule)generate_contact_box_capsule(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Plane)	generate_contact_box_plane(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Mesh)	generate_contact_box_mesh(meshA, meshB, pair);
		}
		if (shapeA->shape == ALP_Collider_shape::Capsule) {
			if (shapeB->shape == ALP_Collider_shape::Sphere)generate_contact_sphere_capsule(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::BOX)generate_contact_box_capsule(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::Capsule)generate_contact_capsule_capsule(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Mesh)generate_contact_capsule_mesh(meshA, meshB, pair);
		}
		if (shapeA->shape == ALP_Collider_shape::Plane) {

			if (shapeB->shape == ALP_Collider_shape::Sphere)generate_contact_sphere_plane(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::BOX)	generate_contact_box_plane(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::Plane) {}
		}
		if (shapeA->shape == ALP_Collider_shape::Mesh) {

			if (shapeB->shape == ALP_Collider_shape::Sphere)generate_contact_sphere_mesh(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::Capsule)generate_contact_capsule_mesh(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::BOX)	generate_contact_box_mesh(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::Plane)	generate_contact_mesh_plane(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Mesh)	generate_contact_mesh_mesh(meshA, meshB, pair);
		}


	}
}


#pragma region data

//分離軸判定を用いる際に使用する
enum class SAT_TYPE {
	POINTA_FACETB,
	POINTB_FACETA,
	EDGE_EDGE
};
struct Ret_S {
	float penetrate = FLT_MAX;
	int smallest_axis[2] = { -1,-1 };
	SAT_TYPE smallest_case = SAT_TYPE::EDGE_EDGE;
	bool hit_point_to_face = true;
};

struct OBB {
	Vector3 world_position; //中心座標
	Vector3 u_axes[3]; //軸の向き
	Quaternion world_orientation;
	Vector3 half_width; //軸ごとの辺の長さ
};

struct Triangle {
	Vector3 world_position; //triangleの中心座標
	Quaternion world_orientation;
	Vector3 world_scale;
	Vector3 normal; //triangleの法線(向き)
	Vector3 world_normal;
	Vector3 vertex_position[3]; //triangleのlocalな頂点座標
	Vector3 world_vertex_position[3]; //triangleのworldな頂点座標

	int Edge_num[3] = { 0,0,0 };
};

//軸に投影した長さ
float sum_of_projected_radii(const OBB& obb, const Vector3& vec) {
	return
		fabsf(vector3_dot(vec, obb.half_width.x * obb.u_axes[0])) +
		fabsf(vector3_dot(vec, obb.half_width.y * obb.u_axes[1])) +
		fabsf(vector3_dot(vec, obb.half_width.z * obb.u_axes[2]));
}

float sum_of_projected_radii(const Triangle& tri, const Vector3& Wvec, float& min, float& max) {

	float A = vector3_dot(Wvec, tri.world_vertex_position[0]);
	float B = vector3_dot(Wvec, tri.world_vertex_position[1]);
	float C = vector3_dot(Wvec, tri.world_vertex_position[2]);

	min = ALmin3(A, B, C);
	max = ALmax3(A, B, C);

	return min;
}
float sum_of_projected_radii(float& max, float& min, const ALP_Collider_mesh* meshcoll, const Vector3& nor) {
	float value;
	max = -FLT_MAX;
	min = +FLT_MAX;
	for (const Vector3& vertex : meshcoll->mesh->vertices) {
		value = vector3_dot(vertex * meshcoll->ALPcollider->world_scale(), nor);
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
			PB_FA.smallest_case = SAT_TYPE::POINTB_FACETA;
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
				PA_FB.smallest_case = SAT_TYPE::POINTA_FACETB;
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
			if (axis.norm() < 1) axis *= (1 + FLT_EPSILON);

			ra = fabsf(sum_of_projected_radii(obbA, axis));
			rb = fabsf(sum_of_projected_radii(obbB, axis));
			//if (vector3_dot(axis, distBtoA) + rb < ra)continue;

			penetration = ra + rb - fabsf(vector3_dot(axis, distBtoA));
			if (penetration < 0) return false;
			if (smallest_penetration > penetration) {

				smallest_penetration = penetration;
				smallest_axis[0] = OB1;
				smallest_axis[1] = OB2;
				smallest_case = SAT_TYPE::EDGE_EDGE;
			}
		}
	}

	// 分離軸が見当たらない場合OBBは交差しているはず
	return (smallest_penetration < FLT_MAX&& smallest_penetration > FLT_EPSILON) ? true : false;
}


//OBBとTriangleの分離軸判定 交差していればtrueを返す ついでに色々引数に入れる
bool sat_obb_Triangle(
	const OBB& obb, const Triangle& triangle, const std::vector<Physics_function::Meshcollider_mesh>::iterator& mesh,
	float& smallest_penetration, //最小の貫通量
	int smallest_axis[2], //どの軸で最近になったか(edge×edge用に2つ分用意)
	SAT_TYPE& smallest_case //どのような形で最近になっているか
) {
	smallest_penetration = FLT_MAX;

	Ret_S PB_FA, PA_FB;

	float penetration = 0; //貫通量
	float ra; //obb,triangleのLに投影された長さ
	Vector3 axis; //投影する軸
	const Vector3 offset_tri_obb = triangle.world_position - obb.world_position; //triangleからobbへのoffset

	//::: obbの軸にtriangleを投影 :::::::::::::::::::
	// obbを0,0に持ってくる(回転はworldのままで)
	for (int i = 0; i < 3; i++)
	{
		axis = obb.u_axes[i];
		ra = fabsf(obb.half_width[i]);

		//triangleの軸方向の各長さ
		float trimin, trimax;
		sum_of_projected_radii(triangle, axis, trimin, trimax);

		//距離の軸方向への長さ
		const float off = vector3_dot(axis, offset_tri_obb);

		//軸に投影した貫通量を求める
		penetration = FLT_MAX;
		penetration = ALmin(penetration, ra - (trimin + off));
		penetration = ALmin(penetration, (trimax + off) - (-ra));

		//axis* mul_valueがmesh->obbの方向になるように値を保存
		int mul_value = 1;
		if (ra - (trimin + off) < (trimax + off) - (-ra)) mul_value *= -1;

		if (penetration < 0) return false; //分離してる

		if (PB_FA.penetrate > penetration) {
			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = i;
			PB_FA.smallest_axis[1] = mul_value; //ずるいけどここを使ってデータを渡す
			PB_FA.smallest_case = SAT_TYPE::POINTB_FACETA;
		}
	}

	//::: triangleの法線にobbを投影 ::::::::::::::::::::
	//obbを0,0に持ってくる(回転はworldのままで)
	if (PA_FB.hit_point_to_face == true) {

		Vector3 triangle_world_normal = vector3_quatrotate(triangle.normal, triangle.world_orientation);
		for (int i = 0; i < 3; i++)
		{
			//辺と法線の外積で分離軸を行う
			axis = vector3_cross(triangle_world_normal, (triangle.world_vertex_position[i] - triangle.world_vertex_position[(i + 1) % 3]).unit_vect());
			if (vector3_dot(axis, triangle.world_position) > vector3_dot(axis, triangle.world_vertex_position[i]))
				axis *= -1;

			if (vector3_dot(axis, triangle.world_vertex_position[i]) > vector3_dot(axis, triangle.world_vertex_position[(i + 2) % 3])) axis *= -1; //axisはtriangle辺から中心に向けて

			//obbの軸方向の長さ
			ra = fabsf(sum_of_projected_radii(obb, axis));

			//triangleの軸方向の各長さ
			float trimin, trimax;
			sum_of_projected_radii(triangle, axis, trimin, trimax);

			//距離の軸方向への長さ
			const float off = vector3_dot(axis, offset_tri_obb);

			//軸に投影した貫通量を求める
			penetration = FLT_MAX;
			penetration = ALmin(penetration, ra - (trimin + off));
			penetration = ALmin(penetration, (trimax + off) - (-ra));

			if (penetration < 0) return false; //分離している

			if (trimin + off < -ra && +ra < trimax + off) {
				PB_FA.hit_point_to_face = false; //triang1がでかい->triangleの頂点はobbに交差しない
			}

			if (PA_FB.penetrate > penetration) {

				PA_FB.penetrate = penetration;
				PA_FB.smallest_axis[0] = -1;
				PA_FB.smallest_axis[1] = i;
				PA_FB.smallest_case = SAT_TYPE::POINTA_FACETB;
			}
		}

		{
			axis = triangle_world_normal;
			ra = fabsf(sum_of_projected_radii(obb, axis));
			//fabsf(vector3_dot(axis, triangle.vertex_position[0])); //もし裏からの衝突を検知しないのであればここを変える

			float trimin, trimax;
			sum_of_projected_radii(triangle, axis, trimin, trimax);

			const float off = vector3_dot(axis, offset_tri_obb);

			penetration = FLT_MAX;
			//penetration = ALmin(penetration, ra - (trimin + off));   //裏からの衝突は検知しない
			penetration = ALmin(penetration, (trimax + off) - (-ra));

			if (penetration < 0) return false; //分離している

			if (PA_FB.penetrate > penetration) {

				PA_FB.penetrate = penetration;
				PA_FB.smallest_axis[0] = -1;
				PA_FB.smallest_axis[1] = 3; //3は面法線
				PA_FB.smallest_case = SAT_TYPE::POINTA_FACETB;
			}
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

	//::: 外積の軸に投影 Waxis
	for (int OB1 = 0; OB1 < 3; OB1++) {
		for (int OB2 = 0; OB2 < 3; OB2++) {

			axis = vector3_cross(obb.u_axes[OB1], (triangle.world_vertex_position[OB2] - triangle.world_vertex_position[(OB2 + 1) % 3]).unit_vect());
			if (axis.norm() <= FLT_EPSILON * FLT_EPSILON)continue;//外積が 0 = 平行

			//meshからobbの向きにaxisを変更(裏から衝突しないようにするため)
			if (vector3_dot(axis, obb.world_position - (triangle.world_vertex_position[OB2] + triangle.world_position)) < 0)//めり込んでいることを忘れずに!!!!
			{
				axis = axis * -1.0f;
			}
			axis = axis.unit_vect();
			if (axis.norm() <= 1) axis *= (1 - FLT_EPSILON);



			//Vector3 obb_vertex_pos;
			//if (vector3_dot(triangle.world_vertex_position[OB2],axis) < vector3_dot(obb.))

			ra = fabsf(sum_of_projected_radii(obb, axis));

			//triangleの軸方向の各長さ
			float trimin, trimax;
			sum_of_projected_radii(triangle, axis, trimin, trimax);

			//距離の軸方向への長さ
			const float off = vector3_dot(axis, offset_tri_obb);

			//軸に投影した貫通量を求める
			penetration = FLT_MAX;
			//penetration = ALmin(penetration, ra - (trimin + off));  //裏からは衝突しない
			penetration = ALmin(penetration, (trimax + off) - (-ra));

			if (penetration < 0) return false; //分離してる

			if (vector3_dot(axis, triangle.world_normal) < 0)continue; //裏からは衝突しない
			if (mesh->edges[triangle.Edge_num[OB2]].type == Edgetype::EdgeConcave) continue; //へこみのEdgeは衝突しない
			if (mesh->edges[triangle.Edge_num[OB2]].type == Edgetype::EdgeFlat) continue; //平面のEdgeは衝突しない

			if (smallest_penetration - penetration > FLT_EPSILON) {

				smallest_penetration = penetration;
				smallest_axis[0] = OB1;
				smallest_axis[1] = OB2;
				smallest_case = SAT_TYPE::EDGE_EDGE;

				if (penetration < 0) return false; //分離してる
			}
		}
	}

	// 分離軸が見当たらない場合OBBは交差しているはず
	return (smallest_penetration < FLT_MAX&& smallest_penetration > FLT_EPSILON) ? true : false;
}

//どちらも凸包の場合
bool sat_convex_mesh_mesh(const ALP_Collider_mesh* meshA, const ALP_Collider_mesh* meshB,
	float& smallest_penetration, //最小の貫通量
	int smallest_axis[2], //どの軸で最近になったか(edge×edge用に2つ分用意)
	SAT_TYPE& smallest_case //どのような形で最近になっているか
) {
	smallest_penetration = FLT_MAX;

	std::list<ALP_Collider>::iterator collA = meshA->ALPcollider;
	std::list<ALP_Collider>::iterator collB = meshB->ALPcollider;

	// Meshcoll_1の各面法線を分離軸とする
	Quaternion offset_quatBA = collB->world_orientation() * collA->world_orientation().inverse();
	Quaternion offset_quatAB = collA->world_orientation() * collB->world_orientation().inverse();
	Vector3 offset_posBA = collB->world_position() - collA->world_position();
	Vector3 offset_posAB = collA->world_position() - collB->world_position();

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;
	float penetration = FLT_MAX;

	////::: obbAの軸にobbBを投影 :::::::::::::::::::
		//collAの座標系で計算を行う
	float maxA, minA, maxB, minB;
	for (u_int f = 0; f < meshA->mesh->facet_num; f++) {
		const Facet& facet = meshA->mesh->facets.at(f); //meshcoord
		const Vector3& axis = facet.normal; //meshcoord

		// collAを分離軸に投影
		sum_of_projected_radii(maxA, minA, meshA, axis);
		assert(maxA >= minA);

		// collBを分離軸に投影
		sum_of_projected_radii(maxB, minB, meshB, vector3_quatrotate(axis, offset_quatAB).unit_vect());
		assert(maxB >= minB);
		float off = vector3_dot(offset_posBA, vector3_quatrotate(axis, collA->world_orientation()).unit_vect());
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
			sum_of_projected_radii(maxB, minB, meshB, vector3_quatrotate(axis, offset_quatAB).unit_vect());

			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = f;
			PB_FA.smallest_axis[1] = -1;
			PB_FA.smallest_case = SAT_TYPE::POINTB_FACETA;
		}
	}

	////::: obbBの軸にobbAを投影 :::::::::::::::::::
		//collBの座標系で計算を行う
	if (PA_FB.hit_point_to_face == true)
		for (u_int f = 0; f < meshB->mesh->facet_num; f++) {
			const Facet& facet = meshB->mesh->facets.at(f);
			const Vector3& axis = facet.normal;

			// collBを分離軸に投影
			sum_of_projected_radii(maxB, minB, meshB, axis);
			assert(maxB >= minB);

			// collAを分離軸に投影
			sum_of_projected_radii(maxA, minA, meshA, vector3_quatrotate(axis, offset_quatBA));
			assert(maxA >= minA);
			float off = vector3_dot(offset_posAB, vector3_quatrotate(axis, meshB->ALPcollider->world_orientation()).unit_vect());
			maxA += off;
			minA += off;

			//貫通の計算
			float d1 = minA - maxB;
			float d2 = minB - maxA;
			if (d1 >= 0.0f || d2 >= 0.0f)
				return false;


			penetration = -1 * ALmax(d1, d2);

			if (PA_FB.penetrate > penetration) {
				sum_of_projected_radii(maxA, minA, meshA, vector3_quatrotate(axis, offset_quatBA));

				PA_FB.penetrate = penetration;
				PA_FB.smallest_axis[0] = -1;
				PA_FB.smallest_axis[1] = f;
				PA_FB.smallest_case = SAT_TYPE::POINTA_FACETB;
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
	float save_crossnorm = 0;
	Vector3 axisA, axisB, axisW;
	for (u_int eA = 0; eA < meshA->mesh->edge_num; eA++) {
		const Edge& edgeA = meshA->mesh->edges.at(eA);
		if (edgeA.type != Edgetype::EdgeConvex) continue;

		const Vector3& edgeVecA = meshA->mesh->vertices.at(edgeA.vertexID[1]) - meshA->mesh->vertices.at(edgeA.vertexID[0]);
		for (u_int eB = 0; eB < meshB->mesh->edge_num; eB++) {
			const Edge& edgeB = meshB->mesh->edges.at(eB);
			if (edgeB.type != Edgetype::EdgeConvex) continue;

			const Vector3 edgeVecB = vector3_quatrotate(meshB->mesh->vertices.at(edgeB.vertexID[1]) - meshB->mesh->vertices.at(edgeB.vertexID[0]), offset_quatBA);

			axisA = vector3_cross(edgeVecA, edgeVecB);
			if (axisA.norm() <= FLT_EPSILON * FLT_EPSILON) continue;
			axisA = axisA.unit_vect();
			axisW = vector3_quatrotate(axisA, meshA->ALPcollider->world_orientation());
			//axisの向きをA->Bにする
			if (vector3_dot(axisW, meshB->ALPcollider->world_position() - meshA->ALPcollider->world_position()) < 0) {
				axisA *= -1;
				axisW *= -1;
			}
			axisB = vector3_quatrotate(axisA, offset_quatAB);

			sum_of_projected_radii(maxA, minA, meshA, axisA);		assert(maxA >= minA);
			sum_of_projected_radii(maxB, minB, meshB, axisB);		assert(maxB >= minB);
			float off = vector3_dot(offset_posBA, axisW);

			maxB += off;
			minB += off;

			//辺と辺の距離
			float CN = fabsf(off) - (vector3_dot(axisA, meshA->mesh->vertices.at(edgeA.vertexID[0])) + -(vector3_dot(axisB, meshB->mesh->vertices.at(edgeB.vertexID[0]))));

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
				smallest_case = SAT_TYPE::EDGE_EDGE;
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
	return (smallest_penetration < FLT_MAX&& smallest_penetration > FLT_EPSILON) ? true : false;

}

bool sat_obb_convex_mesh(const OBB& obb, const ALP_Collider_mesh* mesh,
	float& smallest_penetration, //最小の貫通量
	int smallest_axis[2], //どの軸で最近になったか(edge×edge用に2つ分用意)
	SAT_TYPE& smallest_case //どのような形で最近になっているか
) {
	smallest_penetration = FLT_MAX;

	const std::list<ALP_Collider>::iterator& mesh_coll = mesh->ALPcollider;

	// Meshcoll_1の各面法線を分離軸とする
	Quaternion offset_quatBA = mesh_coll->world_orientation() * obb.world_orientation.inverse();
	Quaternion offset_quatAB = obb.world_orientation * mesh_coll->world_orientation().inverse();
	Vector3 offset_posBA = mesh_coll->world_position() - obb.world_position;
	Vector3 offset_posAB = obb.world_position - mesh_coll->world_position();

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;
	float penetration = FLT_MAX;

	////::: obbの軸にmeshを投影 :::::::::::::::::::
		//obbの座標系で計算を行う
	float maxA, minA;
	for (u_int f = 0; f < 3; f++) {
		const Vector3& axis = obb.u_axes[f]; //worldcoord

		// obbを分離軸に投影
		float ra = obb.half_width[f];

		// meshを分離軸に投影
		sum_of_projected_radii(maxA, minA, mesh, vector3_quatrotate(axis, mesh_coll->world_orientation().inverse()).unit_vect());
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
			PB_FA.smallest_case = SAT_TYPE::POINTB_FACETA;
		}
	}

	////::: obbBの軸にobbAを投影 :::::::::::::::::::
		//meshの座標系で計算を行う
	if (PA_FB.hit_point_to_face == true)
		for (u_int f = 0; f < mesh->mesh->facet_num; f++) {
			const Facet& facet = mesh->mesh->facets.at(f);
			const Vector3& axis = facet.normal; //meshcoord

			// meshを分離軸に投影
			sum_of_projected_radii(maxA, minA, mesh, axis);
			assert(maxA >= minA);

			// obbを分離軸に投影
			float ra = sum_of_projected_radii(obb, vector3_quatrotate(axis, mesh_coll->world_orientation()));

			float off = vector3_dot(offset_posAB, vector3_quatrotate(axis, mesh_coll->world_orientation()).unit_vect());
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
				PA_FB.smallest_case = SAT_TYPE::POINTA_FACETB;
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
	Work_meter::stop("generate_edge_edge");
	// 分離軸が見当たらない場合OBBは交差しているはず
	return (smallest_penetration < FLT_MAX&& smallest_penetration > FLT_EPSILON) ? true : false;
}
//
#pragma endregion

//衝突生成
#pragma region SPHERE-SPHERE
bool Physics_function::generate_contact_sphere_sphere(const ALP_Collider_mesh* SA, const ALP_Collider_mesh* SB, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& collA = SA->ALPcollider;
	const std::list<ALP_Collider>::iterator& collB = SB->ALPcollider;

	Vector3 pA = collA->world_position();
	Vector3 pB = collB->world_position();

	//AddContact用の変数
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	//p1 から p0　方向への法線
	Vector3 n = pA - pB;
	float length = n.norm_sqr();
	n = n.unit_vect();

	if (length < collA->world_scale().x + collB->world_scale().x) {
		//衝突していたらContactオブジェクトを生成用に準備する
		is_AC = true;
		ACpenetration = collA->world_scale().x + collB->world_scale().x - length;
		ACnormal = n;
		ACcontact_pointA = collA->world_scale().x * vector3_quatrotate(-n, collA->world_orientation().inverse());
		ACcontact_pointB = collB->world_scale().x * vector3_quatrotate(+n, collB->world_orientation().inverse());
	}

	if (is_AC)
	{
		SA->ALPcollider->oncoll_bits |= SB->ALPcollider->tag;
		SB->ALPcollider->oncoll_bits |= SA->ALPcollider->tag;

		//oncoll_enterのみの場合addcontactせずにreturn
		if (pair.check_oncoll_only == true) return false;


		pair.contacts.addcontact(
			ACpenetration,
			ACnormal,
			ACcontact_pointA,
			ACcontact_pointB
		);
	}
	return is_AC;
}
#pragma endregion

#pragma region SPHERE-PLANE
bool Physics_function::generate_contact_sphere_plane(const ALP_Collider_mesh* sphere_mesh, const ALP_Collider_mesh* plane_mesh, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& sphere = sphere_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& plane = plane_mesh->ALPcollider;

	//AddContact用の変数
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	//球面と平面の衝突判定を行う
	Matrix rotate, inverse_rotate;
	rotate = plane->world_orientation().get_rotate_matrix();
	rotate._41 = plane->world_position().x; //transpseの入力
	rotate._42 = plane->world_position().y;
	rotate._43 = plane->world_position().z;
	rotate._44 = 1;
	Vector3 n(rotate._21, rotate._22, rotate._23);
	inverse_rotate = matrix_inverse(rotate);

	Vector3 p;
	p = vector3_trans(sphere->world_position(), inverse_rotate); //平面が"原点を通り"法線が(0,1,0)"の状態の時の球の中心座標

	////平面の裏からの衝突判定
	//if (half_space && p.y < 0)return 0;

	if (abs(p.y) < sphere->world_scale().x) {
		//oncoll_enterのみの場合ここでreturn
		if (pair.check_oncoll_only == true) {
			sphere_mesh->ALPcollider->oncoll_bits |= plane_mesh->ALPcollider->tag;
			plane_mesh->ALPcollider->oncoll_bits |= sphere_mesh->ALPcollider->tag;
			return false;
		}

		n = p.y > 0 ? n : -n;

		//衝突していたらContactオブジェクトを生成用に準備する
		is_AC = true;
		ACpenetration = sphere->world_scale().x - abs(p.y);
		ACnormal = n;
		ACcontact_pointA = Vector3(p.x, 0, p.z);
		ACcontact_pointB = sphere->world_scale().x * vector3_quatrotate(-n, sphere->world_orientation().inverse());

	}

	if (is_AC)
	{
		sphere_mesh->ALPcollider->oncoll_bits |= plane_mesh->ALPcollider->tag;
		plane_mesh->ALPcollider->oncoll_bits |= sphere_mesh->ALPcollider->tag;

		//oncoll_enterのみの場合ここでreturn
		if (pair.check_oncoll_only == true) return false;

		pair.contacts.addcontact(
			ACpenetration,
			ACnormal,
			ACcontact_pointA,
			ACcontact_pointB
		);
	}
	return is_AC;
}
#pragma endregion

#pragma region SPHERE-BOX
bool Physics_function::generate_contact_sphere_box(const ALP_Collider_mesh* sphere_mesh, const ALP_Collider_mesh* box_mesh, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& sphere = sphere_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& box = box_mesh->ALPcollider;

	//AddContact用の変数
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	//球とboxの衝突判定を行う
	Matrix rotate, inverse_rotate;
	rotate = matrix_world(Vector3(1, 1, 1), box->world_orientation().get_rotate_matrix(), box->world_position());
	inverse_rotate = matrix_inverse(rotate);

	Vector3 center;
	center = vector3_trans(sphere->world_position(), inverse_rotate); //boxのlocal座標系での球の中心座標

	Vector3 box_halfsize = box->world_scale() * box->half_size;

	if (
		abs(center.x) - sphere->world_scale().x > box_halfsize.x ||
		abs(center.y) - sphere->world_scale().x > box_halfsize.y ||
		abs(center.z) - sphere->world_scale().x > box_halfsize.z
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
	if (sphere->world_scale().x - distance < FLT_EPSILON) return false; //衝突していなかったらfalse

	Vector3 n = (sphere->world_position() - vector3_trans(closest_point, rotate)).unit_vect(); //boxからsphereへのベクトル
	if (vector3_dot(n, sphere->world_position() - box->world_position()) < 0)n *= -1;

	//衝突していたらContactオブジェクトを生成用に準備する
	is_AC = true;
	ACpenetration = sphere->world_scale().x - distance;
	ACnormal = -n;
	ACcontact_pointA = closest_point;
	ACcontact_pointB = sphere->world_scale().x * vector3_quatrotate(-n, sphere->world_orientation().inverse());


	if (is_AC)
	{
		sphere_mesh->ALPcollider->oncoll_bits |= box_mesh->ALPcollider->tag;
		box_mesh->ALPcollider->oncoll_bits |= sphere_mesh->ALPcollider->tag;

		//oncoll_enterのみの場合ここでreturn
		if (pair.check_oncoll_only == true) return false;

		if (pair.body[0]->ALPcollider->shape == box->shape)
			pair.contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair.contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);
	}
	return is_AC;
}
#pragma endregion

#pragma region SPHERE-Capsule
bool Physics_function::generate_contact_sphere_capsule(const ALP_Collider_mesh* sphere_mesh, const ALP_Collider_mesh* capsule_mesh, Contacts::Contact_pair& pair) {

	const std::list<ALP_Collider>::iterator& sphere = sphere_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& capsule = capsule_mesh->ALPcollider;

	//AddContact用の変数
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	//capsuleの座標系で計算する(scaleは変更しない)

	//capsule coord
	Vector3 sphere_pos_capcoord = vector3_quatrotate(sphere->world_position() - capsule->world_position(), capsule->world_orientation().inverse());

	float s;
	Closest_func::get_closestP_point_line(sphere_pos_capcoord, Vector3(0), Vector3(0, 1, 0), s);
	s = ALmax(ALmin(s, +capsule->world_scale().y), -capsule->world_scale().y); //capsuleの長さにクランプ


	//capsuleの座標系でsphere-sphereの判定を行う
	Vector3 pA = sphere_pos_capcoord;
	Vector3 pB = s * Vector3(0, 1, 0);

	//p1 から p0　方向への法線
	Vector3 n = pA - pB;
	float length = n.norm_sqr();
	n = n.unit_vect();

	if (length < sphere->world_scale().x + capsule->world_scale().x) {
		Vector3 Wn = vector3_quatrotate(n, capsule->world_orientation()); //nをcapsuleからワールドに

		//衝突していたらContactオブジェクトを生成用に入力
		is_AC = true;
		ACpenetration = sphere->world_scale().x + capsule->world_scale().x - length;
		ACnormal = +Wn;
		ACcontact_pointA = sphere->world_scale().x * vector3_quatrotate(-Wn, sphere->world_orientation().inverse());
		ACcontact_pointB = capsule->world_scale().x * n + pB;

	}

	if (is_AC)
	{
		sphere_mesh->ALPcollider->oncoll_bits |= capsule_mesh->ALPcollider->tag;
		capsule_mesh->ALPcollider->oncoll_bits |= sphere_mesh->ALPcollider->tag;

		//oncoll_enterのみの場合ここでreturn
		if (pair.check_oncoll_only == true) return false;

		if (pair.body[0]->ALPcollider->shape == sphere->shape)
			pair.contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair.contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);
	}
	return is_AC;
}
#pragma endregion

#pragma region SPHERE-MESH
bool Physics_function::generate_contact_sphere_mesh(const ALP_Collider_mesh* sphere_mesh, const ALP_Collider_mesh* mesh_mesh, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& sphere = sphere_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& mesh = mesh_mesh->ALPcollider;

	//AddContact用の変数
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	if (false && mesh_mesh->mesh->is_Convex == true) {
		//球とmeshの衝突判定を行う
		Matrix rotate, inverse_rotate;
		rotate = matrix_world(Vector3(1, 1, 1), mesh->world_orientation().get_rotate_matrix(), mesh->world_position());
		inverse_rotate = matrix_inverse(rotate);

		Vector3 center;
		center = vector3_trans(sphere->world_position(), inverse_rotate); //meshのlocal座標系での球の中心座標

		//mesh上の最近点
		Vector3 closest_point;

		closest_point = center;
		//各面の外にあれば面平面に持ってくる
		for (u_int i = 0; i < mesh_mesh->mesh->facet_num; i++) {
			const Vector3& nor = mesh_mesh->mesh->facets.at(i).normal.unit_vect();
			const Vector3& pos = mesh_mesh->mesh->vertices.at(mesh_mesh->mesh->facets.at(i).vertexID[0]) * mesh->world_scale();
			float d = vector3_dot(nor, pos) - vector3_dot(nor, closest_point);
			if (d < 0)
				closest_point += d * nor;
		}

		float distance = (closest_point - center).norm_sqr(); //最近点と球中心の距離
		if (sphere->world_scale().x - distance > FLT_EPSILON) { //float誤差も調整
			Vector3 n = (sphere->world_position() - vector3_trans(closest_point, rotate)).unit_vect(); //meshからsphereへのベクトル

			//衝突していたらContactオブジェクトを生成用に入力
			is_AC = true;
			ACpenetration = sphere->world_scale().x - distance;
			ACnormal = -n;
			ACcontact_pointA = closest_point;
			ACcontact_pointB = sphere->world_scale().x * vector3_quatrotate(n, sphere->world_orientation().inverse());
		}

	}
	else {
		//球とmeshの衝突判定を行う

		Vector3 sphere_pos_meshcoord = vector3_quatrotate(sphere->world_position() - mesh->world_position(), mesh->world_orientation().inverse()); //mesh座標系でのsphereのpos

		float min_dis = sphere->world_scale().x;//最低距離をsphereの半径に
		Vector3 closest_point; //mesh上の最近点
		bool is_hit = false; //衝突していたらtrue
		Vector3 nor_meshcoord;
		//球とmeshの判定
		for (const auto& faset : mesh_mesh->mesh->facets) {
			const Vector3& nor = (faset.normal * mesh->world_scale()).unit_vect();
			const Vector3& mesh_pos0 = mesh_mesh->mesh->vertices.at(faset.vertexID[0]) * mesh->world_scale();

			//mesh平面の"d"
			float dis = vector3_dot(nor, mesh_pos0);
			float dis_sp = vector3_dot(nor, sphere_pos_meshcoord);

			//mesh平面とsphereの距離がmin_disより大きければ衝突しない
			if (fabsf(dis_sp - dis) > min_dis) continue;

			const Vector3& mesh_pos1 = mesh_mesh->mesh->vertices.at(faset.vertexID[1]) * mesh->world_scale();
			const Vector3& mesh_pos2 = mesh_mesh->mesh->vertices.at(faset.vertexID[2]) * mesh->world_scale();

			Vector3 closest_p;
			Closest_func::get_closestP_point_triangle(
				sphere_pos_meshcoord,
				mesh_pos0,
				mesh_pos1,
				mesh_pos2,
				nor,
				closest_p
			);

			if ((sphere_pos_meshcoord - closest_p).norm() > min_dis * min_dis)continue; //保存されている距離より大きければcontinue

			//min_dis,最近点の更新
			closest_point = closest_p;
			min_dis = (sphere_pos_meshcoord - closest_point).norm_sqr();
			is_hit = true;
			nor_meshcoord = nor;
		}
		//交差していなければfalseを返す
		if (is_hit == false)return false;

		Vector3 Wn = vector3_quatrotate((sphere_pos_meshcoord - closest_point), mesh->world_orientation()).unit_vect();//meshからsphereへのベクトル
		if (vector3_dot(sphere_pos_meshcoord - closest_point, nor_meshcoord) < 0)Wn *= -1;

		is_AC = true;
		ACpenetration = sphere->world_scale().x - min_dis;
		ACnormal = +Wn;
		ACcontact_pointA = sphere->world_scale().x * vector3_quatrotate(-Wn, sphere->world_orientation().inverse());
		ACcontact_pointB = closest_point;
	}

	if (is_AC)
	{
		sphere_mesh->ALPcollider->oncoll_bits |= mesh_mesh->ALPcollider->tag;
		mesh_mesh->ALPcollider->oncoll_bits |= sphere_mesh->ALPcollider->tag;

		//oncoll_enterのみの場合ここでreturn
		if (pair.check_oncoll_only == true) return false;

		if (pair.body[0]->ALPcollider->shape == sphere->shape)
			pair.contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair.contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);
	}
	return is_AC;
}
#pragma endregion


#pragma region BOX-PLANE
bool Physics_function::generate_contact_box_plane(const ALP_Collider_mesh* box_mesh, const ALP_Collider_mesh* plane_mesh, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& box = box_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& plane = plane_mesh->ALPcollider;

	//AddContact用の変数
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	Vector3 vertices[8] = {
		// obb座標系での各頂点のローカル座標
		Vector3(-box->world_scale().x, -box->world_scale().y, -box->world_scale().z),
		Vector3(-box->world_scale().x, -box->world_scale().y, +box->world_scale().z),
		Vector3(-box->world_scale().x, +box->world_scale().y, -box->world_scale().z),
		Vector3(-box->world_scale().x, +box->world_scale().y, +box->world_scale().z),
		Vector3(+box->world_scale().x, -box->world_scale().y, -box->world_scale().z),
		Vector3(+box->world_scale().x, -box->world_scale().y, +box->world_scale().z),
		Vector3(+box->world_scale().x, +box->world_scale().y, -box->world_scale().z),
		Vector3(+box->world_scale().x, +box->world_scale().y, +box->world_scale().z)
	};

	//Boxと平面の衝突判定を行う
	Matrix rotate, inverse_rotate;
	rotate = plane->world_orientation().get_rotate_matrix();
	rotate._41 = plane->world_position().x; //transpseの入力
	rotate._42 = plane->world_position().y;
	rotate._43 = plane->world_position().z;
	rotate._44 = 1;
	Vector3 n(rotate._21, rotate._22, rotate._23);
	inverse_rotate = matrix_inverse(rotate);

	Vector3 p;
	p = vector3_trans(box->world_position(), inverse_rotate); //平面が"原点を通り"法線が(0,1,0)"の状態の時のBoxの中心座標(p.yが距離になる)
	//n = p.y > 0 ? n : -n; //ボックスが平面下にあったら法線を反対にする

	float max_penetrate = 0.0f;
	Vector3 pointbox, pointplane;
	Vector3 vs;

	for (int i = 0; i < 8; i++) {
		// 頂点のワールド座標
		vs = vector3_quatrotate(vertices[i], box->world_orientation());

		// -面法線の内積をとる(面方向への長さ)
		float rA = vector3_dot(-n, vs);

		if (max_penetrate < rA - p.y) {
			max_penetrate = rA - p.y;
			pointbox = vertices[i];

			// 頂点をplane空間に
			vs = vector3_trans(vs + box->world_position(), inverse_rotate);
			pointplane = vs;
			pointplane.y = 0;
		}
	}

	if (max_penetrate != 0) {
		is_AC = true;

		ACpenetration = max_penetrate;
		ACnormal = -n;
		ACcontact_pointA = pointplane;
		ACcontact_pointB = pointbox;
	}

	if (is_AC)
	{
		box_mesh->ALPcollider->oncoll_bits |= plane_mesh->ALPcollider->tag;
		plane_mesh->ALPcollider->oncoll_bits |= box_mesh->ALPcollider->tag;

		//oncoll_enterのみの場合ここでreturn
		if (pair.check_oncoll_only == true) return false;

		if (pair.body[0]->ALPcollider->shape == box->shape)
			pair.contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair.contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);
	}
	return is_AC;
}
#pragma endregion

#pragma region BOX-BOX

bool Physics_function::generate_contact_box_box(const ALP_Collider_mesh* bA, const ALP_Collider_mesh* bB, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& boxA = bA->ALPcollider;
	const std::list<ALP_Collider>::iterator& boxB = bB->ALPcollider;

	//AddContact用の変数
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	Matrix m;
	m = boxA->world_orientation().get_rotate_matrix();
	OBB obbA;
	obbA.world_position = boxA->world_position();
	obbA.u_axes[0].x = m._11; obbA.u_axes[0].y = m._12; obbA.u_axes[0].z = m._13;
	obbA.u_axes[1].x = m._21; obbA.u_axes[1].y = m._22; obbA.u_axes[1].z = m._23;
	obbA.u_axes[2].x = m._31; obbA.u_axes[2].y = m._32; obbA.u_axes[2].z = m._33;
	obbA.half_width = boxA->half_size * boxA->world_scale();
	obbA.world_orientation = boxA->world_orientation();
	assert(!isnan(obbA.world_orientation.norm()));

	m = boxB->world_orientation().get_rotate_matrix();
	OBB obbB;
	obbB.world_position = boxB->world_position();
	obbB.u_axes[0].x = m._11; obbB.u_axes[0].y = m._12; obbB.u_axes[0].z = m._13;
	obbB.u_axes[1].x = m._21; obbB.u_axes[1].y = m._22; obbB.u_axes[1].z = m._23;
	obbB.u_axes[2].x = m._31; obbB.u_axes[2].y = m._32; obbB.u_axes[2].z = m._33;
	obbB.half_width = boxB->half_size * boxB->world_scale();
	obbB.world_orientation = boxB->world_orientation();
	assert(!isnan(obbB.world_orientation.norm()));

	float smallest_penetration = FLT_MAX;	//最小めり込み量
	int smallest_axis[2];	//最小めり込み量を得た分離軸の作成に使用した各OBBのローカル軸番号 辺×辺用に2つ
	SAT_TYPE smallest_case;	//衝突の種類
	if (!sat_obb_obb(obbA, obbB, smallest_penetration, smallest_axis, smallest_case)) return false;

	//obbBの頂点がobbAの面と衝突した場合
	if (smallest_case == SAT_TYPE::POINTB_FACETA)
	{
		Vector3 d = obbA.world_position - obbB.world_position;	//BからA方向
		Vector3 Wn = obbA.u_axes[smallest_axis[0]];	//obbAの衝突面の法線と平行のobbAのローカル軸ベクトル
		if (vector3_dot(Wn, d) < 0)	//obbAとobbBの位置関係より衝突面の法線ベクトルを決定する
		{
			Wn = Wn * -1.0f;
		}
		Wn = Wn.unit_vect();

		//boxBのローカル座標系での最近点(p1) obb1の8頂点のうちのどれか
		Vector3 p1 = obbB.half_width;	//obb1の各辺の長さは、obb1の重心から接触点(p)への相対位置の手がかりになる
		//obb0とobb1の位置関係(d)より接触点(p)を求める
		if (vector3_dot(obbB.u_axes[0], -Wn) > 0) p1.x = -p1.x;
		if (vector3_dot(obbB.u_axes[1], -Wn) > 0) p1.y = -p1.y;
		if (vector3_dot(obbB.u_axes[2], -Wn) > 0) p1.z = -p1.z;

		//boxAの逆行列の作成
		Matrix rotate, inverse_rotate;
		rotate = boxA->world_orientation().get_rotate_matrix();
		rotate._41 = boxA->world_position().x; //transpseの入力
		rotate._42 = boxA->world_position().y;
		rotate._43 = boxA->world_position().z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p1をobbAのローカル座標系へ
		Vector3 P = vector3_quatrotate(p1, boxB->world_orientation()) + boxB->world_position();
		Vector3 c = vector3_trans(P, inverse_rotate);

		//obbAの最近点を求める
		Vector3 p0 = c;
		Vector3 box_halfsize = boxA->half_size * boxA->world_scale();
		if (c.x > +box_halfsize.x)p0.x = +box_halfsize.x;
		if (c.x < -box_halfsize.x)p0.x = -box_halfsize.x;

		if (c.y > +box_halfsize.y)p0.y = +box_halfsize.y;
		if (c.y < -box_halfsize.y)p0.y = -box_halfsize.y;

		if (c.z > +box_halfsize.z)p0.z = +box_halfsize.z;
		if (c.z < -box_halfsize.z)p0.z = -box_halfsize.z;

		p0 += vector3_quatrotate(-Wn, boxA->world_orientation().inverse()) * smallest_penetration;

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = Wn;
		ACcontact_pointA = p0;
		ACcontact_pointB = p1;
	}
	//obbAの頂点がobbBの面と衝突した場合
	else if (smallest_case == SAT_TYPE::POINTA_FACETB)
	{
		Vector3 d = obbA.world_position - obbB.world_position; //BからA方向
		Vector3 Wn = obbB.u_axes[smallest_axis[1]];
		if (vector3_dot(Wn, d) < 0) //BからA方向でなければ-1をかける
		{
			Wn = Wn * -1.0f;
		}
		Wn = Wn.unit_vect();

		Vector3 p0 = obbA.half_width;
		//obb0とobb1の位置関係(d)より接触点(p)を求める
		if (vector3_dot(obbA.u_axes[0], Wn) > 0) p0.x = -p0.x;
		if (vector3_dot(obbA.u_axes[1], Wn) > 0) p0.y = -p0.y;
		if (vector3_dot(obbA.u_axes[2], Wn) > 0) p0.z = -p0.z;

		//box0の逆行列の作成
		Matrix rotate, inverse_rotate;
		rotate = boxB->world_orientation().get_rotate_matrix();
		rotate._41 = boxB->world_position().x; //transpseの入力
		rotate._42 = boxB->world_position().y;
		rotate._43 = boxB->world_position().z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p0をobb0のローカル座標系へ
		Vector3 P = vector3_quatrotate(p0, boxA->world_orientation()) + boxA->world_position();
		Vector3 c = vector3_trans(P, inverse_rotate);

		//obb0の最近点を求める
		Vector3 p1 = c;
		Vector3 box_halfsize = boxB->half_size * boxB->world_scale();
		if (c.x > +box_halfsize.x)p1.x = +box_halfsize.x;
		if (c.x < -box_halfsize.x)p1.x = -box_halfsize.x;

		if (c.y > +box_halfsize.y)p1.y = +box_halfsize.y;
		if (c.y < -box_halfsize.y)p1.y = -box_halfsize.y;

		if (c.z > +box_halfsize.z)p1.z = +box_halfsize.z;
		if (c.z < -box_halfsize.z)p1.z = -box_halfsize.z;

		p1 += vector3_quatrotate(Wn, boxB->world_orientation().inverse()) * smallest_penetration;

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = Wn;
		ACcontact_pointA = p0;
		ACcontact_pointB = p1;
	}
	//�Bobb0の辺とobb1の辺と衝突した場合
	else if (smallest_case == SAT_TYPE::EDGE_EDGE)
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

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = -n;
		ACcontact_pointA = p[0] + s * b_axis[smallest_axis[0]];
		ACcontact_pointB = p[1] + t * b_axis[smallest_axis[1]];
	}
	else assert(0);

	if (is_AC)
	{
		bA->ALPcollider->oncoll_bits |= bB->ALPcollider->tag;
		bB->ALPcollider->oncoll_bits |= bA->ALPcollider->tag;

		//oncoll_enterのみの場合ここでreturn
		if (pair.check_oncoll_only == true) return false;

		pair.contacts.addcontact(
			ACpenetration,
			ACnormal,
			ACcontact_pointA,
			ACcontact_pointB
		);
	}
	return is_AC;
}
#pragma endregion

#pragma region BOX-Capsule
bool Physics_function::generate_contact_box_capsule(const ALP_Collider_mesh* box_mesh, const ALP_Collider_mesh* capsule_mesh, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& capsule = capsule_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& box = box_mesh->ALPcollider;

	//AddContact用の変数
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;


	Vector3 box_halfsize = box->world_scale() * box->half_size;

	//boxの座標系で計算
	Vector3 closest_box, closest_cap;
	float capsule_t = FLT_MAX;

	//box座標系でのcapsuleの情報
	Vector3 cuppos_boxcoord = vector3_quatrotate(capsule->world_position() - box->world_position(), box->world_orientation().inverse());
	Vector3 cupsca_boxcoord = vector3_quatrotate(Vector3(0, capsule->world_scale().y, 0), capsule->world_orientation() * box->world_orientation().inverse());

	bool is_crossing = false;
	{
		float tmax;
		is_crossing = Crossing_func::getCrossingP_AABB_ray(
			Vector3(0, 0, 0), box_halfsize,
			cuppos_boxcoord, cupsca_boxcoord,
			capsule_t, tmax
		);
		if (is_crossing) {
			capsule_t = ALClamp(capsule_t, -1, 1);
			closest_cap = cuppos_boxcoord + cupsca_boxcoord * capsule_t;
		}
	}

	//線分がBoxと交差していなかったらBoxとCapsuleの最近点を求める
	if (is_crossing == false)
	{
		float dis_save = FLT_MAX;

		//boxのすべての辺とカプセルの線分の総当たりで最近点を求める
		Vector3 vertex[4] = {
			{+box_halfsize.x, +box_halfsize.y, +box_halfsize.z },
			{-box_halfsize.x, +box_halfsize.y, -box_halfsize.z },
			{-box_halfsize.x, -box_halfsize.y, +box_halfsize.z },
			{+box_halfsize.x, -box_halfsize.y, -box_halfsize.z }
		};
		for (int i = 0; i < 4; i++) {
			Vector3 closest_P, closest_Q;
			float s, t;
			Closest_func::get_closestP_two_segment(vertex[i], Vector3(-vertex[i].x, +vertex[i].y, +vertex[i].z), cuppos_boxcoord - cupsca_boxcoord, cuppos_boxcoord + cupsca_boxcoord, closest_P, closest_Q, s, t);
			if ((closest_P - closest_Q).norm() < dis_save) {
				dis_save = (closest_P - closest_Q).norm();
				closest_box = closest_P;
				closest_cap = closest_Q;
				capsule_t = t * 2 - 1;
			}
			Closest_func::get_closestP_two_segment(vertex[i], Vector3(+vertex[i].x, -vertex[i].y, +vertex[i].z), cuppos_boxcoord - cupsca_boxcoord, cuppos_boxcoord + cupsca_boxcoord, closest_P, closest_Q, s, t);
			if ((closest_P - closest_Q).norm() < dis_save) {
				dis_save = (closest_P - closest_Q).norm();
				closest_box = closest_P;
				closest_cap = closest_Q;
				capsule_t = t * 2 - 1;
			}
			Closest_func::get_closestP_two_segment(vertex[i], Vector3(+vertex[i].x, +vertex[i].y, -vertex[i].z), cuppos_boxcoord - cupsca_boxcoord, cuppos_boxcoord + cupsca_boxcoord, closest_P, closest_Q, s, t);
			if ((closest_P - closest_Q).norm() < dis_save) {
				dis_save = (closest_P - closest_Q).norm();
				closest_box = closest_P;
				closest_cap = closest_Q;
				capsule_t = t * 2 - 1;
			}
		}
	}

	//衝突していなければfalseを返す
	if (
		abs(closest_cap.x) - capsule->world_scale().x > box_halfsize.x ||
		abs(closest_cap.y) - capsule->world_scale().x > box_halfsize.y ||
		abs(closest_cap.z) - capsule->world_scale().x > box_halfsize.z
		) return false;

	//box上の最近点
	closest_box = closest_cap;
	if (closest_cap.x > +box_halfsize.x)closest_box.x = +box_halfsize.x;
	if (closest_cap.x < -box_halfsize.x)closest_box.x = -box_halfsize.x;

	if (closest_cap.y > +box_halfsize.y)closest_box.y = +box_halfsize.y;
	if (closest_cap.y < -box_halfsize.y)closest_box.y = -box_halfsize.y;

	if (closest_cap.z > +box_halfsize.z)closest_box.z = +box_halfsize.z;
	if (closest_cap.z < -box_halfsize.z)closest_box.z = -box_halfsize.z;

	float leng = capsule->world_scale().x - (closest_box - closest_cap).norm_sqr(); //貫通量
	if (leng < FLT_EPSILON)return false;

	Vector3 n = (closest_cap - closest_box).unit_vect(); //boxからsphereへのベクトル boxcoord
	Vector3 Wn = vector3_quatrotate(n, box->world_orientation()); //worldcoord

	//
	is_AC = true;
	ACpenetration = leng;
	ACnormal = -Wn;
	ACcontact_pointA = closest_box;
	ACcontact_pointB = (Vector3(0, capsule->world_scale().y, 0) * capsule_t) + vector3_quatrotate(-Wn, capsule->world_orientation().inverse()) * capsule->world_scale().x;
	//vector3_quatrotate(vector3_quatrotate(closest_cap - n * capsule->world_scale().x, box->world_orientation().conjugate()) + box->world_position() - capsule->world_position(), capsule->world_orientation().conjugate())


	if (is_AC)
	{
		box_mesh->ALPcollider->oncoll_bits |= capsule_mesh->ALPcollider->tag;
		capsule_mesh->ALPcollider->oncoll_bits |= box_mesh->ALPcollider->tag;

		//oncoll_enterのみの場合ここでreturn
		if (pair.check_oncoll_only == true) return false;

		if (pair.body[0]->ALPcollider->shape == box->shape)
			pair.contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair.contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);

	}
	return is_AC;

}
#pragma endregion //TODO:なぜかバグる

#pragma region BOX-MESH

bool Physics_function::generate_contact_box_mesh(const ALP_Collider_mesh* box_mesh, const ALP_Collider_mesh* mesh_mesh, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& box = box_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& mesh = mesh_mesh->ALPcollider;

	//AddContact用の変数
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	if (0 && mesh_mesh->mesh->is_Convex == true) {}
	else {
		//boxとmeshの判定
		Matrix m;
		m = box->world_orientation().get_rotate_matrix();
		OBB obb;
		obb.world_position = box->world_position();
		obb.u_axes[0].x = m._11; obb.u_axes[0].y = m._12; obb.u_axes[0].z = m._13;
		obb.u_axes[1].x = m._21; obb.u_axes[1].y = m._22; obb.u_axes[1].z = m._23;
		obb.u_axes[2].x = m._31; obb.u_axes[2].y = m._32; obb.u_axes[2].z = m._33;
		obb.half_width = box->half_size * box->world_scale();
		obb.world_orientation = box->world_orientation();
		assert(!isnan(obb.world_orientation.norm()));

		const Vector3 box_relative_movement_from_triangle = +(box->world_position() - box->old_world_position()) - (mesh->world_position() - mesh->old_world_position());
		const Vector3 triangle_relative_movement_from_box = -(box->world_position() - box->old_world_position()) + (mesh->world_position() - mesh->old_world_position());


		Triangle smallest_triangle;
		float smallest_penetration = FLT_MAX;
		int smallest_axis[2] = { -1,-1 };
		SAT_TYPE smallest_case = SAT_TYPE::EDGE_EDGE;


		Triangle triangle;
		triangle.world_position = mesh->world_position();
		triangle.world_orientation = mesh->world_orientation();
		triangle.world_scale = mesh->world_scale();

		int debug_conut = 0;
		for (const auto& faset : mesh_mesh->mesh->facets) {
			debug_conut++;

			auto& vertices = mesh_mesh->mesh->vertices;
			triangle.vertex_position[0] = (vertices)[faset.vertexID[0]];
			triangle.vertex_position[1] = (vertices)[faset.vertexID[1]];
			triangle.vertex_position[2] = (vertices)[faset.vertexID[2]];

			triangle.world_vertex_position[0] = vector3_quatrotate(triangle.vertex_position[0] * triangle.world_scale, triangle.world_orientation);
			triangle.world_vertex_position[1] = vector3_quatrotate(triangle.vertex_position[1] * triangle.world_scale, triangle.world_orientation);
			triangle.world_vertex_position[2] = vector3_quatrotate(triangle.vertex_position[2] * triangle.world_scale, triangle.world_orientation);

			triangle.Edge_num[0] = faset.edgeID[0];
			triangle.Edge_num[1] = faset.edgeID[1];
			triangle.Edge_num[2] = faset.edgeID[2];

			triangle.normal = faset.normal;
			triangle.world_normal = vector3_quatrotate(triangle.normal, triangle.world_orientation);

			assert(!isnan(triangle.normal.norm()));

			float penetration = FLT_MAX;	//最小めり込み量
			int axis[2];	//最小めり込み量を得た分離軸の作成に使用した各OBBのローカル軸番号 辺×辺用に2つ
			SAT_TYPE s_case;	//衝突の種類
			//assert(!(isnan((obb.u_axes[0] + obb.u_axes[1] + obb.u_axes[2]).norm())));
			if (!sat_obb_Triangle(obb, triangle, mesh_mesh->mesh, penetration, axis, s_case)) continue;

			if (!sat_obb_Triangle(obb, triangle, mesh_mesh->mesh, penetration, axis, s_case)) continue;

			if (0) {
				if (smallest_case == SAT_TYPE::EDGE_EDGE ||
					s_case != SAT_TYPE::EDGE_EDGE) {

					if ((
						smallest_case == SAT_TYPE::EDGE_EDGE &&
						s_case != SAT_TYPE::EDGE_EDGE //Edgeの衝突の優先度を下げる
						) || (
							smallest_penetration > penetration //一番短い衝突
							)) {

						smallest_triangle = triangle;
						smallest_penetration = penetration;
						smallest_axis[0] = axis[0];
						smallest_axis[1] = axis[1];
						smallest_case = s_case;
					}
				}
			}
			else {
				smallest_triangle = triangle;
				smallest_penetration = penetration;
				smallest_axis[0] = axis[0];
				smallest_axis[1] = axis[1];
				smallest_case = s_case;
			}

			if (smallest_axis[0] == -1 && smallest_axis[1] == -1) continue;
		//}
		//{
		//if (smallest_axis[0] == -1 && smallest_axis[1] == -1) return false;


			//obbBの頂点がobbAの面と衝突した場合
			if (smallest_case == SAT_TYPE::POINTB_FACETA)
			{
				Vector3 Wn = obb.u_axes[smallest_axis[0]]; //衝突法線
				Wn *= smallest_axis[1]; //衝突法線をmeshからobbの方向に
				Wn = Wn.unit_vect();

				//triangleのローカル座標系での最近点(p1) triangleの3頂点のうちのどれか
				float save_dis = -FLT_MAX;
				Vector3 p1; //meshの座標系

				if (vector3_dot(smallest_triangle.world_vertex_position[0], +Wn) > save_dis) { save_dis = vector3_dot(smallest_triangle.world_vertex_position[0], +Wn);  p1 = smallest_triangle.vertex_position[0]; };
				if (vector3_dot(smallest_triangle.world_vertex_position[1], +Wn) > save_dis) { save_dis = vector3_dot(smallest_triangle.world_vertex_position[1], +Wn);  p1 = smallest_triangle.vertex_position[1]; };
				if (vector3_dot(smallest_triangle.world_vertex_position[2], +Wn) > save_dis) { save_dis = vector3_dot(smallest_triangle.world_vertex_position[2], +Wn);  p1 = smallest_triangle.vertex_position[2]; };
				p1 *= smallest_triangle.world_scale;

				//boxAの逆行列の作成
				Matrix rotate, inverse_rotate;
				rotate = box->world_orientation().get_rotate_matrix();
				rotate._41 = box->world_position().x; //transpseの入力
				rotate._42 = box->world_position().y;
				rotate._43 = box->world_position().z;
				rotate._44 = 1;
				inverse_rotate = matrix_inverse(rotate);

				//p1をobbAのローカル座標系へ
				Vector3 Wp1 = vector3_quatrotate(p1, smallest_triangle.world_orientation) + smallest_triangle.world_position;
				//Vector3 P = vector3_quatrotate(Wp1, box->world_orientation()) + box->world_position();
				Vector3 c = vector3_trans(Wp1, inverse_rotate);

				//obbAの最近点を求める
				Vector3 p0 = c;
				Vector3 box_halfsize = box->half_size * box->world_scale();
				if (c.x > +box_halfsize.x)p0.x = +box_halfsize.x;
				if (c.x < -box_halfsize.x)p0.x = -box_halfsize.x;

				if (c.y > +box_halfsize.y)p0.y = +box_halfsize.y;
				if (c.y < -box_halfsize.y)p0.y = -box_halfsize.y;

				if (c.z > +box_halfsize.z)p0.z = +box_halfsize.z;
				if (c.z < -box_halfsize.z)p0.z = -box_halfsize.z;

				p0 += vector3_quatrotate(-Wn, obb.world_orientation.inverse()) * smallest_penetration;

				is_AC = true;
				ACpenetration = smallest_penetration;
				ACnormal = Wn;
				ACcontact_pointA = p0;
				ACcontact_pointB = p1;
			}

			//obbの頂点がtriangleの面と衝突した場合
			else if (smallest_case == SAT_TYPE::POINTA_FACETB)
			{
				Vector3 Wn;
				if (smallest_axis[1] == 3)
				{
					Wn = vector3_quatrotate(smallest_triangle.normal, smallest_triangle.world_orientation); //meshからboxに向けての法線(BからA)
				}
				else
				{
					Wn = vector3_quatrotate(vector3_cross(smallest_triangle.vertex_position[smallest_axis[1]], smallest_triangle.vertex_position[(smallest_axis[1] + 1) % 3]), smallest_triangle.world_orientation);

					if (vector3_dot(Wn, smallest_triangle.vertex_position[smallest_axis[1]]) < vector3_dot(Wn, smallest_triangle.vertex_position[(smallest_axis[1] + 2) % 3]))Wn *= -1;
				}
				Wn = Wn.unit_vect();

				Vector3 p0 = obb.half_width;
				//obbとtriangleの位置関係(d)より接触点(p)を求める
				if (vector3_dot(obb.u_axes[0], Wn) > 0) p0.x = -p0.x;
				if (vector3_dot(obb.u_axes[1], Wn) > 0) p0.y = -p0.y;
				if (vector3_dot(obb.u_axes[2], Wn) > 0) p0.z = -p0.z;

				//p0をobbBのローカル座標系へ
				Vector3 WP = vector3_quatrotate(p0, box->world_orientation()) + box->world_position();
				Vector3 c = vector3_quatrotate(WP - mesh->world_position(), mesh->world_orientation().inverse());

				//obbBの最近点を求める
				Vector3 p1;
				if (0) {
					//meshと点の最近点を求める方法
					c /= mesh->world_scale();
					Closest_func::get_closestP_point_triangle(
						c,
						smallest_triangle.vertex_position[0],
						smallest_triangle.vertex_position[1],
						smallest_triangle.vertex_position[2],
						smallest_triangle.normal,
						p1
					);

					p1 *= mesh->world_scale();
				}
				else {
					//法線とめり込み量から求める方法 //こっちのほうが圧倒的に早い けど少し不正確
					p1 = c;
					p1 += Wn * smallest_penetration;
				}

				is_AC = true;
				ACpenetration = smallest_penetration;
				ACnormal = Wn;
				ACcontact_pointA = p0;
				ACcontact_pointB = p1;
			}
			//�Bobb0の辺とobb1の辺と衝突した場合
			else if (smallest_case == SAT_TYPE::EDGE_EDGE)
			{
				//Vector3 d = obbB.world_position - obbA.world_position;

				Vector3 triangle_edge_dir = (smallest_triangle.vertex_position[smallest_axis[1]] - smallest_triangle.vertex_position[(smallest_axis[1] + 1) % 3]).unit_vect();
				Vector3 world_triangle_edge_dir = vector3_quatrotate(triangle_edge_dir, smallest_triangle.world_orientation);

				Vector3 Wn;
				Wn = vector3_cross(obb.u_axes[smallest_axis[0]], world_triangle_edge_dir);
				if (vector3_dot(Wn, box->world_position() - (smallest_triangle.world_vertex_position[smallest_axis[1]] + smallest_triangle.world_position)) < 0)	//
				{
					Wn = Wn * -1.0f;
				}
				Wn = Wn.unit_vect();

				Vector3 p[2] = { obb.half_width ,smallest_triangle.vertex_position[smallest_axis[1]] };
				{
					if (vector3_dot(obb.u_axes[0], Wn) > 0) p[0].x = -p[0].x;
					if (vector3_dot(obb.u_axes[1], Wn) > 0) p[0].y = -p[0].y;
					if (vector3_dot(obb.u_axes[2], Wn) > 0) p[0].z = -p[0].z;
				}

				//world座標系でedgeとedgeの最近点を求める
				Vector3 P0a = vector3_quatrotate(p[0], obb.world_orientation) + obb.world_position;
				Vector3 P1a = vector3_quatrotate(p[1] * smallest_triangle.world_scale, smallest_triangle.world_orientation) + smallest_triangle.world_position;

				float s, t; //最近点を求めることのできるs,tを求める
				Closest_func::get_closestP_two_line(
					P0a, obb.u_axes[smallest_axis[0]],
					P1a, world_triangle_edge_dir * smallest_triangle.world_scale,
					s, t
				);

				//obbのlocal座標系での軸の向き
				Vector3 b_axis[3]{
					Vector3(1,0,0),
					Vector3(0,1,0),
					Vector3(0,0,1)
				};


				is_AC = true;
				ACpenetration = smallest_penetration;
				ACnormal = Wn;
				ACcontact_pointA = p[0] + s * b_axis[smallest_axis[0]];
				ACcontact_pointB = (p[1] + t * triangle_edge_dir) * smallest_triangle.world_scale;
			}

			else assert(0);



			if (is_AC)
			{
				box_mesh->ALPcollider->oncoll_bits |= mesh_mesh->ALPcollider->tag;
				mesh_mesh->ALPcollider->oncoll_bits |= box_mesh->ALPcollider->tag;

				//oncoll_enterのみの場合ここでreturn
				if (pair.check_oncoll_only == true) return false;

				if (pair.body[0]->ALPcollider->shape == box->shape)
					pair.contacts.addcontact(
						ACpenetration,
						ACnormal,
						ACcontact_pointA,
						ACcontact_pointB
					);
				else
					pair.contacts.addcontact(
						ACpenetration,
						-ACnormal,
						ACcontact_pointB,
						ACcontact_pointA
					);
			}

		}
		return is_AC;




		return true;
	}
}
#pragma endregion


#pragma region Capsule-Capsule
bool Physics_function::generate_contact_capsule_capsule(const ALP_Collider_mesh* SA, const ALP_Collider_mesh* SB, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& collA = SA->ALPcollider;
	const std::list<ALP_Collider>::iterator& collB = SB->ALPcollider;

	//AddContact用の変数
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	Vector3 Avec_Wco = vector3_quatrotate(Vector3(0, 1, 0), collA->world_orientation()) * collA->world_scale().y;
	Vector3 Bvec_Wco = vector3_quatrotate(Vector3(0, 1, 0), collB->world_orientation()) * collB->world_scale().y;

	float t, s;
	Closest_func::get_closestP_two_segment(
		collA->world_position() - Avec_Wco,
		collA->world_position() + Avec_Wco,
		collB->world_position() - Bvec_Wco,
		collB->world_position() + Bvec_Wco,
		s, t
	);

	Vector3 pA_Wco = collA->world_position() + Avec_Wco * (s * 2 - 1);
	Vector3 pB_Wco = collB->world_position() + Bvec_Wco * (t * 2 - 1);

	//pB から pA　方向への法線 Acoord
	Vector3 Wn = pA_Wco - pB_Wco;
	float length = Wn.norm_sqr();
	Wn = Wn.unit_vect();

	if (length < collA->world_scale().x + collB->world_scale().x) {

		//衝突していたらContactオブジェクトを生成用に準備する
		is_AC = true;
		ACpenetration = collA->world_scale().x + collB->world_scale().x - length;
		ACnormal = Wn;
		ACcontact_pointA = Vector3(0, 1, 0) * collA->world_scale().y * (s * 2 - 1) + collA->world_scale().x * vector3_quatrotate(-Wn, collA->world_orientation().inverse());
		ACcontact_pointB = Vector3(0, 1, 0) * collB->world_scale().y * (t * 2 - 1) + collB->world_scale().x * vector3_quatrotate(+Wn, collB->world_orientation().inverse());
	}


	if (is_AC)
	{
		SA->ALPcollider->oncoll_bits |= SB->ALPcollider->tag;
		SB->ALPcollider->oncoll_bits |= SA->ALPcollider->tag;

		//oncoll_enterのみの場合ここでreturn
		if (pair.check_oncoll_only == true) return false;

		if (pair.body[0]->ALPcollider->shape == collA->shape)
			pair.contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair.contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);
	}
	return is_AC;
}
#pragma endregion

#pragma region Capsule-Mesh
bool Physics_function::generate_contact_capsule_mesh(const ALP_Collider_mesh* capsule_mesh, const ALP_Collider_mesh* mesh_mesh, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& capsule = capsule_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& mesh = mesh_mesh->ALPcollider;

	//AddContact用の変数
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	if (0 && mesh_mesh->mesh->is_Convex == true) {
		////球とmeshの衝突判定を行う
		//Matrix rotate, inverse_rotate;
		//rotate = matrix_world(Vector3(1, 1, 1), mesh->world_orientation().get_rotate_matrix(), mesh->world_position());
		//inverse_rotate = matrix_inverse(rotate);

		//Vector3 center;
		//center = vector3_trans(sphere->world_position(), inverse_rotate); //meshのlocal座標系での球の中心座標

		////mesh上の最近点
		//Vector3 closest_point;

		//closest_point = center;
		////各面の外にあれば面平面に持ってくる
		//for (u_int i = 0; i < mesh_mesh->mesh->facet_num; i++) {
		//	const Vector3& nor = mesh_mesh->mesh->facets.at(i).normal.unit_vect();
		//	const Vector3& pos = mesh_mesh->mesh->vertices.at(mesh_mesh->mesh->facets.at(i).vertexID[0]) * mesh->world_scale();
		//	float d = vector3_dot(nor, pos) - vector3_dot(nor, closest_point);
		//	if (d < 0)
		//		closest_point += d * nor;
		//}

		//float distance = (closest_point - center).norm_sqr(); //最近点と球中心の距離
		//if (sphere->world_scale().x - distance > FLT_EPSILON) { //float誤差も調整
		//	Vector3 n = (sphere->world_position() - vector3_trans(closest_point, rotate)).unit_vect(); //meshからsphereへのベクトル

		//	if (pair.body[0]->ALPcollider->shape == mesh->shape) {
		//		//body[0]　が　mesh
		//		//body[1]　が　sphere
		//		pair.contacts.addcontact(
		//			sphere->world_scale().x - distance,
		//			-n,
		//			closest_point,
		//			sphere->world_scale().x * vector3_quatrotate(n, sphere->world_orientation().conjugate())
		//		);
		//	}
		//	else {
		//		//body[0]　が　sphere
		//		//body[1]　が　mesh
		//		pair.contacts.addcontact(
		//			sphere->world_scale().x - distance,
		//			n,
		//			sphere->world_scale().x * vector3_quatrotate(n, sphere->world_orientation().conjugate()),
		//			closest_point
		//		);
		//	}
		//	return true;
		//}
	}
	else {
		//球とmeshの衝突判定を行う 球がゆがむと面倒なのでscaleはworldのままで
		Vector3 capsule_pos_meshcoord = vector3_quatrotate(capsule->world_position() - mesh->world_position(), mesh->world_orientation().inverse()); //mesh座標系でのsphereのpos
		Vector3 capsule_dir_meshcoord = vector3_quatrotate(Vector3(0, 1, 0), capsule->world_orientation() * mesh->world_orientation().inverse()) * capsule->world_scale().y; //mesh座標系でのsphereのpos

		float min_dis = capsule->world_scale().x;//最低距離をsphereの半径に
		Vector3 closest_point; //mesh上の最近点
		bool is_hit = false; //衝突していたらtrue
		Vector3 nor_meshcoord; //衝突点での法線
		float min_t; //衝突点のt
		Vector3 min_sphere_pos_meshcoord; //衝突点のcapsuleのsphereのpos
		//capsuleとmeshの判定
		for (const auto& faset : mesh_mesh->mesh->facets) {
			const Vector3 nor = (faset.normal * mesh->world_scale()).unit_vect();
			const Vector3 mesh_pos0 = mesh_mesh->mesh->vertices.at(faset.vertexID[0]) * mesh->world_scale();
			//mesh平面の"d"
			float dis = vector3_dot(nor, mesh_pos0);

			float Crossing_t;

			//capsule光線上の最近点をだいたい求める 正確ではないため求めなおす
			Crossing_func::getCrossingP_plane_line(
				nor,
				dis,
				capsule_pos_meshcoord,
				capsule_dir_meshcoord,
				Crossing_t, false
			);

			Crossing_t = ALClamp(Crossing_t, -1, 1);

			Vector3 sphere_pos_meshcoord = capsule_pos_meshcoord + capsule_dir_meshcoord * Crossing_t;
			float dis_sp = vector3_dot(nor, sphere_pos_meshcoord);

			//sphereがMeshの裏にいる または mesh平面とsphereの距離がmin_disより大きければ衝突しない
			if (dis_sp - dis < 0 || dis_sp - dis > min_dis) continue;

			const Vector3 mesh_pos1 = mesh_mesh->mesh->vertices.at(faset.vertexID[1]) * mesh->world_scale();
			const Vector3 mesh_pos2 = mesh_mesh->mesh->vertices.at(faset.vertexID[2]) * mesh->world_scale();

			Vector3 closest_p;
			Closest_func::get_closestP_segment_triangle(
				capsule_pos_meshcoord - capsule_dir_meshcoord, capsule_pos_meshcoord + capsule_dir_meshcoord,
				mesh_pos0, mesh_pos1, mesh_pos2, nor,
				Crossing_t, closest_p
			);

			Crossing_t = Crossing_t * 2 - 1;
			Crossing_t = ALClamp(Crossing_t, -1, 1);

			sphere_pos_meshcoord = capsule_pos_meshcoord + capsule_dir_meshcoord * Crossing_t;

			if ((sphere_pos_meshcoord - closest_p).norm() > min_dis * min_dis) continue;

			//min_dis,最近点の更新
			closest_point = closest_p;
			min_dis = (sphere_pos_meshcoord - closest_point).norm_sqr();
			nor_meshcoord = nor;
			min_t = Crossing_t;
			min_sphere_pos_meshcoord = sphere_pos_meshcoord;
			is_hit = true;

		}
		//交差していなければfalseを返す
		if (is_hit == false)return false;

		Vector3 Wn = vector3_quatrotate((min_sphere_pos_meshcoord - closest_point), mesh->world_orientation()).unit_vect();//meshからsphereへのベクトル
		//if (vector3_dot(min_sphere_pos_meshcoord - closest_point, nor_meshcoord) < 0)Wn *= -1; //バグの原因

		is_AC = true;
		ACpenetration = capsule->world_scale().x - min_dis;
		ACnormal = -Wn;
		ACcontact_pointA = closest_point;
		ACcontact_pointB = Vector3(0, capsule->world_scale().y, 0) * min_t + capsule->world_scale().x * vector3_quatrotate(-Wn, capsule->world_orientation().inverse());
	}

	if (is_AC)
	{
		mesh_mesh->ALPcollider->oncoll_bits |= capsule_mesh->ALPcollider->tag;
		capsule_mesh->ALPcollider->oncoll_bits |= mesh_mesh->ALPcollider->tag;

		//oncoll_enterのみの場合ここでreturn
		if (pair.check_oncoll_only == true) return false;

		if (pair.body[0]->ALPcollider->shape == mesh->shape)
			pair.contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair.contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);
	}
	return is_AC;
}
#pragma endregion


#pragma region MESH-PLANE
bool Physics_function::generate_contact_mesh_plane(const ALP_Collider_mesh* S1, const ALP_Collider_mesh* S2, Contacts::Contact_pair& pair) {
	return true;
}
#pragma endregion

#pragma region MESH-MESH

bool GC_concave_mesh_mesh(const ALP_Collider& S1, const ALP_Collider_mesh* S2, int concave_num, Contacts::Contact_pair& pair) {
	return false;
}

bool Physics_function::generate_contact_mesh_mesh(const ALP_Collider_mesh* mA, const ALP_Collider_mesh* mB, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& meshA = mA->ALPcollider;
	const std::list<ALP_Collider>::iterator& meshB = mB->ALPcollider;

	float smallest_penetration = FLT_MAX;	//最小めり込み量
	int smallest_facetID[2];	//最小めり込み量を得た分離軸の作成に使用した各OBBのローカル軸番号 辺×辺用に2つ
	SAT_TYPE smallest_case;		//衝突の種類

	//どちらもconvexなobjectの場合
	if (mA->mesh->is_Convex == true && mB->mesh->is_Convex == true) {
		if (!sat_convex_mesh_mesh(mA, mB, smallest_penetration, smallest_facetID, smallest_case))return false;

		Quaternion offset_quatBA = meshB->world_orientation() * meshA->world_orientation().inverse();
		Quaternion offset_quatAB = meshA->world_orientation() * meshB->world_orientation().inverse();
		Vector3 offset_posBA = meshB->world_position() - meshA->world_position();
		Vector3 offset_posAB = meshA->world_position() - meshB->world_position();

		//SBの頂点がSAの面と衝突した場合
		if (smallest_case == SAT_TYPE::POINTB_FACETA)
		{
			const ALP_Collider_mesh* facet_mesh = mA;
			const ALP_Collider_mesh* vertex_mesh = mB;

			const std::list<ALP_Collider>::iterator& facet_coll = facet_mesh->ALPcollider;
			const std::list<ALP_Collider>::iterator& vertex_coll = vertex_mesh->ALPcollider;

			Quaternion offset_quatVF = vertex_coll->world_orientation() * facet_coll->world_orientation().inverse();
			Quaternion offset_quatFV = facet_coll->world_orientation() * vertex_coll->world_orientation().inverse();
			Vector3 offset_posVF = vertex_coll->world_position() - facet_coll->world_position();
			Vector3 offset_posFV = facet_coll->world_position() - vertex_coll->world_position();

			assert(smallest_facetID[1] == -1);
			const Facet& nerest_facet = facet_mesh->mesh->facets.at(smallest_facetID[0]);

			Vector3 axisF = nerest_facet.normal.unit_vect();
			Vector3 axisW = vector3_quatrotate(axisF, facet_coll->world_orientation()).unit_vect();
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

				for (u_int v_num = 0; v_num < vertex_mesh->mesh->vertex_num; v_num++) {

					if (vector3_dot(vertex_mesh->mesh->vertices.at(v_num), axisV) > max_len) {
						max_len = vector3_dot(vertex_mesh->mesh->vertices.at(v_num), axisV);
						nearest_pointID = v_num;
					}
				}
			}
			pB = vertex_mesh->mesh->vertices.at(nearest_pointID);
			pB *= vertex_coll->world_scale();

			//上記のp0がfacet_collの最近面上のどこにあるのか
			Vector3 pA;
			{

				Vector3 p = vector3_quatrotate(vector3_quatrotate(pB, vertex_coll->world_orientation()) + offset_posVF, facet_coll->world_orientation().inverse());
				p /= facet_coll->world_scale();
				float min_len = FLT_MAX;
				Vector3 n_pos;


				for (u_int f_num = 0; f_num < facet_mesh->mesh->facet_num; f_num++) {

					if (vector3_dot(facet_mesh->mesh->facets.at(f_num).normal, -axisF) < 0.5)continue; //衝突法線と比べて

					//メッシュと点の最近点を求める
					get_closestP_point_triangle(p,
						facet_mesh->mesh->vertices.at(facet_mesh->mesh->facets.at(f_num).vertexID[0]),
						facet_mesh->mesh->vertices.at(facet_mesh->mesh->facets.at(f_num).vertexID[1]),
						facet_mesh->mesh->vertices.at(facet_mesh->mesh->facets.at(f_num).vertexID[2]),
						facet_mesh->mesh->facets.at(f_num).normal,
						n_pos
					);

					if ((p - n_pos).norm() < min_len) {
						pA = n_pos;
						min_len = (p - n_pos).norm();
					}
				}

				pA *= facet_coll->world_scale();
			}

			pair.contacts.addcontact(
				smallest_penetration,
				axisW,
				pA,
				pB
			);
		}

		//SAの頂点がSBの面と衝突した場合
		if (smallest_case == SAT_TYPE::POINTA_FACETB)
		{
			const ALP_Collider_mesh* facet_mesh = mB;
			const ALP_Collider_mesh* vertex_mesh = mA;

			const std::list<ALP_Collider>::iterator& facet_coll = facet_mesh->ALPcollider;
			const std::list<ALP_Collider>::iterator& vertex_coll = vertex_mesh->ALPcollider;

			Quaternion offset_quatVF = vertex_coll->world_orientation() * facet_coll->world_orientation().inverse();
			Quaternion offset_quatFV = facet_coll->world_orientation() * vertex_coll->world_orientation().inverse();
			Vector3 offset_posVF = vertex_coll->world_position() - facet_coll->world_position();
			Vector3 offset_posFV = facet_coll->world_position() - vertex_coll->world_position();

			assert(smallest_facetID[0] == -1);
			const Facet& nerest_facet = facet_mesh->mesh->facets.at(smallest_facetID[1]);

			Vector3 axisF = nerest_facet.normal.unit_vect();
			Vector3 axisW = vector3_quatrotate(axisF, facet_coll->world_orientation()).unit_vect();
			if (vector3_dot(axisW, offset_posBA) < 0) {
				axisF = -axisF;
				axisW = -axisW;
			}

			//vertex_collのどの頂点が最近点か求める
			u_int nearest_pointID = 0;
			Vector3 pB;
			{
				float max_len = -FLT_MAX;
				Vector3 axisV = vector3_quatrotate(axisF, offset_quatFV);

				for (u_int v_num = 0; v_num < vertex_mesh->mesh->vertex_num; v_num++) {

					if (vector3_dot(vertex_mesh->mesh->vertices.at(v_num), axisV) > max_len) {
						max_len = vector3_dot(vertex_mesh->mesh->vertices.at(v_num), axisV);
						nearest_pointID = v_num;
					}
				}
			}
			pB = vertex_mesh->mesh->vertices.at(nearest_pointID);
			pB *= vertex_coll->world_scale();

			//上記のpBがfacet_collの最近面上のどこにあるのか
			Vector3 pA;
			{

				Vector3 p = vector3_quatrotate(vector3_quatrotate(pB, vertex_coll->world_orientation()) + offset_posVF, facet_coll->world_orientation().inverse());
				p /= facet_coll->world_scale();
				float min_len = FLT_MAX;
				Vector3 n_pos;


				for (u_int f_num = 0; f_num < facet_mesh->mesh->facet_num; f_num++) {

					if (vector3_dot(facet_mesh->mesh->facets.at(f_num).normal, -axisF) < 0.5)continue; //衝突法線と比べて

					//メッシュと点の最近点を求める
					get_closestP_point_triangle(p,
						facet_mesh->mesh->vertices.at(facet_mesh->mesh->facets.at(f_num).vertexID[0]),
						facet_mesh->mesh->vertices.at(facet_mesh->mesh->facets.at(f_num).vertexID[1]),
						facet_mesh->mesh->vertices.at(facet_mesh->mesh->facets.at(f_num).vertexID[2]),
						facet_mesh->mesh->facets.at(f_num).normal,
						n_pos
					);

					if ((p - n_pos).norm() < min_len) {
						pA = n_pos;
						min_len = (p - n_pos).norm();
					}
				}

				pA *= facet_coll->world_scale();
			}

			pair.contacts.addcontact(
				smallest_penetration,
				-axisW,
				pB,
				pA
			);
		}


		//SAとSBの辺同士が衝突した場合
		else if (smallest_case == SAT_TYPE::EDGE_EDGE)
		{
			Quaternion offset_quatAB = meshA->world_orientation() * meshB->world_orientation().inverse();
			Quaternion offset_quatBA = meshB->world_orientation() * meshA->world_orientation().inverse();
			Vector3 offset_posAB = meshA->world_position() - meshB->world_position();
			Vector3 offset_posBA = meshB->world_position() - meshA->world_position();

			const Edge& edgeA = mA->mesh->edges.at(smallest_facetID[0]);
			const Edge& edgeB = mB->mesh->edges.at(smallest_facetID[1]);

			Vector3 edgeA_p[2] = {
				{mA->mesh->vertices.at(edgeA.vertexID[0]) * meshA->world_scale()},
				{mA->mesh->vertices.at(edgeA.vertexID[1]) * meshA->world_scale()}
			};
			Vector3 edgeB_p[2] = {
				{mB->mesh->vertices.at(edgeB.vertexID[0]) * meshB->world_scale()},
				{mB->mesh->vertices.at(edgeB.vertexID[1]) * meshB->world_scale()}
			};

			Vector3 edgeA_vec = (edgeA_p[0] - edgeA_p[1]).unit_vect();
			Vector3 edgeB_vec = (edgeB_p[0] - edgeB_p[1]).unit_vect();

			//SBの情報をSAの座標系に持ってきた
			//Vector3 edgeB_p_A = vector3_quatrotate(edgeB_p[0], offset_quatBA) + offset_posBA;
			Vector3 edgeB_p_A = vector3_quatrotate(vector3_quatrotate(edgeB_p[0], meshB->world_orientation()) + offset_posBA, meshA->world_orientation().inverse());
			Vector3 edgeB_v_A = vector3_quatrotate(edgeB_vec, offset_quatBA);

			//SAの座標系でaxisの生成
			Vector3 axisA = vector3_cross(edgeA_vec, edgeB_v_A);
			axisA = axisA.unit_vect();

			//axisをworld座標系へ
			Vector3 axisW = vector3_quatrotate(axisA, meshA->world_orientation());

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