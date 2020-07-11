#include "rigit_body.h"

using namespace Adollib;
using namespace Contacts;

#include "gameobject.h"
#include "Adollib.h"
#include "contact.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		#pragma region Rigitbody
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Collider::apply_external_force(float duration) {
	if (is_movable()) {

		//並進移動に加える力(accumulated_force)から加速度を出して並進速度を更新する
		liner_acceleration += accumulated_force / inertial_mass;
		linear_velocity += liner_acceleration * duration;
		

		//各回転に加える力(accumulated_torque)から加速度を出して角速度を更新する
		matrix inverse_inertia_tensor = matrix_inverse(inertial_tensor);
		matrix rotation = local_orientation.get_rotate_matrix();
		matrix transposed_rotation = matrix_trans(rotation);
		inverse_inertia_tensor = transposed_rotation * inverse_inertia_tensor * rotation; //QUESTION : 何してるの?対角化?
		angular_acceleration += vector3_trans(accumulated_torque, inverse_inertia_tensor);

		angular_velocity += angular_acceleration * duration;
		if (angular_velocity.norm() < FLT_EPSILON)angular_velocity = vector3(0, 0, 0);

		//QUESTION : 上の式じゃダメなの?
		//各速度による姿勢の更新
#if 1

#else 
		//角速度による姿勢の更新
		quaternion w(0, angular_velocity.x, angular_velocity.y, angular_velocity.z);
		w = orientation * w;
		orientation += 0.5f * w * duration;
		orientation = orientation.unit_vect();
#endif
	}

	//加速を0にする
	accumulated_force = vector3(0, 0, 0);
	accumulated_torque = vector3(0, 0, 0);

	liner_acceleration = vector3(0, 0, 0);
	angular_acceleration = vector3(0, 0, 0);
}
void Collider::integrate(float duration) {
	//位置の更新
	if (linear_velocity.norm() >= FLT_EPSILON)
		world_position += linear_velocity * duration;

	world_orientation *= quaternion_radian_axis(angular_velocity.norm_sqr() * duration * 0.5, angular_velocity.unit_vect());
	world_orientation = world_orientation.unit_vect();

	//quaternion dAng = quaternion(0, angular_velocity.x, angular_velocity.y, angular_velocity.z) * world_orientation * 0.5f;
	//world_orientation = (world_orientation + dAng * duration).unit_vect();

}


void Collider::resolve_gameobject() {
	gameobject->co_e.orient *= local_orientation.conjugate() * gameobject->get_world_orientate().conjugate() * world_orientation;
	gameobject->co_e.position += world_position -  vector3_be_rotated_by_quaternion(local_position * gameobject->get_world_scale(), world_orientation) - gameobject->get_world_position();
	
}
void Box::update_world_trans() {
	world_orientation = gameobject->get_world_orientate() * local_orientation;
	world_size = gameobject->get_world_scale() * local_scale * half_size;
	world_position = gameobject->get_world_position() + vector3_be_rotated_by_quaternion(local_position * gameobject->get_world_scale(), world_orientation);

	update_inertial(world_size, density);
}
void Sphere::update_world_trans() {
	world_orientation = gameobject->get_world_orientate() * local_orientation;
	world_size = gameobject->get_world_scale() * local_scale * r;
	world_position = gameobject->get_world_position() + vector3_be_rotated_by_quaternion(local_position * world_size, world_orientation);
	update_inertial(world_size, density);
}
void Plane::update_world_trans() {
	world_orientation = gameobject->get_world_orientate() * local_orientation;
	world_size = gameobject->get_world_scale() * local_scale;
	world_position = gameobject->get_world_position() + vector3_be_rotated_by_quaternion(local_position * world_size, world_orientation);

	update_inertial(world_size, density);
}
void Box::update_dop6() {
	dop6.pos = gameobject->get_world_position();
	vector3 axis[6]{
	{1,0,0},
	{0,1,0},
	{0,0,1},
	{1,1,0},
	{0,1,1},
	{1,0,1}
	};

	//各頂点のローカル座標
	vector3 half[8]{
	{+half_size.x, -half_size.y, -half_size.z},
	{+half_size.x, -half_size.y, +half_size.z},
	{+half_size.x, +half_size.y, -half_size.z},
	{+half_size.x, +half_size.y, +half_size.z},
	{-half_size.x, -half_size.y, -half_size.z},
	{-half_size.x, -half_size.y, +half_size.z},
	{-half_size.x, +half_size.y, -half_size.z},
	{-half_size.x, +half_size.y, +half_size.z},
	};
	quaternion WO = gameobject->get_world_orientate();
	for (int i = 0; i < 8; i++) {
		half[i] = vector3_be_rotated_by_quaternion(half[i], WO);
	}

	//DOP_6の更新
	for (int i = 0; i < 6; i++) {
		float MAX_DIS = 0;
		for (int o = 0; o < 8; o++) {
			float dis = fabsf(vector3_dot(axis[i].unit_vect(), half[o]));
			if (MAX_DIS < dis * 1.01f) {
				dop6.max[i] = dis * 1.01f;//確実にするためちょっと大きめにとる
				MAX_DIS = dis; 
			}

		}
	}

}
void Sphere::update_dop6() {
	dop6.pos = gameobject->get_world_position();	
	for (int i = 0; i < 6; i++) {
		dop6.max[i] = r * 1.01;
	}
}
void Plane::update_dop6() {
	dop6.pos = gameobject->get_world_position();
	for (int i = 0; i < 6; i++) {
		dop6.max[i] = FLT_MAX;
	}
}

void Collider::add_force(const vector3& force) {
	accumulated_force += force;
}
void Collider::add_torque(const vector3& force) {
	accumulated_torque += force;
}

bool Collider::is_movable() const {
	return (move && inertial_mass > 0 && inertial_mass < FLT_MAX);
}
float Collider::inverse_mass() const {
	if (is_movable()) return 1.0f / inertial_mass;
	//非可動オブジェクトなら0を返す
	else return 0;
}

matrix Collider::inverse_inertial_tensor() const{
	matrix inverse_inertial_tensor;
	if (is_movable()) {
		inverse_inertial_tensor = matrix_inverse(inertial_tensor);
		if(1){
			matrix rotation, transposed_rotation;
			rotation = world_orientation.get_rotate_matrix();
			transposed_rotation = matrix_trans(rotation);
			inverse_inertial_tensor = transposed_rotation * inverse_inertial_tensor * rotation;
		}
	}
	else {
		inverse_inertial_tensor = matrix_identity();
		// QUESTIION : 上だとオブジェクトの中心から離れた衝突判定でバグる　下にすべき?
		//inverse_inertial_tensor._11 = FLT_EPSILON;
		//inverse_inertial_tensor._22 = FLT_EPSILON;
		//inverse_inertial_tensor._33 = FLT_EPSILON;
		inverse_inertial_tensor._11 = 0;
		inverse_inertial_tensor._22 = 0;
		inverse_inertial_tensor._33 = 0;
	}
	return inverse_inertial_tensor;
}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/*
		#pragma region generate
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


bool intersect_sphere_sphere(vector3& normal, float& penetration, vector3& point, Sphere& S0, Sphere& S1) {
	//:::::::
	//Ray (p + td = X)と　Sphere((C - X)・(C = X) = r * r)の二次方程式から計算
	//p = S0.position C = S1.positionとする
	//:::::::

	//相対速度からRayを求める
	vector3 d = S0.linear_velocity - S1.linear_velocity;
	if (vector3_dot(d, S1.world_position - S0.world_position) <= 0) return false; //離れていってる
	penetration = d.norm_sqr();
	d = d.unit_vect();

	vector3 m = S0.world_position - S1.world_position;
	float b = vector3_dot(m, d);
	float c = vector3_dot(m, m) - (S0.r + S1.r) * (S0.r + S1.r);

	//判別式のルート内部
	float discr = b * b - c;
	if (discr < 0) return false; //衝突しない
	float time = -b - sqrtf(discr); 
	//if (time < 0)return false; //離れていってる
	if (penetration < time)return false; //このフレームの移動では衝突しない

	penetration -= time;
	point = S0.world_position + time * d;
	normal = (point - S1.world_position).unit_vect();
	point -= normal * S0.r;
}

int Adollib::Contacts::generate_contact_sphere_sphere(Sphere& S0, Sphere& S1, std::vector<Contact>& contacts, float restitution) {

//球と球の衝突判定を行う
	if (&S1 == &S0)return 0;

#if USE_CHECK_CONTACT
	float penetration;
	vector3 point, normal;

	if (intersect_sphere_sphere(normal, penetration, point, S0, S1)) {

		//衝突していたらContactオブジェクトを生成する
		Contact contact;
		contact.normal = normal;
		contact.penetration = penetration;
		contact.point = point;
		contact.body[0] = &S0;
		contact.body[1] = &S1;
		contact.restitution = restitution;
 		contacts.push_back(contact);

		return 1;

	}

#else
	vector3 p0 = S0.world_position;
	vector3 p1 = S1.world_position;

	vector3 n = p0 - p1;
	float length = n.norm_sqr();
	n = n.unit_vect();

	if (length < S0.world_size.x + S0.world_size.x) {

		//衝突していたらContactオブジェクトを生成する
		Contact contact;
		contact.normal = n;
		contact.penetration = S0.world_size.x + S0.world_size.x - length;
		contact.point = p1 + 0.5f * length * n;
		contact.body[0] = &S0;
		contact.body[1] = &S1;
		contact.restitution = restitution;
		contacts.push_back(contact);
		S0.contacts.push_back(&contacts.back());
		S1.contacts.push_back(&contacts.back());
		return 1;
	}
#endif
	return 0;

}
int Adollib::Contacts::generate_contact_sphere_plane(Sphere& sphere, Plane& plane, std::vector<Contact>& contacts, float restitution, bool half_space) {
	//球面と平面の衝突判定を行う
	matrix rotate, inverse_rotate;
	rotate = plane.world_orientation.get_rotate_matrix();
	rotate._41 = plane.world_position.x; //transpseの入力
	rotate._42 = plane.world_position.y;
	rotate._43 = plane.world_position.z;
	rotate._44 = 1;
	vector3 n(rotate._21, rotate._22, rotate._23); //QUESTION : なんでこれで法線になるの?
	inverse_rotate = matrix_inverse(rotate);

	vector3 p;
	p = vector3_trans(sphere.world_position, inverse_rotate); //平面が"原点を通り"法線が(0,1,0)"の状態の時の球の中心座標

	//平面の裏からの衝突判定
	if (half_space && p.y < 0)return 0;

	if (abs(p.y) < sphere.world_size.x) {

		Contact contact;
		contact.normal = p.y > 0 ? n : -n;
		contact.point = sphere.world_position + p.y * -n;
		contact.penetration = sphere.world_size.x - abs(p.y);
		contact.body[0] = &sphere;
		contact.body[1] = &plane;
		contact.restitution = restitution;
		contacts.push_back(contact);
		sphere.contacts.push_back(&contacts.back());
		plane.contacts.push_back(&contacts.back());
		return 1;
	}
	return 0;

}
int Adollib::Contacts::generate_contact_sphere_box(Sphere& sphere, Box& box, std::vector<Contact>& contacts, float restitution) {
	//球とboxの衝突判定を行う
	matrix rotate, inverse_rotate;
	//rotate = box.world_orientation.get_rotate_matrix();
	//rotate._41 = box.world_position.x; //transpseの入力
	//rotate._42 = box.world_position.y;
	//rotate._43 = box.world_position.z;
	//rotate._44 = 1;
	rotate = matrix_world(vector3(1,1,1), box.world_orientation.get_rotate_matrix(), box.world_position);
	inverse_rotate = matrix_inverse(rotate);

	vector3 center;
	center = vector3_trans(sphere.world_position, inverse_rotate); //boxが"中心原点"回転してない(orientが(0,0,1))"の状態の時の球の中心座標

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

		closest_point = vector3_trans(closest_point, rotate); //通常の座標に戻す
		Contact contact;
		contact.normal = (sphere.world_position - closest_point).unit_vect();
		contact.point = closest_point;
		contact.penetration = sphere.world_size.x - distance;
		contact.penetration *= 2;
		contact.body[0] = &sphere;
		contact.body[1] = &box;
		contact.restitution = restitution;
		contacts.push_back(contact);
		sphere.contacts.push_back(&contacts.back());
		box.contacts.push_back(&contacts.back());

		return 1;
	}

	return 0;
}
int Adollib::Contacts::generate_contact_box_plane(Box& box, Plane& plane, std::vector<Contact>& contacts, float restitution) {

	//boxとhウィメンの衝突判定
	//QUESTION : なぜ頂点ごとにContact作ってるの? 
#if 1
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

	vector3 n;
	n = vector3_be_rotated_by_quaternion(vector3(0, 1, 0), plane.world_orientation); //orientationをベクトルに直す
	float plane_distance = vector3_dot(n, plane.world_position);

	int contacts_used = 0;
	for (int i = 0; i < 8; i++) {

		//頂点をもとの座標に
		vertices[i] = vector3_be_rotated_by_quaternion(vertices[i], box.world_orientation);
		vertices[i] += box.world_position;

		float vertices_distance = vector3_dot(vertices[i], n); //平面の法線上に投影

		if (vertices_distance < plane_distance) {
			Contact contact;
			contact.normal = n;
			contact.point = vertices[i];
			contact.penetration = plane_distance - vertices_distance;
			contact.body[0] = &box;
			contact.body[1] = &plane;
			contact.restitution = restitution;
			contacts.push_back(contact);
			box.contacts.push_back(&contacts.back());
			plane.contacts.push_back(&contacts.back());

			contacts_used++;
		}

	}
	return contacts_used; //いくつの頂点が裏にあったか


#else
	vector3 vertices[8] = {
		//原点AABBの頂点の位置
		vector3(-box.half_size.x, -box.half_size.y, -box.half_size.z),
		vector3(-box.half_size.x, -box.half_size.y, +box.half_size.z),
		vector3(-box.half_size.x, +box.half_size.y, -box.half_size.z),
		vector3(-box.half_size.x, +box.half_size.y, +box.half_size.z),
		vector3(+box.half_size.x, -box.half_size.y, -box.half_size.z),
		vector3(+box.half_size.x, -box.half_size.y, +box.half_size.z),
		vector3(+box.half_size.x, +box.half_size.y, -box.half_size.z),
		vector3(+box.half_size.x, +box.half_size.y, +box.half_size.z)
	};

	vector3 n;
	n = vector3_be_rotated_by_quaternion(vector3(0, 1, 0), plane.orientation); //orientationをベクトルに直す
	float plane_distance = vector3_dot(n, plane.position);

	int contacts_used = 0;
	Contact contact;
	contact.penetration = -1;
	contact.point = vector3(0, 0, 0);
	for (int i = 0; i < 8; i++) {

		//頂点をもとの座標に
		vertices[i] = vector3_be_rotated_by_quaternion(vertices[i], box.orientation);
		vertices[i] += box.position;

		float vertices_distance = vector3_dot(vertices[i], n); //平面の法線上に投影

		if (vertices_distance < plane_distance) {
			contact.normal = n.unit_vect();
			contact.point += vertices[i];
			contact.penetration = contact.penetration < plane_distance - vertices_distance ? plane_distance - vertices_distance : contact.penetration;
			contact.body[0] = &box;
			contact.body[1] = &plane;
			contact.restitution = restitution;

			contacts_used++;
		}

	}
	if (contacts_used != 0) {
		contact.point / contacts_used;
		contacts.push_back(contact);
	}

	return contacts_used; //いくつの頂点が裏にあったか

#endif

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
float sum_of_projected_radii(const OBB& obb,const vector3 &vec) {
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




std::vector<vector3> vertex_in_obb(const OBB& obb1, const OBB& obb2) {

	std::vector<vector3> ret;

	{
		vector3 vertexs[8]{
			{+obb1.half_width.x * obb1.u_axes[0] + obb1.half_width.y * obb1.u_axes[1] + obb1.half_width.z * obb1.u_axes[2]},
			{+obb1.half_width.x * obb1.u_axes[0] + obb1.half_width.y * obb1.u_axes[1] - obb1.half_width.z * obb1.u_axes[2]},
			{+obb1.half_width.x * obb1.u_axes[0] - obb1.half_width.y * obb1.u_axes[1] + obb1.half_width.z * obb1.u_axes[2]},
			{+obb1.half_width.x * obb1.u_axes[0] - obb1.half_width.y * obb1.u_axes[1] - obb1.half_width.z * obb1.u_axes[2]},
			{-obb1.half_width.x * obb1.u_axes[0] + obb1.half_width.y * obb1.u_axes[1] + obb1.half_width.z * obb1.u_axes[2]},
			{-obb1.half_width.x * obb1.u_axes[0] + obb1.half_width.y * obb1.u_axes[1] - obb1.half_width.z * obb1.u_axes[2]},
			{-obb1.half_width.x * obb1.u_axes[0] - obb1.half_width.y * obb1.u_axes[1] + obb1.half_width.z * obb1.u_axes[2]},
			{-obb1.half_width.x * obb1.u_axes[0] - obb1.half_width.y * obb1.u_axes[1] - obb1.half_width.z * obb1.u_axes[2]}
		};
		float ra, rb; //obb1,obb2のLに投影された長さ
		vector3 L; //投影する軸
		vector3 T = obb1.center - obb2.center; //2obbの中心座標の距離

		for (int i = 0; i < 8; i++) {
			bool vertex_in_obb = true;

			for (int o = 0; o < 3; o++) {
				{
					vector3 L = obb2.u_axes[o];
					float ra = obb2.half_width[o];
					float rb = vector3_dot(L, vertexs[i]);
					float AA = vector3_dot(L, T) + rb;
					if (AA > ra) {
						vertex_in_obb = false;
						break;
					}
				}
				{
					vector3 L = -obb2.u_axes[o];
					float ra = obb2.half_width[o];
					float rb = vector3_dot(L, vertexs[i]);
					float AA = vector3_dot(L, T) + rb;
					if (AA > ra) {
						vertex_in_obb = false;
						break;
					}
				}
			}

			if (vertex_in_obb == true)
				ret.push_back(vertexs[i]);
		}
	}

	//{
	//	vector3 vertexs[8]{
	//		{+obb2.half_width.x * obb2.u_axes[0] + obb2.half_width.y * obb2.u_axes[1] + obb2.half_width.z * obb2.u_axes[2]},
	//		{+obb2.half_width.x * obb2.u_axes[0] + obb2.half_width.y * obb2.u_axes[1] - obb2.half_width.z * obb2.u_axes[2]},
	//		{+obb2.half_width.x * obb2.u_axes[0] - obb2.half_width.y * obb2.u_axes[1] + obb2.half_width.z * obb2.u_axes[2]},
	//		{+obb2.half_width.x * obb2.u_axes[0] - obb2.half_width.y * obb2.u_axes[1] - obb2.half_width.z * obb2.u_axes[2]},
	//		{-obb2.half_width.x * obb2.u_axes[0] + obb2.half_width.y * obb2.u_axes[1] + obb2.half_width.z * obb2.u_axes[2]},
	//		{-obb2.half_width.x * obb2.u_axes[0] + obb2.half_width.y * obb2.u_axes[1] - obb2.half_width.z * obb2.u_axes[2]},
	//		{-obb2.half_width.x * obb2.u_axes[0] - obb2.half_width.y * obb2.u_axes[1] + obb2.half_width.z * obb2.u_axes[2]},
	//		{-obb2.half_width.x * obb2.u_axes[0] - obb2.half_width.y * obb2.u_axes[1] - obb2.half_width.z * obb2.u_axes[2]}
	//	};
	//	float ra, rb; //obb1,obb2のLに投影された長さ
	//	vector3 L; //投影する軸
	//	vector3 T = obb2.center - obb1.center; //2obbの中心座標の距離

	//	for (int i = 0; i < 8; i++) {
	//		bool vertex_in_obb = true;

	//		for (int o = 0; o < 3; o++) {
	//			vector3 L = obb1.u_axes[o];
	//			float ra = obb1.half_width[o];
	//			float rb = vector3_dot(L, vertexs[i]);
	//			float AA = vector3_dot(L, T) + rb;
	//			if (AA > ra) {
	//				vertex_in_obb = false;
	//				break;
	//			}
	//		}

	//		if (vertex_in_obb == true)
	//			ret.push_back(vertexs[i]);
	//	}
	//}
	return ret;
}

#if 1
int Adollib::Contacts::generate_contact_box_box(Box& b0, Box& b1, std::vector<Contact>& contacts, float restitution)
{
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
	if (!sat_obb_obb(obb0, obb1, smallest_penetration, smallest_axis, smallest_case)) return 0;

	//obb1の頂点がobb0の面と衝突した場合
	if (smallest_case == POINTB_FACETA)
	{
		vector3 d = obb1.center - obb0.center;	//obb0からobb1への相対位置
		vector3 n = obb0.u_axes[smallest_axis[0]];	//obb0の衝突面の法線と平行のobb0のローカル軸ベクトル
		if (vector3_dot(n, d) > 0)	//obb0とobb1の位置関係より衝突面の法線ベクトルを決定する
		{
			n = n * -1.0f;
		}
		n = n.unit_vect();

		std::vector<vector3> vertexs = vertex_in_obb(obb1, obb0);
		for (int i = 0; i < vertexs.size(); i++) {
			//ワールド空間へ座標変換
			vector3 p = vertexs[i];
			p += b1.world_position;

			//Contactオブジェクトを生成し、全てのメンバ変数に値をセットし、コンテナ(contacts)に追加する
			Contact contact;
			contact.normal = n;
			contact.point = p;
			contact.penetration = smallest_penetration;
			contact.body[0] = &b0;
			contact.body[1] = &b1;
			contact.restitution = restitution;
			contacts.push_back(contact);
		}
	}
	//②obb0の頂点がobb1の面と衝突した場合
	//Contactオブジェクトを生成し、全てのメンバ変数に値をセットし、コンテナ(contacts)に追加する
	else if (smallest_case == POINTA_FACETB)
	{
		vector3 d = obb0.center - obb1.center;
		vector3 n = obb1.u_axes[smallest_axis[1]];
		if (vector3_dot(n, d) > 0)
		{
			n = n * -1.0f;
		}
		n = n.unit_vect();

		std::vector<vector3> vertexs = vertex_in_obb(obb0, obb1);
		for (int i = 0; i < vertexs.size(); i++) {
			if (vertexs.size() == 2) {
				int sadsfdf = 0;
			}

			//ワールド空間へ座標変換
			vector3 p = vertexs[i];
			p += b0.world_position;

			Contact contact;
			contact.normal = n;
			contact.point = p;
			contact.penetration = smallest_penetration;
			contact.body[0] = &b1;
			contact.body[1] = &b0;
			contact.restitution = restitution;
			contacts.push_back(contact);
		}
	}
	//③obb0の辺とobb1の辺と衝突した場合
	//Contactオブジェクトを生成し、全てのメンバ変数に値をセットし、コンテナ(contacts)に追加する
	else if (smallest_case == EDGE_EDGE)
	{
		vector3 d = obb1.center - obb0.center;
		vector3 n;
		n = vector3_cross(obb0.u_axes[smallest_axis[0]], obb1.u_axes[smallest_axis[1]]);
		n = n.unit_vect();
		if (vector3_dot(n, d) > 0)
		{
			n = n * -1.0f;
		}

		vector3 p[2] = { obb0.half_width, obb1.half_width };
		{
			if (vector3_dot(obb0.u_axes[0], n) > 0) p[0].x = -p[0].x;
			if (vector3_dot(obb0.u_axes[1], n) > 0) p[0].y = -p[0].y;
			if (vector3_dot(obb0.u_axes[2], n) > 0) p[0].z = -p[0].z;
			p[0][smallest_axis[0]] = 0;				 
			p[0] = vector3_be_rotated_by_quaternion(p[0], b0.world_orientation);
			p[0] += b0.world_position;

			if (vector3_dot(obb1.u_axes[0], n) > 0) p[1].x = -p[1].x;
			if (vector3_dot(obb1.u_axes[1], n) > 0) p[1].y = -p[1].y;
			if (vector3_dot(obb1.u_axes[2], n) > 0) p[1].z = -p[1].z;
			p[1][smallest_axis[1]] = 0;
			p[1] = vector3_be_rotated_by_quaternion(p[1], b1.world_orientation);
			p[1] += b1.world_position;

		}

		Contact contact;
		contact.normal = n;
		contact.point = (p[0] + p[1]) * 0.5f;
		contact.penetration = smallest_penetration;
		contact.body[0] = &b0;
		contact.body[1] = &b1;
		contact.restitution = restitution;
		contacts.push_back(contact);
	}
	else assert(0);

	return 1;
}
#else
int Adollib::Contacts::generate_contact_box_box(Box& b0, Box& b1, std::vector<Contact>& contacts, float restitution)
{
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
	if (!sat_obb_obb(obb0, obb1, smallest_penetration, smallest_axis, smallest_case)) return 0;

	//obb1の頂点がobb0の面と衝突した場合
	if (smallest_case == POINTB_FACETA)
	{
		vector3 d = obb1.center - obb0.center;	//obb0からobb1への相対位置
		vector3 n = obb0.u_axes[smallest_axis[0]];	//obb0の衝突面の法線と平行のobb0のローカル軸ベクトル
		if (vector3_dot(n, d) > 0)	//obb0とobb1の位置関係より衝突面の法線ベクトルを決定する
		{
			n = n * -1.0f;
		}
		n = n.unit_vect();

		//接触点(p)はobb1の8頂点のうちのどれか
		vector3 p = obb1.half_width;	//obb1の各辺の長さは、obb1の重心から接触点(p)への相対位置の手がかりになる
		//obb0とobb1の位置関係(d)より接触点(p)を求める
		if (vector3_dot(obb1.u_axes[0], d) > 0) p.x = -p.x;
		if (vector3_dot(obb1.u_axes[1], d) > 0) p.y = -p.y;
		if (vector3_dot(obb1.u_axes[2], d) > 0) p.z = -p.z;
		p = vector3_be_rotated_by_quaternion(p, b1.world_orientation);
		p += b1.world_position;

		//Contactオブジェクトを生成し、全てのメンバ変数に値をセットし、コンテナ(contacts)に追加する
		Contact contact;
		contact.normal = n;
		contact.point = p;
		contact.penetration = smallest_penetration;
		contact.body[0] = &b0;
		contact.body[1] = &b1;
		contact.restitution = restitution;
		contacts.push_back(contact);
		b0.contacts.push_back(&contacts.back());
		b1.contacts.push_back(&contacts.back());

	}
	//②obb0の頂点がobb1の面と衝突した場合
	//Contactオブジェクトを生成し、全てのメンバ変数に値をセットし、コンテナ(contacts)に追加する
	else if (smallest_case == POINTA_FACETB)
	{
		vector3 d = obb0.center - obb1.center;
		vector3 n = obb1.u_axes[smallest_axis[1]];
		if (vector3_dot(n, d) > 0)
		{
			n = n * -1.0f;
		}
		n = n.unit_vect();

		vector3 p = obb0.half_width;
		if (vector3_dot(obb0.u_axes[0], d) > 0) p.x = -p.x;
		if (vector3_dot(obb0.u_axes[1], d) > 0) p.y = -p.y;
		if (vector3_dot(obb0.u_axes[2], d) > 0) p.z = -p.z;
		p = vector3_be_rotated_by_quaternion(p, b0.world_orientation);
		p += b0.world_position;

		Contact contact;
		contact.normal = n;
		contact.point = p;
		contact.penetration = smallest_penetration;
		contact.body[0] = &b1;
		contact.body[1] = &b0;
		contact.restitution = restitution;
		contacts.push_back(contact);
		b0.contacts.push_back(&contacts.back());
		b1.contacts.push_back(&contacts.back());



	}
	//③obb0の辺とobb1の辺と衝突した場合
	//Contactオブジェクトを生成し、全てのメンバ変数に値をセットし、コンテナ(contacts)に追加する
	else if (smallest_case == EDGE_EDGE)
	{
		vector3 d = obb1.center - obb0.center;
		vector3 n;
		n = vector3_cross(obb0.u_axes[smallest_axis[0]], obb1.u_axes[smallest_axis[1]]);
		n = n.unit_vect();
		if (vector3_dot(n, d) > 0)
		{
			n = n * -1.0f;
		}

		vector3 p[2] = { obb0.half_width, obb1.half_width };
		{
			if (vector3_dot(obb0.u_axes[0], n) > 0) p[0].x = -p[0].x;
			if (vector3_dot(obb0.u_axes[1], n) > 0) p[0].y = -p[0].y;
			if (vector3_dot(obb0.u_axes[2], n) > 0) p[0].z = -p[0].z;
			p[0][smallest_axis[0]] = 0;
			p[0] = vector3_be_rotated_by_quaternion(p[0], b0.world_orientation);
			p[0] += b0.world_position;

			//_DDM::I().AddCross(p[0], 1);
			//_DDM::I().AddLine(b0.position, b0.position + smallest_penetration * 100 * obb0.u_axes[smallest_axis[0]]);

			if (vector3_dot(obb1.u_axes[0], n) > 0) p[1].x = -p[1].x;
			if (vector3_dot(obb1.u_axes[1], n) > 0) p[1].y = -p[1].y;
			if (vector3_dot(obb1.u_axes[2], n) > 0) p[1].z = -p[1].z;
			p[1][smallest_axis[1]] = 0;
			p[1] = vector3_be_rotated_by_quaternion(p[1], b1.world_orientation);
			p[1] += b1.world_position;

			//_DDM::I().AddCross(p[1], 1);
			//_DDM::I().AddLine(b1.position, b1.position + smallest_penetration * 100 * obb1.u_axes[smallest_axis[1]]);
		}

		Contact contact;
		contact.normal = n;
		contact.point = (p[0] + p[1]) * 0.5f;
		contact.penetration = smallest_penetration;
		contact.body[0] = &b0;
		contact.body[1] = &b1;
		contact.restitution = restitution;
		contacts.push_back(contact);
		b0.contacts.push_back(&contacts.back());
		b1.contacts.push_back(&contacts.back());
	}
	else assert(0);

	return 1;
}
#endif
#pragma endregion
*/
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
