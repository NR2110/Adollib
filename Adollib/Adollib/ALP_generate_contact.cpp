#include "ALP_generate_contact.h"

#include "closest_func.h"

using namespace Adollib;
using namespace Contacts;

//:::::::::::::::::::::::::::
#pragma region generate_contact
//:::::::::::::::::::::::::::

//�Փː���(Narrowphase)
//void physics_function::generate_contact(std::vector<Adollib::Plane*> planes, std::vector<Adollib::Box*> boxes, std::vector<Adollib::Sphere*> spheres, std::vector<Contacts::Contact_pair>& pairs) {

void physics_function::generate_contact(std::vector<Contacts::Contact_pair>& pairs) {

	//::: dynamic_cast ���p�n�� �댯! �댯!
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

//�Փː���
#pragma region SPHERE-SPHERE
bool physics_function::generate_contact_sphere_sphere(const Sphere& S0, const Sphere& S1, Contacts::Contact_pair& pair) {
	vector3 p0 = S0.world_position;
	vector3 p1 = S1.world_position;

	//p1 ���� p0�@�����ւ̖@��
	vector3 n = p0 - p1;
	float length = n.norm_sqr();
	n = n.unit_vect();

	if (length < S0.world_size.x + S1.world_size.x) {
		//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐�����
		pair.contacts.addcontact(
			S0.world_size.x + S1.world_size.x - length,
			n,
			S0.world_size.x * vector3_be_rotated_by_quaternion(-n, S0.world_orientation.conjugate()),
			S1.world_size.x * vector3_be_rotated_by_quaternion(n, S1.world_orientation.conjugate())
		);
		return true;
	}

	return false;
}
#pragma endregion

#pragma region SPHERE-PLANE
bool physics_function::generate_contact_sphere_plane(const Sphere& sphere, const Plane& plane, Contacts::Contact_pair& pair) {
	//���ʂƕ��ʂ̏Փ˔�����s��
	matrix rotate, inverse_rotate;
	rotate = plane.world_orientation.get_rotate_matrix();
	rotate._41 = plane.world_position.x; //transpse�̓���
	rotate._42 = plane.world_position.y;
	rotate._43 = plane.world_position.z;
	rotate._44 = 1;
	vector3 n(rotate._21, rotate._22, rotate._23);
	inverse_rotate = matrix_inverse(rotate);

	vector3 p;
	p = vector3_trans(sphere.world_position, inverse_rotate); //���ʂ�"���_��ʂ�"�@����(0,1,0)"�̏�Ԃ̎��̋��̒��S���W

	////���ʂ̗�����̏Փ˔���
	//if (half_space && p.y < 0)return 0;

	if (abs(p.y) < sphere.world_size.x) {
		n = p.y > 0 ? n : -n;

		if (pair.body[0]->shape == plane.shape) {
			//body[0]�@���@plane
			//body[1]�@���@sphere
			pair.contacts.addcontact(
				sphere.world_size.x - abs(p.y),
				n,
				vector3(p.x, 0, p.z),
				sphere.world_size.x * vector3_be_rotated_by_quaternion(-n, sphere.world_orientation.conjugate())
			);
		}
		else {

			//body[0]�@���@sphere
			//body[1]�@���@plane
			pair.contacts.addcontact(
				sphere.world_size.x - abs(p.y),
				n,
				sphere.world_size.x * vector3_be_rotated_by_quaternion(-n, sphere.world_orientation.conjugate()),
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
	//����box�̏Փ˔�����s��
	matrix rotate, inverse_rotate;
	//rotate = box.world_orientation.get_rotate_matrix();
	//rotate._41 = box.world_position.x; //transpse�̓���
	//rotate._42 = box.world_position.y;
	//rotate._43 = box.world_position.z;
	//rotate._44 = 1;
	rotate = matrix_world(vector3(1, 1, 1), box.world_orientation.get_rotate_matrix(), box.world_position);
	inverse_rotate = matrix_inverse(rotate);

	vector3 center;
	center = vector3_trans(sphere.world_position, inverse_rotate); //box��local���W�n�ł̋��̒��S���W

	vector3 box_halfsize = box.world_size;

	if (
		abs(center.x) - sphere.world_size.x > box_halfsize.x ||
		abs(center.y) - sphere.world_size.x > box_halfsize.y ||
		abs(center.z) - sphere.world_size.x > box_halfsize.z
		) return 0;

	//box��̍ŋߓ_
	vector3 closest_point;

	closest_point = center;
	if (center.x > +box_halfsize.x)closest_point.x = +box_halfsize.x;
	if (center.x < -box_halfsize.x)closest_point.x = -box_halfsize.x;

	if (center.y > +box_halfsize.y)closest_point.y = +box_halfsize.y;
	if (center.y < -box_halfsize.y)closest_point.y = -box_halfsize.y;

	if (center.z > +box_halfsize.z)closest_point.z = +box_halfsize.z;
	if (center.z < -box_halfsize.z)closest_point.z = -box_halfsize.z;

	float distance = (closest_point - center).norm_sqr(); //�ŋߓ_�Ƌ����S�̋���
	if (sphere.world_size.x - distance > FLT_EPSILON) { //float�덷������
		vector3 n = (sphere.world_position - vector3_trans(closest_point, rotate)).unit_vect(); //box����sphere�ւ̃x�N�g��

		if (pair.body[0]->shape == box.shape) {
			//body[0]�@���@box
			//body[1]�@���@sphere
			pair.contacts.addcontact(
				sphere.world_size.x - distance,
				-n,
				closest_point,
				sphere.world_size.x * vector3_be_rotated_by_quaternion(n, sphere.world_orientation.conjugate())
			);
		}
		else {
			//body[0]�@���@sphere
			//body[1]�@���@box
			pair.contacts.addcontact(
				sphere.world_size.x - distance,
				n,
				sphere.world_size.x * vector3_be_rotated_by_quaternion(n, sphere.world_orientation.conjugate()),
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
		// obb���W�n�ł̊e���_�̃��[�J�����W
		vector3(-box.world_size.x, -box.world_size.y, -box.world_size.z),
		vector3(-box.world_size.x, -box.world_size.y, +box.world_size.z),
		vector3(-box.world_size.x, +box.world_size.y, -box.world_size.z),
		vector3(-box.world_size.x, +box.world_size.y, +box.world_size.z),
		vector3(+box.world_size.x, -box.world_size.y, -box.world_size.z),
		vector3(+box.world_size.x, -box.world_size.y, +box.world_size.z),
		vector3(+box.world_size.x, +box.world_size.y, -box.world_size.z),
		vector3(+box.world_size.x, +box.world_size.y, +box.world_size.z)
	};

	//Box�ƕ��ʂ̏Փ˔�����s��
	matrix rotate, inverse_rotate;
	rotate = plane.world_orientation.get_rotate_matrix();
	rotate._41 = plane.world_position.x; //transpse�̓���
	rotate._42 = plane.world_position.y;
	rotate._43 = plane.world_position.z;
	rotate._44 = 1;
	vector3 n(rotate._21, rotate._22, rotate._23);
	inverse_rotate = matrix_inverse(rotate);

	vector3 p;
	p = vector3_trans(box.world_position, inverse_rotate); //���ʂ�"���_��ʂ�"�@����(0,1,0)"�̏�Ԃ̎���Box�̒��S���W(p.y�������ɂȂ�)
	//n = p.y > 0 ? n : -n; //�{�b�N�X�����ʉ��ɂ�������@���𔽑΂ɂ���

	float max_penetrate = 0.0f;
	vector3 pointbox, pointplane;
	vector3 vs;

	if (p.y < 2) {
		int dasfgdf = 0;
	}

	for (int i = 0; i < 8; i++) {
		// ���_�̃��[���h���W
		vs = vector3_be_rotated_by_quaternion(vertices[i], box.world_orientation);

		// -�ʖ@���̓��ς��Ƃ�(�ʕ����ւ̒���)
		float rA = vector3_dot(-n, vs);

		if (max_penetrate < rA - p.y) {
			max_penetrate = rA - p.y;
			pointbox = vertices[i];

			// ���_��plane��Ԃ�
			vs = vector3_trans(vs + box.world_position, inverse_rotate);
			pointplane = vs;
			pointplane.y = 0;
		}
	}

	if (max_penetrate != 0) {
		if (pair.body[0]->shape == plane.shape) {
			//body[0]�@���@plane
			//body[1]�@���@box
			pair.contacts.addcontact(
				max_penetrate,
				-n,
				pointplane,
				pointbox
			);
		}
		else {
			//body[0]�@���@box
			//body[1]�@���@plane
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

enum SAT_TYPE {
	POINTA_FACETB,
	POINTB_FACETA,
	EDGE_EDGE
};
struct OBB {
	vector3 center; //���S���W
	vector3 u_axes[3]; //���̌���
	quaternion orient;
	vector3 half_width; //�����Ƃ̕ӂ̒���
};
struct Ret_S {
	float penetrate;
	int smallest_axis[2];
	SAT_TYPE smallest_case;
	bool hit_point_to_face;
};
//���ɓ��e��������
float sum_of_projected_radii(const OBB& obb, const vector3& vec) {
	return
		fabsf(vector3_dot(vec, obb.half_width.x * obb.u_axes[0])) +
		fabsf(vector3_dot(vec, obb.half_width.y * obb.u_axes[1])) +
		fabsf(vector3_dot(vec, obb.half_width.z * obb.u_axes[2]));
}
//OBB��OBB�̕��������� �Փ˂��Ă����true��Ԃ� ���łɐF�X�����ɓ����
bool sat_obb_obb(
	const OBB& obb1, const OBB& obb2,
	float& smallest_penetration, //�ŏ��̊ђʗ�
	int smallest_axis[2], //�ǂ̎��ōŋ߂ɂȂ�����(edge�~edge�p��2���p��)
	SAT_TYPE& smallest_case //�ǂ̂悤�Ȍ`�ōŋ߂ɂȂ��Ă��邩
) {
	smallest_penetration = FLT_MAX;

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;


	float penetration = 0; //�ђʗ�
	float ra, rb; //obb1,obb2��L�ɓ��e���ꂽ����
	vector3 axis; //���e���鎲
	vector3 distBtoA = obb1.center - obb2.center; //2obb�̒��S���W�̋���

	//::: obb1�̎���obb2�𓊉e :::::::::::::::::::
	for (int i = 0; i < 3; i++)
	{
		axis = obb1.u_axes[i];
		ra = fabsf(obb1.half_width[i]);
		rb = fabsf(sum_of_projected_radii(obb2, axis));

		assert(ra > 0 && rb > 0);

		penetration = ra + rb - abs(vector3_dot(axis, distBtoA));
		if (penetration < 0) return 0;

		if (fabsf(vector3_dot(axis, distBtoA)) + rb < ra) {
			PA_FB.hit_point_to_face = false;
		}

		if (PB_FA.penetrate > penetration) {
			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = i;
			PB_FA.smallest_axis[1] = -1;
			PB_FA.smallest_case = POINTB_FACETA;
		}
	}

	//::: obb2�̎���obb1�𓊉e ::::::::::::::::::::
	for (int i = 0; i < 3; i++)
	{
		axis = obb2.u_axes[i];
		ra = fabsf(sum_of_projected_radii(obb1, axis));
		rb = fabsf(obb2.half_width[i]);

		penetration = ra + rb - abs(vector3_dot(axis, distBtoA));
		if (penetration < 0) return 0;

		if (fabsf(vector3_dot(axis, distBtoA)) + ra < rb) {
			PB_FA.hit_point_to_face = false;
		}

		if (PA_FB.penetrate > penetration) {

			PA_FB.penetrate = penetration;
			PA_FB.smallest_axis[0] = -1;
			PA_FB.smallest_axis[1] = i;
			PA_FB.smallest_case = POINTA_FACETB;
		}
	}

	if (PA_FB.hit_point_to_face && PA_FB.penetrate < smallest_penetration) {
		smallest_penetration = PA_FB.penetrate;
		smallest_axis[0] = PA_FB.smallest_axis[0];
		smallest_axis[1] = PA_FB.smallest_axis[1];
		smallest_case = PA_FB.smallest_case;

	}
	if (PB_FA.hit_point_to_face && PB_FA.penetrate < smallest_penetration) {
		smallest_penetration = PB_FA.penetrate;
		smallest_axis[0] = PB_FA.smallest_axis[0];
		smallest_axis[1] = PB_FA.smallest_axis[1];
		smallest_case = PB_FA.smallest_case;
	}


	//::: �O�ς̎��ɓ��e
	for (int OB1 = 0; OB1 < 3; OB1++) {
		for (int OB2 = 0; OB2 < 3; OB2++) {

			axis = vector3_cross(obb1.u_axes[OB1], obb2.u_axes[OB2]);
			if (axis.norm() <= FLT_EPSILON * FLT_EPSILON)continue;
			//if (L == vector3(0,0,0))continue; //�O�ς� 0 = ���s

			axis = axis.unit_vect();

			ra = fabsf(sum_of_projected_radii(obb1, axis));
			rb = fabsf(sum_of_projected_radii(obb2, axis));
			//if (vector3_dot(axis, distBtoA) + rb < ra)continue;

			penetration = ra + rb - fabsf(vector3_dot(axis, distBtoA));
			if (penetration < 0) return 0;
			if (smallest_penetration > penetration) {

				smallest_penetration = penetration;
				smallest_axis[0] = OB1;
				smallest_axis[1] = OB2;
				smallest_case = EDGE_EDGE;
			}
		}
	}

	// ����������������Ȃ��ꍇOBB�͌������Ă���͂�
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

	float smallest_penetration = FLT_MAX;	//�ŏ��߂荞�ݗ�
	int smallest_axis[2];	//�ŏ��߂荞�ݗʂ𓾂��������̍쐬�Ɏg�p�����eOBB�̃��[�J�����ԍ� �Ӂ~�ӗp��2��
	SAT_TYPE smallest_case;	//�Փ˂̎�� 
	if (!sat_obb_obb(obb0, obb1, smallest_penetration, smallest_axis, smallest_case)) return false;

	//obb1�̒��_��obb0�̖ʂƏՓ˂����ꍇ
	if (smallest_case == POINTB_FACETA)
	{
		vector3 d = obb1.center - obb0.center;	//obb0����obb1�ւ̑��Έʒu
		vector3 n = obb0.u_axes[smallest_axis[0]];	//obb0�̏Փ˖ʂ̖@���ƕ��s��obb0�̃��[�J�����x�N�g��
		if (vector3_dot(n, d) < 0)	//obb0��obb1�̈ʒu�֌W���Փ˖ʂ̖@���x�N�g�������肷��
		{
			n = n * -1.0f;
		}
		n = n.unit_vect();

		//box1�̃��[�J�����W�n�ł̍ŋߓ_(p1) obb1��8���_�̂����̂ǂꂩ
		vector3 p1 = obb1.half_width;	//obb1�̊e�ӂ̒����́Aobb1�̏d�S����ڐG�_(p)�ւ̑��Έʒu�̎肪����ɂȂ�
		//obb0��obb1�̈ʒu�֌W(d)���ڐG�_(p)�����߂�
		if (vector3_dot(obb1.u_axes[0], n) > 0) p1.x = -p1.x;
		if (vector3_dot(obb1.u_axes[1], n) > 0) p1.y = -p1.y;
		if (vector3_dot(obb1.u_axes[2], n) > 0) p1.z = -p1.z;

		//box0�̋t�s��̍쐬
		matrix rotate, inverse_rotate;
		rotate = b0.world_orientation.get_rotate_matrix();
		rotate._41 = b0.world_position.x; //transpse�̓���
		rotate._42 = b0.world_position.y;
		rotate._43 = b0.world_position.z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p1��obb0�̃��[�J�����W�n��
		vector3 P = vector3_be_rotated_by_quaternion(p1, b1.world_orientation) + b1.world_position;
		vector3 c = vector3_trans(P, inverse_rotate);

		//obb0�̍ŋߓ_�����߂�
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
	//�Aobb0�̒��_��obb1�̖ʂƏՓ˂����ꍇ
	//Contact�I�u�W�F�N�g�𐶐����A�S�Ẵ����o�ϐ��ɒl���Z�b�g���A�R���e�i(contacts)�ɒǉ�����
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

		//box0�̋t�s��̍쐬
		matrix rotate, inverse_rotate;
		rotate = b1.world_orientation.get_rotate_matrix();
		rotate._41 = b1.world_position.x; //transpse�̓���
		rotate._42 = b1.world_position.y;
		rotate._43 = b1.world_position.z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p1��obb0�̃��[�J�����W�n��
		vector3 P = vector3_be_rotated_by_quaternion(p0, b0.world_orientation) + b0.world_position;
		vector3 c = vector3_trans(P, inverse_rotate);

		//obb0�̍ŋߓ_�����߂�
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
	//�Bobb0�̕ӂ�obb1�̕ӂƏՓ˂����ꍇ
	//Contact�I�u�W�F�N�g�𐶐����A�S�Ẵ����o�ϐ��ɒl���Z�b�g���A�R���e�i(contacts)�ɒǉ�����
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

		vector3 P0a = vector3_be_rotated_by_quaternion(p[0], obb0.orient) + obb0.center;
		vector3 P1a = vector3_be_rotated_by_quaternion(p[1], obb1.orient) + obb1.center;


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

bool physics_function::genetate_contact_mesh_mesh(const Meshcoll& S1, const Meshcoll& S2, Contacts::Contact_pair& pair) {
	return true;
}

#pragma endregion

#pragma endregion
//:::::::::::::::::::::::::::