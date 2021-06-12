#include "ALP_raycast.h"

#include "../Math/crossing_func.h"

using namespace Adollib;
using namespace Physics_function;

#include "../Imgui/debug.h"
#include "collider_shape.h"

//slabによるraycast
bool ray_cast_14DOP(const Vector3& Ray_pos, const Vector3& Ray_dir,
	const DOP::DOP_14& dop,
	float& tmin, float& tmax
	) {

	//tminからtmaxにかけてrayがslabと交差している
	tmin = -FLT_MAX;
	tmax = +FLT_MAX;

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
	const Collider_shape* coll,
	const float ray_min,
	float& tmin, float& tmax,
	Vector3& normal
) {

	Vector3 m = Ray_pos - coll->world_position();
	float b = vector3_dot(m, Ray_dir);
	float c = vector3_dot(m, m) - coll->world_scale().x * coll->world_scale().x;

	float D = b * b - c;
	if (D < 0)return false;

	tmin = -b - sqrtf(D);
	if (tmin < ray_min)return false;//最低値より小さければfalse

	tmax = -b + sqrtf(D);

	normal = (Ray_pos + tmin * Ray_dir) - coll->world_position();
	normal = normal.unit_vect();

	return true;
}

bool ray_cast_box(const Vector3& Ray_pos, const Vector3& Ray_dir,
	const Collider_shape* coll,
	const float ray_min,
	float& tmin, float& tmax,
	Vector3& normal
) {
	tmin = -FLT_MAX;
	tmax = +FLT_MAX;

	//slabの向き
	Vector3 xyz[3] = {
		vector3_quatrotate(Vector3(1, 0, 0), coll->world_orientation()),
		vector3_quatrotate(Vector3(0, 1, 0), coll->world_orientation()),
		vector3_quatrotate(Vector3(0, 0, 1), coll->world_orientation())
	};

	for (int i = 0; i < 3; i++) {
		float D = vector3_dot(xyz[i], Ray_dir);
		float P = vector3_dot(xyz[i], Ray_pos - coll->world_position());


		{
			//各軸の二つのslabにrayが交差するtを求める
			float ood = 1.0f / D;
			float t1 = (+coll->world_scale()[i] - P) * ood;
			float t2 = (-coll->world_scale()[i] - P) * ood;

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

	if (tmin < ray_min)return false;//最低値より小さければfalse

	return true;

}

bool ray_cast_plane(const Vector3& Ray_pos, const Vector3& Ray_dir,
	const Collider_shape* coll,
	const float ray_min,
	float& tmin, float& tmax,
	Vector3& normal
) {
	tmin = +FLT_MAX;
	tmax = -FLT_MAX;

	normal = vector3_quatrotate(Vector3(0, 1, 0), coll->world_orientation());
	if (Crossing_func::getCrossingP_plane_line(
		normal,
		coll->world_position().y,
		Ray_pos,
		Ray_dir,
		tmin
	) == false) return false;

	tmax = tmin;

	return true;
}

#include "meshcoll_resorce_manager.h"
bool ray_cast_mesh(const Vector3& l_Ray_pos, const Vector3& l_Ray_dir,
	const Collider_shape* mesh,
	const float ray_min,
	float& tmin, float& tmax,
	Vector3& normal
) {
	tmin = +FLT_MAX;
	tmax = -FLT_MAX;

	bool crossing = false; //どこかが交差していたらtrueに変更
	const std::vector<Vector3>& vertices = mesh->get_mesh_data()->vertices;

	Matrix44 mat = matrix_world(mesh->world_scale(), mesh->world_orientation().get_rotate_matrix(), mesh->world_position());
	Matrix44 mat_inv = matrix_inverse(mat);

	//Rayの情報をmeshの座標系にに持ってくる
	const Vector3 Ray_pos = vector3_trans(l_Ray_pos, mat_inv);
	const Vector3 Ray_dir = vector3_quatrotate(l_Ray_dir, mesh->world_orientation().inverse()) / mesh->world_scale();

	for (auto& facet : mesh->get_mesh_data()->facets) {

		const Vector3& n = facet.normal;
		const Vector3& PA = vertices.at(facet.vertexID[0]);
		const Vector3& PB = vertices.at(facet.vertexID[1]);
		const Vector3& PC = vertices.at(facet.vertexID[2]);

		float d = vector3_dot(PA, n);
		float t = 0;
		if (Crossing_func::getCrossingP_plane_line(n, d, Ray_pos, Ray_dir, t, false) == false) continue;

		if (ray_min > t) continue; // Rayが半直線の場合

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

	normal = vector3_quatrotate(normal, mesh->world_orientation());

	return crossing;
}






bool Physics_function::ray_cast(const Vector3& Ray_pos, const Vector3& Ray_dir,
	const ALP_Collider* coll,
	const float ray_min,
	float& tmin, float& tmax,
	Vector3& normal
) {

	//tminからtmaxにかけてrayが交差している
	tmin = +FLT_MAX;
	tmax = -FLT_MAX;

	Vector3 normal_;

	float DOPtmin = 0,DOPtmax = 0;
	bool ret = false;

	for (const auto& mesh : coll->get_shapes()) {
		if (ray_cast_14DOP(Ray_pos, Ray_dir, mesh->get_DOP(), DOPtmin, DOPtmax) == false)continue;
		if (DOPtmax < ray_min)continue;

		float tmin_ = 0;
		float tmax_ = 0;
		bool hit = false;

		if (mesh->get_shape_tag() == ALPCollider_shape_type::Sphere)
			hit = ray_cast_sphere(Ray_pos, Ray_dir, mesh, ray_min, tmin_, tmax_, normal_);

		if (mesh->get_shape_tag() == ALPCollider_shape_type::BOX)
			hit = ray_cast_box(Ray_pos, Ray_dir, mesh, ray_min, tmin_, tmax_, normal_);

		if (mesh->get_shape_tag() == ALPCollider_shape_type::Plane)
			hit = ray_cast_plane(Ray_pos, Ray_dir, mesh, ray_min, tmin_, tmax_, normal_);

		if (mesh->get_shape_tag() == ALPCollider_shape_type::Mesh)
			hit = ray_cast_mesh(Ray_pos, Ray_dir, mesh, ray_min, tmin_, tmax_, normal_);

		if (hit == false)continue;
		ret = true;

		if (tmin > tmin_) {
			tmin = tmin_;
			normal = normal_;
		}
		tmax = ALmax(tmax, tmax_);
	}
	return ret;

		//if (tmin > DOPtmin)tmin = DOPtmin;
	//}
	//normal = Vector3(0, 1, 0);
	//return true;


	////tminからtmaxにかけてrayが交差している
	//tmin = -FLT_MAX;
	//tmax = +FLT_MAX;

	//float min_ABB_dis = FLT_MAX;
	//ALP_Collider_mesh const* min_AABB_mesh = nullptr;

	//for (auto& mesh : coll.collider_meshes) {
	//	float min_AABB_ = 0;
	//	if (ray_cast_14DOP(Ray_pos, Ray_dir, mesh.dop14, min_AABB_) == false)continue;
	//	if (min_ABB_dis > min_AABB_) {
	//		min_ABB_dis = min_AABB_;
	//		min_AABB_mesh = &mesh;
	//	}
	//}

	//if (min_AABB_mesh == nullptr)return false;


	//if (coll->get_shape_tag() == ALP_Collider_shape::Sphere)
	//	if (ray_cast_sphere(Ray_pos, Ray_dir, coll, tmin, tmax, normal)) return false;

	//if (coll->get_shape_tag() == ALP_Collider_shape::BOX)
	//	if (ray_cast_box(Ray_pos, Ray_dir, coll, tmin, tmax, normal)) return false;

	//if (coll->get_shape_tag() == ALP_Collider_shape::Plane)
	//	if (ray_cast_plane(Ray_pos, Ray_dir, coll, tmin, tmax, normal)) return false;

	//if (coll->get_shape_tag() == ALP_Collider_shape::Mesh)
	//	if (ray_cast_mesh(Ray_pos, Ray_dir, *min_AABB_mesh, tmin, tmax, normal)) return false;

	//return true;
}