#include "physics_function.h"
#include <string>

using namespace Adollib;
using namespace Contacts;

//:::::::::::::::::::::::::::::::::::::::::::::::::::
		#pragma region physics_function
//:::::::::::::::::::::::::::::::::::::::::::::::::::

using namespace physics_function;
//�O�͂ɂ�鑬�x�Ȃǂ̍X�V
void physics_function::applyexternalforce(std::vector<Collider*>& coll) {
	for (int i = 0; i < coll.size(); i++) {
		coll[i]->apply_external_force(timeStep);
	}
}

//DOP6�ɂ���G�c�ȓ����蔻��
bool Check_insert_DOP6(Collider* collA, Collider* collB) {
	//����Plane��DOP�����Ȃ�����narrow�ɓ�����
	if (collA->shape == Collider_shape::shape_plane || collB->shape == Collider_shape::shape_plane) return true;

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

	//DOP�̍X�V
	for (int i = 0; i < coll.size(); i++) {
		coll[i]->update_dop6();
	}

	//DOP_6�ɂ���G�c�ȓ����蔻��
	std::vector<Contacts::Contact_pair> new_pairs;
	for (int i = 0; i < coll.size(); i++) {
		for (int o = i + 1; o < coll.size(); o++) {

			// �^�O�ɂ��Փ˂̐���
			bool hit = true;
			for (int q = 0; q < coll[q]->No_hit_tag.size(); q++) {
				if (coll[i]->No_hit_tag[q] == std::string("all")) hit = false;
				if (coll[i]->No_hit_tag[q] == coll[o]->tag) hit = false;
				if (hit == false)break;
			}
			if (hit == false)continue;
			for (int q = 0; q < coll[q]->No_hit_tag.size(); q++) {
				if (coll[o]->No_hit_tag[q] == std::string("all")) hit = false;
				if (coll[o]->No_hit_tag[q] == coll[i]->tag) hit = false;
				if (hit == false)break;
			}
			if (hit == false)continue;

			//DOP�ɂ���G�c�ȓ����蔻��
			if (Check_insert_DOP6(coll[i], coll[o]) == false)continue;
			Contact_pair new_pair;
			//new_pair.body[0]�ɃA�h���X�̑傫���ق������܂�
			if (coll[i] > coll[o]) {
				new_pair.body[0] = coll[i];
				new_pair.body[1] = coll[o];
			}
			else {
				new_pair.body[0] = coll[i];
				new_pair.body[1] = coll[o];
			}
			new_pair.type = Pairtype::new_pair;

			new_pairs.emplace_back(new_pair);
		}
	}

	//��������pair���O��pair���瑶�݂��Ă��邩�̊m�F
	for (int old_num = 0; old_num < pairs.size(); old_num++) {
		for (int new_num = 0; new_num < new_pairs.size(); new_num++) {
			if (new_pairs[new_num].type == Pairtype::keep_pair) continue;

			if (
				new_pairs[new_num].body[0] == pairs[old_num].body[0] &&
				new_pairs[new_num].body[1] == pairs[old_num].body[1]
				) {
				//�O���瑶�݂��Ă�����f�[�^�������p��
				new_pairs[new_num] = pairs[old_num];
				new_pairs[new_num].type = Pairtype::keep_pair;
			}
			else {
				new_pairs[new_num].type = Pairtype::new_pair;
			}
		}
	}
	//���ݎg�p���Ă��Ȃ��Փ˓_���폜
	for (int i = 0; i < new_pairs.size(); i++) {
		new_pairs[i].contacts.chack_remove_contact_point(
			new_pairs[i].body[0]->world_position,
			new_pairs[i].body[0]->world_orientation,
			new_pairs[i].body[1]->world_position,
			new_pairs[i].body[1]->world_orientation
		);
	}

	pairs.clear();
	pairs = new_pairs;

}

//:::::::::::::::::::::::::::
	#pragma region generate_contact
//:::::::::::::::::::::::::::

//�Փː���(Narrowphase)
void physics_function::generate_contact(std::vector<Contacts::Contact_pair>& pairs) {

	//::: dynamic_cast ���p�n�� �댯! �댯!
	for (int i = 0; i < pairs.size(); i++) {

		if (pairs[i].body[0]->shape == Collider_shape::shape_sphere) {
			Sphere* shape0 = dynamic_cast<Sphere*>(pairs[i].body[0]);

			if (pairs[i].body[1]->shape == Collider_shape::shape_sphere) {
				Sphere* shape1 = dynamic_cast<Sphere*>(pairs[i].body[1]);
				assert(shape0 && shape1); 
				generate_contact_sphere_sphere(*shape0, *shape1, pairs[i]);
			}
			if (pairs[i].body[1]->shape == Collider_shape::shape_box) {
				Box* shape1 = dynamic_cast<Box*>(pairs[i].body[1]);
				assert(shape0 && shape1);
				generate_contact_sphere_box(*shape0, *shape1, pairs[i]);
			}
			if (pairs[i].body[1]->shape == Collider_shape::shape_plane) {
				Plane* shape1 = dynamic_cast<Plane*>(pairs[i].body[1]);
				assert(shape0 && shape1);
				generate_contact_sphere_plane(*shape0, *shape1, pairs[i]);
			}
		}
		if (pairs[i].body[0]->shape == Collider_shape::shape_box) {
			Box* shape0 = dynamic_cast<Box*>(pairs[i].body[0]);

			if (pairs[i].body[1]->shape == Collider_shape::shape_sphere) {
				Sphere* shape1 = dynamic_cast<Sphere*>(pairs[i].body[1]);
				assert(shape0 && shape1);
				generate_contact_sphere_box(*shape1, *shape0, pairs[i]);
			}
			if (pairs[i].body[1]->shape == Collider_shape::shape_box) {
				Box* shape1 = dynamic_cast<Box*>(pairs[i].body[1]);
				assert(shape0 && shape1);
				generate_contact_box_box(*shape0, *shape1, pairs[i]);
			}
			if (pairs[i].body[1]->shape == Collider_shape::shape_plane) {
				Plane* shape1 = dynamic_cast<Plane*>(pairs[i].body[1]);
				assert(shape0 && shape1);
				generate_contact_box_plane(*shape0, *shape1, pairs[i]);
			}
		}
		if (pairs[i].body[0]->shape == Collider_shape::shape_plane) {
			Plane* shape0 = dynamic_cast<Plane*>(pairs[i].body[0]);

			if (pairs[i].body[1]->shape == Collider_shape::shape_sphere) {
				Sphere* shape1 = dynamic_cast<Sphere*>(pairs[i].body[1]);
				assert(shape0 && shape1);
				generate_contact_sphere_plane(*shape1, *shape0, pairs[i]);
			}

			if (pairs[i].body[1]->shape == Collider_shape::shape_box) {
				Box* shape1 = dynamic_cast<Box*>(pairs[i].body[1]);
				assert(shape0 && shape1);
				generate_contact_box_plane(*shape1, *shape0, pairs[i]);
			}
			//if (pairs[i]->body[1]->shape == Collider_shape::shape_plane) {}
		}


	}
}

//�Փː���
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
			S0.world_size.x * n,
			S1.world_size.x * -n
		);
		return true;
	}

	return false;
}
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
				sphere.world_size.x * -n
			);
		}
		else {
			//body[0]�@���@sphere
			//body[1]�@���@plane
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
	if (distance < sphere.world_size.x && distance > FLT_EPSILON) { //float�덷������
		vector3 n = (sphere.world_position - closest_point).unit_vect();
		if (pair.body[0]->shape == box.shape) {
			//body[0]�@���@box
			//body[1]�@���@sphere
			pair.contacts.addcontact(
				sphere.world_size.x - distance,
				n,
				closest_point,
				sphere.world_size.x * -n
			);
		}
		else {
			//body[0]�@���@sphere
			//body[1]�@���@box
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

enum SAT_TYPE {
	POINTA_FACETB,
	POINTB_FACETA,
	EDGE_EDGE
};
struct OBB {
	vector3 center; //���S���W
	vector3 u_axes[3]; //���̌���
	vector3 half_width; //�����Ƃ̕ӂ̒���
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
	float penetration = 0; //�ђʗ�

	std::vector<vector3> contact_point;

	float ra, rb; //obb1,obb2��L�ɓ��e���ꂽ����
	vector3 L; //���e���鎲
	vector3 T = obb2.center - obb1.center; //2obb�̒��S���W�̋���

	// obb1�̎���obb2�𓊉e
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

	// obb2�̎���obb1�𓊉e
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

	//::: �O�ς̎��ɓ��e
	for (int OB1 = 0; OB1 < 3; OB1++) {
		for (int OB2 = 0; OB2 < 3; OB2++) {

			L = vector3_cross(obb1.u_axes[OB1], obb2.u_axes[OB2]);
			if (L.norm() <= FLT_EPSILON * FLT_EPSILON)continue;
			//if (L == vector3(0,0,0))continue; //�O�ς� 0 = ���s

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

	// ����������������Ȃ��ꍇOBB�͌������Ă���͂�
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

		//box1�̍ŋߓ_(p1)��obb1��8���_�̂����̂ǂꂩ
		vector3 p1 = obb1.half_width;	//obb1�̊e�ӂ̒����́Aobb1�̏d�S����ڐG�_(p)�ւ̑��Έʒu�̎肪����ɂȂ�
		//obb0��obb1�̈ʒu�֌W(d)���ڐG�_(p)�����߂�
		if (vector3_dot(obb1.u_axes[0], d) > 0) p1.x = -p1.x;
		if (vector3_dot(obb1.u_axes[1], d) > 0) p1.y = -p1.y;
		if (vector3_dot(obb1.u_axes[2], d) > 0) p1.z = -p1.z;
		p1 = vector3_be_rotated_by_quaternion(p1, b1.world_orientation);
		p1 += b1.world_position;

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
		if (vector3_dot(obb0.u_axes[0], d) > 0) p0.x = -p0.x;
		if (vector3_dot(obb0.u_axes[1], d) > 0) p0.y = -p0.y;
		if (vector3_dot(obb0.u_axes[2], d) > 0) p0.z = -p0.z;
		p0 = vector3_be_rotated_by_quaternion(p0, b0.world_orientation);
		p0 += b0.world_position;

		//box0�̋t�s��̍쐬
		matrix rotate, inverse_rotate;
		rotate = b1.world_orientation.get_rotate_matrix();
		rotate._41 = b1.world_position.x; //transpse�̓���
		rotate._42 = b1.world_position.y;
		rotate._43 = b1.world_position.z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);
		//p1��obb0�̃��[�J�����W�n��
		vector3 P = vector3_be_rotated_by_quaternion(p0, b1.world_orientation) + b1.world_position;
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

		pair.contacts.addcontact(
			smallest_penetration,
			n,
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

//�Փˉ���
// �\���o�[�{�f�B(�����p�̂Ȃ�)
// �{�[���W���C���g
struct Balljoint {
	float bias; // �S���̋����̒����l
	int rigidBodyA; // ����A�ւ̃C���f�b�N�X
	int rigidBodyB; // ����B�ւ̃C���f�b�N�X
	vector3 anchorA; // ����A�̃��[�J�����W�n�ɂ�����ڑ��_
	vector3 anchorB; // ����B�̃��[�J�����W�n�ɂ�����ڑ��_
	Constraint constraint; // �S��

	// ������
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
	tangent1 = (vector3_cross(normal, vec)).unit_vect();
	tangent2 = (vector3_cross(tangent1, normal)).unit_vect();
}

void physics_function::resolve_contact(std::vector<Collider*> colliders, std::vector<Contacts::Contact_pair>& pairs) {

	//::: �����p�I�u�W�F�N�g�̐��� :::::::::::
	std::vector<Solverbody> SBs;
	for (int i = 0; i < colliders.size(); i++) {
		Solverbody SB;
		SB.orientation = colliders[i]->world_orientation;
		SB.delta_LinearVelocity = vector3(0.0f);
		SB.delta_AngularVelocity = vector3(0.0f);
		SB.inv_inertia = colliders[i]->inverse_inertial_tensor();
		SB.inv_mass = colliders[i]->inverse_mass();

		SBs.emplace_back(SB);
	}
	for (int i = 0; i < colliders.size(); i++) {
		colliders[i]->solve = &SBs[i];
	}

	//std::vector<Balljoint> balljoints; //����͂Ȃ�
	Collider* coll[2];
	Solverbody* solverbody[2];

	for (int P_num = 0; P_num < pairs.size(); P_num++) {
		Contact_pair& pair = pairs[P_num];

		coll[0] = pairs[P_num].body[0];
		coll[1] = pairs[P_num].body[1];
		solverbody[0] = coll[0]->solve;
		solverbody[1] = coll[1]->solve;

		// ���C�̊l��
		pair.contacts.friction = sqrtf(coll[0]->friction * coll[1]->friction);

		//::: �Փ˓_���̍X�V ::::::::
		for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
			Contactpoint& cp = pair.contacts.contactpoints[C_num];

			vector3 rA = vector3_be_rotated_by_quaternion(cp.point[0], solverbody[0]->orientation);
			vector3 rB = vector3_be_rotated_by_quaternion(cp.point[1], solverbody[1]->orientation);

			// �����W���̊l��
			// �p���̏Փ˂̏ꍇ�����W����0�ɂ���
			float restitution = pair.type == Pairtype::new_pair ? 0.5f * (coll[0]->restitution + coll[1]->restitution) : 0.0f;
			restitution = 0.5f * (coll[0]->restitution + coll[1]->restitution);

			//�Փˎ��̂��ꂼ��̑��x
			vector3 pdota;
			pdota = vector3_cross(coll[0]->angular_velocity, rA);
			pdota += coll[0]->linear_velocity;

			vector3 pdotb;
			pdotb = vector3_cross(coll[1]->angular_velocity, rB);
			pdotb += coll[1]->linear_velocity;

			//�Փˎ��̏Փ˕��ʖ@�������̑��Α��x(���ǏՓ˂Ɏg���͖̂@�������ւ̑���)
			vector3 vrel = pdota - pdotb;

			vector3
				tangent1, //normal�ɑ΂���z����
				tangent2; //normal�ɑ΂���x����
			CalcTangentVector(cp.normal, tangent1, tangent2);

			//Baraff[1997]�̎�(8-18)�̕���(denominator)�����߂�
			float term1 = coll[0]->inverse_mass();
			float term2 = coll[1]->inverse_mass();
			vector3 tA, tB;

			float term3, term4, denominator;
			// Normal
			{
				//Baraff[1997]�̎�(8-18)�̕���(denominator)�����߂�
				vector3 axis = cp.normal;
				tA = vector3_cross(rA, axis);
				tB = vector3_cross(rB, axis);
				tA = vector3_trans(tA, coll[0]->inverse_inertial_tensor());
				tB = vector3_trans(tB, coll[1]->inverse_inertial_tensor());
				tA = vector3_cross(tA, rA);
				tB = vector3_cross(tB, rB);
				term3 = vector3_dot(axis, tA);
				term4 = vector3_dot(axis, tB);
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[0].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���
				cp.constraint[0].rhs = -(1.0f + restitution) * vector3_dot(axis, vrel); //Baraff1997(8-18)�̕��q

				//if(0.0f > cp.distance + slop)
				//cp.constraint[0].rhs -= (bias * (cp.distance + slop)) / timeStep; //�߂荞�݂𒼂��l

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
				term3 = vector3_dot(axis, tA);
				term4 = vector3_dot(axis, tB);
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[1].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���
				cp.constraint[1].rhs = -(1.0f + restitution) * vector3_dot(axis, vrel); //Baraff1997(8-18)�̕��q
				cp.constraint[1].rhs *= cp.constraint[1].jacDiagInv;
				cp.constraint[1].lowerlimit = 0.0f;
				cp.constraint[1].upperlimit = FLT_MAX;
				cp.constraint[1].axis = axis;
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
				term3 = vector3_dot(axis, tA);
				term4 = vector3_dot(axis, tB);
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[2].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���?
				cp.constraint[2].rhs = -(1.0f + restitution) * vector3_dot(axis, vrel); //Baraff1997(8-18)�̕��q
				cp.constraint[2].rhs *= cp.constraint[2].jacDiagInv;
				cp.constraint[2].lowerlimit = 0.0f;
				cp.constraint[2].upperlimit = FLT_MAX;
				cp.constraint[2].axis = axis;
			}

		}
	}

	//::: �ω��ʂ����߂� :::::::::::::::
	for (int P_num = 0; P_num < pairs.size(); P_num++) {
		Contact_pair& pair = pairs[P_num];

		coll[0] = pair.body[0];
		coll[1] = pair.body[1];
		solverbody[0] = coll[0]->solve;
		solverbody[1] = coll[1]->solve;

		for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
			//�Փ˓_�̏��@
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
				//�Փ˓_�̏��@
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
					solverbody[0]->delta_AngularVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngularVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);
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
					solverbody[0]->delta_AngularVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngularVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);
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
					solverbody[0]->delta_AngularVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngularVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);
				}

			}
		}
	}

	if (pairs[0].contacts.contact_num > 0) {
		int dafsgd = 0;
	}

	// ���x�̍X�V
	for (int i = 0; i < colliders.size(); i++) {
		colliders[i]->linear_velocity += colliders[i]->solve->delta_LinearVelocity;
		colliders[i]->angular_velocity += colliders[i]->solve->delta_AngularVelocity;
	}


}
#pragma endregion
//:::::::::::::::::::::::::::

void physics_function::integrate(std::vector<Collider*>& coll) {
	for (int i = 0; i < coll.size(); i++) {
		coll[i]->integrate(timeStep);
	}

}

#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::