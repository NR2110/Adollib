#include "contact.h"
using namespace Adollib;
using namespace Contacts;

//:::::::::::::::::::::::::::::::::::::::::::::::::::
		#pragma region Contact
//:::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::
// 衝突点の追加
// float penetration : 貫通量
// vector3 normal : 衝突法線 (ワールド座標系)
// vector3 contactpointA : 衝突点 (point[0]のローカル座標系)
// vector3 contactpointB : 衝突点 (point[1]のローカル座標系)
//::::::::
void Contact::addcontact(
	float penetration,
	const vector3& normal,
	const vector3& contact_pointA,
	const vector3& contact_pointB
) {
	//すでにそのpointは存在するのか
	int num = find_contact_point(contact_pointA, contact_pointB, normal);

	//していなかったら追加
	if (num == -1 && contact_num < Contact_max_per_pair) {
		num = contact_num;
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

int Contact::find_exclusion_point(const vector3& newPoint, float newDistance) {

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

	vector3 point[4] = {
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

float Contact::area_4points(const vector3& p0, const vector3& p1, const vector3& p2, const vector3& p3) {
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
	const vector3& contact_pointA,
	const vector3& contact_pointB,
	const vector3& normal
) {
	int ret = -1;
	float min = Allowable_error;
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

void Contact::chack_remove_contact_point(
	const vector3& pointA,
	const quaternion& rotA,
	const vector3& pointB,
	const quaternion& rotB
) {
	for (int i = 0; i < contact_num; i++) {
		vector3& normal = contactpoints[i].normal;
		vector3 contactpointA = pointA + vector3_be_rotated_by_quaternion(contactpoints[i].point[0], rotA);
		vector3 contactpointB = pointB + vector3_be_rotated_by_quaternion(contactpoints[i].point[1], rotB);

		// normal方向の距離を比べる
		float dis_N = vector3_dot(normal, contactpointA - contactpointB);
		if (dis_N > Contact_threrhold_normal) {
			remove_contactpoint(i);
			i--;
			continue;
		}
		contactpoints[i].distance = dis_N;

		// normal方向を除去して両点の距離をチェック
		// contactpointAをcontactpointBとnormal軸上で同じ場所に持ってくる
		contactpointA = contactpointA - contactpoints[i].distance * normal;
		float dis_T = (contactpointA - contactpointB).norm();
		if (dis_T > Contact_threrhold_tangent) {
			remove_contactpoint(i);
			i--;
			continue;
		}
	}

}
#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::