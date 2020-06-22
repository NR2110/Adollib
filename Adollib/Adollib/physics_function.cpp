#include "physics_function.h"
#include <string>

using namespace Adollib;
using namespace Contacts;

//:::::::::::::::::::::::::::::::::::::::::::::::::::
		#pragma region physics_function
//:::::::::::::::::::::::::::::::::::::::::::::::::::

using namespace physics_function;
//外力による速度などの更新
void physics_function::applyexternalforce(std::vector<Collider*>& coll) {
	for (int i = 0; i < coll.size(); i++) {
		coll[i]->apply_external_force();
	}
}

//DOP6による大雑把な当たり判定
bool Check_insert_DOP6(Collider* collA, Collider* collB) {
	vector3 dis = collA->dop6.pos - collB->dop6.pos;
	vector3 axis[6]{
		{1,0,0},
		{0,1,0},
		{0,0,1},
		{1,1,0},
		{0,1,1},
		{1,0,1}
	};

	for (int i = 0; i < 6; i++) {
		if (fabsf(vector3_dot(axis[i].unit_vect(), dis)) > collA->dop6.halfsize[i] + collB->dop6.halfsize[i]) return false;
	}
	return true;
}

void physics_function::Boardphase(const std::vector<Collider*>& coll, std::vector<Contacts::Contact_pair>& pairs) {

	//DOP_6による大雑把な当たり判定
	std::vector<Contacts::Contact_pair> new_pairs;
	for (int i = 0; i < coll.size(); i++) {
		for (int o = i + 1; o < coll.size(); o++) {

			if (Check_insert_DOP6(coll[i], coll[o]) == false)continue;

			Contact_pair new_pair;
			//new_pair.body[0]にアドレスの大きいほうをしまう
			if (coll[i] > coll[o]) {
				new_pair.body[0] = coll[i];
				new_pair.body[1] = coll[o];
			}
			else {
				new_pair.body[0] = coll[i];
				new_pair.body[1] = coll[o];
			}
			new_pairs.push_back(new_pair);
		}
	}

	//生成したpairが前のpairから存在しているかの確認
	for (int new_num = 0; new_num < new_pairs.size(); new_num++) {
		for (int old_num = 0; old_num < pairs.size(); old_num++) {
			if (
				new_pairs[new_num].body[0] == pairs[old_num].body[0] &&
				new_pairs[new_num].body[1] == pairs[old_num].body[1]
				) {
				new_pairs[new_num].type = Pairtype::keep_pair;
			}
			else {
				new_pairs[new_num].type = Pairtype::new_pair;
			}
		}
	}

}

//:::::::::::::::::::::::::::
	#pragma region generate_contact
//:::::::::::::::::::::::::::

//衝突生成(Narrowphase)
void physics_function::generate_contact(std::vector<Contacts::Contact_pair*>& pairs) {

	for (int i = 0; i < pairs.size(); i++) {

		if (pairs[i]->body[0]->shape == Collider_shape::shape_sphere) {
			if (pairs[i]->body[1]->shape == Collider_shape::shape_sphere) {}
			if (pairs[i]->body[1]->shape == Collider_shape::shape_box) {}
			if (pairs[i]->body[1]->shape == Collider_shape::shape_plane) {}
		}
		if (pairs[i]->body[0]->shape == Collider_shape::shape_box) {
			if (pairs[i]->body[1]->shape == Collider_shape::shape_sphere) {}
			if (pairs[i]->body[1]->shape == Collider_shape::shape_box) {}
			if (pairs[i]->body[1]->shape == Collider_shape::shape_plane) {}
		}
		if (pairs[i]->body[0]->shape == Collider_shape::shape_plane) {
			if (pairs[i]->body[1]->shape == Collider_shape::shape_sphere) {}
			if (pairs[i]->body[1]->shape == Collider_shape::shape_box) {}
			//if (pairs[i]->body[1]->shape == Collider_shape::shape_plane) {}
		}


	}
}

//衝突生成
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
			S0.world_size.x * n,
			S1.world_size.x * -n
		);
		return true;
	}

	return false;
}
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
				sphere.world_size.x * -n
			);
		}
		else {
			//body[0]　が　sphere
			//body[1]　が　plane
			pair.contacts.addcontact(
				sphere.world_size.x - abs(p.y),
				-n,
				sphere.world_size.x * -n,
				vector3(p.x, 0, p.z)
			);
		}
		return true;
	}

	return false;
}
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
	if (distance < sphere.world_size.x && distance > FLT_EPSILON) { //float誤差も調整
		vector3 n = (sphere.world_position - closest_point).unit_vect();
		if (pair.body[0]->shape == box.shape) {
			//body[0]　が　box
			//body[1]　が　sphere
			pair.contacts.addcontact(
				sphere.world_size.x - distance,
				n,
				closest_point,
				sphere.world_size.x * -n
			);
		}
		else {
			//body[0]　が　sphere
			//body[1]　が　box
			pair.contacts.addcontact(
				sphere.world_size.x - distance,
				-n,
				sphere.world_size.x * -n,
				closest_point
			);
		}
		return true;
	}

	return false;
}
bool physics_function::generate_contact_box_plane(const Box& box, const Plane& plane, Contacts::Contact_pair& pair) {

	vector3 vertices[8] = {
		//原点AABBの頂点の位置
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
	p = vector3_trans(box.world_position, inverse_rotate); //平面が"原点を通り"法線が(0,1,0)"の状態の時のBoxの中心座標
	n = p.y > 0 ? n : -n; //ボックスが平面下にあったら法線を反対にする

	float max_penetrate = 0.0f;
	vector3 pointbox, pointplane;
	for (int i = 0; i < 8; i++) {

		float rA = vector3_dot(-n, vertices[i]);

		if (fabsf(p.y) < rA) {
			max_penetrate = rA - p.y;
			pointbox = vertices[i];
			pointplane = vector3(p.x, 0, p.z);
		}
	}

	if (max_penetrate != 0) {
		if (pair.body[0]->shape == plane.shape) {
			//body[0]　が　plane
			//body[1]　が　box
			pair.contacts.addcontact(
				max_penetrate,
				n,
				pointplane,
				pointbox
			);
		}
		else {
			//body[0]　が　box
			//body[1]　が　plane
			pair.contacts.addcontact(
				max_penetrate,
				-n,
				pointbox,
				pointplane
			);
		}

		return true;
	}
	return false;
}

enum SAT_TYPE {
	POINTA_FACETB,
	POINTB_FACETA,
	EDGE_EDGE
};
struct OBB {
	vector3 center; //中心座標
	vector3 u_axes[3]; //軸の向き
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
	float penetration = 0; //貫通量

	std::vector<vector3> contact_point;

	float ra, rb; //obb1,obb2のLに投影された長さ
	vector3 L; //投影する軸
	vector3 T = obb2.center - obb1.center; //2obbの中心座標の距離

	// obb1の軸にobb2を投影
	for (int i = 0; i < 3; i++)
	{
		L = obb1.u_axes[i];
		ra = obb1.half_width[i];
		rb = sum_of_projected_radii(obb2, L);
		if (vector3_dot(L, T) + rb < ra)continue;

		penetration = ra + rb - abs(vector3_dot(L, T));
		if (penetration < 0) return 0;
		if (smallest_penetration > penetration)
		{
			smallest_penetration = penetration;
			smallest_axis[0] = i;
			smallest_axis[1] = -1;
			smallest_case = POINTB_FACETA;
		}
	}

	// obb2の軸にobb1を投影
	for (int i = 0; i < 3; i++)
	{
		L = obb2.u_axes[i];
		ra = sum_of_projected_radii(obb1, L);
		if (vector3_dot(L, T) + rb < ra)continue;

		rb = obb2.half_width[i];
		penetration = ra + rb - abs(vector3_dot(L, T));
		if (penetration < 0) return 0;

		if (smallest_penetration > penetration)
		{
			smallest_penetration = penetration;
			smallest_axis[0] = -1;
			smallest_axis[1] = i;
			smallest_case = POINTA_FACETB;
		}
	}

	//::: 外積の軸に投影
	for (int OB1 = 0; OB1 < 3; OB1++) {
		for (int OB2 = 0; OB2 < 3; OB2++) {

			L = vector3_cross(obb1.u_axes[OB1], obb2.u_axes[OB2]);
			if (L.norm() <= FLT_EPSILON * FLT_EPSILON)continue;
			//if (L == vector3(0,0,0))continue; //外積が 0 = 平行

			L = L.unit_vect();

			ra = sum_of_projected_radii(obb1, L);
			rb = sum_of_projected_radii(obb2, L);
			if (vector3_dot(L, T) + rb < ra)continue;

			penetration = ra + rb - abs(vector3_dot(L, T));
			if (penetration < 0) return 0;
			if (smallest_penetration > penetration)
			{
				smallest_penetration = penetration;
				smallest_axis[0] = OB1;
				smallest_axis[1] = OB2;
				smallest_case = EDGE_EDGE;
			}
		}
	}

	//assert(smallest_penetration < FLT_MAX);
	//assert(smallest_penetration > FLT_EPSILON);

	// 分離軸が見当たらない場合OBBは交差しているはず
	return (smallest_penetration < FLT_MAX && smallest_penetration > FLT_EPSILON) ? true : false;
}

bool physics_function::generate_contact_box_box(const Box& b0, const  Box& b1, Contacts::Contact_pair& pair) {
	matrix m;
	m = b0.world_orientation.get_rotate_matrix();
	OBB obb0;
	obb0.center = b0.world_position;
	obb0.u_axes[0].x = m._11; obb0.u_axes[0].y = m._12; obb0.u_axes[0].z = m._13;
	obb0.u_axes[1].x = m._21; obb0.u_axes[1].y = m._22; obb0.u_axes[1].z = m._23;
	obb0.u_axes[2].x = m._31; obb0.u_axes[2].y = m._32; obb0.u_axes[2].z = m._33;
	obb0.half_width = b0.world_size;

	m = b1.world_orientation.get_rotate_matrix();
	OBB obb1;
	obb1.center = b1.world_position;
	obb1.u_axes[0].x = m._11; obb1.u_axes[0].y = m._12; obb1.u_axes[0].z = m._13;
	obb1.u_axes[1].x = m._21; obb1.u_axes[1].y = m._22; obb1.u_axes[1].z = m._23;
	obb1.u_axes[2].x = m._31; obb1.u_axes[2].y = m._32; obb1.u_axes[2].z = m._33;
	obb1.half_width = b1.world_size;

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

		//box1の最近点(p1)はobb1の8頂点のうちのどれか
		vector3 p1 = obb1.half_width;	//obb1の各辺の長さは、obb1の重心から接触点(p)への相対位置の手がかりになる
		//obb0とobb1の位置関係(d)より接触点(p)を求める
		if (vector3_dot(obb1.u_axes[0], d) > 0) p1.x = -p1.x;
		if (vector3_dot(obb1.u_axes[1], d) > 0) p1.y = -p1.y;
		if (vector3_dot(obb1.u_axes[2], d) > 0) p1.z = -p1.z;
		p1 = vector3_be_rotated_by_quaternion(p1, b1.world_orientation);
		p1 += b1.world_position;

		//box0の逆行列の作成
		matrix rotate, inverse_rotate;
		rotate = b0.world_orientation.get_rotate_matrix();
		rotate._41 = b0.world_position.x; //transpseの入力
		rotate._42 = b0.world_position.y;
		rotate._43 = b0.world_position.z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);
		//p1をobb0のローカル座標系へ
		vector3 P = vector3_be_rotated_by_quaternion(p1, b1.world_orientation) + b1.world_position;
		vector3 c = vector3_trans(P, inverse_rotate);

		//obb0の最近点を求める
		vector3 p0 = c;
		vector3 box_halfsize = b1.world_size;
		if (c.x > +box_halfsize.x)p0.x = +box_halfsize.x;
		if (c.x < -box_halfsize.x)p0.x = -box_halfsize.x;

		if (c.y > +box_halfsize.y)p0.y = +box_halfsize.y;
		if (c.y < -box_halfsize.y)p0.y = -box_halfsize.y;

		if (c.z > +box_halfsize.z)p0.z = +box_halfsize.z;
		if (c.z < -box_halfsize.z)p0.z = -box_halfsize.z;

		pair.contacts.addcontact(
			smallest_penetration,
			n,
			p0,
			p1
		);
	}
	//②obb0の頂点がobb1の面と衝突した場合
	//Contactオブジェクトを生成し、全てのメンバ変数に値をセットし、コンテナ(contacts)に追加する
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
		if (vector3_dot(obb0.u_axes[0], d) > 0) p0.x = -p0.x;
		if (vector3_dot(obb0.u_axes[1], d) > 0) p0.y = -p0.y;
		if (vector3_dot(obb0.u_axes[2], d) > 0) p0.z = -p0.z;
		p0 = vector3_be_rotated_by_quaternion(p0, b0.world_orientation);
		p0 += b0.world_position;

		//box0の逆行列の作成
		matrix rotate, inverse_rotate;
		rotate = b1.world_orientation.get_rotate_matrix();
		rotate._41 = b1.world_position.x; //transpseの入力
		rotate._42 = b1.world_position.y;
		rotate._43 = b1.world_position.z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);
		//p1をobb0のローカル座標系へ
		vector3 P = vector3_be_rotated_by_quaternion(p0, b1.world_orientation) + b1.world_position;
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

		pair.contacts.addcontact(
			smallest_penetration,
			n,
			p0,
			p1
		);
	}
	//③obb0の辺とobb1の辺と衝突した場合
	//Contactオブジェクトを生成し、全てのメンバ変数に値をセットし、コンテナ(contacts)に追加する
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
			if (vector3_dot(obb0.u_axes[0], n) > 0) p[0].x = -p[0].x;
			if (vector3_dot(obb0.u_axes[1], n) > 0) p[0].y = -p[0].y;
			if (vector3_dot(obb0.u_axes[2], n) > 0) p[0].z = -p[0].z;
			p[0][smallest_axis[0]] = 0;

			if (vector3_dot(obb1.u_axes[0], n) > 0) p[1].x = -p[1].x;
			if (vector3_dot(obb1.u_axes[1], n) > 0) p[1].y = -p[1].y;
			if (vector3_dot(obb1.u_axes[2], n) > 0) p[1].z = -p[1].z;
			p[1][smallest_axis[1]] = 0;
		}

		pair.contacts.addcontact(
			smallest_penetration,
			n,
			p[0],
			p[1]
		);
	}
	else assert(0);

	return true;
}

#pragma endregion
//:::::::::::::::::::::::::::

//:::::::::::::::::::::::::::
	#pragma region resolve_contact
//:::::::::::::::::::::::::::

//衝突解決
// ソルバーボディ(解決用のなんか)
// ボールジョイント
struct Balljoint {
	float bias; // 拘束の強さの調整値
	int rigidBodyA; // 剛体Aへのインデックス
	int rigidBodyB; // 剛体Bへのインデックス
	vector3 anchorA; // 剛体Aのローカル座標系における接続点
	vector3 anchorB; // 剛体Bのローカル座標系における接続点
	Constraint constraint; // 拘束

	// 初期化
	void reset()
	{
		bias = 0.1f;
		constraint.accuminpulse = 0.0f;
	}
};
void CalcTangentVector(const vector3& normal, vector3& tangent1, vector3& tangent2)
{
	vector3 vec(1.0f, 0.0f, 0.0f);
	vector3 n(normal);
	n[0] = 0.0f;
	if (n.norm() < FLT_EPSILON) {
		vec = vector3(0.0f, 1.0f, 0.0f);
	}
	tangent1 = (vector3_cross(normal, vec)).norm_sqr();
	tangent2 = (vector3_cross(tangent1, normal)).norm_sqr();
}

void physics_function::resolve_contact(std::vector<Collider*> colliders, std::vector<Contacts::Contact_pair>& pairs) {

	//::: 解決用オブジェクトの生成 :::::::::::
	std::vector<Solverbody> solverBodies;
	for (int i = 0; i < colliders.size(); i++) {
		Solverbody SB;
		SB.orientation = colliders[i]->world_orientation;
		SB.delta_LinearVelocity = vector3(0.0f);
		SB.delta_AngularVelocity = vector3(0.0f);
		SB.inv_inertia = colliders[i]->inverse_inertial_tensor();
		SB.inv_mass = colliders[i]->inverse_mass();

		solverBodies.emplace_back(SB);
		colliders[i]->solve = &solverBodies.back();
	}

	//std::vector<Balljoint> balljoints; //今回はなし
	Collider* coll[2];
	Solverbody* solverbody[2];

	for (int P_num = 0; P_num < pairs.size(); P_num++) {
		Contact_pair& pair = pairs[P_num];

		coll[0] = pairs[P_num].body[0];
		coll[1] = pairs[P_num].body[1];
		solverbody[0] = coll[0]->solve;
		solverbody[1] = coll[1]->solve;

		// 摩擦の獲得
		pair.contacts.friction = sqrtf(coll[0]->friction * coll[1]->friction);

		//::: 衝突点情報の更新 ::::::::
		for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
			Contactpoint& cp = pair.contacts.contactpoints[C_num];

			vector3 rA = vector3_be_rotated_by_quaternion(cp.point[0], solverbody[0]->orientation);
			vector3 rB = vector3_be_rotated_by_quaternion(cp.point[1], solverbody[1]->orientation);

			// 反発係数の獲得
			// 継続の衝突の場合反発係数を0にする
			float restitution = pair.type == Pairtype::new_pair ? 0.5f * (coll[0]->restitution + coll[1]->restitution) : 0.0f;

			//衝突時のそれぞれの速度
			vector3 pdota;
			pdota = vector3_cross(coll[0]->angular_velocity, rA);
			pdota += coll[0]->linear_velocity;

			vector3 pdotb;
			pdotb = vector3_cross(coll[1]->angular_velocity, rB);
			pdotb += coll[1]->linear_velocity;

			//衝突時の衝突平面法線方向の相対速度(結局衝突に使うのは法線方向への速さ)
			vector3 vrel = pdota - pdotb;

			vector3
				tangent1, //normalに対するz方向
				tangent2; //normalに対するx方向
			CalcTangentVector(cp.normal, tangent1, tangent2);

			//Baraff[1997]の式(8-18)の分母(denominator)を求める
			float term1 = coll[0]->inverse_mass();
			float term2 = coll[1]->inverse_mass();
			vector3 tA, tB;


			// Normal
			{
				//Baraff[1997]の式(8-18)の分母(denominator)を求める
				vector3 axis = cp.normal;
				tA = vector3_cross(rA, axis);
				tB = vector3_cross(rB, axis);
				tA = vector3_trans(tA, coll[0]->inverse_inertial_tensor());
				tB = vector3_trans(tB, coll[1]->inverse_inertial_tensor());
				tA = vector3_cross(tA, rA);
				tB = vector3_cross(tB, rB);
				float term3 = vector3_dot(axis, tA);
				float term4 = vector3_dot(axis, tB);
				float denominator = term1 + term2 + term3 + term4;

				cp.constraint[0].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)の分母?
				cp.constraint[0].rhs = -(1.0f + restitution) * vector3_dot(vrel, axis); //Baraff1997(8-18)の分子
				//cp.constraint[0].rhs -= (bias * EPX_MIN(0.0f, cp.distance + slop)) / timeStep; //めり込みを直す値
				cp.constraint[0].rhs *= cp.constraint[0].jacDiagInv;
				cp.constraint[0].lowerlimit = 0.0f;
				cp.constraint[0].upperlimit = FLT_MAX;
				cp.constraint[0].axis = axis;
			}

			// Tangent1
			{
				vector3 axis = tangent1;
				tA = vector3_cross(rA, axis);
				tB = vector3_cross(rB, axis);
				tA = vector3_trans(tA, coll[0]->inverse_inertial_tensor());
				tB = vector3_trans(tB, coll[1]->inverse_inertial_tensor());
				tA = vector3_cross(tA, rA);
				tB = vector3_cross(tB, rB);
				float term3 = vector3_dot(axis, tA);
				float term4 = vector3_dot(axis, tB);
				float denominator = term1 + term2 + term3 + term4;

				cp.constraint[0].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)の分母?
				cp.constraint[0].rhs = -(1.0f + restitution) * vector3_dot(vrel, axis); //Baraff1997(8-18)の分子
				//cp.constraint[0].rhs -= (bias * EPX_MIN(0.0f, cp.distance + slop)) / timeStep;
				cp.constraint[0].rhs *= cp.constraint[0].jacDiagInv;
				cp.constraint[0].lowerlimit = 0.0f;
				cp.constraint[0].upperlimit = FLT_MAX;
				cp.constraint[0].axis = axis;
			}

			// Tangent2
			{
				vector3 axis = tangent2;
				tA = vector3_cross(rA, axis);
				tB = vector3_cross(rB, axis);
				tA = vector3_trans(tA, coll[0]->inverse_inertial_tensor());
				tB = vector3_trans(tB, coll[1]->inverse_inertial_tensor());
				tA = vector3_cross(tA, rA);
				tB = vector3_cross(tB, rB);
				float term3 = vector3_dot(axis, tA);
				float term4 = vector3_dot(axis, tB);
				float denominator = term1 + term2 + term3 + term4;

				cp.constraint[0].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)の分母?
				cp.constraint[0].rhs = -(1.0f + restitution) * vector3_dot(vrel, axis); //Baraff1997(8-18)の分子
				//cp.constraint[0].rhs -= (bias * EPX_MIN(0.0f, cp.distance + slop)) / timeStep;
				cp.constraint[0].rhs *= cp.constraint[0].jacDiagInv;
				cp.constraint[0].lowerlimit = 0.0f;
				cp.constraint[0].upperlimit = FLT_MAX;
				cp.constraint[0].axis = axis;
			}

		}
	}

	//::: 各フレームで変化する速度などを求める :::::::::::::::
	for (int P_num = 0; P_num < pairs.size(); P_num++) {
		Contact_pair& pair = pairs[P_num];

		coll[0] = pair.body[0];
		coll[1] = pair.body[1];
		solverbody[0] = coll[0]->solve;
		solverbody[1] = coll[1]->solve;

		for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
			//衝突点の情報　
			Contactpoint& cp = pair.contacts.contactpoints[C_num];
			vector3 rA = vector3_be_rotated_by_quaternion(cp.point[0], solverbody[0]->orientation);
			vector3 rB = vector3_be_rotated_by_quaternion(cp.point[1], solverbody[1]->orientation);

			for (int k = 0; k < 3; k++) {
				float deltaImpulse = cp.constraint[k].accuminpulse;
				solverbody[0]->delta_LinearVelocity += deltaImpulse * solverbody[0]->inv_mass * cp.constraint[k].axis;
				solverbody[0]->delta_AngularVelocity += deltaImpulse * vector3_trans(vector3_cross(rA, cp.constraint[k].axis), solverbody[0]->inv_inertia);
				solverbody[1]->delta_LinearVelocity -= deltaImpulse * solverbody[1]->inv_mass * cp.constraint[k].axis;
				solverbody[1]->delta_AngularVelocity -= deltaImpulse * vector3_trans(vector3_cross(rB, cp.constraint[k].axis), solverbody[1]->inv_inertia);
			}
		}
	}

	for (int i = 0; i < 10; i++) {
		for (int P_num = 0; P_num < pairs.size(); P_num++) {
			Contact_pair& pair = pairs[P_num];

			coll[0] = pair.body[0];
			coll[1] = pair.body[1];
			solverbody[0] = coll[0]->solve;
			solverbody[1] = coll[1]->solve;

			for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
				//衝突点の情報　
				Contactpoint& cp = pair.contacts.contactpoints[C_num];
				vector3 rA = vector3_be_rotated_by_quaternion(cp.point[0], solverbody[0]->orientation);
				vector3 rB = vector3_be_rotated_by_quaternion(cp.point[1], solverbody[1]->orientation);

				{
					Constraint& constraint = cp.constraint[0];
					float delta_impulse = constraint.rhs;
					vector3 delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngularVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngularVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
					solverbody[0]->delta_AngularVelocity += delta_impulse * vector3_trans(vector3_cross(rA, constraint.axis), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngularVelocity -= delta_impulse * vector3_trans(vector3_cross(rB, constraint.axis), solverbody[1]->inv_inertia);
				}

				float max_friction = pair.contacts.friction * fabsf(cp.constraint[0].accuminpulse);
				cp.constraint[1].lowerlimit = -max_friction;
				cp.constraint[1].upperlimit = +max_friction;
				cp.constraint[2].lowerlimit = -max_friction;
				cp.constraint[2].upperlimit = +max_friction;

				{
					Constraint& constraint = cp.constraint[1];
					float delta_impulse = constraint.rhs;
					vector3 delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngularVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngularVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
					solverbody[0]->delta_AngularVelocity += delta_impulse * vector3_trans(vector3_cross(rA, constraint.axis), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngularVelocity -= delta_impulse * vector3_trans(vector3_cross(rB, constraint.axis), solverbody[1]->inv_inertia);
				}
				{
					Constraint& constraint = cp.constraint[2];
					float delta_impulse = constraint.rhs;
					vector3 delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngularVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngularVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
					solverbody[0]->delta_AngularVelocity += delta_impulse * vector3_trans(vector3_cross(rA, constraint.axis), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngularVelocity -= delta_impulse * vector3_trans(vector3_cross(rB, constraint.axis), solverbody[1]->inv_inertia);
				}




			}


		}
	}

	// 速度の更新
	for (int i = 0; i < colliders.size(); i++) {
		colliders[i]->linear_velocity += colliders[i]->solve->delta_LinearVelocity;
		colliders[i]->angular_velocity += colliders[i]->solve->delta_AngularVelocity;
	}


}

#pragma endregion
//:::::::::::::::::::::::::::

void physics_function::integrate(std::vector<Collider*>& coll) {





}

#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::