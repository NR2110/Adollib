#include "contact.h"
using namespace Adollib;
using namespace Contacts;

//:::::::::::::::::::::::::::::::::::::::::::::::::::
		#pragma region Contact
//:::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::
// �Փ˓_�̒ǉ�
// float penetration : �ђʗ�
// vector3 normal : �Փ˖@�� (���[���h���W�n)
// vector3 contactpointA : �Փ˓_ (point[0]�̃��[�J�����W�n)
// vector3 contactpointB : �Փ˓_ (point[1]�̃��[�J�����W�n)
//::::::::
void Contact::addcontact(
	float penetration,
	const vector3& normal,
	const vector3& contact_pointA,
	const vector3& contact_pointB
) {
	//���łɂ���point�͑��݂���̂�
	int num = find_contact_point(contact_pointA, contact_pointB, normal);

	//���Ă��Ȃ�������ǉ�
	if (num == -1 && contact_num < Contact_max_per_pair) {
		num = contact_num;
		contactpoints[num].reset();
		contact_num++;
	}
	//���ł�Contact_max_per_pair�̐���������Ă���ꍇ
	//"�ő�ђʂ��Ă���_��ێ�"
	//"��Ԗʐς̑傫���Ȃ�_�����O"
	//�̏����ŏ�������
	else if(num == -1){
		num = find_exclusion_point(contact_pointA, penetration);
		contactpoints[num].reset();
	}

	assert(!(num == -1) && "Contact.cpp num == -1");

	//�Փ˓_���̍X�V
	contactpoints[num].distance = penetration;
	contactpoints[num].normal = normal;
	contactpoints[num].point[0] = contact_pointA;
	contactpoints[num].point[1] = contact_pointB;
}

int Contact::find_exclusion_point(const vector3& newPoint, float newDistance) {

	//"�ő�ђʂ��Ă���_��ێ�"
	int max_penetrate_num = -1;
	float max_penetrate = newDistance;
	for (int i = 0; i < 4; i++) {
		if (contactpoints[i].distance < max_penetrate) {
			max_penetrate_num = i;
			max_penetrate = contactpoints[i].distance;
		}
	}

	//"��Ԗʐς̑傫���Ȃ�_�����O"
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
	//�Ίp���𑍓�����Œ���
	//�O�ςŖʐςɂ���
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
	//��Ԍ��̃f�[�^�ŏ㏑��
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

		//��������̒l���߂�point������΂����Ԃ�
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

		// normal�����̋������ׂ�
		float dis_N = vector3_dot(normal, contactpointA - contactpointB);
		if (dis_N > Contact_threrhold_normal) {
			remove_contactpoint(i);
			i--;
			continue;
		}
		contactpoints[i].distance = dis_N;

		// normal�������������ė��_�̋������`�F�b�N
		// contactpointA��contactpointB��normal����œ����ꏊ�Ɏ����Ă���
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