
#include "ALP_contact.h"

#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

//:::::::::::::::::::::::::::::::::::::::::::::::::::
		#pragma region Contact
//:::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::
// �Փ˓_�̒ǉ�
// float penetration : �ђʗ�
// vector3 normal : pointB����pointA�ւ̏Փ˖@�� (���[���h���W�n)
// vector3 contactpointA : �Փ˓_ (pointA�̃��[�J�����W�n)
// vector3 contactpointB : �Փ˓_ (pointB�̃��[�J�����W�n)
//::::::::
void Contact::addcontact(
	float penetration,
	const Vector3& normal,
	const Vector3& contact_pointA,
	const Vector3& contact_pointB
) {
	//���łɂ���point�͑��݂���̂�
	int num = find_contact_point(contact_pointA, contact_pointB, normal);

	//���Ă��Ȃ�������ǉ�
	if (num == -1 && contact_num < contact_max_per_pair) {
		num = contact_num;
		find_contact_point(contact_pointA, contact_pointB, normal);
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

int Contact::find_exclusion_point(const Vector3& newPoint, float newDistance) {

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
	const Vector3& contact_pointA,
	const Vector3& contact_pointB,
	const Vector3& normal
) {
	int ret = -1;
	float min = Physics_manager::physicsParams.contact_allowable_error;
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

bool Contact::chack_remove_contact_point(
	const Collider_shape* shape0,
	const Collider_shape* shape1
) {
	bool ret = false;
	for (int i = 0; i < contact_num;) {
		Vector3& normal = contactpoints[i].normal;

		Vector3 contactpointA = shape0->world_position() + vector3_quatrotate(contactpoints[i].point[0], shape0->world_orientation());
		Vector3 contactpointB = shape1->world_position() + vector3_quatrotate(contactpoints[i].point[1], shape1->world_orientation());

		// normal�����̋������ׂ�
		const float dis_N = vector3_dot(normal, contactpointB - contactpointA);
		if ( dis_N < -Physics_manager::physicsParams.contact_threrhold_normal) {
			remove_contactpoint(i);
			ret = true;
			continue;
		}
		contactpoints[i].distance = -dis_N;

		// normal�������������ė��_�̋������`�F�b�N
		// contactpointA��contactpointB��normal����œ����ꏊ�Ɏ����Ă���
		contactpointA = contactpointA - contactpoints[i].distance * normal;
		float dis_T = (contactpointA - contactpointB).norm();
		if (dis_T > Physics_manager::physicsParams.contact_threrhold_tangent) {
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