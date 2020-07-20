
#include "physics_function.h"

#include "closest_func.h"
#include <string>

using namespace Adollib;
using namespace Contacts;
using namespace Compute_S;

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

//:::::::::::::::::::::::::::
	#pragma region Boardphase
//:::::::::::::::::::::::::::
//DOP6�ɂ���G�c�ȓ����蔻��
bool Check_insert_DOP7(Collider* collA, Collider* collB) {
	//����Plane��DOP�����Ȃ�����narrow�ɓ�����?
	if (collA->shape == Collider_shape::shape_plane || collB->shape == Collider_shape::shape_plane) return true;

	vector3 dis = collA->dop7.pos - collB->dop7.pos;

	for (int i = 0; i < DOP_size; i++) {
		if (
			+vector3_dot(DOP_7_axis[i], collA->dop7.pos - collB->dop7.pos) < collB->dop7.min[i] - collA->dop7.max[i] || 
			-vector3_dot(DOP_7_axis[i], collA->dop7.pos - collB->dop7.pos) < collA->dop7.min[i] - collB->dop7.max[i]
			)
			return false;
	}

	return true;
}
bool Check_insert_Plane(Collider* plane, Collider* coll) {
	vector3 axis[6]{
	{1,0,0},
	{0,1,0},
	{0,0,1},
	{1,1,0},
	{0,1,1},
	{1,0,1}
	};
	vector3 V;
	float plane_dis = 0, coll_dis = FLT_MAX;

	V = vector3_be_rotated_by_quaternion(vector3(0, 1, 0), plane->world_orientation);
	plane_dis = vector3_dot(V, plane->world_position);

	for (int i = 0; i < DOP_size; i++) {
		float coll_len = vector3_dot(V, coll->world_position + axis[i] * coll->dop7.max[i]);
		if (plane_dis > coll_len)return true;
	}

	return false;
}

void physics_function::Boardphase(const std::vector<Collider*>& coll, std::vector<Contacts::Contact_pair>& pairs) {


	//DOP�̍X�V
	for (int i = 0; i < coll.size(); i++) {
		coll[i]->update_dop7();
	}

	//DOP_6�ɂ���G�c�ȓ����蔻��
	std::vector<Contacts::Contact_pair> new_pairs;
	Contact_pair new_pair;
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
			if (coll[i]->shape != Collider_shape::shape_plane && coll[o]->shape != Collider_shape::shape_plane) {
				if (Check_insert_DOP7(coll[i], coll[o]) == false)continue;
			}
			else if (coll[i]->shape == Collider_shape::shape_plane && coll[o]->shape != Collider_shape::shape_plane) {
				if (Check_insert_Plane(coll[i], coll[o]) == false)continue;
			}
			else if (coll[i]->shape != Collider_shape::shape_plane && coll[o]->shape == Collider_shape::shape_plane) {
				if (Check_insert_Plane(coll[o], coll[i]) == false)continue;
			}
			else continue;

			//new_pair.body[0]�ɃA�h���X�̑傫���ق������܂�
			if (coll[i] > coll[o]) {
				new_pair.body[0] = coll[i];
				new_pair.body[1] = coll[o];
			}
			else {
				new_pair.body[0] = coll[o];
				new_pair.body[1] = coll[i];
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
#pragma endregion
//:::::::::::::::::::::::::::

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
			//if (pairs[i]->body[1]->shape == Collider_shape::shape_plane) {}
		}


	}

	int o = 0;
	for (int i = 0; i < 1000000; i++) {
		o++;
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
			S0.world_size.x * vector3_be_rotated_by_quaternion(-n,S0.world_orientation.conjugate()),
			S1.world_size.x * vector3_be_rotated_by_quaternion(n,S1.world_orientation.conjugate())
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
				sphere.world_size.x * vector3_be_rotated_by_quaternion(-n, sphere.world_orientation.conjugate())
			);
		}
		else {

			//body[0]�@���@sphere
			//body[1]�@���@plane
			pair.contacts.addcontact(
				sphere.world_size.x - abs(p.y),
				n,
				sphere.world_size.x*vector3_be_rotated_by_quaternion(-n, sphere.world_orientation.conjugate()),
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
				-n,
				closest_point,
				sphere.world_size.x * vector3_be_rotated_by_quaternion(-n,sphere.world_orientation.conjugate())
			);
		}
		else {
			//body[0]�@���@sphere
			//body[1]�@���@box
			pair.contacts.addcontact(
				sphere.world_size.x - distance,
				n,
				sphere.world_size.x * vector3_be_rotated_by_quaternion(-n, sphere.world_orientation.conjugate()),
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

bool physics_function::generate_contact_box_box(const Box& b0, const  Box& b1, Contacts::Contact_pair& pair) {
	matrix m;
	m = b0.world_orientation.get_rotate_matrix();
	OBB obb0;
	obb0.center = b0.world_position;
	obb0.u_axes[0].x = m._11; obb0.u_axes[0].y = m._12; obb0.u_axes[0].z = m._13;
	obb0.u_axes[1].x = m._21; obb0.u_axes[1].y = m._22; obb0.u_axes[1].z = m._23;
	obb0.u_axes[2].x = m._31; obb0.u_axes[2].y = m._32; obb0.u_axes[2].z = m._33;
	obb0.half_width = b0.world_size;
	obb0.orient = b0.world_orientation;

	m = b1.world_orientation.get_rotate_matrix();
	OBB obb1;
	obb1.center = b1.world_position;
	obb1.u_axes[0].x = m._11; obb1.u_axes[0].y = m._12; obb1.u_axes[0].z = m._13;
	obb1.u_axes[1].x = m._21; obb1.u_axes[1].y = m._22; obb1.u_axes[1].z = m._23;
	obb1.u_axes[2].x = m._31; obb1.u_axes[2].y = m._32; obb1.u_axes[2].z = m._33;
	obb1.half_width = b1.world_size;
	obb1.orient = b1.world_orientation;

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

#if 0
		////�e���_
		//vector3 vertexs_0[8]{
		//	{vector3(+obb0.half_width.x, +obb0.half_width.y, +obb0.half_width.z) },
		//	{vector3(+obb0.half_width.x, +obb0.half_width.y, -obb0.half_width.z) },
		//	{vector3(+obb0.half_width.x, -obb0.half_width.y, +obb0.half_width.z) },
		//	{vector3(+obb0.half_width.x, -obb0.half_width.y, -obb0.half_width.z) },
		//	{vector3(-obb0.half_width.x, +obb0.half_width.y, +obb0.half_width.z) },
		//	{vector3(-obb0.half_width.x, +obb0.half_width.y, -obb0.half_width.z) },
		//	{vector3(-obb0.half_width.x, -obb0.half_width.y, +obb0.half_width.z) },
		//	{vector3(-obb0.half_width.x, -obb0.half_width.y, -obb0.half_width.z) },
		//};
		//vector3 vertexs_1[8]{
		//	{vector3(+obb1.half_width.x, +obb1.half_width.y, +obb1.half_width.z) }, //0
		//	{vector3(+obb1.half_width.x, +obb1.half_width.y, -obb1.half_width.z) }, //1
		//	{vector3(+obb1.half_width.x, -obb1.half_width.y, +obb1.half_width.z) }, //2
		//	{vector3(+obb1.half_width.x, -obb1.half_width.y, -obb1.half_width.z) }, //3
		//	{vector3(-obb1.half_width.x, +obb1.half_width.y, +obb1.half_width.z) }, //4
		//	{vector3(-obb1.half_width.x, +obb1.half_width.y, -obb1.half_width.z) }, //5
		//	{vector3(-obb1.half_width.x, -obb1.half_width.y, +obb1.half_width.z) }, //6
		//	{vector3(-obb1.half_width.x, -obb1.half_width.y, -obb1.half_width.z) }, //7
		//};
		//
		////vertexs_0��vertexs_1�̍��W�n��
		//vector3 offsetPos_0to1 = obb1.center - obb0.center;
		//quaternion offsetOrient_0to1 = obb1.orient * -obb0.orient;

		//for (int i = 0; i < 8; i++) {
		//	vertexs_0[i] = vector3_be_rotated_by_quaternion(vertexs_0[i], offsetOrient_0to1);
		//	vertexs_0[i] += offsetPos_0to1;
		//}
		//

		////vertexs_1���W�n�ł̊e�ӂ̏��
		//struct segment {
		//	vector3 s_point;
		//	vector3 g_point;
		//	segment(vector3 s, vector3 g) :s_point(s), g_point(g) {};
		//};
		/*segment edge_0[12]{
			{vertexs_0[0],vertexs_0[1]},
			{vertexs_0[0],vertexs_0[2]},
			{vertexs_0[0],vertexs_0[4]},

			{vertexs_0[5],vertexs_0[1]},
			{vertexs_0[5],vertexs_0[4]},
			{vertexs_0[5],vertexs_0[7]},

			{vertexs_0[3],vertexs_0[1]},
			{vertexs_0[3],vertexs_0[2]},
			{vertexs_0[3],vertexs_0[7]},

			{vertexs_0[6],vertexs_0[2]},
			{vertexs_0[6],vertexs_0[4]},
			{vertexs_0[6],vertexs_0[7]}
		};

		segment edge_1[12]{
			{vertexs_1[0],vertexs_1[1]},
			{vertexs_1[0],vertexs_1[2]},
			{vertexs_1[0],vertexs_1[4]},

			{vertexs_1[5],vertexs_1[1]},
			{vertexs_1[5],vertexs_1[4]},
			{vertexs_1[5],vertexs_1[7]},

			{vertexs_1[3],vertexs_1[1]},
			{vertexs_1[3],vertexs_1[2]},
			{vertexs_1[3],vertexs_1[7]},

			{vertexs_1[6],vertexs_1[2]},
			{vertexs_1[6],vertexs_1[4]},
			{vertexs_1[6],vertexs_1[7]}
		};


		vector3 sA, sB;
		float s_penetrate = FLT_MAX;
		vector3 cp_A, cp_B;
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 12; j++) {

				Closest_func::get_closestP_two_segment(
					edge_0[i].s_point, edge_0[i].g_point,
					edge_1[j].s_point, edge_1[j].g_point,
					sA, sB
				);

				float dis = (sA - sB).norm();
				if (dis * dis < s_penetrate * s_penetrate) {
					s_penetrate = sqrtf(dis);
					cp_A = sA;
					cp_B = sB;
				}

			}
		}*/

		//vector3 n = vector3_cross(obb0.u_axes[smallest_axis[0]], obb1.u_axes[smallest_axis[1]]);
		//pair.contacts.addcontact(
		//	s_penetrate,
		//	n,
		//	cp_A,
		//	cp_B
		//);
#else
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
#endif
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
//GPU�ɓn�����
struct cs_Pair
{
	int solv_num[2]; //solverbody�z��̉��Ԗڂɏ�񂪓����Ă��邩
	Contact contacts;
};
struct  Pair_max
{
	int num;
};

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
		SB.num = i;

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

				if (0.0f < cp.distance - slop) {
					cp.constraint[0].rhs += (bias * (cp.distance - slop)) / timeStep; //�߂荞�݂𒼂��l
				}

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


	//Compute_S::ComputeShader compute_shader;
	//ID3D11ComputeShader** AAA = compute_shader.computeShader.GetAddressOf();
	//compute_shader.Load("./DefaultShader/physics_resolve.cso");
	//if (AAA != nullptr && AAA != compute_shader.computeShader.GetAddressOf()) {
	//	int dafsd = 0;
	////}
	//for (int i = 0; i < 10000; i++) {
	//	Compute_S::ComputeShader aaaaaaaa;
	//	ID3D11ComputeShader** AAA = compute_shader.computeShader.GetAddressOf();
	//	aaaaaaaa.Load("./DefaultShader/physics_resolve.cso");
	//}

	//��������GPU�ɔC����
#if 0
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

#elif 0

	//QUESTION : ������static�O���Ƃ���������resourceanager���Ă������킩�炸�@��
	//static Compute_S::ComputeShader compute_shader;
	//compute_shader.Load("./DefaultShader/physics_resolve.cso");



	for (int i = 0; i < 10; i++) {

		std::vector<cs_Pair> cs_pair;
		Pair_max pair_max;
		pair_max.num = pairs.size();
		cs_Pair VVV;
		for (int i = 0; i < pairs.size(); i++) {
			VVV.contacts = pairs[i].contacts;
			VVV.solv_num[0] = pairs[i].body[0]->solve->num;
			VVV.solv_num[1] = pairs[i].body[1]->solve->num;
			cs_pair.emplace_back(VVV);
		}

		static Microsoft::WRL::ComPtr<StructureBuffer> pair_SB = nullptr;
		static Microsoft::WRL::ComPtr<StructureBuffer> max_SB = nullptr;
		static Microsoft::WRL::ComPtr<StructureBuffer> solve_SB = nullptr;
		static Microsoft::WRL::ComPtr<UAVBuffer> solve_out = nullptr;

		//assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(cs_Pair), pairs.size(), cs_pair.data(), pair_SB)));
		//assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Pair_max), 1, &pair_max, max_SB)));
		//assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Solverbody), SBs.size(), SBs.data(), solve_SB)));
		//assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Solverbody) * 2, pairs.size(), nullptr, solve_out)));

		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView > pair_SRV = nullptr;
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView > max_SRV = nullptr;
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  solve_SRV = nullptr;
		static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> solve_OUT = nullptr;

		//assert(SUCCEEDED(compute_shader.createSRV_fromSB(pair_SB,   &pair_SRV)));
		//assert(SUCCEEDED(compute_shader.createSRV_fromSB(max_SB,    &max_SRV)));
		//assert(SUCCEEDED(compute_shader.createSRV_fromSB(solve_SB,  &solve_SRV)));
		//assert(SUCCEEDED(compute_shader.createUAV_fromSB(solve_out, &solve_OUT)));

		//ID3D11ShaderResourceView* SRVs[3] = { pair_SRV.Get() ,max_SRV.Get(), solve_SRV.Get() };
		//compute_shader.run(SRVs, 3, solve_OUT.Get(), 16, 1, 1);

		//Solverbody* S = compute_shader.Read_UAV<Solverbody>(solve_out);
		//for (int i = 0; i < pairs.size() * 2 - 1; i++) {
		//	SBs[S[i].num].delta_AngularVelocity += S[i].delta_AngularVelocity;
		//	SBs[S[i].num].delta_LinearVelocity  += S[i].delta_LinearVelocity;

		//	if (S[i].delta_LinearVelocity.norm() > 1) {
		//		int dafsgdf = 0;
		//	}
		//}

	}

#endif

	// ���x�̍X�V
	for (int i = 0; i < colliders.size(); i++) {
		colliders[i]->linear_velocity  += colliders[i]->solve->delta_LinearVelocity;
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
