
#include "contact.h"

#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

//:::::::::::::::::::::::::::::::::::::::::::::::::::
		#pragma region Contact
//:::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::
// 衝突点の追加
// float penetration : 貫通量
// vector3 normal : pointBからpointAへの衝突法線 (ワールド座標系)
// vector3 contactpointA : 衝突点 (pointAのローカル座標系)
// vector3 contactpointB : 衝突点 (pointBのローカル座標系)
//::::::::
void Contact::addcontact(
	float penetration,
	const Vector3& normal,
	const Vector3& contact_pointA,
	const Vector3& contact_pointB
) {
	//すでにそのpointは存在するのか
	int num = find_contact_point(contact_pointA, contact_pointB, normal);

	//していなかったら追加
	if (num == -1 && contact_num < contact_max_per_pair) {
		num = contact_num;
		find_contact_point(contact_pointA, contact_pointB, normal);
		contactpoints[num].reset();
		contact_num++;
	}
	//すでにContact_max_per_pairの数だけ作られている場合
	//"最大貫通している点を保持"
	//"一番面積の大きくなる点を除外"
	//の条件で処理する
	else if(num == -1){
		num = find_exclusion_point(contact_pointA, penetration);
		contactpoints[num].reset();
	}

	assert(!(num == -1) && "Contact.cpp num == -1");

	//衝突点情報の更新
	contactpoints[num].distance = penetration;
	contactpoints[num].normal = normal;
	contactpoints[num].point[0] = contact_pointA;
	contactpoints[num].point[1] = contact_pointB;
}

int Contact::find_exclusion_point(const Vector3& newPoint, float newDistance) {

	//"最大貫通している点を保持"
	int max_penetrate_num = -1;
	float max_penetrate = newDistance;
	for (int i = 0; i < 4; i++) {
		if (contactpoints[i].distance < max_penetrate) {
			max_penetrate_num = i;
			max_penetrate = contactpoints[i].distance;
		}
	}

	//"一番面積の大きくなる点を除外"
	float area[4] = { 0.0 };

	Vector3 point[4] = {
		contactpoints[0].point[0],
		contactpoints[1].point[0],
		contactpoints[2].point[0],
		contactpoints[3].point[0]
	};

	if (max_penetrate_num != 0) {
		area[0] = area_4points(newPoint, point[1], point[2], point[3]);
	}

	if (max_penetrate_num != 1) {
		area[1] = area_4points(newPoint, point[0], point[2], point[3]);
	}

	if (max_penetrate_num != 2) {
		area[2] = area_4points(newPoint, point[0], point[1], point[3]);
	}

	if (max_penetrate_num != 3) {
		area[3] = area_4points(newPoint, point[0], point[1], point[2]);
	}

	int ret = -1;
	float max_area = -FLT_MAX;
	for (int i = 0; i < 4; i++) {
		if (area[i] > max_area) {
			ret = i;
			max_area = area[i];
		}
	}
	return ret;

}

float Contact::area_4points(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3) {
	//対角線を総当たりで調べ
	//外積で面積にする
	float pat[3] = { 0.0 };
	pat[0] = (vector3_cross(p0 - p1, p2 - p3)).norm();
	pat[1] = (vector3_cross(p0 - p2, p3 - p1)).norm();
	pat[2] = (vector3_cross(p0 - p3, p1 - p2)).norm();

	float ret = 0;
	for (int i = 0; i < 3; i++) {
		if (ret > pat[i]) {
			ret = pat[i];
		}
	}

	return ret;
}

void Contact::remove_contactpoint(int i) {
	//一番後ろのデータで上書き
	contactpoints[i] = contactpoints[contact_num - 1];
	contact_num--;
}

int Contact::find_contact_point(
	const Vector3& contact_pointA,
	const Vector3& contact_pointB,
	const Vector3& normal
) {
	int ret = -1;
	float min = Phyisics_manager::physicsParams.contact_allowable_error;
	for (int i = 0; i < contact_num; i++) {
		float lenA = (contactpoints[i].point[0] - contact_pointA).norm();
		float lenB = (contactpoints[i].point[1] - contact_pointB).norm();

		//もし既定の値より近いpointがあればそれを返す
		if (lenA < min && lenB < min && vector3_dot(contactpoints[i].normal, normal) > 0.99) {
			ret = i;
			min = lenA < lenB ? lenA : lenB;
		}
	}
	return ret;
}

bool Contact::chack_remove_contact_point(
	const Collider_shape* shape0,
	const Collider_shape* shape1
) {
	bool ret = false;
	for (int i = 0; i < contact_num;) {
		Vector3& normal = contactpoints[i].normal;

		Vector3 contactpointA = shape0->world_position() + vector3_quatrotate(contactpoints[i].point[0], shape0->world_orientation());
		Vector3 contactpointB = shape1->world_position() + vector3_quatrotate(contactpoints[i].point[1], shape1->world_orientation());

		// normal方向の距離を比べる
		const float dis_N = vector3_dot(normal, contactpointB - contactpointA);
		if (dis_N < -Phyisics_manager::physicsParams.contact_threrhold_normal) {
			remove_contactpoint(i);
			ret = true;
			continue;
		}
		contactpoints[i].distance = -dis_N;

		// normal方向を除去して両点の距離をチェック
		// contactpointAをcontactpointBとnormal軸上で同じ場所に持ってくる
		contactpointA = contactpointA - contactpoints[i].distance * normal;
		float dis_T = (contactpointA - contactpointB).norm();
		if (dis_T > Phyisics_manager::physicsParams.contact_threrhold_tangent) {
			remove_contactpoint(i);
			ret = true;
			continue;
		}

		i++;
	}

	return ret;
}
#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::