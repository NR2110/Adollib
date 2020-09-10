#include "ALP_generate_contact.h"

#include "closest_func.h"

using namespace Adollib;
using namespace Contacts;

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


	}

	int o = 0;
	//for (int i = 0; i < 4000000; i++) {
	//	o++;
	//}
}


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

//衝突生成
#pragma region SPHERE-SPHERE
bool physics_function::generate_contact_sphere_sphere(const Sphere& S0, const Sphere& S1, Contacts::Contact_pair& pair) {
	vector3 p0 = S0.world_position;
	vector3 p1 = S1.world_position;

	//p1 から p0　方向への法線
	vector3 n = p0 - p1;
	float length = n.norm_sqr();
	n = n.unit_vect();

	if (length < S0.world_size.x + S1.world_size.x) {
		//衝突していたらContactオブジェクトを生成する
		pair.contacts.addcontact(
			S0.world_size.x + S1.world_size.x - length,
			n,
			S0.world_size.x * vector3_Irotated_Bquaternion(-n, S0.world_orientation.conjugate()),
			S1.world_size.x * vector3_Irotated_Bquaternion(n, S1.world_orientation.conjugate())
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
	vector3 n(rotate._21, rotate._22, rotate._23);
	inverse_rotate = matrix_inverse(rotate);

	vector3 p;
	p = vector3_trans(sphere.world_position, inverse_rotate); //平面が"原点を通り"法線が(0,1,0)"の状態の時の球の中心座標

	////平面の裏からの衝突判定
	//if (half_space && p.y < 0)return 0;

	if (abs(p.y) < sphere.world_size.x) {
		n = p.y > 0 ? n : -n;

		if (pair.body[0]->shape == plane.shape) {
			//body[0]　が　plane
			//body[1]　が　sphere
			pair.contacts.addcontact(
				sphere.world_size.x - abs(p.y),
				n,
				vector3(p.x, 0, p.z),
				sphere.world_size.x * vector3_Irotated_Bquaternion(-n, sphere.world_orientation.conjugate())
			);
		}
		else {

			//body[0]　が　sphere
			//body[1]　が　plane
			pair.contacts.addcontact(
				sphere.world_size.x - abs(p.y),
				n,
				sphere.world_size.x * vector3_Irotated_Bquaternion(-n, sphere.world_orientation.conjugate()),
				vector3(p.x, 0, p.z)
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
	rotate = matrix_world(vector3(1, 1, 1), box.world_orientation.get_rotate_matrix(), box.world_position);
	inverse_rotate = matrix_inverse(rotate);

	vector3 center;
	center = vector3_trans(sphere.world_position, inverse_rotate); //boxのlocal座標系での球の中心座標

	vector3 box_halfsize = box.world_size;

	if (
		abs(center.x) - sphere.world_size.x > box_halfsize.x ||
		abs(center.y) - sphere.world_size.x > box_halfsize.y ||
		abs(center.z) - sphere.world_size.x > box_halfsize.z
		) return 0;

	//box上の最近点
	vector3 closest_point;

	closest_point = center;
	if (center.x > +box_halfsize.x)closest_point.x = +box_halfsize.x;
	if (center.x < -box_halfsize.x)closest_point.x = -box_halfsize.x;

	if (center.y > +box_halfsize.y)closest_point.y = +box_halfsize.y;
	if (center.y < -box_halfsize.y)closest_point.y = -box_halfsize.y;

	if (center.z > +box_halfsize.z)closest_point.z = +box_halfsize.z;
	if (center.z < -box_halfsize.z)closest_point.z = -box_halfsize.z;

	float distance = (closest_point - center).norm_sqr(); //最近点と球中心の距離
	if (sphere.world_size.x - distance > FLT_EPSILON) { //float誤差も調整
		vector3 n = (sphere.world_position - vector3_trans(closest_point, rotate)).unit_vect(); //boxからsphereへのベクトル

		if (pair.body[0]->shape == box.shape) {
			//body[0]　が　box
			//body[1]　が　sphere
			pair.contacts.addcontact(
				sphere.world_size.x - distance,
				-n,
				closest_point,
				sphere.world_size.x * vector3_Irotated_Bquaternion(n, sphere.world_orientation.conjugate())
			);
		}
		else {
			//body[0]　が　sphere
			//body[1]　が　box
			pair.contacts.addcontact(
				sphere.world_size.x - distance,
				n,
				sphere.world_size.x * vector3_Irotated_Bquaternion(n, sphere.world_orientation.conjugate()),
				closest_point
			);
		}
		return true;
	}

	return false;
}
#pragma endregion

#pragma region SPHERE-MESH
bool physics_function::generate_contact_sphere_mesh(const Sphere& S1, const Meshcoll& S2, Contacts::Contact_pair& pair) {
	return true;
}
#pragma endregion


#pragma region BOX-PLANE
bool physics_function::generate_contact_box_plane(const Box& box, const Plane& plane, Contacts::Contact_pair& pair) {

	vector3 vertices[8] = {
		// obb座標系での各頂点のローカル座標
		vector3(-box.world_size.x, -box.world_size.y, -box.world_size.z),
		vector3(-box.world_size.x, -box.world_size.y, +box.world_size.z),
		vector3(-box.world_size.x, +box.world_size.y, -box.world_size.z),
		vector3(-box.world_size.x, +box.world_size.y, +box.world_size.z),
		vector3(+box.world_size.x, -box.world_size.y, -box.world_size.z),
		vector3(+box.world_size.x, -box.world_size.y, +box.world_size.z),
		vector3(+box.world_size.x, +box.world_size.y, -box.world_size.z),
		vector3(+box.world_size.x, +box.world_size.y, +box.world_size.z)
	};

	//Boxと平面の衝突判定を行う
	matrix rotate, inverse_rotate;
	rotate = plane.world_orientation.get_rotate_matrix();
	rotate._41 = plane.world_position.x; //transpseの入力
	rotate._42 = plane.world_position.y;
	rotate._43 = plane.world_position.z;
	rotate._44 = 1;
	vector3 n(rotate._21, rotate._22, rotate._23);
	inverse_rotate = matrix_inverse(rotate);

	vector3 p;
	p = vector3_trans(box.world_position, inverse_rotate); //平面が"原点を通り"法線が(0,1,0)"の状態の時のBoxの中心座標(p.yが距離になる)
	//n = p.y > 0 ? n : -n; //ボックスが平面下にあったら法線を反対にする

	float max_penetrate = 0.0f;
	vector3 pointbox, pointplane;
	vector3 vs;

	if (p.y < 2) {
		int dasfgdf = 0;
	}

	for (int i = 0; i < 8; i++) {
		// 頂点のワールド座標
		vs = vector3_Irotated_Bquaternion(vertices[i], box.world_orientation);

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

struct OBB {
	vector3 center; //中心座標
	vector3 u_axes[3]; //軸の向き
	quaternion orient;
	vector3 half_width; //軸ごとの辺の長さ
};
//軸に投影した長さ
float sum_of_projected_radii(const OBB& obb, const vector3& vec) {
	return
		fabsf(vector3_dot(vec, obb.half_width.x * obb.u_axes[0])) +
		fabsf(vector3_dot(vec, obb.half_width.y * obb.u_axes[1])) +
		fabsf(vector3_dot(vec, obb.half_width.z * obb.u_axes[2]));
}
//OBBとOBBの分離軸判定 衝突していればtrueを返す ついでに色々引数に入れる
bool sat_obb_obb(
	const OBB& obb1, const OBB& obb2,
	float& smallest_penetration, //最小の貫通量
	int smallest_axis[2], //どの軸で最近になったか(edge×edge用に2つ分用意)
	SAT_TYPE& smallest_case //どのような形で最近になっているか
) {
	smallest_penetration = FLT_MAX;

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;


	float penetration = 0; //貫通量
	float ra, rb; //obb1,obb2のLに投影された長さ
	vector3 axis; //投影する軸
	vector3 distBtoA = obb1.center - obb2.center; //2obbの中心座標の距離

	//::: obb1の軸にobb2を投影 :::::::::::::::::::
	for (int i = 0; i < 3; i++)
	{
		axis = obb1.u_axes[i];
		ra = fabsf(obb1.half_width[i]);
		rb = fabsf(sum_of_projected_radii(obb2, axis));

		assert(ra > 0 && rb > 0);

		penetration = ra + rb - abs(vector3_dot(axis, distBtoA));
		if (penetration < 0) return false;

		if (fabsf(vector3_dot(axis, distBtoA)) + rb < ra) {
			//PA_FB.hit_point_to_face = false;
		}

		if (PB_FA.penetrate > penetration) {
			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = i;
			PB_FA.smallest_axis[1] = -1;
			PB_FA.smallest_case = POINTB_FACETA;
		}
	}

	//::: obb2の軸にobb1を投影 ::::::::::::::::::::
	if(PA_FB.hit_point_to_face == true)
	for (int i = 0; i < 3; i++)
	{
		axis = obb2.u_axes[i];
		ra = fabsf(sum_of_projected_radii(obb1, axis));
		rb = fabsf(obb2.half_width[i]);

		penetration = ra + rb - abs(vector3_dot(axis, distBtoA));
		if (penetration < 0) return false;

		if (fabsf(vector3_dot(axis, distBtoA)) + ra < rb) {
			//PB_FA.hit_point_to_face = false;
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

			axis = vector3_cross(obb1.u_axes[OB1], obb2.u_axes[OB2]);
			if (axis.norm() <= FLT_EPSILON * FLT_EPSILON)continue;//外積が 0 = 平行

			axis = axis.unit_vect();

			ra = fabsf(sum_of_projected_radii(obb1, axis));
			rb = fabsf(sum_of_projected_radii(obb2, axis));
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

bool physics_function::generate_contact_box_box(const Box& b0, const Box& b1, Contacts::Contact_pair& pair) {
	matrix m;
	m = b0.world_orientation.get_rotate_matrix();
	OBB obb0;
	obb0.center = b0.world_position;
	obb0.u_axes[0].x = m._11; obb0.u_axes[0].y = m._12; obb0.u_axes[0].z = m._13;
	obb0.u_axes[1].x = m._21; obb0.u_axes[1].y = m._22; obb0.u_axes[1].z = m._23;
	obb0.u_axes[2].x = m._31; obb0.u_axes[2].y = m._32; obb0.u_axes[2].z = m._33;
	obb0.half_width = b0.world_size;
	obb0.orient = b0.world_orientation;
	assert(!isnan(obb0.orient.norm()));

	m = b1.world_orientation.get_rotate_matrix();
	OBB obb1;
	obb1.center = b1.world_position;
	obb1.u_axes[0].x = m._11; obb1.u_axes[0].y = m._12; obb1.u_axes[0].z = m._13;
	obb1.u_axes[1].x = m._21; obb1.u_axes[1].y = m._22; obb1.u_axes[1].z = m._23;
	obb1.u_axes[2].x = m._31; obb1.u_axes[2].y = m._32; obb1.u_axes[2].z = m._33;
	obb1.half_width = b1.world_size;
	obb1.orient = b1.world_orientation;
	assert(!isnan(obb1.orient.norm()));

	float smallest_penetration = FLT_MAX;	//最小めり込み量
	int smallest_axis[2];	//最小めり込み量を得た分離軸の作成に使用した各OBBのローカル軸番号 辺×辺用に2つ
	SAT_TYPE smallest_case;	//衝突の種類 
	if (!sat_obb_obb(obb0, obb1, smallest_penetration, smallest_axis, smallest_case)) return false;

	//obb1の頂点がobb0の面と衝突した場合
	if (smallest_case == POINTB_FACETA)
	{
		vector3 d = obb1.center - obb0.center;	//obb0からobb1への相対位置
		vector3 n = obb0.u_axes[smallest_axis[0]];	//obb0の衝突面の法線と平行のobb0のローカル軸ベクトル
		if (vector3_dot(n, d) < 0)	//obb0とobb1の位置関係より衝突面の法線ベクトルを決定する
		{
			n = n * -1.0f;
		}
		n = n.unit_vect();

		//box1のローカル座標系での最近点(p1) obb1の8頂点のうちのどれか
		vector3 p1 = obb1.half_width;	//obb1の各辺の長さは、obb1の重心から接触点(p)への相対位置の手がかりになる
		//obb0とobb1の位置関係(d)より接触点(p)を求める
		if (vector3_dot(obb1.u_axes[0], n) > 0) p1.x = -p1.x;
		if (vector3_dot(obb1.u_axes[1], n) > 0) p1.y = -p1.y;
		if (vector3_dot(obb1.u_axes[2], n) > 0) p1.z = -p1.z;

		//box0の逆行列の作成
		matrix rotate, inverse_rotate;
		rotate = b0.world_orientation.get_rotate_matrix();
		rotate._41 = b0.world_position.x; //transpseの入力
		rotate._42 = b0.world_position.y;
		rotate._43 = b0.world_position.z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p1をobb0のローカル座標系へ
		vector3 P = vector3_Irotated_Bquaternion(p1, b1.world_orientation) + b1.world_position;
		vector3 c = vector3_trans(P, inverse_rotate);

		//obb0の最近点を求める
		vector3 p0 = c;
		vector3 box_halfsize = b0.world_size;
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
		vector3 d = obb1.center - obb0.center;
		vector3 n = obb1.u_axes[smallest_axis[1]];
		if (vector3_dot(n, d) < 0)
		{
			n = n * -1.0f;
		}
		n = n.unit_vect();

		vector3 p0 = obb0.half_width;
		if (vector3_dot(obb0.u_axes[0], -n) > 0) p0.x = -p0.x;
		if (vector3_dot(obb0.u_axes[1], -n) > 0) p0.y = -p0.y;
		if (vector3_dot(obb0.u_axes[2], -n) > 0) p0.z = -p0.z;

		//box0の逆行列の作成
		matrix rotate, inverse_rotate;
		rotate = b1.world_orientation.get_rotate_matrix();
		rotate._41 = b1.world_position.x; //transpseの入力
		rotate._42 = b1.world_position.y;
		rotate._43 = b1.world_position.z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p0をobb0のローカル座標系へ
		vector3 P = vector3_Irotated_Bquaternion(p0, b0.world_orientation) + b0.world_position;
		vector3 c = vector3_trans(P, inverse_rotate);

		//obb0の最近点を求める
		vector3 p1 = c;
		vector3 box_halfsize = b1.world_size;
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

		vector3 d = obb1.center - obb0.center;
		vector3 n;
		n = vector3_cross(obb0.u_axes[smallest_axis[0]], obb1.u_axes[smallest_axis[1]]);
		n = n.unit_vect();
		if (vector3_dot(n, d) < 0)
		{
			n = n * -1.0f;
		}

		vector3 p[2] = { obb0.half_width, obb1.half_width };
		{
			if (vector3_dot(obb0.u_axes[0], -n) > 0) p[0].x = -p[0].x;
			if (vector3_dot(obb0.u_axes[1], -n) > 0) p[0].y = -p[0].y;
			if (vector3_dot(obb0.u_axes[2], -n) > 0) p[0].z = -p[0].z;

			if (vector3_dot(obb1.u_axes[0], +n) > 0) p[1].x = -p[1].x;
			if (vector3_dot(obb1.u_axes[1], +n) > 0) p[1].y = -p[1].y;
			if (vector3_dot(obb1.u_axes[2], +n) > 0) p[1].z = -p[1].z;
		}

		vector3 P0a = vector3_Irotated_Bquaternion(p[0], obb0.orient) + obb0.center;
		vector3 P1a = vector3_Irotated_Bquaternion(p[1], obb1.orient) + obb1.center;

		float s, t;
		Closest_func::get_closestP_two_line(
			P0a, obb0.u_axes[smallest_axis[0]],
			P1a, obb1.u_axes[smallest_axis[1]],
			s, t
		);
		vector3 b_axis[3]{
			vector3(1,0,0),
			vector3(0,1,0),
			vector3(0,0,1)
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
bool physics_function::genetate_contact_mesh_plane(const Meshcoll& S1, const Plane& S2, Contacts::Contact_pair& pair) {
	return true;
}
#pragma endregion

#pragma region MESH-MESH

float sum_of_projected_radii(float& max,float& min, const Meshcoll& meshcoll, const vector3& nor) {
	int value;
	max = -FLT_MAX;
	min = +FLT_MAX;
	for (vector3 vertex : meshcoll.vertices) {
		value = vector3_dot(vertex, nor);
		if (max < value)max = value;
		if (min > value)min = value;
	}

	return max - min;
}

//どちらも凸包の場合
bool sat_convex_mesh_mesh(const Meshcoll& collA, const Meshcoll& collB,
	float& smallest_penetration, //最小の貫通量
	int smallest_axis[2], //どの軸で最近になったか(edge×edge用に2つ分用意)
	SAT_TYPE& smallest_case //どのような形で最近になっているか
) {
	smallest_penetration = FLT_MAX;

	// Meshcoll_1の各面法線を分離軸とする
	quaternion offset_quatBA = collB.world_orientation * collA.world_orientation.conjugate();
	quaternion offset_quatAB = collA.world_orientation * collB.world_orientation.conjugate();
	vector3 offset_posBA = collB.world_position - collA.world_position;
	vector3 offset_posAB = collA.world_position - collB.world_position;

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;
	float penetration = FLT_MAX;

	////::: obbAの軸にobbBを投影 :::::::::::::::::::
		//collAの座標系で計算を行う
	float maxA, minA, maxB, minB;
	for (u_int f = 0; f < collA.facet_num; f++) {
		const Facet& facet = collA.facets[f];
		const vector3& axis = facet.normal;

		// collAを分離軸に投影
		sum_of_projected_radii(maxA, minA, collA, axis);
		assert(maxA >= minA);

		// collBを分離軸に投影
		sum_of_projected_radii(maxB, minB, collB, vector3_Irotated_Bquaternion(axis, offset_quatAB));
		assert(maxB >= minB);
		float off = vector3_dot(offset_posBA, axis);
		maxB += off;
		minB += off;

		//貫通の計算
		float d1 = minA - maxB;
		float d2 = minB - maxA;
		if (d1 >= 0.0f || d2 >= 0.0f)return false;

		penetration = -ALmin(d1, d2);

			if (PB_FA.penetrate > penetration) {
			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = f;
			PB_FA.smallest_axis[1] = -1;
			PB_FA.smallest_case = POINTB_FACETA;
		}
	}

	////::: obbBの軸にobbAを投影 :::::::::::::::::::
		//collBの座標系で計算を行う
	if(PA_FB.hit_point_to_face == false)
	for (u_int f = 0; f < collB.facet_num; f++) {
		const Facet& facet = collB.facets[f];
		const vector3& axis = facet.normal;
	
		// collBを分離軸に投影
		sum_of_projected_radii(maxB, minB, collB, axis);
		assert(maxB >= minB);

		// collAを分離軸に投影
		sum_of_projected_radii(maxA, minA, collA, vector3_Irotated_Bquaternion(axis, offset_quatBA));
		assert(maxA >= minA);
		float off = vector3_dot(offset_posAB, axis);
		maxA += off;
		minA += off;

		//貫通の計算
		float d1 = minA - maxB;
		float d2 = minB - maxA;
		if (d1 >= 0.0f || d2 >= 0.0f)return false;

		penetration = -ALmin(d1, d2);

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

	//::: 外積の軸に投影(最近距離を求めるため)
		//collAの座標系で計算を行う
	for (u_int fA = 0; fA < collA.edge_num; fA++) {
		const Edge& edgeA = collA.edges[fA];

		const vector3& edgeVecA = collA.vertices[edgeA.vertexID[1]] - collA.vertices[edgeA.vertexID[0]];
		for (u_int fB = 0; fB < collB.edge_num; fB++) {
			const Edge& edgeB = collB.edges[fB];

			const vector3 edgeVecB = vector3_Irotated_Bquaternion(collB.vertices[edgeB.vertexID[1]] - collB.vertices[edgeB.vertexID[0]],offset_quatBA);

			vector3 axis = vector3_cross(edgeVecA, edgeVecB);
			if (axis.norm() <= FLT_EPSILON * FLT_EPSILON) continue;
			axis = axis.unit_vect();

			sum_of_projected_radii(maxA, minA, collA, axis);		assert(maxA >= minA);
			sum_of_projected_radii(maxB, minB, collB, axis);		assert(maxB >= minB);
			float off = vector3_dot(offset_posBA, axis);
			maxB += off;
			minB += off;

			//貫通の計算
			float d1 = minA - maxB;
			float d2 = minB - maxA;
			if (d1 >= 0.0f || d2 >= 0.0f)return false;

			penetration = -ALmin(d1, d2);

			if (smallest_penetration > penetration) {

				smallest_penetration = penetration;
				smallest_axis[0] = fA;
				smallest_axis[1] = fB;
				smallest_case = EDGE_EDGE;
			}
		}
	}

	// 分離軸が見当たらない場合OBBは交差しているはず
	return (smallest_penetration < FLT_MAX && smallest_penetration > FLT_EPSILON) ? true : false;
}
//
bool GC_concave_mesh_mesh(const Meshcoll& S1, const Meshcoll& S2, int concave_num, Contacts::Contact_pair& pair) {
	return false;
}

bool physics_function::genetate_contact_mesh_mesh(const Meshcoll& SA, const Meshcoll& SB, Contacts::Contact_pair& pair) {
	float smallest_penetration = FLT_MAX;	//最小めり込み量
	int smallest_facetID[2];	//最小めり込み量を得た分離軸の作成に使用した各OBBのローカル軸番号 辺×辺用に2つ
	SAT_TYPE smallest_case;	//衝突の種類 

	if (SA.is_Convex == true && SB.is_Convex == true) {
		if (!sat_convex_mesh_mesh(SA, SB, smallest_penetration, smallest_facetID, smallest_case))return false;

		quaternion offset_quatBA = SB.world_orientation * SA.world_orientation.conjugate();
		quaternion offset_quatAB = SA.world_orientation * SB.world_orientation.conjugate();
		vector3 offset_posBA = SB.world_position - SA.world_position;
		vector3 offset_posAB = SA.world_position - SB.world_position;

		//SBの頂点がSAの面と衝突した場合
		if (smallest_case == POINTB_FACETA)
		{
			const Meshcoll& facet_coll = SA;
			const Meshcoll& vertex_coll = SB;

			quaternion offset_quatVF = vertex_coll.world_orientation * facet_coll.world_orientation.conjugate();
			quaternion offset_quatFV = facet_coll.world_orientation * vertex_coll.world_orientation.conjugate();
			vector3 offset_posVF = vertex_coll.world_position - facet_coll.world_position;
			vector3 offset_posFV = facet_coll.world_position - vertex_coll.world_position;

			assert(smallest_facetID[1] == -1);
			const Facet& nerest_facet = facet_coll.facets[smallest_facetID[0]];

			vector3 axisF = nerest_facet.normal;
			axisF = axisF.unit_vect();

			//vertex_collのどの頂点が最近点か求める
			u_int nearest_pointID;
			vector3 p0;
			{
				float max_len = -FLT_MAX;
				vector3 axisV = vector3_Irotated_Bquaternion(axisF, offset_quatFV);

				for (u_int v_num = 0; v_num < vertex_coll.vertex_num; v_num++) {

					if (vector3_dot(vertex_coll.vertices[v_num], axisV) > max_len) {
						max_len = vector3_dot(vertex_coll.vertices[v_num], axisV);
						nearest_pointID = v_num;
					}
				}
			}
			p0 = vertex_coll.vertices[nearest_pointID];

			//上記のp0がfacet_collの最近面上のどこにあるのか
			vector3 p1;
			{
				quaternion off = quaternion_from_to_rotate(vector3(0, 1, 0), axisF);
				p1 = vector3_Irotated_Bquaternion(vertex_coll.vertices[nearest_pointID], off.conjugate());
				p1.y = 0;
				p1 = vector3_Irotated_Bquaternion(p1, off);
			}

			pair.contacts.addcontact(
				smallest_penetration,
				vector3_Irotated_Bquaternion(axisF, facet_coll.world_orientation),
				p0,
				p1
			);
		}

		//SAの頂点がSBの面と衝突した場合
		if (smallest_case == POINTA_FACETB)
		{
			const Meshcoll& facet_coll = SB;
			const Meshcoll& vertex_coll = SA;

			quaternion offset_quatVF = vertex_coll.world_orientation * facet_coll.world_orientation.conjugate();
			quaternion offset_quatFV = facet_coll.world_orientation * vertex_coll.world_orientation.conjugate();
			vector3 offset_posVF = vertex_coll.world_position - facet_coll.world_position;
			vector3 offset_posFV = facet_coll.world_position - vertex_coll.world_position;

			assert(smallest_facetID[0] == -1);
			const Facet& nerest_facet = facet_coll.facets[smallest_facetID[1]];

			vector3 axisF = nerest_facet.normal;
			axisF = axisF.unit_vect();

			//vertex_collのどの頂点が最近点か求める
			u_int nearest_pointID;
			vector3 p0;
			{
				float max_len = -FLT_MAX;
				vector3 axisV = vector3_Irotated_Bquaternion(axisF, offset_quatFV);

				for (u_int v_num = 0; v_num < vertex_coll.vertex_num; v_num++) {

					if (vector3_dot(vertex_coll.vertices[v_num], axisV) > max_len) {
						max_len = vector3_dot(vertex_coll.vertices[v_num], axisV);
						nearest_pointID = v_num;
					}
				}
			}
			p0 = vertex_coll.vertices[nearest_pointID];

			//上記のp0がnerest_facetの最近面上のどこにあるのか
			vector3 p1;
			{
				quaternion off = quaternion_from_to_rotate(vector3(0, 1, 0), axisF);
				p1 = vector3_Irotated_Bquaternion(vertex_coll.vertices[nearest_pointID], off.conjugate());
				p1.y = 0;
				p1 = vector3_Irotated_Bquaternion(p1, off);
			}

			pair.contacts.addcontact(
				smallest_penetration,
				vector3_Irotated_Bquaternion(-axisF, facet_coll.world_orientation),
				p1,
				p0
			);
		}

		//SAとSBの辺同士が衝突した場合
		else if (smallest_case == EDGE_EDGE)
		{
			quaternion offset_quatAB = SA.world_orientation * SB.world_orientation.conjugate();
			quaternion offset_quatBA = SB.world_orientation * SA.world_orientation.conjugate();
			vector3 offset_posAB = SA.world_position - SB.world_position;
			vector3 offset_posBA = SB.world_position - SA.world_position;

			const Edge& edgeA = SA.edges[smallest_facetID[0]];
			const Edge& edgeB = SB.edges[smallest_facetID[1]];

			vector3 edgeA_p[2] = {
				{SA.vertices[edgeA.vertexID[0]]},
				{SA.vertices[edgeA.vertexID[1]]}
			};
			vector3 edgeB_p[2] = {
				{SB.vertices[edgeB.vertexID[0]]},
				{SB.vertices[edgeB.vertexID[1]]}
			};


			vector3 edgeA_vec = edgeA_p[0] - edgeA_p[1];
			vector3 edgeB_vec = edgeB_p[0] - edgeB_p[1];

			vector3 axis = vector3_cross(edgeA_vec, edgeB_vec);
			axis = axis.unit_vect();

			//axisの向きをSB->SAの向きへ
			if (vector3_dot(axis, SA.vertices[edgeA.vertexID[0]]) < vector3_dot(axis, SB.vertices[edgeB.vertexID[1]]))
			{
				axis = axis * -1.0f;
			}

			//SAの座標系に持ってきて直線と直線の最近点を獲得
			float s, t;
			Closest_func::get_closestP_two_line(
				edgeA_p[0], edgeA_vec,
				edgeB_p[0], edgeB_vec,
				s, t
			);
			pair.contacts.addcontact(
				smallest_penetration,
				axis,
				edgeA_p[0] + s * edgeA_vec,
				edgeB_p[0] + t * edgeB_vec
			);
		}

	}
	else {

	}




	return true;
}

#pragma endregion

#pragma endregion
//:::::::::::::::::::::::::::