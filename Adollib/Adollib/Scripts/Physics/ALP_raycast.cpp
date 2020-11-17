#include "ALP_raycast.h"

#include "../Math/crossing_func.h"

using namespace Adollib;
using namespace Physics_function;

#include "../Imgui/debug.h"

//slabによるraycast
bool ray_cast_14DOP(const Vector3& Ray_pos, const Vector3& Ray_dir,
	const DOP::DOP_14& dop
	) {

	//tminからtmaxにかけてrayがslabと交差している
	float tmin = -FLT_MAX;
	float tmax = +FLT_MAX;

	for (int i = 0; i < DOP::DOP14_size; i++) {
		float D = vector3_dot(DOP::DOP_14_axis[i], Ray_dir);
		float P = vector3_dot(DOP::DOP_14_axis[i], Ray_pos - dop.pos);
		{
			//各軸の二つのslabにrayが交差するtを求める
			float ood = 1.0f / D;
			float t1 = (dop.min[i] - P) * ood;
			float t2 = (dop.max[i] - P) * ood;

			//もしtsのほうに大きい方を保存
			if (t1 > t2) std::swap(t1, t2);

			//tmin,tmaxを更新
			tmin = std::max<float>(tmin, t1);
			tmax = std::min<float>(tmax, t2);

			//もしtmaxがtminより小さくなった時、rayはDOPと交差していない
			if (tmin > tmax) return false;
		}

	}

	return true;
}

bool ray_cast_sphere(const Vector3& Ray_pos, const Vector3& Ray_dir,
	const ALP_Collider& coll,
	float& tmin, float& tmax,
	Vector3& normal
) {

	Vector3 m = Ray_pos - coll.world_position;
	float b = vector3_dot(m, Ray_dir);
	float c = vector3_dot(m, m) - coll.world_scale.x * coll.world_scale.x;

	float D = b * b - c;
	if (D < 0)return false;

	tmin = -b - sqrtf(D);
	tmax = -b + sqrtf(D);

	normal = (Ray_pos + tmin * Ray_dir) - coll.world_position;
	normal = normal.unit_vect();

	return true;
}

bool ray_cast_box(const Vector3& Ray_pos, const Vector3& Ray_dir,
	const ALP_Collider& coll,
	float& tmin, float& tmax,
	Vector3& normal
) {

	Vector3 xyz[3] = {
		vector3_quatrotate(Vector3(1, 0, 0), coll.world_orientation),
		vector3_quatrotate(Vector3(0, 1, 0), coll.world_orientation),
		vector3_quatrotate(Vector3(0, 0, 1), coll.world_orientation)
	};

	for (int i = 0; i < 3; i++) {
		float D = vector3_dot(xyz[i], Ray_dir);
		float P = vector3_dot(xyz[i], Ray_pos - coll.world_position);


		{
			//各軸の二つのslabにrayが交差するtを求める
			float ood = 1.0f / D;
			float t1 = (vector3_dot(+coll.world_scale, xyz[i]) - P) * ood;
			float t2 = (vector3_dot(-coll.world_scale, xyz[i]) - P) * ood;

			//もしtsのほうに大きい方を保存
			int reverce = 1;
			if (t1 > t2){
				reverce *= -1;
				std::swap(t1, t2);
			}

			//tmin,tmaxを更新
			if (tmin < t1) {
				tmin = t1;
				normal = xyz[i] * reverce;
			}

			tmax = std::min<float>(tmax, t2);

			//もしtmaxがtminより小さくなった時、rayはDOPと交差していない
			if (tmin > tmax) return false;
		}

	}

	return true;

}

bool ray_cast_plane(const Vector3& Ray_pos, const Vector3& Ray_dir,
	const ALP_Collider& coll,
	float& tmin, float& tmax,
	Vector3& normal
) {
	normal = vector3_quatrotate(Vector3(0, 1, 0), coll.world_orientation);
	if (Crossing_func::getCrossingP_plane_line(
		normal,
		coll.world_position.y,
		Ray_pos,
		Ray_dir,
		tmin
	) == false) return false;

	tmax = tmin;

	return true;
}

//// return value is index of intersected triangle / -1 is a meaning of failure
//int collision::ray_triangles_intersection
//(
//	const float* positions,	// model space
//	const unsigned int stride, // byte size of one vertex
//	const unsigned int* indices,
//	const size_t number_of_indices,
//	const XMFLOAT3& ray_position, // model space
//	const XMFLOAT3& ray_direction, // model space 
//	float ray_length_limit, // model space 
//	XMFLOAT3* intersection // model space
//)
//{
//	//bool RH = true; // Right handed coordinate system
//	bool CCW = true; // Counterclockwise 3D triangle ABC
//	const int c0 = 0;
//	const int c1 = CCW ? 1 : 2;
//	const int c2 = CCW ? 2 : 1;
//
//	// Ray-triangle intersection 
//	// In this handout, we explore the steps needed to compute the intersection of a ray with a triangle, and then to
//	// compute the barycentric coordinates of that intersection.First, we consider the geometry of such an intersection :
//	//
//	// where a ray with origin P and direction D intersects a triangle defined by its vertices, A, B, and C at intersection
//	// point Q.The square region diagrammatically surrounding triangle ABC represents the supporting plane of the
//	// triangle, i.e., the plane that the triangle lies on.
//	//
//	// To perform ray - triangle intersection, we must perform two steps :
//	//
//	//  1. Determine the point of intersection, Q.
//	//  2. Determine if Q lies inside of triangle ABC.
//	//
//	int number_of_intersections = 0;
//	int index_of_intersected_triangle = -1;
//
//	float shortest_distance = FLT_MAX;
//	XMVECTOR X; // closest cross point
//	XMVECTOR P = XMVectorSet(ray_position.x, ray_position.y, ray_position.z, 1);
//	XMVECTOR D = XMVector3Normalize(XMVectorSet(ray_direction.x, ray_direction.y, ray_direction.z, 0));
//
//	const unsigned char* p = reinterpret_cast<const unsigned char*>(positions);
//	const size_t number_of_triangles = number_of_indices / 3;
//	for (size_t index_of_triangle = 0; index_of_triangle < number_of_triangles; index_of_triangle++)
//	{
//		XMFLOAT3 p0 = XMFLOAT3(
//			(reinterpret_cast<const float*>(p + indices[index_of_triangle * 3 + c0] * stride))[0],
//			(reinterpret_cast<const float*>(p + indices[index_of_triangle * 3 + c0] * stride))[1],
//			(reinterpret_cast<const float*>(p + indices[index_of_triangle * 3 + c0] * stride))[2]);
//		XMFLOAT3 p1 = XMFLOAT3(
//			(reinterpret_cast<const float*>(p + indices[index_of_triangle * 3 + c1] * stride))[0],
//			(reinterpret_cast<const float*>(p + indices[index_of_triangle * 3 + c1] * stride))[1],
//			(reinterpret_cast<const float*>(p + indices[index_of_triangle * 3 + c1] * stride))[2]);
//		XMFLOAT3 p2 = XMFLOAT3(
//			(reinterpret_cast<const float*>(p + indices[index_of_triangle * 3 + c2] * stride))[0],
//			(reinterpret_cast<const float*>(p + indices[index_of_triangle * 3 + c2] * stride))[1],
//			(reinterpret_cast<const float*>(p + indices[index_of_triangle * 3 + c2] * stride))[2]);
//
//		XMVECTOR A, B, C;
//		A = XMLoadFloat3(&p0);
//		B = XMLoadFloat3(&p1);
//		C = XMLoadFloat3(&p2);
//
//		// Determining Q
//		// Determining the point of intersection Q in turn requires us to take two more steps :
//		//  1. Determine the equation of the supporting plane for triangle ABC.
//		//  2. Intersect the ray with the supporting plane.
//		// We’ll handle these steps in reverse order.
//		XMVECTOR Q;
//
//		// 1. Determine the equation of the supporting plane for triangle ABC.
//		XMVECTOR N = XMVector3Normalize(XMVector3Cross(XMVectorSubtract(B, A), XMVectorSubtract(C, A)));
//		float d = XMVectorGetByIndex(XMVector3Dot(N, A), 0);
//
//		// 2. Intersect the ray with the supporting plane.
//		float denominator = XMVectorGetByIndex(XMVector3Dot(N, D), 0);
//		if (denominator < 0) // Note that if N.D = 0 , then D is parallel to the plane and the ray does not intersect the plane.
//		{
//			float numerator = d - XMVectorGetByIndex(XMVector3Dot(N, P), 0);
//			float t = numerator / denominator;
//
//			if (t > 0 && t < ray_length_limit) // Forward and Length limit of Ray
//			{
//				Q = XMVectorAdd(P, XMVectorScale(D, t));
//
//				// Triangle inside - outside testing
//
//				// CHRISTER ERICSON. "REAL-TIME COLLISION DETECTION" pp.204
//				// Test if point Q lies inside the counterclockwise 3D triangle ABC
//
//				// Translate point and triangle so that point lies at origin
//				static const XMVECTOR Z = XMLoadFloat3(&XMFLOAT3(0, 0, 0));
//				static const XMVECTOR E = XMLoadFloat3(&XMFLOAT3(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON));
//
//				XMVECTOR QA = XMVectorSubtract(A, Q);
//				XMVECTOR QB = XMVectorSubtract(B, Q);
//				XMVECTOR QC = XMVectorSubtract(C, Q);
//
//				XMVECTOR U = XMVector3Cross(QB, QC);
//				if (XMVector3NearEqual(U, Z, E))
//				{
//					U = Z;
//				}
//				XMVECTOR V = XMVector3Cross(QC, QA);
//				if (XMVector3NearEqual(V, Z, E))
//				{
//					V = Z;
//				}
//				if (XMVectorGetByIndex(XMVector3Dot(U, V), 0) < 0)
//				{
//					continue;
//				}
//
//				XMVECTOR W = XMVector3Cross(QA, QB);
//				if (XMVector3NearEqual(W, Z, E))
//				{
//					W = Z;
//				}
//				if (XMVectorGetByIndex(XMVector3Dot(U, W), 0) < 0)
//				{
//					continue;
//				}
//				if (XMVectorGetByIndex(XMVector3Dot(V, W), 0) < 0)
//				{
//					continue;
//				}
//
//				// Otherwise Q must be in (or on) the triangle
//				if (t < shortest_distance)
//				{
//					shortest_distance = t;
//					index_of_intersected_triangle = index_of_triangle;
//					X = Q;
//
//				}
//				number_of_intersections++;
//			}
//		}
//	}
//	if (number_of_intersections > 0)
//	{
//		XMStoreFloat3(intersection, X);
//	}
//	return index_of_intersected_triangle;
//}
#include "meshcoll_resorce_manager.h"
bool ray_cast_mesh(const Vector3& Ray_pos, const Vector3& Ray_dir,
	const ALP_Collider_mesh& mesh,
	float& tmin, float& tmax,
	Vector3& normal
) {

	bool crossing = false; //どこかが交差していたらtrueに変更
	const std::vector<Vector3>& vertices = *mesh.mesh->vertices;
	auto& A = Collider_ResourceManager::vertices_;

	for (auto& facet : mesh.mesh->facets) {

		const Vector3& n = facet.normal;
		Vector3 PA = vector3_quatrotate(vertices.at(facet.vertexID[0]) * mesh.ALPcollider->world_scale, mesh.ALPcollider->world_orientation) + mesh.ALPcollider->world_position;
		Vector3 PB = vector3_quatrotate(vertices.at(facet.vertexID[1]) * mesh.ALPcollider->world_scale, mesh.ALPcollider->world_orientation) + mesh.ALPcollider->world_position;
		Vector3 PC = vector3_quatrotate(vertices.at(facet.vertexID[2]) * mesh.ALPcollider->world_scale, mesh.ALPcollider->world_orientation) + mesh.ALPcollider->world_position;

		//Vector3 PA = vertices[facet.vertexID[0]];
		//Vector3 PB = vertices[facet.vertexID[1]];
		//Vector3 PC = vertices[facet.vertexID[2]];

		float d = vector3_dot(PA, n);
		float t = 0;
		if (Crossing_func::getCrossingP_plane_line(n, d, Ray_pos, Ray_dir, t) == false) continue;

		//if (tmin < t)continue; Rayが半直線の場合

		//点がポリゴン内にあるかどうかの判定
		Vector3 crossing_p = Ray_pos + t * Ray_dir;

		Vector3 QA = PA - crossing_p;
		Vector3 QB = PB - crossing_p;
		Vector3 QC = PC - crossing_p;

		Vector3 NA = vector3_cross(QA, QB);
		Vector3 NB = vector3_cross(QB, QC);
		Vector3 NC = vector3_cross(QC, QA);

		if (vector3_dot(NA, NB) < 0)continue;
		if (vector3_dot(NB, NC) < 0)continue;

		crossing = true;
		if (tmin > t) {
			tmin = t;
			normal = n;
		}
		tmax = ALmax(tmax, t);

	}




	return crossing;
}






bool Physics_function::ray_cast(const Vector3& Ray_pos, const Vector3& Ray_dir,
	const ALP_Collider& coll,
	float& tmin, float& tmax,
	Vector3& normal
) {

	//tminからtmaxにかけてrayが交差している
	tmin = -FLT_MAX;
	tmax = +FLT_MAX;

	bool ret = false;
	for (const auto& mesh : coll.collider_meshes) {
		//if (ray_cast_14DOP(Ray_pos, Ray_dir, mesh.dop14) == false)continue;

		if (coll.shape == ALP_Collider_shape::Sphere)
			if (ray_cast_sphere(Ray_pos, Ray_dir, coll, tmin, tmax, normal)) ret = true;

		if (coll.shape == ALP_Collider_shape::BOX)
			if (ray_cast_box(Ray_pos, Ray_dir, coll, tmin, tmax, normal)) ret = true;

		if (coll.shape == ALP_Collider_shape::Plane)
			if (ray_cast_plane(Ray_pos, Ray_dir, coll, tmin, tmax, normal)) ret = true;

		if (coll.shape == ALP_Collider_shape::Mesh)
			if (ray_cast_mesh(Ray_pos, Ray_dir, mesh, tmin, tmax, normal)) ret = true;
	}
	return ret;
}