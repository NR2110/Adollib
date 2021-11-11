#include "ALP_raycast.h"

#include "../Math/crossing_func.h"

using namespace Adollib;
using namespace Physics_function;

#include "../Imgui/debug.h"
#include "collider_shape.h"
#include "meshcoll_resorce_manager.h"

#pragma region Ray_cast

//slabによるraycast
bool ray_cast_14DOP(const Vector3& Ray_pos, const Vector3& Ray_dir,
	const DOP::DOP_14& dop,
	float& tmin, float& tmax
) {

	//tminからtmaxにかけてrayがslabと交差している
	tmin = -FLT_MAX;
	tmax = +FLT_MAX;

	for (int i = 0; i < DOP::DOP_size; i++) {
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
	// Ray : p + td
	// Sphere: (X - C)・(X - C) = r ^ 2
	// X に Ray: (p + td)を代入して 計算


	Vector3 m = Ray_pos - coll->world_position();
	float b = vector3_dot(m, Ray_dir);
	float c = vector3_dot(m, m) - coll->world_scale().x * coll->world_scale().x;

	// 判別式
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

	// slabの向き
	Vector3 xyz[3] = {
		vector3_quatrotate(Vector3(1, 0, 0), coll->world_orientation()),
		vector3_quatrotate(Vector3(0, 1, 0), coll->world_orientation()),
		vector3_quatrotate(Vector3(0, 0, 1), coll->world_orientation())
	};

	for (int i = 0; i < 3; i++) {
		float D = vector3_dot(xyz[i], Ray_dir);
		float P = vector3_dot(xyz[i], Ray_pos - coll->world_position());

		if (D == 0) {
			// ある軸に平行な時 含まれていなければ即座にreturn
			if ((+coll->world_scale()[i] - P) * (-coll->world_scale()[i] - P) > 0)return false;
		}

		{
			// 各軸の二つのslabにrayが交差するtを求める
			float ood = 1.0f / D;
			float t1 = (+coll->world_scale()[i] - P) * ood;
			float t2 = (-coll->world_scale()[i] - P) * ood;

			// tsのほうに大きい方を保存
			int reverce = 1;
			if (t1 > t2) {
				reverce *= -1;
				std::swap(t1, t2);
			}

			// tmin,tmaxを更新
			if (tmin < t1) {
				tmin = t1;
				normal = xyz[i] * reverce;
			}

			tmax = std::min<float>(tmax, t2);

			// もしtmaxがtminより小さくなった時、rayはBoxと交差していない
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
	const float ray_min,
	const ALP_Collider* coll,
	float& tmin, float& tmax,
	Vector3& normal
) {

	//tminからtmaxにかけてrayが交差している
	tmin = +FLT_MAX;
	tmax = -FLT_MAX;

	Vector3 normal_;

	float DOPtmin = 0, DOPtmax = 0;
	bool ret = false;

	for (const auto& shape : coll->get_shapes()) {
		// 大雑把なraycastを行う
		if (ray_cast_14DOP(Ray_pos, Ray_dir, shape->get_DOP(), DOPtmin, DOPtmax) == false)continue;
		if (DOPtmax < ray_min)continue;

		float tmin_ = 0;
		float tmax_ = 0;
		bool hit = false;

		if (shape->get_shape_tag() == ALPCollider_shape_type::Sphere)
			hit = ray_cast_sphere(Ray_pos, Ray_dir, shape, ray_min, tmin_, tmax_, normal_);

		if (shape->get_shape_tag() == ALPCollider_shape_type::BOX)
			hit = ray_cast_box(Ray_pos, Ray_dir, shape, ray_min, tmin_, tmax_, normal_);

		if (shape->get_shape_tag() == ALPCollider_shape_type::Plane)
			hit = ray_cast_plane(Ray_pos, Ray_dir, shape, ray_min, tmin_, tmax_, normal_);

		if (shape->get_shape_tag() == ALPCollider_shape_type::Mesh)
			hit = ray_cast_mesh(Ray_pos, Ray_dir, shape, ray_min, tmin_, tmax_, normal_);

		if (hit == false)continue;
		ret = true;

		if (tmin > tmin_) {
			tmin = tmin_;
			normal = normal_;
		}
		tmax = ALmax(tmax, tmax_);
	}
	return ret;
}

#pragma endregion

#pragma region Sphere_cast

//slabによるraycast
bool sphere_cast_14DOP(const Vector3& Ray_pos, const Vector3& Ray_dir, const float& radius, Vector3& contact_point,
	const DOP::DOP_14& dop,
	float& tmin, float& tmax
) {

	// tminからtmaxにかけてrayがslabと交差している
	tmin = -FLT_MAX;
	tmax = +FLT_MAX;

	for (int i = 0; i < DOP::DOP_size; i++) {
		float D = vector3_dot(DOP::DOP_14_axis[i], Ray_dir);
		float P = vector3_dot(DOP::DOP_14_axis[i], Ray_pos - dop.pos);
		{
			// 各軸の二つのslabにrayが交差するtを求める
			// 半径分大きめにslabをとる
			float ood = 1.0f / D;
			float t1 = (dop.min[i] - radius - P) * ood;
			float t2 = (dop.max[i] + radius - P) * ood;

			// tsのほうに大きい方を保存
			if (t1 > t2) std::swap(t1, t2);

			// tmin,tmaxを更新
			tmin = std::max<float>(tmin, t1);
			tmax = std::min<float>(tmax, t2);

			// もしtmaxがtminより小さくなった時、rayはDOPと交差していない
			if (tmin > tmax) return false;
		}

	}

	return true;
}

bool sphere_cast_sphere(const Vector3& Ray_pos, const Vector3& Ray_dir, const float& radius, Vector3& contact_point,
	const Collider_shape* coll,
	const float ray_min,
	float& tmin, float& tmax,
	Vector3& normal
) {

	float sum_radius = coll->world_scale().x + radius;

	Vector3 m = Ray_pos - coll->world_position();
	float b = vector3_dot(m, Ray_dir);
	float c = vector3_dot(m, m) - sum_radius * sum_radius;

	float D = b * b - c;
	if (D < 0)return false;

	tmin = -b - sqrtf(D);
	if (tmin < ray_min)return false;//最低値より小さければfalse

	tmax = -b + sqrtf(D);

	normal = (Ray_pos + tmin * Ray_dir) - coll->world_position();
	normal = normal.unit_vect();

	contact_point = coll->world_position() + normal * radius;


	return true;
}

bool sphere_cast_box(const Vector3& Ray_pos, const Vector3& Ray_dir, const float& radius, Vector3& contact_point,
	const Collider_shape* coll,
	const float ray_min,
	float& tmin, float& tmax,
	Vector3& normal
) {
	tmin = -FLT_MAX;
	tmax = +FLT_MAX;

	// slabの向き
	Vector3 xyz[3] = {
		vector3_quatrotate(Vector3(1, 0, 0), coll->world_orientation()).unit_vect(),
		vector3_quatrotate(Vector3(0, 1, 0), coll->world_orientation()).unit_vect(),
		vector3_quatrotate(Vector3(0, 0, 1), coll->world_orientation()).unit_vect()
	};

	// 面とのcast
	for (int i = 0; i < 3; ++i) {
		float D = vector3_dot(xyz[i], Ray_dir);
		float P = vector3_dot(xyz[i], Ray_pos - coll->world_position());

		{
			// 各軸の二つのslabにrayが交差するtを求める
			// radiusだけ大きくする
			float ood = 1.0f / D;
			float t1 = (+coll->world_scale()[i] + radius - P) * ood;
			float t2 = (-coll->world_scale()[i] - radius - P) * ood;

			// tsのほうに大きい方を保存
			int reverce = 1;
			if (t1 > t2) {
				reverce *= -1;
				std::swap(t1, t2);
			}

			// tmin,tmaxを更新
			if (tmin < t1) {
				tmin = t1;
				normal = xyz[i] * reverce;
			}

			tmax = std::min<float>(tmax, t2);

			// もしtmaxがtminより小さくなった時、rayはBoxと交差していない
			if (tmin > tmax) return false;
		}
	}
	if (tmin < ray_min)return false;//最低値より小さければfalse

	// box座標系での面とのcastで求めた点
	Vector3 min_point_boxcoord = Ray_pos + Ray_dir * tmin - coll->world_position();

	// その点がどの領域に含まれているのか調べる
	int area[3] = { 0 };
	if (vector3_dot(min_point_boxcoord, xyz[0]) > +coll->world_scale()[0]) area[0] = +1;
	if (vector3_dot(min_point_boxcoord, xyz[0]) < -coll->world_scale()[0]) area[0] = -1;
	if (vector3_dot(min_point_boxcoord, xyz[1]) > +coll->world_scale()[1]) area[1] = +1;
	if (vector3_dot(min_point_boxcoord, xyz[1]) < -coll->world_scale()[1]) area[1] = -1;
	if (vector3_dot(min_point_boxcoord, xyz[2]) > +coll->world_scale()[2]) area[2] = +1;
	if (vector3_dot(min_point_boxcoord, xyz[2]) < -coll->world_scale()[2]) area[2] = -1;

	int count_area0 = 0;
	if (area[0] == 0)++count_area0;
	if (area[1] == 0)++count_area0;
	if (area[2] == 0)++count_area0;

	// めり込んでる
	if (count_area0 == 3) {
		assert(0 && "Sphere_cast::cast_box min_point is insert");
		return false;
	}

	// 面の領域
	if (count_area0 == 2) {
		// 先ほど求めたminで正しい
		contact_point = Ray_pos + Ray_dir * (tmin + radius);
		return true;
	}

	// 辺の領域
	if (count_area0 == 1) {

		// edgeの2頂点を求める
		Vector3 edge_vertex[2] = {
			xyz[0] * area[0] * coll->world_scale()[0] +
			xyz[1] * area[1] * coll->world_scale()[1] +
			xyz[2] * area[2] * coll->world_scale()[2],

			xyz[0] * area[0] * coll->world_scale()[0] +
			xyz[1] * area[1] * coll->world_scale()[1] +
			xyz[2] * area[2] * coll->world_scale()[2]
		};
		int zero_num = -1;
		for (int i = 0; i < 3; ++i) if (area[i] == 0) zero_num = i;
		edge_vertex[0] += xyz[zero_num] * +1 * coll->world_scale()[zero_num];
		edge_vertex[1] += xyz[zero_num] * -1 * coll->world_scale()[zero_num];

		// edgeの方向に投影
		Vector3 edge_z = (edge_vertex[0] - edge_vertex[1]).unit_vect();

		const Vector3 Ray_dir_projection = (Ray_dir -  edge_z * vector3_dot(edge_z, Ray_dir)).unit_vect();
		const Vector3 Ray_pos_projection = Ray_pos -  edge_z * vector3_dot(edge_z, Ray_pos);
		const Vector3 vertex_projection = (coll->world_position() + edge_vertex[0]) - edge_z * vector3_dot(edge_z, coll->world_position() + edge_vertex[0]);
		const float dir_ood = vector3_dot(Ray_dir, Ray_dir_projection); //投影した時 Ray_dir_projection * dir_ood = Ray_dir / 1

		Vector3 m = Ray_pos_projection - vertex_projection;
		float b = vector3_dot(m, Ray_dir_projection);
		float c = vector3_dot(m, m) - radius * radius;

		float D = b * b - c;
		if (D < 0)return false; //衝突しなければfalse

		tmin = -b - sqrtf(D);
		if (tmin < ray_min)return false; //最低値より小さければfalse

		tmax = -b + sqrtf(D);

		normal = (Ray_pos_projection + tmin * Ray_dir_projection) - vertex_projection;
		normal = normal.unit_vect();

		tmin /= dir_ood;
		tmax /= dir_ood;

		contact_point = Ray_pos + Ray_dir * tmin - normal * radius;

		// edgeを通り過ぎて頂点の領域に行っている可能性があるため 確認
		{
			min_point_boxcoord = Ray_pos + Ray_dir * tmin - coll->world_position();
			// その点がどの領域に含まれているのか調べる
			area[0] = 0;
			area[1] = 0;
			area[2] = 0;
			if (vector3_dot(min_point_boxcoord, xyz[0]) > +coll->world_scale()[0]) area[0] = +1;
			if (vector3_dot(min_point_boxcoord, xyz[0]) < -coll->world_scale()[0]) area[0] = -1;
			if (vector3_dot(min_point_boxcoord, xyz[1]) > +coll->world_scale()[1]) area[1] = +1;
			if (vector3_dot(min_point_boxcoord, xyz[1]) < -coll->world_scale()[1]) area[1] = -1;
			if (vector3_dot(min_point_boxcoord, xyz[2]) > +coll->world_scale()[2]) area[2] = +1;
			if (vector3_dot(min_point_boxcoord, xyz[2]) < -coll->world_scale()[2]) area[2] = -1;

			count_area0 = 0;
			if (area[0] == 0)++count_area0;
			if (area[1] == 0)++count_area0;
			if (area[2] == 0)++count_area0;

			if (count_area0 == 2)return true;
			if (count_area0 == 3) {
				const Vector3 vertex =
					xyz[0] * area[0] * coll->world_scale()[0] +
					xyz[1] * area[1] * coll->world_scale()[1] +
					xyz[2] * area[2] * coll->world_scale()[2];

				Vector3 m = Ray_pos - (coll->world_position() + vertex);
				float b = vector3_dot(m, Ray_dir);
				float c = vector3_dot(m, m) - radius * radius;

				float D = b * b - c;
				if (D < 0)return false; //衝突しなければfalse

				tmin = -b - sqrtf(D);
				if (tmin < ray_min)return false; //最低値より小さければfalse

				tmax = -b + sqrtf(D);

				normal = (Ray_pos + tmin * Ray_dir) - coll->world_position();
				normal = normal.unit_vect();

				contact_point = Ray_pos + Ray_dir * tmin - normal * radius;

				return true;
			}
		}

		return true;
	}

	// 頂点の領域
	if (count_area0 == 0) {
		const Vector3 vertex =
			xyz[0] * area[0] * coll->world_scale()[0] +
			xyz[1] * area[1] * coll->world_scale()[1] +
			xyz[2] * area[2] * coll->world_scale()[2];

		Vector3 m = Ray_pos - (coll->world_position() + vertex);
		float b = vector3_dot(m, Ray_dir);
		float c = vector3_dot(m, m) - radius * radius;

		float D = b * b - c + FLT_EPSILON; //誤差があるためFLT_EPSILONを加える
		if (D < 0)return false; //衝突しなければfalse

		tmin = -b - sqrtf(D);
		if (tmin < ray_min)return false; //最低値より小さければfalse

		tmax = -b + sqrtf(D);

		normal = (Ray_pos + tmin * Ray_dir) - (coll->world_position() + vertex);
		normal = normal.unit_vect();

		contact_point = Ray_pos + Ray_dir * tmin - normal * radius;

		return true;
	}



	return true;
}

bool sphere_cast_plane(const Vector3& Ray_pos, const Vector3& Ray_dir, const float& radius, Vector3& contact_point,
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
		coll->world_position().y + radius,
		Ray_pos,
		Ray_dir,
		tmin
	) == false) return false;

	tmax = tmin;

	return true;
}

bool sphere_cast_mesh(const Vector3& l_Ray_pos, const Vector3& l_Ray_dir, const float& radius, Vector3& contact_point,
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

bool Physics_function::sphere_cast(const Vector3& Ray_pos, const Vector3& Ray_dir, const float& radius, Vector3& contact_point,
	const float ray_min,
	const ALP_Collider* coll,
	float& tmin, float& tmax,
	Vector3& normal
) {

	//tminからtmaxにかけてrayが交差している
	tmin = +FLT_MAX;
	tmax = -FLT_MAX;

	Vector3 normal_;
	Vector3 contact_point_;

	float DOPtmin = 0, DOPtmax = 0;
	bool ret = false;

	for (const auto& shape : coll->get_shapes()) {
		// 大雑把なraycastを行う
		if (sphere_cast_14DOP(Ray_pos, Ray_dir, radius, contact_point_, shape->get_DOP(), DOPtmin, DOPtmax) == false)continue;
		if (DOPtmax < ray_min)continue;

		float tmin_ = 0;
		float tmax_ = 0;
		bool hit = false;

		if (shape->get_shape_tag() == ALPCollider_shape_type::Sphere)
			hit = sphere_cast_sphere(Ray_pos, Ray_dir, radius, contact_point_, shape, ray_min, tmin_, tmax_, normal_);

		if (shape->get_shape_tag() == ALPCollider_shape_type::BOX)
			hit = sphere_cast_box(Ray_pos, Ray_dir, radius, contact_point_, shape, ray_min, tmin_, tmax_, normal_);

		if (shape->get_shape_tag() == ALPCollider_shape_type::Plane)
			hit = sphere_cast_plane(Ray_pos, Ray_dir, radius, contact_point_, shape, ray_min, tmin_, tmax_, normal_);

		if (shape->get_shape_tag() == ALPCollider_shape_type::Mesh)
			hit = sphere_cast_mesh(Ray_pos, Ray_dir, radius, contact_point_, shape, ray_min, tmin_, tmax_, normal_);

		if (hit == false)continue;
		ret = true;

		if (tmin > tmin_) {
			tmin = tmin_;
			normal = normal_;
			contact_point = contact_point_;
		}
		tmax = ALmax(tmax, tmax_);
	}
	return ret;
}
#pragma endregion
