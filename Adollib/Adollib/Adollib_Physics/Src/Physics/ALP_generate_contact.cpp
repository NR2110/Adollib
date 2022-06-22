#include "ALP_generate_contact.h"


#include "collider_shape.h"
#include "ALP__meshcoll_data.h"
#include "ALP_contact.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

using namespace Closest_func;

#ifndef USE_SIM
//:::::::::::::::::::::::::::
#pragma region generate_contact
//:::::::::::::::::::::::::::

void Physics_function::generate_contact(std::vector<Contacts::Contact_pair*>& pairs) {

	const std::string work_meter_tag = std::string("Narrowphase");

	bool is_crossing = false;
	for (auto& pair : pairs) {

		const Collider_shape* shapeA = pair->body[0];
		const Collider_shape* shapeB = pair->body[1];
		is_crossing = false;

		if (shapeA->get_shape_tag() == ALPCollider_shape_type::Sphere) {

			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Sphere)	generate_contact_sphere_sphere(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::BOX)		generate_contact_sphere_box(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Capsule)	generate_contact_sphere_capsule(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Plane)	generate_contact_sphere_plane(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Mesh)	generate_contact_sphere_mesh(shapeA, shapeB, pair, is_crossing);
		}
		if (shapeA->get_shape_tag()     == ALPCollider_shape_type::BOX) {

			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Sphere)generate_contact_sphere_box(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::BOX)	generate_contact_box_box(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Capsule)generate_contact_box_capsule(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Plane)	generate_contact_box_plane(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Mesh)	generate_contact_box_mesh(shapeA, shapeB, pair, is_crossing);
		}
		if (shapeA->get_shape_tag()     == ALPCollider_shape_type::Capsule) {

			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Sphere)generate_contact_sphere_capsule(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::BOX)generate_contact_box_capsule(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Capsule)generate_contact_capsule_capsule(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Mesh)generate_contact_capsule_mesh(shapeA, shapeB, pair, is_crossing);
		}
		if (shapeA->get_shape_tag()     == ALPCollider_shape_type::Plane) {

			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Sphere)generate_contact_sphere_plane(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::BOX)	generate_contact_box_plane(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Plane) {}
		}
		if (shapeA->get_shape_tag()     == ALPCollider_shape_type::Mesh) {

			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Sphere)generate_contact_sphere_mesh(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Capsule)generate_contact_capsule_mesh(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::BOX)	generate_contact_box_mesh(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Plane)	generate_contact_mesh_plane(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Mesh)	generate_contact_mesh_mesh(shapeA, shapeB, pair, is_crossing);
		}

		//�����������Ă����炻�ꂼ���oncoll_bit�ɑ����tag��ǉ�
		if (is_crossing) {
			pair->body[0]->get_ALPcollider()->add_oncoll_bits(pair->body[1]->get_ALPcollider()->tag);
			pair->body[1]->get_ALPcollider()->add_oncoll_bits(pair->body[0]->get_ALPcollider()->tag);

			if (!pair->check_oncoll_only) {
				pair->body[0]->get_ALPcollider()->add_contacted_collider(pair, 0); //���g�̊ւ��Փˏ���ۑ�����
				pair->body[1]->get_ALPcollider()->add_contacted_collider(pair, 1);
			}

		}


	}
}


#pragma region data

//�����������p����ۂɎg�p����
enum class SAT_TYPE {
	POINTA_FACETB,
	POINTB_FACETA,
	EDGE_EDGE,
	NONE
};
struct Ret_S {
	float penetrate = FLT_MAX;
	int smallest_axis[2] = { -1,-1 };
	SAT_TYPE smallest_case = SAT_TYPE::NONE;
	bool hit_point_to_face = true;
};

struct OBB_struct {
	Vector3 world_position; //���S���W
	Vector3 u_axes[3]; //���̌���
	Quaternion world_orientation;
	Vector3 half_width; //�����Ƃ̕ӂ̒���
};

struct Capsule_struct {
	Vector3 world_position;
	Vector3 position;
	Vector3 y_axis;
	float hight = 0;
	float r = 0;
};

struct Triangle_struct {
	Vector3 world_position; //triangle�̒��S���W
	Quaternion world_orientation;
	Vector3 world_scale;
	Vector3 normal; //triangle�̖@��(����)
	Vector3 world_normal;
	Vector3 vertex_position[3]; //triangle��local�Ȓ��_���W
	Vector3 world_vertex_position[3]; //triangle��world�Ȓ��_���W

	int Edge_num[3] = { 0,0,0 };
};

//���ɓ��e��������
float sum_of_projected_radii(const OBB_struct& obb, const Vector3& vec) {
	return
		fabsf(vector3_dot(vec, obb.half_width.x * obb.u_axes[0])) +
		fabsf(vector3_dot(vec, obb.half_width.y * obb.u_axes[1])) +
		fabsf(vector3_dot(vec, obb.half_width.z * obb.u_axes[2]));
}

float sum_of_projected_radii(const Triangle_struct& tri, const Vector3& Wvec, float& min, float& max) {

	float A = vector3_dot(Wvec, tri.world_vertex_position[0]);
	float B = vector3_dot(Wvec, tri.world_vertex_position[1]);
	float C = vector3_dot(Wvec, tri.world_vertex_position[2]);

	min = ALmin3(A, B, C);
	max = ALmax3(A, B, C);

	return min;
}
float sum_of_projected_radii(float& max, float& min, const Collider_shape* meshcoll, const Vector3& nor) {
	float value;
	max = -FLT_MAX;
	min = +FLT_MAX;
	for (const Vector3& vertex : meshcoll->get_mesh_data()->vertices) {
		value = vector3_dot(vertex * meshcoll->world_scale(), nor);
		if (max < value)max = value;
		if (min > value)min = value;
	}

	return max - min;
}

//OBB��OBB�̕��������� �Փ˂��Ă����true��Ԃ� ���łɐF�X�����ɓ����
bool sat_obb_obb(
	const OBB_struct& obbA, const OBB_struct& obbB,
	float& smallest_penetration, //�ŏ��̊ђʗ�
	int smallest_axis[2], //�ǂ̎��ōŋ߂ɂȂ�����(edge�~edge�p��2���p��)
	SAT_TYPE& smallest_case //�ǂ̂悤�Ȍ`�ōŋ߂ɂȂ��Ă��邩
) {
	smallest_penetration = FLT_MAX;

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;


	float penetration = 0; //�ђʗ�
	float ra, rb; //obbA,obbB��L�ɓ��e���ꂽ����
	Vector3 axis; //���e���鎲
	Vector3 distBtoA = obbA.world_position - obbB.world_position; //2obb�̒��S���W�̋���

	//::: obbA�̎���obbB�𓊉e :::::::::::::::::::
	for (int i = 0; i < 3; i++)
	{
		axis = obbA.u_axes[i];
		ra = fabsf(obbA.half_width[i]);
		rb = fabsf(sum_of_projected_radii(obbB, axis));

		assert(ra > 0 && rb > 0);

		penetration = ra + rb - abs(vector3_dot(axis, distBtoA));
		if (penetration < 0) return false;

		if (fabsf(vector3_dot(axis, distBtoA)) + rb < ra) {
			PA_FB.hit_point_to_face = false;
		}

		if (PB_FA.penetrate > penetration) {
			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = i;
			PB_FA.smallest_axis[1] = -1;
			PB_FA.smallest_case = SAT_TYPE::POINTB_FACETA;
		}
	}

	//::: obbB�̎���obbA�𓊉e ::::::::::::::::::::
	if (PA_FB.hit_point_to_face == true)
		for (int i = 0; i < 3; i++)
		{
			axis = obbB.u_axes[i];
			ra = fabsf(sum_of_projected_radii(obbA, axis));
			rb = fabsf(obbB.half_width[i]);

			penetration = ra + rb - abs(vector3_dot(axis, distBtoA));
			if (penetration < 0) return false;

			if (fabsf(vector3_dot(axis, distBtoA)) + ra < rb) {
				PB_FA.hit_point_to_face = false;
			}

			if (PA_FB.penetrate > penetration) {

				PA_FB.penetrate = penetration;
				PA_FB.smallest_axis[0] = -1;
				PA_FB.smallest_axis[1] = i;
				PA_FB.smallest_case = SAT_TYPE::POINTA_FACETB;
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

	//::: �O�ς̎��ɓ��e
	for (int OB1 = 0; OB1 < 3; OB1++) {
		for (int OB2 = 0; OB2 < 3; OB2++) {

			axis = vector3_cross(obbA.u_axes[OB1], obbB.u_axes[OB2]);
			if (axis.norm() <= FLT_EPSILON * FLT_EPSILON)continue;//�O�ς� 0 = ���s

			axis = axis.unit_vect();
			if (axis.norm() < 1) axis *= (1 + FLT_EPSILON);

			ra = fabsf(sum_of_projected_radii(obbA, axis));
			rb = fabsf(sum_of_projected_radii(obbB, axis));
			//if (vector3_dot(axis, distBtoA) + rb < ra)continue;

			penetration = ra + rb - fabsf(vector3_dot(axis, distBtoA));
			if (penetration < 0) return false;
			if (smallest_penetration > penetration) {

				smallest_penetration = penetration;
				smallest_axis[0] = OB1;
				smallest_axis[1] = OB2;
				smallest_case = SAT_TYPE::EDGE_EDGE;
			}
		}
	}

	// ����������������Ȃ��ꍇOBB�͌������Ă���͂�
	return (smallest_penetration < FLT_MAX&& smallest_penetration > FLT_EPSILON) ? true : false;
}


//OBB��Triangle�̕��������� �������Ă����true��Ԃ� ���łɐF�X�����ɓ����
bool sat_obb_Triangle(
	const OBB_struct& obb, const Triangle_struct& triangle, const Physics_function::Meshcollider_data* mesh,
	float& smallest_penetration, //�ŏ��̊ђʗ�
	int smallest_axis[2], //�ǂ̎��ōŋ߂ɂȂ�����(edge�~edge�p��2���p��)
	SAT_TYPE& smallest_case //�ǂ̂悤�Ȍ`�ōŋ߂ɂȂ��Ă��邩
) {
	smallest_penetration = FLT_MAX;

	Ret_S PB_FA, PA_FB;

	float penetration = 0; //�ђʗ�
	float ra; //obb,triangle��L�ɓ��e���ꂽ����
	Vector3 axis; //���e���鎲
	const Vector3 offset_tri_obb = triangle.world_position - obb.world_position; //triangle����obb�ւ�offset

	//::: obb�̎���triangle�𓊉e :::::::::::::::::::
	// obb��0,0�Ɏ����Ă���(��]��world�̂܂܂�)
	for (int i = 0; i < 3; i++)
	{
		axis = obb.u_axes[i];
		ra = fabsf(obb.half_width[i]);

		//triangle�̎������̊e����
		float trimin, trimax;
		sum_of_projected_radii(triangle, axis, trimin, trimax);

		//�����̎������ւ̒���
		const float off = vector3_dot(axis, offset_tri_obb);

		//���ɓ��e�����ђʗʂ����߂�
		penetration = FLT_MAX;
		penetration = ALmin(penetration, ra - (trimin + off));
		penetration = ALmin(penetration, (trimax + off) - (-ra));

		//axis* mul_value��mesh->obb�̕����ɂȂ�悤�ɒl��ۑ�
		int mul_value = 1;
		if (ra - (trimin + off) < (trimax + off) - (-ra)) mul_value *= -1;

		if (penetration < 0) return false; //�������Ă�

		if (PB_FA.penetrate > penetration) {
			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = i;
			PB_FA.smallest_axis[1] = mul_value; //���邢���ǂ������g���ăf�[�^��n��
			PB_FA.smallest_case = SAT_TYPE::POINTB_FACETA;
		}
	}

	//::: triangle�̖@����obb�𓊉e ::::::::::::::::::::
	//obb��0,0�Ɏ����Ă���(��]��world�̂܂܂�)
	if (PA_FB.hit_point_to_face == true) {

		Vector3 triangle_world_normal = vector3_quatrotate(triangle.normal, triangle.world_orientation);
		for (int i = 0; i < 3; i++)
		{
			//�ӂƖ@���̊O�ςŕ��������s��
			axis = vector3_cross(triangle_world_normal, (triangle.world_vertex_position[i] - triangle.world_vertex_position[(i + 1) % 3]).unit_vect());
			if (vector3_dot(axis, triangle.world_position) > vector3_dot(axis, triangle.world_vertex_position[i]))
				axis *= -1;

			if (vector3_dot(axis, triangle.world_vertex_position[i]) > vector3_dot(axis, triangle.world_vertex_position[(i + 2) % 3])) axis *= -1; //axis��triangle�ӂ��璆�S�Ɍ�����

			//obb�̎������̒���
			ra = fabsf(sum_of_projected_radii(obb, axis));

			//triangle�̎������̊e����
			float trimin, trimax;
			sum_of_projected_radii(triangle, axis, trimin, trimax);

			//�����̎������ւ̒���
			const float off = vector3_dot(axis, offset_tri_obb);

			//���ɓ��e�����ђʗʂ����߂�
			penetration = FLT_MAX;
			penetration = ALmin(penetration, ra - (trimin + off));
			penetration = ALmin(penetration, (trimax + off) - (-ra));

			if (penetration < 0) return false; //�������Ă���

			if (trimin + off < -ra && +ra < trimax + off) {
				PB_FA.hit_point_to_face = false; //triang1���ł���->triangle�̒��_��obb�Ɍ������Ȃ�
			}

			if (PA_FB.penetrate > penetration) {

				PA_FB.penetrate = penetration;
				PA_FB.smallest_axis[0] = -1;
				PA_FB.smallest_axis[1] = -1;
				PA_FB.smallest_case = SAT_TYPE::NONE;
			}
		}

		{
			axis = triangle_world_normal;
			ra = fabsf(sum_of_projected_radii(obb, axis));
			//fabsf(vector3_dot(axis, triangle.vertex_position[0])); //����������̏Փ˂����m���Ȃ��̂ł���΂�����ς���

			float trimin, trimax;
			sum_of_projected_radii(triangle, axis, trimin, trimax);

			const float off = vector3_dot(axis, offset_tri_obb);

			penetration = FLT_MAX;
			//penetration = ALmin(penetration, ra - (trimin + off));   //������̏Փ˂͌��m���Ȃ�
			penetration = ALmin(penetration, (trimax + off) - (-ra));

			if (penetration < 0) return false; //�������Ă���

			if (PA_FB.penetrate > penetration) {

				PA_FB.penetrate = penetration;
				PA_FB.smallest_axis[0] = -1;
				PA_FB.smallest_axis[1] = 3; //3�͖ʖ@��
				PA_FB.smallest_case = SAT_TYPE::POINTA_FACETB;
			}
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

	//::: �O�ς̎��ɓ��e Waxis
	for (int OB1 = 0; OB1 < 3; OB1++) {
		for (int OB2 = 0; OB2 < 3; OB2++) {

			const Vector3& tri_vertex_p0 = mesh->vertices[mesh->edges[triangle.Edge_num[OB2]].vertexID[0]];
			const Vector3& tri_vertex_p1 = mesh->vertices[mesh->edges[triangle.Edge_num[OB2]].vertexID[1]];

			const Vector3& world_tri_vertex_p0 = vector3_quatrotate(tri_vertex_p0 * triangle.world_scale, triangle.world_orientation);
			const Vector3& world_tri_vertex_p1 = vector3_quatrotate(tri_vertex_p1 * triangle.world_scale, triangle.world_orientation);


			axis = vector3_cross(obb.u_axes[OB1], (world_tri_vertex_p0 - world_tri_vertex_p1).unit_vect());
			if (axis.norm() <= FLT_EPSILON * FLT_EPSILON)continue;//�O�ς� 0 = ���s

			//mesh����obb�̌�����axis��ύX(������Փ˂��Ȃ��悤�ɂ��邽��)
			if (vector3_dot(axis, obb.world_position - (world_tri_vertex_p0 + triangle.world_position)) < 0)//�߂荞��ł��邱�Ƃ�Y�ꂸ��!!!!
			{
				axis = axis * -1.0f;
			}
			axis = axis.unit_vect();
			if (axis.norm() <= 1) axis *= (1 - FLT_EPSILON);



			//Vector3 obb_vertex_pos;
			//if (vector3_dot(triangle.world_vertex_position[OB2],axis) < vector3_dot(obb.))

			ra = fabsf(sum_of_projected_radii(obb, axis));

			//triangle�̎������̊e����
			float trimin, trimax;
			sum_of_projected_radii(triangle, axis, trimin, trimax);

			//�����̎������ւ̒���
			const float off = vector3_dot(axis, offset_tri_obb);

			//���ɓ��e�����ђʗʂ����߂�
			penetration = FLT_MAX;
			//penetration = ALmin(penetration, ra - (trimin + off));  //������͏Փ˂��Ȃ�
			penetration = ALmin(penetration, (trimax + off) - (-ra));

			if (penetration < 0) return false; //�������Ă�

			if (vector3_dot(axis, triangle.world_normal) < 0)continue; //������͏Փ˂��Ȃ�
			if (mesh->edges[triangle.Edge_num[OB2]].type == Edgetype::EdgeConcave) continue; //�ւ��݂�Edge�͏Փ˂��Ȃ�
			if (mesh->edges[triangle.Edge_num[OB2]].type == Edgetype::EdgeFlat) continue; //���ʂ�Edge�͏Փ˂��Ȃ�

			if (smallest_penetration - penetration > FLT_EPSILON) {

				smallest_penetration = penetration;
				smallest_axis[0] = OB1;
				smallest_axis[1] = OB2;
				smallest_case = SAT_TYPE::EDGE_EDGE;

				//				axis = vector3_cross(obb.u_axes[OB1], (world_tri_vertex_p0 - world_tri_vertex_p1).unit_vect());
				//				if (axis.norm() <= FLT_EPSILON * FLT_EPSILON)continue;//�O�ς� 0 = ���s
				//
				////mesh����obb�̌�����axis��ύX(������Փ˂��Ȃ��悤�ɂ��邽��)
				//				if (vector3_dot(axis, obb.world_position - (world_tri_vertex_p0 + triangle.world_position)) < 0)//�߂荞��ł��邱�Ƃ�Y�ꂸ��!!!!
				//				{
				//					axis = axis * -1.0f;
				//				}
				//				axis = axis.unit_vect();
				//				if (axis.norm() <= 1) axis *= (1 - FLT_EPSILON);

				if (penetration < 0) return false; //�������Ă�
			}
		}
	}

	if (smallest_case == SAT_TYPE::NONE)return false;

	// ����������������Ȃ��ꍇOBB�͌������Ă���͂�
	return (smallest_penetration < FLT_MAX&& smallest_penetration > FLT_EPSILON) ? true : false;
}

bool sat_obb_capsule(
	const OBB_struct& obb, const Capsule_struct& capsule,
	float& smallest_penetration, //�ŏ��̊ђʗ�
	int smallest_axis[2], //�ǂ̎��ōŋ߂ɂȂ�����(edge�~edge�p��2���p��)
	SAT_TYPE& smallest_case //�ǂ̂悤�Ȍ`�ōŋ߂ɂȂ��Ă��邩
) {
	smallest_penetration = FLT_MAX;

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;


	float penetration = 0; //�ђʗ�
	Vector3 axis; //���e���鎲
	Vector3 distCapsuleToObb = capsule.position; //2obb�̒��S���W�̋���

	//box�̎���cupsule�𓊉e
	for (int i = 0; i < 3; i++) {

		const Vector3& axis = DOP::AABB_axis[i];
		float ra = obb.half_width[i];
		float rb = fabsf(vector3_dot(axis, capsule.y_axis)) * capsule.hight + capsule.r;

		assert(ra > 0 && rb > 0);

		float penetration = ra + rb - abs(vector3_dot(axis, distCapsuleToObb));
		if (penetration < 0) return false;

		if (PB_FA.penetrate > penetration) {
			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = i;
			PB_FA.smallest_axis[1] = -1;
			PB_FA.smallest_case = SAT_TYPE::POINTB_FACETA;
		}
	}

	//obb�̍ŋߒ��_��capsule�̎��̋�������
	{
		Vector3 nearext_vertex = obb.half_width;
		Vector3 capsule_nearest_pos;
		//obb�̒��S����capsule�֐����ȃx�N�g�����Ƃ�Aobb�̍ŋߒ��_�����߂�
		Closest_func::get_closestP_point_segment(obb.world_position, capsule.position - capsule.y_axis, capsule.position + capsule.y_axis, capsule_nearest_pos);
		if (capsule_nearest_pos.x < 0)nearext_vertex.x *= -1;
		if (capsule_nearest_pos.y < 0)nearext_vertex.y *= -1;
		if (capsule_nearest_pos.z < 0)nearext_vertex.z *= -1;

		//box�̕ӂ�capusle��y���Ő����̍ŋߓ_�����
		for (int i = 0; i < 3; i++) {
			Vector3 boxseg_g = nearext_vertex; //box����(nearext_vertex ~ boxseg_g)
			boxseg_g[i] *= -1;

			//box�̕�,capsuleY���̍ŋߓ_�����߁A�����������߂�
			Vector3 box_pos, capsule_pos;
			Closest_func::get_closestP_two_segment(
				nearext_vertex, boxseg_g,
				capsule.position - capsule.y_axis * capsule.hight, capsule.position + capsule.y_axis * capsule.hight,
				box_pos, capsule_pos
			);

			const Vector3 axis = (capsule_pos - box_pos).unit_vect();

			float ra = fabsf(vector3_dot(axis, obb.half_width.x * DOP::AABB_axis[0])) +
				fabsf(vector3_dot(axis, obb.half_width.y * DOP::AABB_axis[1])) +
				fabsf(vector3_dot(axis, obb.half_width.z * DOP::AABB_axis[2]));
			float rb = fabsf(vector3_dot(axis, capsule.y_axis) * capsule.hight) + capsule.r;

			float penetration = ra + rb - fabsf(vector3_dot(axis, distCapsuleToObb));
			if (penetration < 0) return false;



			if (PA_FB.penetrate > penetration) {
				PA_FB.penetrate = penetration;
				PA_FB.smallest_axis[0] = i;
				PA_FB.smallest_axis[1] = -1;
				PA_FB.smallest_case = SAT_TYPE::POINTA_FACETB;
			}
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

	return true;
}

//�ǂ�����ʕ�̏ꍇ
bool sat_convex_mesh_mesh(const Collider_shape* meshA, const Collider_shape* meshB,
	float& smallest_penetration, //�ŏ��̊ђʗ�
	int smallest_axis[2], //�ǂ̎��ōŋ߂ɂȂ�����(edge�~edge�p��2���p��)
	SAT_TYPE& smallest_case //�ǂ̂悤�Ȍ`�ōŋ߂ɂȂ��Ă��邩
) {
	smallest_penetration = FLT_MAX;

	//const std::list<ALP_Collider>::iterator collA = meshA->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator collB = meshB->get_ALPcollider();

	// Meshcoll_1�̊e�ʖ@���𕪗����Ƃ���
	Quaternion offset_quatBA = meshB->world_orientation() * meshA->world_orientation().inverse();
	Quaternion offset_quatAB = meshA->world_orientation() * meshB->world_orientation().inverse();
	Vector3 offset_posBA = meshB->world_position() - meshA->world_position();
	Vector3 offset_posAB = meshA->world_position() - meshB->world_position();

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;
	float penetration = FLT_MAX;

	////::: obbA�̎���obbB�𓊉e :::::::::::::::::::
		//collA�̍��W�n�Ōv�Z���s��
	float maxA, minA, maxB, minB;
	for (u_int f = 0; f < meshA->get_mesh_data()->facet_num; f++) {
		const Facet& facet = meshA->get_mesh_data()->facets.at(f); //meshcoord
		const Vector3& axis = facet.normal; //meshcoord

		// collA�𕪗����ɓ��e
		sum_of_projected_radii(maxA, minA, meshA, axis);
		assert(maxA >= minA);

		// collB�𕪗����ɓ��e
		sum_of_projected_radii(maxB, minB, meshB, vector3_quatrotate(axis, offset_quatAB).unit_vect());
		assert(maxB >= minB);
		float off = vector3_dot(offset_posBA, vector3_quatrotate(axis, meshA->world_orientation()).unit_vect());
		maxB += off;
		minB += off;

		//�ђʂ̌v�Z
		float d1 = minA - maxB;
		float d2 = minB - maxA;
		if (d1 >= 0.0f || d2 >= 0.0f)
			return false;


		penetration = -1 * ALmax(d1, d2);

		//if (fabsf(vector3_dot(axis, distBtoA)) + rb < ra) {
		//	PA_FB.hit_point_to_face = false;
		//}

		if (PB_FA.penetrate > penetration) {
			sum_of_projected_radii(maxB, minB, meshB, vector3_quatrotate(axis, offset_quatAB).unit_vect());

			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = f;
			PB_FA.smallest_axis[1] = -1;
			PB_FA.smallest_case = SAT_TYPE::POINTB_FACETA;
		}
	}

	////::: obbB�̎���obbA�𓊉e :::::::::::::::::::
		//collB�̍��W�n�Ōv�Z���s��
	if (PA_FB.hit_point_to_face == true)
		for (u_int f = 0; f < meshB->get_mesh_data()->facet_num; f++) {
			const Facet& facet = meshB->get_mesh_data()->facets.at(f);
			const Vector3& axis = facet.normal;

			// collB�𕪗����ɓ��e
			sum_of_projected_radii(maxB, minB, meshB, axis);
			assert(maxB >= minB);

			// collA�𕪗����ɓ��e
			sum_of_projected_radii(maxA, minA, meshA, vector3_quatrotate(axis, offset_quatBA));
			assert(maxA >= minA);
			float off = vector3_dot(offset_posAB, vector3_quatrotate(axis, meshB->world_orientation()).unit_vect());
			maxA += off;
			minA += off;

			//�ђʂ̌v�Z
			float d1 = minA - maxB;
			float d2 = minB - maxA;
			if (d1 >= 0.0f || d2 >= 0.0f)
				return false;


			penetration = -1 * ALmax(d1, d2);

			if (PA_FB.penetrate > penetration) {
				sum_of_projected_radii(maxA, minA, meshA, vector3_quatrotate(axis, offset_quatBA));

				PA_FB.penetrate = penetration;
				PA_FB.smallest_axis[0] = -1;
				PA_FB.smallest_axis[1] = f;
				PA_FB.smallest_case = SAT_TYPE::POINTA_FACETB;
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

	//Work_meter::start("generate_edge_edge");
	//::: �O�ς̎��ɓ��e(�ŋߋ��������߂邽��)
		//collA�̍��W�n�Ōv�Z���s��
	Vector3 save_axisW;
	float save_crossnorm = 0;
	Vector3 axisA, axisB, axisW;
	for (u_int eA = 0; eA < meshA->get_mesh_data()->edge_num; eA++) {
		const Edge& edgeA = meshA->get_mesh_data()->edges.at(eA);
		if (edgeA.type != Edgetype::EdgeConvex) continue;

		const Vector3& edgeVecA = meshA->get_mesh_data()->vertices.at(edgeA.vertexID[1]) - meshA->get_mesh_data()->vertices.at(edgeA.vertexID[0]);
		for (u_int eB = 0; eB < meshB->get_mesh_data()->edge_num; eB++) {
			const Edge& edgeB = meshB->get_mesh_data()->edges.at(eB);
			if (edgeB.type != Edgetype::EdgeConvex) continue;

			const Vector3 edgeVecB = vector3_quatrotate(meshB->get_mesh_data()->vertices.at(edgeB.vertexID[1]) - meshB->get_mesh_data()->vertices.at(edgeB.vertexID[0]), offset_quatBA);

			axisA = vector3_cross(edgeVecA, edgeVecB);
			if (axisA.norm() <= FLT_EPSILON * FLT_EPSILON) continue;
			axisA = axisA.unit_vect();
			axisW = vector3_quatrotate(axisA, meshA->world_orientation());
			//axis�̌�����A->B�ɂ���
			if (vector3_dot(axisW, meshB->world_position() - meshA->world_position()) < 0) {
				axisA *= -1;
				axisW *= -1;
			}
			axisB = vector3_quatrotate(axisA, offset_quatAB);

			sum_of_projected_radii(maxA, minA, meshA, axisA);		assert(maxA >= minA);
			sum_of_projected_radii(maxB, minB, meshB, axisB);		assert(maxB >= minB);
			float off = vector3_dot(offset_posBA, axisW);

			maxB += off;
			minB += off;

			//�ӂƕӂ̋���
			float CN = fabsf(off) - (vector3_dot(axisA, meshA->get_mesh_data()->vertices.at(edgeA.vertexID[0])) + -(vector3_dot(axisB, meshB->get_mesh_data()->vertices.at(edgeB.vertexID[0]))));

			//�ђʂ̌v�Z
			float d1 = minA - maxB;
			float d2 = minB - maxA;
			if (d1 >= 0.0f || d2 >= 0.0f)
				return false;

			penetration = -1 * ALmax(d1, d2) * 1.0001f;

			if (smallest_penetration > penetration) {
				smallest_penetration = penetration;
				smallest_axis[0] = eA;
				smallest_axis[1] = eB;
				smallest_case = SAT_TYPE::EDGE_EDGE;
				save_axisW = axisW;
				save_crossnorm = CN;
			}
			else if (fabsf(vector3_dot(axisW, save_axisW)) > 0.999 && CN < save_crossnorm) {
				//�������͓����������߂��������������̏���
				smallest_axis[0] = eA;
				smallest_axis[1] = eB;
				save_axisW = axisW;
				save_crossnorm = CN;
			}
		}

	}
	//Work_meter::stop("generate_edge_edge");
	// ����������������Ȃ��ꍇOBB�͌������Ă���͂�
	return (smallest_penetration < FLT_MAX&& smallest_penetration > FLT_EPSILON) ? true : false;

}

bool sat_obb_convex_mesh(const OBB_struct& obb, const Collider_shape* mesh,
	float& smallest_penetration, //�ŏ��̊ђʗ�
	int smallest_axis[2], //�ǂ̎��ōŋ߂ɂȂ�����(edge�~edge�p��2���p��)
	SAT_TYPE& smallest_case //�ǂ̂悤�Ȍ`�ōŋ߂ɂȂ��Ă��邩
) {
	smallest_penetration = FLT_MAX;

	//const std::list<ALP_Collider>::iterator& mesh_coll = mesh->get_ALPcollider();

	// Meshcoll_1�̊e�ʖ@���𕪗����Ƃ���
	Quaternion offset_quatBA = mesh->world_orientation() * obb.world_orientation.inverse();
	Quaternion offset_quatAB = obb.world_orientation * mesh->world_orientation().inverse();
	Vector3 offset_posBA = mesh->world_position() - obb.world_position;
	Vector3 offset_posAB = obb.world_position - mesh->world_position();

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;
	float penetration = FLT_MAX;

	////::: obb�̎���mesh�𓊉e :::::::::::::::::::
		//obb�̍��W�n�Ōv�Z���s��
	float maxA, minA;
	for (u_int f = 0; f < 3; f++) {
		const Vector3& axis = obb.u_axes[f]; //worldcoord

		// obb�𕪗����ɓ��e
		float ra = obb.half_width[f];

		// mesh�𕪗����ɓ��e
		sum_of_projected_radii(maxA, minA, mesh, vector3_quatrotate(axis, mesh->world_orientation().inverse()).unit_vect());
		assert(maxA >= minA);
		float off = vector3_dot(offset_posBA, axis);
		maxA += off;
		minA += off;

		//�ђʂ̌v�Z
		float d1 = -ra - maxA;
		float d2 = minA - ra;
		if (d1 >= 0.0f || d2 >= 0.0f)
			return false;

		penetration = -1 * ALmax(d1, d2);

		if (PB_FA.penetrate > penetration) {

			PB_FA.penetrate = penetration;
			PB_FA.smallest_axis[0] = f;
			PB_FA.smallest_axis[1] = -1;
			PB_FA.smallest_case = SAT_TYPE::POINTB_FACETA;
		}
	}

	////::: obbB�̎���obbA�𓊉e :::::::::::::::::::
		//mesh�̍��W�n�Ōv�Z���s��
	if (PA_FB.hit_point_to_face == true)
		for (u_int f = 0; f < mesh->get_mesh_data()->facet_num; f++) {
			const Facet& facet = mesh->get_mesh_data()->facets.at(f);
			const Vector3& axis = facet.normal; //meshcoord

			// mesh�𕪗����ɓ��e
			sum_of_projected_radii(maxA, minA, mesh, axis);
			assert(maxA >= minA);

			// obb�𕪗����ɓ��e
			float ra = sum_of_projected_radii(obb, vector3_quatrotate(axis, mesh->world_orientation()));

			float off = vector3_dot(offset_posAB, vector3_quatrotate(axis, mesh->world_orientation()).unit_vect());
			maxA += off;
			minA += off;

			//�ђʂ̌v�Z
			float d1 = minA - ra;
			float d2 = -ra - maxA;
			if (d1 >= 0.0f || d2 >= 0.0f)
				return false;


			penetration = -1 * ALmax(d1, d2);

			if (PA_FB.penetrate > penetration) {

				PA_FB.penetrate = penetration;
				PA_FB.smallest_axis[0] = -1;
				PA_FB.smallest_axis[1] = f;
				PA_FB.smallest_case = SAT_TYPE::POINTA_FACETB;
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
	//Work_meter::stop("generate_edge_edge");
	// ����������������Ȃ��ꍇOBB�͌������Ă���͂�
	return (smallest_penetration < FLT_MAX&& smallest_penetration > FLT_EPSILON) ? true : false;
}
//
#pragma endregion

//�Փː���
#pragma region SPHERE-SPHERE
bool Physics_function::generate_contact_sphere_sphere(const Collider_shape* SA, const Collider_shape* SB, Contacts::Contact_pair*& pair, bool& is_crossing) {
	//const std::list<ALP_Collider>::iterator& SA = SA->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator& SB = SB->get_ALPcollider();

	Vector3 pA = SA->world_position();
	Vector3 pB = SB->world_position();

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	//p1 ���� p0�@�����ւ̖@��
	Vector3 n = pA - pB;
	float length = n.norm_sqr();
	n = n.unit_vect();

	if (length < SA->world_scale().x + SB->world_scale().x) {
		//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐��p�ɏ�������
		is_AC = true;
		ACpenetration = SA->world_scale().x + SB->world_scale().x - length;
		ACnormal = n;
		ACcontact_pointA = SA->world_scale().x * vector3_quatrotate(-n, SA->world_orientation().inverse());
		ACcontact_pointB = SB->world_scale().x * vector3_quatrotate(+n, SB->world_orientation().inverse());
	}

	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇaddcontact������return
		if (pair->check_oncoll_only == true) return false;


		pair->contacts.addcontact(
			ACpenetration,
			ACnormal,
			ACcontact_pointA,
			ACcontact_pointB
		);
	}
	return is_AC;
}
#pragma endregion

#pragma region SPHERE-PLANE
bool Physics_function::generate_contact_sphere_plane(const Collider_shape* sphere, const Collider_shape* plane, Contacts::Contact_pair*& pair, bool& is_crossing) {
	//const std::list<ALP_Collider>::iterator& sphere = sphere_mesh->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator& plane = plane_mesh->get_ALPcollider();

	sphere->get_shape_tag();

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	//���ʂƕ��ʂ̏Փ˔�����s��
	Matrix44 rotate, inverse_rotate;
	rotate = plane->world_orientation().get_rotate_matrix();
	rotate._41 = plane->world_position().x; //transpse�̓���
	rotate._42 = plane->world_position().y;
	rotate._43 = plane->world_position().z;
	rotate._44 = 1;
	Vector3 n(rotate._21, rotate._22, rotate._23);
	inverse_rotate = matrix_inverse(rotate);

	Vector3 p;
	p = vector3_trans(sphere->world_position(), inverse_rotate); //���ʂ�"���_��ʂ�"�@����(0,1,0)"�̏�Ԃ̎��̋��̒��S���W

	////���ʂ̗�����̏Փ˔���
	//if (half_space && p.y < 0)return 0;

	if (abs(p.y) < sphere->world_scale().x) {

		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		n = p.y > 0 ? n : -n;

		//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐��p�ɏ�������
		is_AC = true;
		ACpenetration = sphere->world_scale().x - abs(p.y);
		ACnormal = n;
		ACcontact_pointA = Vector3(p.x, 0, p.z);
		ACcontact_pointB = sphere->world_scale().x * vector3_quatrotate(-n, sphere->world_orientation().inverse());

	}

	if (is_AC)
	{
		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		pair->contacts.addcontact(
			ACpenetration,
			ACnormal,
			ACcontact_pointA,
			ACcontact_pointB
		);
	}
	return is_AC;
}
#pragma endregion

#pragma region SPHERE-BOX
bool Physics_function::generate_contact_sphere_box(const Collider_shape* sphere, const Collider_shape* box, Contacts::Contact_pair*& pair, bool& is_crossing) {
	//const std::list<ALP_Collider>::iterator& sphere = sphere_mesh->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator& box = box_part->get_ALPcollider();

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	//����box�̏Փ˔�����s��
	Matrix44 rotate, inverse_rotate;
	rotate = matrix_world(Vector3(1, 1, 1), box->world_orientation().get_rotate_matrix(), box->world_position());
	inverse_rotate = matrix_inverse(rotate);

	Vector3 center;
	center = vector3_trans(sphere->world_position(), inverse_rotate); //box��local���W�n�ł̋��̒��S���W

	Vector3 box_halfsize = box->world_scale();

	if (
		abs(center.x) - sphere->world_scale().x > box_halfsize.x ||
		abs(center.y) - sphere->world_scale().x > box_halfsize.y ||
		abs(center.z) - sphere->world_scale().x > box_halfsize.z
		) return 0;

	//box��̍ŋߓ_
	Vector3 closest_point;

	closest_point = center;
	if (center.x > +box_halfsize.x)closest_point.x = +box_halfsize.x;
	if (center.x < -box_halfsize.x)closest_point.x = -box_halfsize.x;

	if (center.y > +box_halfsize.y)closest_point.y = +box_halfsize.y;
	if (center.y < -box_halfsize.y)closest_point.y = -box_halfsize.y;

	if (center.z > +box_halfsize.z)closest_point.z = +box_halfsize.z;
	if (center.z < -box_halfsize.z)closest_point.z = -box_halfsize.z;

	float distance = (closest_point - center).norm_sqr(); //�ŋߓ_�Ƌ����S�̋���
	if (sphere->world_scale().x - distance < FLT_EPSILON) return false; //�Փ˂��Ă��Ȃ�������false
	float penetrate = sphere->world_scale().x - distance;

	//closest_point��center�̂܂� -> sphere�̒��S��box�����ɂ���
	if (closest_point == center) {
		//sphere�̒��S�����ԋ߂��ʂ�T��
		float min_value = FLT_MAX;
		int min_num = -1;

		for (int i = 0; i < 3; i++) {
			if(box_halfsize[i] - center[i] < min_value) {
				min_value = box_halfsize[i] - center[i];
				min_num = i;
			}
			if (box_halfsize[i] + center[i] < min_value) {
				min_value = box_halfsize[i] + center[i];
				min_num = i;
			}
		}
		if (center[min_num] > 0)closest_point[min_num] = +box_halfsize[min_num];
		else closest_point[min_num] = -box_halfsize[min_num];

		distance = (closest_point - center).norm_sqr(); //�ŋߓ_�Ƌ����S�̋���
		penetrate = sphere->world_scale().x + distance;
	}


	Vector3 n = (sphere->world_position() - vector3_trans(closest_point, rotate)).unit_vect(); //box����sphere�ւ̃x�N�g��
	if (vector3_dot(n, sphere->world_position() - box->world_position()) < 0)n *= -1;

	//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐��p�ɏ�������
	is_AC = true;
	ACpenetration = penetrate;
	ACnormal = -n;
	ACcontact_pointA = closest_point;
	ACcontact_pointB = sphere->world_scale().x * vector3_quatrotate(-n, sphere->world_orientation().inverse());


	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		if (pair->body[0]->get_shape_tag() == box->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);
	}
	return is_AC;
}
#pragma endregion

#pragma region SPHERE-Capsule
bool Physics_function::generate_contact_sphere_capsule(const Collider_shape* sphere, const Collider_shape* capsule, Contacts::Contact_pair*& pair, bool& is_crossing) {

	//const std::list<ALP_Collider>::iterator& sphere = sphere_mesh->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator& capsule = capsule_mesh->get_ALPcollider();

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	//capsule�̍��W�n�Ōv�Z����(scale�͕ύX���Ȃ�)

	//capsule coord
	Vector3 sphere_pos_capcoord = vector3_quatrotate(sphere->world_position() - capsule->world_position(), capsule->world_orientation().inverse());

	float s;
	Closest_func::get_closestP_point_line(sphere_pos_capcoord, Vector3(0), Vector3(0, 1, 0), s);
	s = ALmax(ALmin(s, +capsule->world_scale().y), -capsule->world_scale().y); //capsule�̒����ɃN�����v


	//capsule�̍��W�n��sphere-sphere�̔�����s��
	Vector3 pA = sphere_pos_capcoord;
	Vector3 pB = s * Vector3(0, 1, 0);

	//p1 ���� p0�@�����ւ̖@��
	Vector3 n = pA - pB;
	float length = n.norm_sqr();
	n = n.unit_vect();

	if (length < sphere->world_scale().x + capsule->world_scale().x) {
		Vector3 Wn = vector3_quatrotate(n, capsule->world_orientation()); //n��capsule���烏�[���h��

		//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐��p�ɓ���
		is_AC = true;
		ACpenetration = sphere->world_scale().x + capsule->world_scale().x - length;
		ACnormal = +Wn;
		ACcontact_pointA = sphere->world_scale().x * vector3_quatrotate(-Wn, sphere->world_orientation().inverse());
		ACcontact_pointB = capsule->world_scale().x * n + pB;

	}

	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		if (pair->body[0]->get_shape_tag() == sphere->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);
	}
	return is_AC;
}
#pragma endregion

#pragma region SPHERE-MESH
bool Physics_function::generate_contact_sphere_mesh(const Collider_shape* sphere, const Collider_shape* mesh, Contacts::Contact_pair*& pair, bool& is_crossing) {
	//const std::list<ALP_Collider>::iterator& sphere = sphere_mesh->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator& mesh = mesh_part->get_ALPcollider();

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	if (false && mesh->get_mesh_data()->is_Convex == true) {
		//����mesh�̏Փ˔�����s��
		Matrix44 rotate, inverse_rotate;
		rotate = matrix_world(Vector3(1, 1, 1), mesh->world_orientation().get_rotate_matrix(), mesh->world_position());
		inverse_rotate = matrix_inverse(rotate);

		Vector3 center;
		center = vector3_trans(sphere->world_position(), inverse_rotate); //mesh��local���W�n�ł̋��̒��S���W

		//mesh��̍ŋߓ_
		Vector3 closest_point;

		closest_point = center;
		//�e�ʂ̊O�ɂ���Ζʕ��ʂɎ����Ă���
		for (u_int i = 0; i < mesh->get_mesh_data()->facet_num; i++) {
			const Vector3& nor = mesh->get_mesh_data()->facets.at(i).normal.unit_vect();
			const Vector3& pos = mesh->get_mesh_data()->vertices.at(mesh->get_mesh_data()->facets.at(i).vertexID[0]) * mesh->world_scale();
			float d = vector3_dot(nor, pos) - vector3_dot(nor, closest_point);
			if (d < 0)
				closest_point += d * nor;
		}

		float distance = (closest_point - center).norm_sqr(); //�ŋߓ_�Ƌ����S�̋���
		if (sphere->world_scale().x - distance > FLT_EPSILON) { //float�덷������
			Vector3 n = (sphere->world_position() - vector3_trans(closest_point, rotate)).unit_vect(); //mesh����sphere�ւ̃x�N�g��

			//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐��p�ɓ���
			is_AC = true;
			ACpenetration = sphere->world_scale().x - distance;
			ACnormal = -n;
			ACcontact_pointA = closest_point;
			ACcontact_pointB = sphere->world_scale().x * vector3_quatrotate(n, sphere->world_orientation().inverse());
		}

	}
	else {
		//����mesh�̏Փ˔�����s��

		Vector3 sphere_pos_meshcoord = vector3_quatrotate(sphere->world_position() - mesh->world_position(), mesh->world_orientation().inverse()); //mesh���W�n�ł�sphere��pos

		float min_dis = sphere->world_scale().x;//�Œ዗����sphere�̔��a��
		Vector3 closest_point; //mesh��̍ŋߓ_
		bool is_hit = false; //�Փ˂��Ă�����true
		Vector3 nor_meshcoord;
		//����mesh�̔���
		for (const auto& faset : mesh->get_mesh_data()->facets) {
			const Vector3& nor = (faset.normal * mesh->world_scale()).unit_vect();
			const Vector3& mesh_pos0 = mesh->get_mesh_data()->vertices.at(faset.vertexID[0]) * mesh->world_scale();

			//mesh���ʂ�"d"
			float dis = vector3_dot(nor, mesh_pos0);
			float dis_sp = vector3_dot(nor, sphere_pos_meshcoord);

			//mesh���ʂ�sphere�̋�����min_dis���傫����ΏՓ˂��Ȃ�
			if (fabsf(dis_sp - dis) > min_dis) continue;

			const Vector3& mesh_pos1 = mesh->get_mesh_data()->vertices.at(faset.vertexID[1]) * mesh->world_scale();
			const Vector3& mesh_pos2 = mesh->get_mesh_data()->vertices.at(faset.vertexID[2]) * mesh->world_scale();

			Vector3 closest_p;
			Closest_func::get_closestP_point_triangle(
				sphere_pos_meshcoord,
				mesh_pos0,
				mesh_pos1,
				mesh_pos2,
				nor,
				closest_p
			);

			if ((sphere_pos_meshcoord - closest_p).norm() > min_dis * min_dis)continue; //�ۑ�����Ă��鋗�����傫�����continue

			//min_dis,�ŋߓ_�̍X�V
			closest_point = closest_p;
			min_dis = (sphere_pos_meshcoord - closest_point).norm_sqr();
			is_hit = true;
			nor_meshcoord = nor;
		}
		//�������Ă��Ȃ����false��Ԃ�
		if (is_hit == false)return false;

		Vector3 Wn = vector3_quatrotate((sphere_pos_meshcoord - closest_point), mesh->world_orientation()).unit_vect();//mesh����sphere�ւ̃x�N�g��
		if (vector3_dot(sphere_pos_meshcoord - closest_point, nor_meshcoord) < 0)Wn *= -1;

		is_AC = true;
		ACpenetration = sphere->world_scale().x - min_dis;
		ACnormal = +Wn;
		ACcontact_pointA = sphere->world_scale().x * vector3_quatrotate(-Wn, sphere->world_orientation().inverse());
		ACcontact_pointB = closest_point;
	}

	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		if (pair->body[0]->get_shape_tag() == sphere->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);
	}
	return is_AC;
}
#pragma endregion


#pragma region BOX-PLANE
bool Physics_function::generate_contact_box_plane(const Collider_shape* box, const Collider_shape* plane, Contacts::Contact_pair*& pair, bool& is_crossing) {
	//const std::list<ALP_Collider>::iterator& box = box_part->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator& plane = plane_mesh->get_ALPcollider();

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	Vector3 vertices[8] = {
		// obb���W�n�ł̊e���_�̃��[�J�����W
		Vector3(-box->world_scale().x, -box->world_scale().y, -box->world_scale().z),
		Vector3(-box->world_scale().x, -box->world_scale().y, +box->world_scale().z),
		Vector3(-box->world_scale().x, +box->world_scale().y, -box->world_scale().z),
		Vector3(-box->world_scale().x, +box->world_scale().y, +box->world_scale().z),
		Vector3(+box->world_scale().x, -box->world_scale().y, -box->world_scale().z),
		Vector3(+box->world_scale().x, -box->world_scale().y, +box->world_scale().z),
		Vector3(+box->world_scale().x, +box->world_scale().y, -box->world_scale().z),
		Vector3(+box->world_scale().x, +box->world_scale().y, +box->world_scale().z)
	};

	//Box�ƕ��ʂ̏Փ˔�����s��
	Matrix44 rotate, inverse_rotate;
	rotate = plane->world_orientation().get_rotate_matrix();
	rotate._41 = plane->world_position().x; //transpse�̓���
	rotate._42 = plane->world_position().y;
	rotate._43 = plane->world_position().z;
	rotate._44 = 1;
	Vector3 n(rotate._21, rotate._22, rotate._23);
	inverse_rotate = matrix_inverse(rotate);

	Vector3 p;
	p = vector3_trans(box->world_position(), inverse_rotate); //���ʂ�"���_��ʂ�"�@����(0,1,0)"�̏�Ԃ̎���Box�̒��S���W(p.y�������ɂȂ�)
	//n = p.y > 0 ? n : -n; //�{�b�N�X�����ʉ��ɂ�������@���𔽑΂ɂ���

	float max_penetrate = 0.0f;
	Vector3 pointbox, pointplane;
	Vector3 vs;

	for (int i = 0; i < 8; i++) {
		// ���_�̃��[���h���W
		vs = vector3_quatrotate(vertices[i], box->world_orientation());

		// -�ʖ@���̓��ς��Ƃ�(�ʕ����ւ̒���)
		float rA = vector3_dot(-n, vs);

		if (max_penetrate < rA - p.y) {
			max_penetrate = rA - p.y;
			pointbox = vertices[i];

			// ���_��plane��Ԃ�
			vs = vector3_trans(vs + box->world_position(), inverse_rotate);
			pointplane = vs;
			pointplane.y = 0;
		}
	}

	if (max_penetrate != 0) {
		is_AC = true;

		ACpenetration = max_penetrate;
		ACnormal = -n;
		ACcontact_pointA = pointplane;
		ACcontact_pointB = pointbox;
	}

	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		if (pair->body[0]->get_shape_tag() == box->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);
	}
	return is_AC;
}
#pragma endregion

#pragma region BOX-BOX

bool Physics_function::generate_contact_box_box(const Collider_shape* boxA, const Collider_shape* boxB, Contacts::Contact_pair*& pair, bool& is_crossing) {
	//const std::list<ALP_Collider>::iterator& boxA = bA->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator& boxB = bB->get_ALPcollider();

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	Matrix33 m;
	m = boxA->world_orientation().get_rotate_matrix();
	OBB_struct obbA;
	obbA.world_position = boxA->world_position();
	obbA.u_axes[0].x = m._11; obbA.u_axes[0].y = m._12; obbA.u_axes[0].z = m._13;
	obbA.u_axes[1].x = m._21; obbA.u_axes[1].y = m._22; obbA.u_axes[1].z = m._23;
	obbA.u_axes[2].x = m._31; obbA.u_axes[2].y = m._32; obbA.u_axes[2].z = m._33;
	obbA.half_width = boxA->world_scale();
	obbA.world_orientation = boxA->world_orientation();
	assert(!isnan(obbA.world_orientation.norm()));

	m = boxB->world_orientation().get_rotate_matrix();
	OBB_struct obbB;
	obbB.world_position = boxB->world_position();
	obbB.u_axes[0].x = m._11; obbB.u_axes[0].y = m._12; obbB.u_axes[0].z = m._13;
	obbB.u_axes[1].x = m._21; obbB.u_axes[1].y = m._22; obbB.u_axes[1].z = m._23;
	obbB.u_axes[2].x = m._31; obbB.u_axes[2].y = m._32; obbB.u_axes[2].z = m._33;
	obbB.half_width = boxB->world_scale();
	obbB.world_orientation = boxB->world_orientation();
	assert(!isnan(obbB.world_orientation.norm()));

	float smallest_penetration = FLT_MAX;	//�ŏ��߂荞�ݗ�
	int smallest_axis[2];	//�ŏ��߂荞�ݗʂ𓾂��������̍쐬�Ɏg�p�����eOBB�̃��[�J�����ԍ� �Ӂ~�ӗp��2��
	SAT_TYPE smallest_case;	//�Փ˂̎��
	if (!sat_obb_obb(obbA, obbB, smallest_penetration, smallest_axis, smallest_case)) return false;

	//obbB�̒��_��obbA�̖ʂƏՓ˂����ꍇ
	if (smallest_case == SAT_TYPE::POINTB_FACETA)
	{
		Vector3 d = obbA.world_position - obbB.world_position;	//B����A����
		Vector3 Wn = obbA.u_axes[smallest_axis[0]];	//obbA�̏Փ˖ʂ̖@���ƕ��s��obbA�̃��[�J�����x�N�g��
		if (vector3_dot(Wn, d) < 0)	//obbA��obbB�̈ʒu�֌W���Փ˖ʂ̖@���x�N�g�������肷��
		{
			Wn = Wn * -1.0f;
		}
		Wn = Wn.unit_vect();

		//boxB�̃��[�J�����W�n�ł̍ŋߓ_(p1) obb1��8���_�̂����̂ǂꂩ
		Vector3 p1 = obbB.half_width;	//obb1�̊e�ӂ̒����́Aobb1�̏d�S����ڐG�_(p)�ւ̑��Έʒu�̎肪����ɂȂ�
		//obb0��obb1�̈ʒu�֌W(d)���ڐG�_(p)�����߂�
		if (vector3_dot(obbB.u_axes[0], -Wn) > 0) p1.x = -p1.x;
		if (vector3_dot(obbB.u_axes[1], -Wn) > 0) p1.y = -p1.y;
		if (vector3_dot(obbB.u_axes[2], -Wn) > 0) p1.z = -p1.z;

		//boxA�̋t�s��̍쐬
		Matrix44 rotate, inverse_rotate;
		rotate = boxA->world_orientation().get_rotate_matrix();
		rotate._41 = boxA->world_position().x; //transpse�̓���
		rotate._42 = boxA->world_position().y;
		rotate._43 = boxA->world_position().z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p1��obbA�̃��[�J�����W�n��
		Vector3 P = vector3_quatrotate(p1, boxB->world_orientation()) + boxB->world_position();
		Vector3 c = vector3_trans(P, inverse_rotate);

		//obbA�̍ŋߓ_�����߂�
		Vector3 p0 = c;
		Vector3 box_halfsize = boxA->world_scale();
		if (c.x > +box_halfsize.x)p0.x = +box_halfsize.x;
		if (c.x < -box_halfsize.x)p0.x = -box_halfsize.x;

		if (c.y > +box_halfsize.y)p0.y = +box_halfsize.y;
		if (c.y < -box_halfsize.y)p0.y = -box_halfsize.y;

		if (c.z > +box_halfsize.z)p0.z = +box_halfsize.z;
		if (c.z < -box_halfsize.z)p0.z = -box_halfsize.z;

		p0 += vector3_quatrotate(-Wn, boxA->world_orientation().inverse()) * smallest_penetration;

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = Wn;
		ACcontact_pointA = p0;
		ACcontact_pointB = p1;
	}
	//obbA�̒��_��obbB�̖ʂƏՓ˂����ꍇ
	else if (smallest_case == SAT_TYPE::POINTA_FACETB)
	{
		Vector3 d = obbA.world_position - obbB.world_position; //B����A����
		Vector3 Wn = obbB.u_axes[smallest_axis[1]];
		if (vector3_dot(Wn, d) < 0) //B����A�����łȂ����-1��������
		{
			Wn = Wn * -1.0f;
		}
		Wn = Wn.unit_vect();

		Vector3 p0 = obbA.half_width;
		//obb0��obb1�̈ʒu�֌W(d)���ڐG�_(p)�����߂�
		if (vector3_dot(obbA.u_axes[0], Wn) > 0) p0.x = -p0.x;
		if (vector3_dot(obbA.u_axes[1], Wn) > 0) p0.y = -p0.y;
		if (vector3_dot(obbA.u_axes[2], Wn) > 0) p0.z = -p0.z;

		//box0�̋t�s��̍쐬
		Matrix44 rotate, inverse_rotate;
		rotate = boxB->world_orientation().get_rotate_matrix();
		rotate._41 = boxB->world_position().x; //transpse�̓���
		rotate._42 = boxB->world_position().y;
		rotate._43 = boxB->world_position().z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p0��obb0�̃��[�J�����W�n��
		Vector3 P = vector3_quatrotate(p0, boxA->world_orientation()) + boxA->world_position();
		Vector3 c = vector3_trans(P, inverse_rotate);

		//obb0�̍ŋߓ_�����߂�
		Vector3 p1 = c;
		Vector3 box_halfsize = boxB->world_scale();
		if (c.x > +box_halfsize.x)p1.x = +box_halfsize.x;
		if (c.x < -box_halfsize.x)p1.x = -box_halfsize.x;

		if (c.y > +box_halfsize.y)p1.y = +box_halfsize.y;
		if (c.y < -box_halfsize.y)p1.y = -box_halfsize.y;

		if (c.z > +box_halfsize.z)p1.z = +box_halfsize.z;
		if (c.z < -box_halfsize.z)p1.z = -box_halfsize.z;

		p1 += vector3_quatrotate(Wn, boxB->world_orientation().inverse()) * smallest_penetration;

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = Wn;
		ACcontact_pointA = p0;
		ACcontact_pointB = p1;
	}
	//�Bobb0�̕ӂ�obb1�̕ӂƏՓ˂����ꍇ
	else if (smallest_case == SAT_TYPE::EDGE_EDGE)
	{

		Vector3 d = obbB.world_position - obbA.world_position;
		Vector3 n;
		n = vector3_cross(obbA.u_axes[smallest_axis[0]], obbB.u_axes[smallest_axis[1]]);
		n = n.unit_vect();
		if (vector3_dot(n, d) < 0)
		{
			n = n * -1.0f;
		}

		Vector3 p[2] = { obbA.half_width, obbB.half_width };
		{
			if (vector3_dot(obbA.u_axes[0], -n) > 0) p[0].x = -p[0].x;
			if (vector3_dot(obbA.u_axes[1], -n) > 0) p[0].y = -p[0].y;
			if (vector3_dot(obbA.u_axes[2], -n) > 0) p[0].z = -p[0].z;

			if (vector3_dot(obbB.u_axes[0], +n) > 0) p[1].x = -p[1].x;
			if (vector3_dot(obbB.u_axes[1], +n) > 0) p[1].y = -p[1].y;
			if (vector3_dot(obbB.u_axes[2], +n) > 0) p[1].z = -p[1].z;
		}

		Vector3 P0a = vector3_quatrotate(p[0], obbA.world_orientation) + obbA.world_position;
		Vector3 P1a = vector3_quatrotate(p[1], obbB.world_orientation) + obbB.world_position;

		float s, t;
		Closest_func::get_closestP_two_line(
			P0a, obbA.u_axes[smallest_axis[0]],
			P1a, obbB.u_axes[smallest_axis[1]],
			s, t
		);
		Vector3 b_axis[3]{
			Vector3(1,0,0),
			Vector3(0,1,0),
			Vector3(0,0,1)
		};

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = -n;
		ACcontact_pointA = p[0] + s * b_axis[smallest_axis[0]];
		ACcontact_pointB = p[1] + t * b_axis[smallest_axis[1]];
	}
	else assert(0);

	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		pair->contacts.addcontact(
			ACpenetration,
			ACnormal,
			ACcontact_pointA,
			ACcontact_pointB
		);
	}
	return is_AC;
}
#pragma endregion

#pragma region BOX-Capsule
bool Physics_function::generate_contact_box_capsule(const Collider_shape* box, const Collider_shape* capsule, Contacts::Contact_pair*& pair, bool& is_crossing) {
#if 1
	//const std::list<ALP_Collider>::iterator& capsule = capsule_mesh->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator& box = box_part->get_ALPcollider();

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	Vector3 cuppos_boxcoord = vector3_quatrotate(capsule->world_position() - box->world_position(), box->world_orientation().inverse());
	Vector3 cupYaxis_boxcoord = vector3_quatrotate(Vector3(0, capsule->world_scale().y, 0), capsule->world_orientation() * box->world_orientation().inverse());

	//box�̏����g���₷���悤�ɕύX
	Matrix33 m;
	m = box->world_orientation().get_rotate_matrix();
	OBB_struct obb;
	obb.world_position = box->world_position();
	obb.u_axes[0].x = m._11; obb.u_axes[0].y = m._12; obb.u_axes[0].z = m._13;
	obb.u_axes[1].x = m._21; obb.u_axes[1].y = m._22; obb.u_axes[1].z = m._23;
	obb.u_axes[2].x = m._31; obb.u_axes[2].y = m._32; obb.u_axes[2].z = m._33;
	obb.half_width = box->world_scale();
	obb.world_orientation = box->world_orientation();
	assert(!isnan(obb.world_orientation.norm()));

	//capsule�̏���
	Capsule_struct capsule_s;
	capsule_s.world_position = capsule->world_position();
	capsule_s.position = cuppos_boxcoord;
	capsule_s.r = capsule->world_scale().x;
	capsule_s.hight = capsule->world_scale().y;
	capsule_s.y_axis = cupYaxis_boxcoord.unit_vect();

	float smallest_penetration = FLT_MAX;	//�ŏ��߂荞�ݗ�
	int smallest_axis[2];	//�ŏ��߂荞�ݗʂ𓾂��������̍쐬�Ɏg�p�����eOBB�̃��[�J�����ԍ� �Ӂ~�ӗp��2��
	SAT_TYPE smallest_case;	//�Փ˂̎��
	if (!sat_obb_capsule(obb, capsule_s, smallest_penetration, smallest_axis, smallest_case))return false;

	//capsule��obb�̖ʂƏՓ˂����ꍇ
	if (smallest_case == SAT_TYPE::POINTB_FACETA) {

		Vector3 d = obb.world_position - capsule_s.world_position;	//capsule����obb����
		Vector3 Wn = obb.u_axes[smallest_axis[0]];	//box�Փ˖ʂ̖@�� = �Փ˖@��
		if (vector3_dot(Wn, d) < 0)	//obbA��obbB�̈ʒu�֌W���Փ˖ʂ̖@���x�N�g�������肷��
		{
			Wn = Wn * -1.0f;
		}
		Wn = Wn.unit_vect();
		Vector3 boxN = vector3_quatrotate(Wn, obb.world_orientation.inverse());

		//capsule�̃��[�J�����W�n�ł̍ŋߓ_(p1)
		Vector3 p1 = Vector3(0, 1, 0) * capsule_s.hight;
		if (vector3_dot(capsule_s.y_axis, -boxN) > 0) p1 = -p1;
		float a = vector3_dot(capsule_s.y_axis, -boxN);
		p1 += capsule_s.r * vector3_quatrotate(Wn, capsule->world_orientation().inverse());


		//box���W�n�ł�capsule�̏Փ˓_�����߂�
		Vector3 c = cuppos_boxcoord + cupYaxis_boxcoord;
		if (vector3_dot(capsule_s.y_axis, -boxN) > 0) c = cuppos_boxcoord - cupYaxis_boxcoord;
		c += capsule_s.r * boxN;

		//box�̍ŋߓ_�����߂�
		Vector3 p0 = c;
		Vector3 box_halfsize = box->world_scale();
		if (c.x > +box_halfsize.x)p0.x = +box_halfsize.x;
		if (c.x < -box_halfsize.x)p0.x = -box_halfsize.x;

		if (c.y > +box_halfsize.y)p0.y = +box_halfsize.y;
		if (c.y < -box_halfsize.y)p0.y = -box_halfsize.y;

		if (c.z > +box_halfsize.z)p0.z = +box_halfsize.z;
		if (c.z < -box_halfsize.z)p0.z = -box_halfsize.z;

		p0 -= boxN * smallest_penetration;

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = Wn;
		ACcontact_pointA = p0;
		ACcontact_pointB = p1;
	}

	//obb�̒��_��capsule�ƏՓ˂����ꍇ
	else if (smallest_case == SAT_TYPE::POINTA_FACETB) {
		Vector3 nearext_vertex = obb.half_width;
		Vector3 capsule_nearest_pos;
		//obb�̒��S����capsule�֐����ȃx�N�g�����Ƃ�Aobb�̍ŋߒ��_�����߂�
		Closest_func::get_closestP_point_segment(Vector3(0), capsule_s.position - capsule_s.y_axis * capsule_s.hight, capsule_s.position + capsule_s.y_axis * capsule_s.hight, capsule_nearest_pos);
		if (capsule_nearest_pos.x < 0)nearext_vertex.x *= -1;
		if (capsule_nearest_pos.y < 0)nearext_vertex.y *= -1;
		if (capsule_nearest_pos.z < 0)nearext_vertex.z *= -1;

		//TODO : nearext_vertex��

		//box�̕ӂ�capusle��y���Ő����̍ŋߓ_�����
		Vector3 boxseg_g = nearext_vertex; //box����(nearext_vertex ~ boxseg_g)
		boxseg_g[smallest_axis[0]] *= -1;

		//box�̕�,capsuleY���̍ŋߓ_�����߁A�����������߂�
		Vector3 box_pos, capsule_pos;
		float s, t;
		Closest_func::get_closestP_two_segment(
			nearext_vertex, boxseg_g,
			capsule_s.position - capsule_s.y_axis * capsule_s.hight, capsule_s.position + capsule_s.y_axis * capsule_s.hight,
			box_pos, capsule_pos,
			s, t
		);

		const Vector3 BoxN = (capsule_pos - box_pos).unit_vect();


		//obb����capsule�ւ̏Փ˖@��
		const Vector3 Wn = vector3_quatrotate(BoxN, box->world_orientation());

		Vector3 p0 = Vector3(0, capsule_s.hight, 0) * (t * 2 - 1) + vector3_quatrotate(-Wn, capsule->world_orientation().inverse()) * capsule->world_scale().x;
		Vector3 Debbbb = capsule_pos + BoxN * capsule->world_scale().x;
		Debbbb = capsule_pos - BoxN * capsule->world_scale().x;
		Debbbb.x = (capsule_pos - box_pos).norm_sqr();


		{

			Vector3 box_halfsize = box->world_scale();

			//box�̍��W�n�Ōv�Z
			Vector3 closest_box, closest_cap;
			float capsule_t = FLT_MAX;

			//box���W�n�ł�capsule�̏��
			//Vector3 cuppos_boxcoord = vector3_quatrotate(capsule->world_position() - box->world_position(), box->world_orientation().inverse());
			Vector3 cupsca_boxcoord = vector3_quatrotate(Vector3(0, capsule->world_scale().y, 0), capsule->world_orientation() * box->world_orientation().inverse());

			{
				float tmax;
				is_crossing = Crossing_func::getCrossingP_AABB_ray(
					Vector3(0, 0, 0), box_halfsize,
					cuppos_boxcoord, cupsca_boxcoord,
					capsule_t, tmax
				);
				if (is_crossing) {
					capsule_t = ALClamp(capsule_t, -1, 1);
					closest_cap = cuppos_boxcoord + cupsca_boxcoord * capsule_t;
				}
			}
			//TODO:�ʂƐ����̍ŋߓ_�ɕύX����!!!

			//������Box�ƌ������Ă��Ȃ�������Box��Capsule�̍ŋߓ_�����߂�
			if (is_crossing == false)
			{
				float dis_save = FLT_MAX;

				//box�̂��ׂĂ̕ӂƃJ�v�Z���̐����̑�������ōŋߓ_�����߂�
				Vector3 vertex[4] = {
					{+box_halfsize.x, +box_halfsize.y, +box_halfsize.z },
					{-box_halfsize.x, +box_halfsize.y, -box_halfsize.z },
					{-box_halfsize.x, -box_halfsize.y, +box_halfsize.z },
					{+box_halfsize.x, -box_halfsize.y, -box_halfsize.z }
				};
				for (int i = 0; i < 4; i++) {
					Vector3 closest_P, closest_Q;
					float s, t;
					Closest_func::get_closestP_two_segment(vertex[i], Vector3(-vertex[i].x, +vertex[i].y, +vertex[i].z), cuppos_boxcoord - cupsca_boxcoord, cuppos_boxcoord + cupsca_boxcoord, closest_P, closest_Q, s, t);
					if ((closest_P - closest_Q).norm() < dis_save) {
						dis_save = (closest_P - closest_Q).norm();
						closest_box = closest_P;
						closest_cap = closest_Q;
						capsule_t = t * 2 - 1;

						if (isnan(capsule_t) || isinf(capsule_t)) {
							int dafsdgf = 0;
						}
					}
					Closest_func::get_closestP_two_segment(vertex[i], Vector3(+vertex[i].x, -vertex[i].y, +vertex[i].z), cuppos_boxcoord - cupsca_boxcoord, cuppos_boxcoord + cupsca_boxcoord, closest_P, closest_Q, s, t);
					if ((closest_P - closest_Q).norm() < dis_save) {
						dis_save = (closest_P - closest_Q).norm();
						closest_box = closest_P;
						closest_cap = closest_Q;
						capsule_t = t * 2 - 1;

						if (isnan(capsule_t) || isinf(capsule_t)) {
							int dafsdgf = 0;
						}
					}
					Closest_func::get_closestP_two_segment(vertex[i], Vector3(+vertex[i].x, +vertex[i].y, -vertex[i].z), cuppos_boxcoord - cupsca_boxcoord, cuppos_boxcoord + cupsca_boxcoord, closest_P, closest_Q, s, t);
					if ((closest_P - closest_Q).norm() < dis_save) {
						dis_save = (closest_P - closest_Q).norm();
						closest_box = closest_P;
						closest_cap = closest_Q;
						capsule_t = t * 2 - 1;

						if (isnan(capsule_t) || isinf(capsule_t)) {
							int dafsdgf = 0;
						}
					}
				}
			}

			//�Փ˂��Ă��Ȃ����false��Ԃ�
			if (
				abs(closest_cap.x) - capsule->world_scale().x > box_halfsize.x ||
				abs(closest_cap.y) - capsule->world_scale().x > box_halfsize.y ||
				abs(closest_cap.z) - capsule->world_scale().x > box_halfsize.z
				) return false;

			//box��̍ŋߓ_
			closest_box = closest_cap;
			if (closest_cap.x > +box_halfsize.x)closest_box.x = +box_halfsize.x;
			if (closest_cap.x < -box_halfsize.x)closest_box.x = -box_halfsize.x;

			if (closest_cap.y > +box_halfsize.y)closest_box.y = +box_halfsize.y;
			if (closest_cap.y < -box_halfsize.y)closest_box.y = -box_halfsize.y;

			if (closest_cap.z > +box_halfsize.z)closest_box.z = +box_halfsize.z;
			if (closest_cap.z < -box_halfsize.z)closest_box.z = -box_halfsize.z;

			float leng = capsule->world_scale().x - (closest_box - closest_cap).norm_sqr(); //�ђʗ�
			if (leng < FLT_EPSILON)return false;

			Vector3 ns = (closest_cap - closest_box).unit_vect(); //box����sphere�ւ̃x�N�g�� boxcoord
			Vector3 Wns = vector3_quatrotate(ns, box->world_orientation()); //worldcoord

			//
			is_AC = true;
			ACpenetration = leng;
			ACnormal = -Wns;
			ACcontact_pointA = closest_box;
			ACcontact_pointB = (Vector3(0, capsule->world_scale().y, 0) * capsule_t) + vector3_quatrotate(-Wn, capsule->world_orientation().inverse()) * capsule->world_scale().x;

			if (isnan(ACcontact_pointA.norm() + ACcontact_pointB.norm())) {
				int adsfgh = 0;
			}
		}


		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = -Wn;
		ACcontact_pointA = box_pos;
		ACcontact_pointB = p0;


	}

	//obb�̕ӂ�capsule���Փ˂����ꍇ
	else if (smallest_case == SAT_TYPE::EDGE_EDGE) {
		Vector3 boxN = -vector3_cross(obb.u_axes[smallest_axis[0]], capsule_s.y_axis);	//box���W�n�ł̏Փ˖@�� obx����capsule
		if (vector3_dot(capsule_s.position, boxN) < 0)boxN *= -1;

		//box�̏Փː���
		Vector3 box_seg[2];
		box_seg[0] = obb.half_width;
		{
			if (boxN.x < 0) box_seg[0].x = -box_seg[0].x;
			if (boxN.y < 0) box_seg[0].y = -box_seg[0].y;
			if (boxN.z < 0) box_seg[0].z = -box_seg[0].z;
		}
		box_seg[1] = box_seg[0];
		box_seg[1][smallest_axis[0]] *= -1;

		float s, t;
		Closest_func::get_closestP_two_segment(
			box_seg[0], box_seg[1],
			cuppos_boxcoord - cupYaxis_boxcoord, cuppos_boxcoord + cupYaxis_boxcoord,
			s, t
		);

		Vector3 Wn = vector3_quatrotate(boxN, box->world_orientation());
		Vector3 cupsuleN = vector3_quatrotate(Wn, capsule->world_orientation().inverse());

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = -Wn;
		ACcontact_pointA = box_seg[0] + (box_seg[1] - box_seg[0]) * s;
		ACcontact_pointB = Vector3(0, capsule->world_scale().y, 0) * (t - 0.5f) * 2 + -cupsuleN * capsule->world_scale().x;

	}
	else assert(0);

	if (is_AC)
	{

		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		if (pair->body[0]->get_shape_tag() == box->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);

	}
	return is_AC;
#else
	//const std::list<ALP_Collider>::iterator& capsule = capsule_mesh->get_ALPcollider();
		//const std::list<ALP_Collider>::iterator& box = box_part->get_ALPcollider();

		//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;


	Vector3 box_halfsize = box->world_scale();

	//box�̍��W�n�Ōv�Z
	Vector3 closest_box, closest_cap;
	float capsule_t = FLT_MAX;

	//box���W�n�ł�capsule�̏��
	Vector3 cuppos_boxcoord = vector3_quatrotate(capsule->world_position() - box->world_position(), box->world_orientation().inverse());
	Vector3 cupsca_boxcoord = vector3_quatrotate(Vector3(0, capsule->world_scale().y, 0), capsule->world_orientation() * box->world_orientation().inverse());

	{
		float tmax;
		is_crossing = Crossing_func::getCrossingP_AABB_ray(
			Vector3(0, 0, 0), box_halfsize,
			cuppos_boxcoord, cupsca_boxcoord,
			capsule_t, tmax
		);
		if (is_crossing) {
			capsule_t = ALClamp(capsule_t, -1, 1);
			closest_cap = cuppos_boxcoord + cupsca_boxcoord * capsule_t;
		}
	}
	//TODO:�ʂƐ����̍ŋߓ_�ɕύX����!!!

	//������Box�ƌ������Ă��Ȃ�������Box��Capsule�̍ŋߓ_�����߂�
	if (is_crossing == false)
	{
		float dis_save = FLT_MAX;

		//box�̂��ׂĂ̕ӂƃJ�v�Z���̐����̑�������ōŋߓ_�����߂�
		Vector3 vertex[4] = {
			{+box_halfsize.x, +box_halfsize.y, +box_halfsize.z },
			{-box_halfsize.x, +box_halfsize.y, -box_halfsize.z },
			{-box_halfsize.x, -box_halfsize.y, +box_halfsize.z },
			{+box_halfsize.x, -box_halfsize.y, -box_halfsize.z }
		};
		for (int i = 0; i < 4; i++) {
			Vector3 closest_P, closest_Q;
			float s, t;
			Closest_func::get_closestP_two_segment(vertex[i], Vector3(-vertex[i].x, +vertex[i].y, +vertex[i].z), cuppos_boxcoord - cupsca_boxcoord, cuppos_boxcoord + cupsca_boxcoord, closest_P, closest_Q, s, t);
			if ((closest_P - closest_Q).norm() < dis_save) {
				dis_save = (closest_P - closest_Q).norm();
				closest_box = closest_P;
				closest_cap = closest_Q;
				capsule_t = t * 2 - 1;
			}
			Closest_func::get_closestP_two_segment(vertex[i], Vector3(+vertex[i].x, -vertex[i].y, +vertex[i].z), cuppos_boxcoord - cupsca_boxcoord, cuppos_boxcoord + cupsca_boxcoord, closest_P, closest_Q, s, t);
			if ((closest_P - closest_Q).norm() < dis_save) {
				dis_save = (closest_P - closest_Q).norm();
				closest_box = closest_P;
				closest_cap = closest_Q;
				capsule_t = t * 2 - 1;
			}
			Closest_func::get_closestP_two_segment(vertex[i], Vector3(+vertex[i].x, +vertex[i].y, -vertex[i].z), cuppos_boxcoord - cupsca_boxcoord, cuppos_boxcoord + cupsca_boxcoord, closest_P, closest_Q, s, t);
			if ((closest_P - closest_Q).norm() < dis_save) {
				dis_save = (closest_P - closest_Q).norm();
				closest_box = closest_P;
				closest_cap = closest_Q;
				capsule_t = t * 2 - 1;
			}
		}
	}

	//�Փ˂��Ă��Ȃ����false��Ԃ�
	if (
		abs(closest_cap.x) - capsule->world_scale().x > box_halfsize.x ||
		abs(closest_cap.y) - capsule->world_scale().x > box_halfsize.y ||
		abs(closest_cap.z) - capsule->world_scale().x > box_halfsize.z
		) return false;

	//box��̍ŋߓ_
	closest_box = closest_cap;
	if (closest_cap.x > +box_halfsize.x)closest_box.x = +box_halfsize.x;
	if (closest_cap.x < -box_halfsize.x)closest_box.x = -box_halfsize.x;

	if (closest_cap.y > +box_halfsize.y)closest_box.y = +box_halfsize.y;
	if (closest_cap.y < -box_halfsize.y)closest_box.y = -box_halfsize.y;

	if (closest_cap.z > +box_halfsize.z)closest_box.z = +box_halfsize.z;
	if (closest_cap.z < -box_halfsize.z)closest_box.z = -box_halfsize.z;

	float leng = capsule->world_scale().x - (closest_box - closest_cap).norm_sqr(); //�ђʗ�
	if (leng < FLT_EPSILON)return false;

	Vector3 n = (closest_cap - closest_box).unit_vect(); //box����sphere�ւ̃x�N�g�� boxcoord
	Vector3 Wn = vector3_quatrotate(n, box->world_orientation()); //worldcoord

	//
	is_AC = true;
	ACpenetration = leng;
	ACnormal = -Wn;
	ACcontact_pointA = closest_box;
	ACcontact_pointB = (Vector3(0, capsule->world_scale().y, 0) * capsule_t) + vector3_quatrotate(-Wn, capsule->world_orientation().inverse()) * capsule->world_scale().x;
	//vector3_quatrotate(vector3_quatrotate(closest_cap - n * capsule->world_scale().x, box->world_orientation().conjugate()) + box->world_position() - capsule->world_position(), capsule->world_orientation().conjugate())


	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		if (pair->body[0]->get_shape_tag() == box->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);

	}
	return is_AC;
#endif

}
#pragma endregion //TODO:�Ȃ����o�O��

#pragma region BOX-MESH

struct Crossing_struct {
	float ACpenetration = 0;
	Vector3 ACnormal = Vector3(0);
	Vector3 ACcontact_pointA = Vector3(0);
	Vector3 ACcontact_pointB = Vector3(0);
};

bool Physics_function::generate_contact_box_mesh(const Collider_shape* box, const Collider_shape* mesh, Contacts::Contact_pair*& pair, bool& is_crossing) {
	//const std::list<ALP_Collider>::iterator& box = box_part->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator& mesh = mesh_part->get_ALPcollider();

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	if (0 && mesh->get_mesh_data()->is_Convex == true) {}
	else {
		//box��mesh�̔���
		Matrix33 m;
		m = box->world_orientation().get_rotate_matrix();
		OBB_struct obb;
		obb.world_position = box->world_position();
		obb.u_axes[0].x = m._11; obb.u_axes[0].y = m._12; obb.u_axes[0].z = m._13;
		obb.u_axes[1].x = m._21; obb.u_axes[1].y = m._22; obb.u_axes[1].z = m._23;
		obb.u_axes[2].x = m._31; obb.u_axes[2].y = m._32; obb.u_axes[2].z = m._33;
		obb.half_width = box->world_scale();
		obb.world_orientation = box->world_orientation();
		assert(!isnan(obb.world_orientation.norm()));

		//const Vector3 box_relative_movement_from_triangle = +(box->world_position() - box->old_world_position()) - (mesh->world_position() - mesh->old_world_position());
		//const Vector3 triangle_relative_movement_from_box = -(box->world_position() - box->old_world_position()) + (mesh->world_position() - mesh->old_world_position());


		Triangle_struct smallest_triangle;
		float smallest_penetration = FLT_MAX;
		int smallest_axis[2] = { -1,-1 };
		SAT_TYPE smallest_case = SAT_TYPE::EDGE_EDGE;


		Triangle_struct triangle;
		triangle.world_position = mesh->world_position();
		triangle.world_orientation = mesh->world_orientation();
		triangle.world_scale = mesh->world_scale();

		std::vector<Crossing_struct> crossing_structs;
		for (const auto& faset : mesh->get_mesh_data()->facets) {

			auto& vertices = mesh->get_mesh_data()->vertices;
			triangle.vertex_position[0] = (vertices)[faset.vertexID[0]];
			triangle.vertex_position[1] = (vertices)[faset.vertexID[1]];
			triangle.vertex_position[2] = (vertices)[faset.vertexID[2]];

			triangle.world_vertex_position[0] = vector3_quatrotate(triangle.vertex_position[0] * triangle.world_scale, triangle.world_orientation);
			triangle.world_vertex_position[1] = vector3_quatrotate(triangle.vertex_position[1] * triangle.world_scale, triangle.world_orientation);
			triangle.world_vertex_position[2] = vector3_quatrotate(triangle.vertex_position[2] * triangle.world_scale, triangle.world_orientation);

			triangle.Edge_num[0] = faset.edgeID[0];
			triangle.Edge_num[1] = faset.edgeID[1];
			triangle.Edge_num[2] = faset.edgeID[2];

			triangle.normal = faset.normal;
			triangle.world_normal = vector3_quatrotate(triangle.normal, triangle.world_orientation);

			assert(!isnan(triangle.normal.norm()));

			float penetration = FLT_MAX;	//�ŏ��߂荞�ݗ�
			int axis[2];	//�ŏ��߂荞�ݗʂ𓾂��������̍쐬�Ɏg�p�����eOBB�̃��[�J�����ԍ� �Ӂ~�ӗp��2��
			SAT_TYPE s_case;	//�Փ˂̎��
			//assert(!(isnan((obb.u_axes[0] + obb.u_axes[1] + obb.u_axes[2]).norm())));
			if (!sat_obb_Triangle(obb, triangle, mesh->get_mesh_data(), penetration, axis, s_case)) continue;

			if (!sat_obb_Triangle(obb, triangle, mesh->get_mesh_data(), penetration, axis, s_case)) continue;

			if (0) {
				if (smallest_case == SAT_TYPE::EDGE_EDGE ||
					s_case != SAT_TYPE::EDGE_EDGE) {

					if ((
						smallest_case == SAT_TYPE::EDGE_EDGE &&
						s_case != SAT_TYPE::EDGE_EDGE //Edge�̏Փ˂̗D��x��������
						) || (
							smallest_penetration > penetration //��ԒZ���Փ�
							)) {

						smallest_triangle = triangle;
						smallest_penetration = penetration;
						smallest_axis[0] = axis[0];
						smallest_axis[1] = axis[1];
						smallest_case = s_case;
					}
				}
			}
			else {
				smallest_triangle = triangle;
				smallest_penetration = penetration;
				smallest_axis[0] = axis[0];
				smallest_axis[1] = axis[1];
				smallest_case = s_case;
			}

			if (smallest_axis[0] == -1 && smallest_axis[1] == -1) continue;
			//}
			//{
			//if (smallest_axis[0] == -1 && smallest_axis[1] == -1) return false;


				//obbB�̒��_��obbA�̖ʂƏՓ˂����ꍇ
			if (smallest_case == SAT_TYPE::POINTB_FACETA)
			{
				Vector3 Wn = obb.u_axes[smallest_axis[0]]; //�Փ˖@��
				Wn *= smallest_axis[1]; //�Փ˖@����mesh����obb�̕�����
				Wn = Wn.unit_vect();

				//triangle�̃��[�J�����W�n�ł̍ŋߓ_(p1) triangle��3���_�̂����̂ǂꂩ
				float save_dis = -FLT_MAX;
				Vector3 p1; //mesh�̍��W�n

				if (vector3_dot(smallest_triangle.world_vertex_position[0], +Wn) > save_dis) { save_dis = vector3_dot(smallest_triangle.world_vertex_position[0], +Wn);  p1 = smallest_triangle.vertex_position[0]; };
				if (vector3_dot(smallest_triangle.world_vertex_position[1], +Wn) > save_dis) { save_dis = vector3_dot(smallest_triangle.world_vertex_position[1], +Wn);  p1 = smallest_triangle.vertex_position[1]; };
				if (vector3_dot(smallest_triangle.world_vertex_position[2], +Wn) > save_dis) { save_dis = vector3_dot(smallest_triangle.world_vertex_position[2], +Wn);  p1 = smallest_triangle.vertex_position[2]; };
				p1 *= smallest_triangle.world_scale;

				//boxA�̋t�s��̍쐬
				Matrix44 rotate, inverse_rotate;
				rotate = box->world_orientation().get_rotate_matrix();
				rotate._41 = box->world_position().x; //transpse�̓���
				rotate._42 = box->world_position().y;
				rotate._43 = box->world_position().z;
				rotate._44 = 1;
				inverse_rotate = matrix_inverse(rotate);

				//p1��obbA�̃��[�J�����W�n��
				Vector3 Wp1 = vector3_quatrotate(p1, smallest_triangle.world_orientation) + smallest_triangle.world_position;
				//Vector3 P = vector3_quatrotate(Wp1, box->world_orientation()) + box->world_position();
				Vector3 c = vector3_trans(Wp1, inverse_rotate);

				//obbA�̍ŋߓ_�����߂�
				Vector3 p0 = c;
				Vector3 box_halfsize = box->world_scale();
				if (c.x > +box_halfsize.x)p0.x = +box_halfsize.x;
				if (c.x < -box_halfsize.x)p0.x = -box_halfsize.x;

				if (c.y > +box_halfsize.y)p0.y = +box_halfsize.y;
				if (c.y < -box_halfsize.y)p0.y = -box_halfsize.y;

				if (c.z > +box_halfsize.z)p0.z = +box_halfsize.z;
				if (c.z < -box_halfsize.z)p0.z = -box_halfsize.z;

				p0 += vector3_quatrotate(-Wn, obb.world_orientation.inverse()) * smallest_penetration;

				Crossing_struct crossing;
				is_AC = true;
				crossing.ACpenetration = smallest_penetration;
				crossing.ACnormal = Wn;
				crossing.ACcontact_pointA = p0;
				crossing.ACcontact_pointB = p1;
				crossing_structs.emplace_back(crossing);

			}

			//obb�̒��_��triangle�̖ʂƏՓ˂����ꍇ
			else if (smallest_case == SAT_TYPE::POINTA_FACETB)
			{
				Vector3 Wn;
				if (smallest_axis[1] == 3)
				{
					Wn = vector3_quatrotate(smallest_triangle.normal, smallest_triangle.world_orientation); //mesh����box�Ɍ����Ă̖@��(B����A)
				}
				else
				{
					Wn = vector3_quatrotate(vector3_cross(smallest_triangle.vertex_position[smallest_axis[1]], smallest_triangle.vertex_position[(smallest_axis[1] + 1) % 3]), smallest_triangle.world_orientation);

					if (vector3_dot(Wn, smallest_triangle.vertex_position[smallest_axis[1]]) < vector3_dot(Wn, smallest_triangle.vertex_position[(smallest_axis[1] + 2) % 3]))Wn *= -1;
				}
				Wn = Wn.unit_vect();

				Vector3 p0 = obb.half_width;
				//obb��triangle�̈ʒu�֌W(d)���ڐG�_(p)�����߂�
				if (vector3_dot(obb.u_axes[0], Wn) > 0) p0.x = -p0.x;
				if (vector3_dot(obb.u_axes[1], Wn) > 0) p0.y = -p0.y;
				if (vector3_dot(obb.u_axes[2], Wn) > 0) p0.z = -p0.z;

				//p0��obbB�̃��[�J�����W�n��
				Vector3 WP = vector3_quatrotate(p0, box->world_orientation()) + box->world_position();
				Vector3 c = vector3_quatrotate(WP - mesh->world_position(), mesh->world_orientation().inverse());

				//obbB�̍ŋߓ_�����߂�
				Vector3 p1;
				if (0) {
					//mesh�Ɠ_�̍ŋߓ_�����߂���@
					c /= mesh->world_scale();
					Closest_func::get_closestP_point_triangle(
						c,
						smallest_triangle.vertex_position[0],
						smallest_triangle.vertex_position[1],
						smallest_triangle.vertex_position[2],
						smallest_triangle.normal,
						p1
					);

					p1 *= mesh->world_scale();
				}
				else {
					//�@���Ƃ߂荞�ݗʂ��狁�߂���@ //�������̂ق������|�I�ɑ��� ���Ǐ����s���m
					p1 = c;
					p1 += Wn * smallest_penetration;
				}

				Crossing_struct crossing;
				is_AC = true;
				crossing.ACpenetration = smallest_penetration;
				crossing.ACnormal = Wn;
				crossing.ACcontact_pointA = p0;
				crossing.ACcontact_pointB = p1;
				crossing_structs.emplace_back(crossing);
			}
			//�Bobb0�̕ӂ�obb1�̕ӂƏՓ˂����ꍇ
			else if (smallest_case == SAT_TYPE::EDGE_EDGE)
			{
				//Vector3 d = obbB.world_position - obbA.world_position;

				const Vector3& tri_vertex_p0 = mesh->get_mesh_data()->vertices[mesh->get_mesh_data()->edges[smallest_triangle.Edge_num[smallest_axis[1]]].vertexID[0]];
				const Vector3& tri_vertex_p1 = mesh->get_mesh_data()->vertices[mesh->get_mesh_data()->edges[smallest_triangle.Edge_num[smallest_axis[1]]].vertexID[1]];

				const Vector3& world_tri_vertex_p0 = vector3_quatrotate(tri_vertex_p0 * triangle.world_scale, triangle.world_orientation);
				const Vector3& world_tri_vertex_p1 = vector3_quatrotate(tri_vertex_p1 * triangle.world_scale, triangle.world_orientation);

				Vector3 triangle_edge_dir = (tri_vertex_p0 - tri_vertex_p1).unit_vect();
				Vector3 world_triangle_edge_dir = vector3_quatrotate(triangle_edge_dir, smallest_triangle.world_orientation);

				Vector3 Wn;
				Wn = vector3_cross(obb.u_axes[smallest_axis[0]], world_triangle_edge_dir);
				if (vector3_dot(Wn, box->world_position() - (world_tri_vertex_p0 + smallest_triangle.world_position)) < 0)	//
				{
					Wn = Wn * -1.0f;
				}
				Wn = Wn.unit_vect();

				Vector3 p[2] = { obb.half_width ,tri_vertex_p0 };
				{
					if (vector3_dot(obb.u_axes[0], Wn) > 0) p[0].x = -p[0].x;
					if (vector3_dot(obb.u_axes[1], Wn) > 0) p[0].y = -p[0].y;
					if (vector3_dot(obb.u_axes[2], Wn) > 0) p[0].z = -p[0].z;
				}

				//world���W�n��edge��edge�̍ŋߓ_�����߂�
				Vector3 P0a = vector3_quatrotate(p[0], obb.world_orientation) + obb.world_position;
				Vector3 P1a = vector3_quatrotate(p[1] * smallest_triangle.world_scale, smallest_triangle.world_orientation) + smallest_triangle.world_position;

				float s, t; //�ŋߓ_�����߂邱�Ƃ̂ł���s,t�����߂�
				Closest_func::get_closestP_two_line(
					P0a, obb.u_axes[smallest_axis[0]],
					P1a, world_triangle_edge_dir * smallest_triangle.world_scale,
					s, t
				);

				//obb��local���W�n�ł̎��̌���
				Vector3 b_axis[3]{
					Vector3(1,0,0),
					Vector3(0,1,0),
					Vector3(0,0,1)
				};

				Crossing_struct crossing;
				is_AC = true;
				crossing.ACpenetration = smallest_penetration;
				crossing.ACnormal = Wn;
				crossing.ACcontact_pointA = p[0] + s * b_axis[smallest_axis[0]];
				crossing.ACcontact_pointB = (p[1] + t * triangle_edge_dir) * smallest_triangle.world_scale;
				crossing_structs.emplace_back(crossing);
			}

			else assert(0);

		}

		if (is_AC)
		{
			is_crossing = true;
			if (pair->check_oncoll_only == true) return false;

			float smallest_penetrate = FLT_MAX;
			Vector3 smallest_axis = Vector3(0);
			for (const auto& crossing : crossing_structs) {
				if (smallest_penetrate > crossing.ACpenetration) {
					smallest_penetrate = crossing.ACpenetration;
					smallest_axis = crossing.ACnormal;
				}
			}

			for (const auto& crossing : crossing_structs) {
				if (vector3_dot(crossing.ACnormal, smallest_axis) < 0)continue;

				//oncoll_enter�݂̂̏ꍇ������return

				if (pair->body[0]->get_shape_tag() == box->get_shape_tag())
					pair->contacts.addcontact(
						crossing.ACpenetration,
						crossing.ACnormal,
						crossing.ACcontact_pointA,
						crossing.ACcontact_pointB
					);
				else
					pair->contacts.addcontact(
						crossing.ACpenetration,
						-crossing.ACnormal,
						crossing.ACcontact_pointB,
						crossing.ACcontact_pointA
					);
			}


		}


		return is_AC;




		return true;
	}
}
#pragma endregion


#pragma region Capsule-Capsule
bool Physics_function::generate_contact_capsule_capsule(const Collider_shape* capsuleA, const Collider_shape* capsuleB, Contacts::Contact_pair*& pair, bool& is_crossing) {
	//const std::list<ALP_Collider>::iterator& capsuleA = SA->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator& capsuleB = SB->get_ALPcollider();

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	Vector3 Avec_Wco = vector3_quatrotate(Vector3(0, 1, 0), capsuleA->world_orientation()) * capsuleA->world_scale().y;
	Vector3 Bvec_Wco = vector3_quatrotate(Vector3(0, 1, 0), capsuleB->world_orientation()) * capsuleB->world_scale().y;

	float t, s;
	Closest_func::get_closestP_two_segment(
		capsuleA->world_position() - Avec_Wco,
		capsuleA->world_position() + Avec_Wco,
		capsuleB->world_position() - Bvec_Wco,
		capsuleB->world_position() + Bvec_Wco,
		s, t
	);

	Vector3 pA_Wco = capsuleA->world_position() + Avec_Wco * (s * 2 - 1);
	Vector3 pB_Wco = capsuleB->world_position() + Bvec_Wco * (t * 2 - 1);

	//pB ���� pA�@�����ւ̖@�� Acoord
	Vector3 Wn = pA_Wco - pB_Wco;
	float length = Wn.norm_sqr();
	Wn = Wn.unit_vect();

	if (length < capsuleA->world_scale().x + capsuleB->world_scale().x) {

		//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐��p�ɏ�������
		is_AC = true;
		ACpenetration = capsuleA->world_scale().x + capsuleB->world_scale().x - length;
		ACnormal = Wn;
		ACcontact_pointA = Vector3(0, 1, 0) * capsuleA->world_scale().y * (s * 2 - 1) + capsuleA->world_scale().x * vector3_quatrotate(-Wn, capsuleA->world_orientation().inverse());
		ACcontact_pointB = Vector3(0, 1, 0) * capsuleB->world_scale().y * (t * 2 - 1) + capsuleB->world_scale().x * vector3_quatrotate(+Wn, capsuleB->world_orientation().inverse());
	}


	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		if (pair->body[0]->get_shape_tag() == capsuleA->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);
	}
	return is_AC;
}
#pragma endregion

#pragma region Capsule-Mesh
bool Physics_function::generate_contact_capsule_mesh(const Collider_shape* capsule, const Collider_shape* mesh, Contacts::Contact_pair*& pair, bool& is_crossing) {
	//const std::list<ALP_Collider>::iterator& capsule = capsule_mesh->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator& mesh = mesh_part->get_ALPcollider();

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	Vector3 ACnormal;
	Vector3 ACcontact_pointA;
	Vector3 ACcontact_pointB;

	if (0 && mesh->get_mesh_data()->is_Convex == true) {
		////����mesh�̏Փ˔�����s��
		//Matrix rotate, inverse_rotate;
		//rotate = matrix_world(Vector3(1, 1, 1), mesh->world_orientation().get_rotate_matrix(), mesh->world_position());
		//inverse_rotate = matrix_inverse(rotate);

		//Vector3 center;
		//center = vector3_trans(sphere->world_position(), inverse_rotate); //mesh��local���W�n�ł̋��̒��S���W

		////mesh��̍ŋߓ_
		//Vector3 closest_point;

		//closest_point = center;
		////�e�ʂ̊O�ɂ���Ζʕ��ʂɎ����Ă���
		//for (u_int i = 0; i < mesh_part->mesh->facet_num; i++) {
		//	const Vector3& nor = mesh_part->mesh->facets.at(i).normal.unit_vect();
		//	const Vector3& pos = mesh_part->mesh->vertices.at(mesh_part->mesh->facets.at(i).vertexID[0]) * mesh->world_scale();
		//	float d = vector3_dot(nor, pos) - vector3_dot(nor, closest_point);
		//	if (d < 0)
		//		closest_point += d * nor;
		//}

		//float distance = (closest_point - center).norm_sqr(); //�ŋߓ_�Ƌ����S�̋���
		//if (sphere->world_scale().x - distance > FLT_EPSILON) { //float�덷������
		//	Vector3 n = (sphere->world_position() - vector3_trans(closest_point, rotate)).unit_vect(); //mesh����sphere�ւ̃x�N�g��

		//	if (pair->body[0]->get_ALPcollider()->shape == mesh->shape) {
		//		//body[0]�@���@mesh
		//		//body[1]�@���@sphere
		//		pair->contacts.addcontact(
		//			sphere->world_scale().x - distance,
		//			-n,
		//			closest_point,
		//			sphere->world_scale().x * vector3_quatrotate(n, sphere->world_orientation().conjugate())
		//		);
		//	}
		//	else {
		//		//body[0]�@���@sphere
		//		//body[1]�@���@mesh
		//		pair->contacts.addcontact(
		//			sphere->world_scale().x - distance,
		//			n,
		//			sphere->world_scale().x * vector3_quatrotate(n, sphere->world_orientation().conjugate()),
		//			closest_point
		//		);
		//	}
		//	return true;
		//}
	}
	else {
		//����mesh�̏Փ˔�����s�� �����䂪�ނƖʓ|�Ȃ̂�scale��world�̂܂܂�
		Vector3 capsule_pos_meshcoord = vector3_quatrotate(capsule->world_position() - mesh->world_position(), mesh->world_orientation().inverse()); //mesh���W�n�ł�sphere��pos
		Vector3 capsule_dir_meshcoord = vector3_quatrotate(Vector3(0, 1, 0), capsule->world_orientation() * mesh->world_orientation().inverse()) * capsule->world_scale().y; //mesh���W�n�ł�sphere��pos

		float min_dis = capsule->world_scale().x;//�Œ዗����sphere�̔��a��
		Vector3 closest_point; //mesh��̍ŋߓ_
		bool is_hit = false; //�Փ˂��Ă�����true
		Vector3 nor_meshcoord; //�Փ˓_�ł̖@��
		float min_t; //�Փ˓_��t
		Vector3 min_sphere_pos_meshcoord; //�Փ˓_��capsule��sphere��pos
		//capsule��mesh�̔���
		for (const auto& faset : mesh->get_mesh_data()->facets) {
			const Vector3 nor = (faset.normal * mesh->world_scale()).unit_vect();
			const Vector3 mesh_pos0 = mesh->get_mesh_data()->vertices.at(faset.vertexID[0]) * mesh->world_scale();
			//mesh���ʂ�"d"
			float dis = vector3_dot(nor, mesh_pos0);

			float Crossing_t;

			//capsule������̍ŋߓ_�������������߂� ���m�ł͂Ȃ����ߋ��߂Ȃ���
			Crossing_func::getCrossingP_plane_line(
				nor,
				dis,
				capsule_pos_meshcoord,
				capsule_dir_meshcoord,
				Crossing_t, false
			);

			Crossing_t = ALClamp(Crossing_t, -1, 1);

			Vector3 sphere_pos_meshcoord = capsule_pos_meshcoord + capsule_dir_meshcoord * Crossing_t;
			float dis_sp = vector3_dot(nor, sphere_pos_meshcoord);

			//sphere��Mesh�̗��ɂ��� �܂��� mesh���ʂ�sphere�̋�����min_dis���傫����ΏՓ˂��Ȃ�
			if (dis_sp - dis < 0 || dis_sp - dis > min_dis) continue;

			const Vector3 mesh_pos1 = mesh->get_mesh_data()->vertices.at(faset.vertexID[1]) * mesh->world_scale();
			const Vector3 mesh_pos2 = mesh->get_mesh_data()->vertices.at(faset.vertexID[2]) * mesh->world_scale();

			Vector3 closest_p;
			Closest_func::get_closestP_segment_triangle(
				capsule_pos_meshcoord - capsule_dir_meshcoord, capsule_pos_meshcoord + capsule_dir_meshcoord,
				mesh_pos0, mesh_pos1, mesh_pos2, nor,
				Crossing_t, closest_p
			);

			Crossing_t = Crossing_t * 2 - 1;
			Crossing_t = ALClamp(Crossing_t, -1, 1);

			sphere_pos_meshcoord = capsule_pos_meshcoord + capsule_dir_meshcoord * Crossing_t;

			if ((sphere_pos_meshcoord - closest_p).norm() > min_dis * min_dis) continue;

			//min_dis,�ŋߓ_�̍X�V
			closest_point = closest_p;
			min_dis = (sphere_pos_meshcoord - closest_point).norm_sqr();
			nor_meshcoord = nor;
			min_t = Crossing_t;
			min_sphere_pos_meshcoord = sphere_pos_meshcoord;
			is_hit = true;

		}
		//�������Ă��Ȃ����false��Ԃ�
		if (is_hit == false)return false;

		Vector3 Wn = vector3_quatrotate((min_sphere_pos_meshcoord - closest_point), mesh->world_orientation()).unit_vect();//mesh����sphere�ւ̃x�N�g��
		//if (vector3_dot(min_sphere_pos_meshcoord - closest_point, nor_meshcoord) < 0)Wn *= -1; //�o�O�̌���

		is_AC = true;
		ACpenetration = capsule->world_scale().x - min_dis;
		ACnormal = -Wn;
		ACcontact_pointA = closest_point;
		ACcontact_pointB = Vector3(0, capsule->world_scale().y, 0) * min_t + capsule->world_scale().x * vector3_quatrotate(-Wn, capsule->world_orientation().inverse());
	}

	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		if (pair->body[0]->get_shape_tag() == mesh->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal,
				ACcontact_pointA,
				ACcontact_pointB
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal,
				ACcontact_pointB,
				ACcontact_pointA
			);
	}
	return is_AC;
}
#pragma endregion


#pragma region MESH-PLANE
bool Physics_function::generate_contact_mesh_plane(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair*& pair, bool& is_crossing) {
	return true;
}
#pragma endregion

#pragma region MESH-MESH

bool GC_concave_mesh_mesh(const ALP_Collider& S1, const Collider_shape* S2, int concave_num, Contacts::Contact_pair*& pair, bool& is_crossing) {
	return false;
}

bool Physics_function::generate_contact_mesh_mesh(const Collider_shape* meshA, const Collider_shape* meshB, Contacts::Contact_pair*& pair, bool& is_crossing) {
	//const std::list<ALP_Collider>::iterator& meshA = mA->get_ALPcollider();
	//const std::list<ALP_Collider>::iterator& meshB = mB->get_ALPcollider();

	float smallest_penetration = FLT_MAX;	//�ŏ��߂荞�ݗ�
	int smallest_facetID[2];	//�ŏ��߂荞�ݗʂ𓾂��������̍쐬�Ɏg�p�����eOBB�̃��[�J�����ԍ� �Ӂ~�ӗp��2��
	SAT_TYPE smallest_case;		//�Փ˂̎��

	//�ǂ����convex��object�̏ꍇ
	if (meshA->get_mesh_data()->is_Convex == true && meshB->get_mesh_data()->is_Convex == true) {
		if (!sat_convex_mesh_mesh(meshA, meshB, smallest_penetration, smallest_facetID, smallest_case))return false;

		Quaternion offset_quatBA = meshB->world_orientation() * meshA->world_orientation().inverse();
		Quaternion offset_quatAB = meshA->world_orientation() * meshB->world_orientation().inverse();
		Vector3 offset_posBA = meshB->world_position() - meshA->world_position();
		Vector3 offset_posAB = meshA->world_position() - meshB->world_position();

		//SB�̒��_��SA�̖ʂƏՓ˂����ꍇ
		if (smallest_case == SAT_TYPE::POINTB_FACETA)
		{
			const Collider_shape* facet_shape = meshA;
			const Collider_shape* vertex_shape = meshB;

			//const std::list<ALP_Collider>::iterator& facet_mesh = facet_mesh->get_ALPcollider();
			//const std::list<ALP_Collider>::iterator& vertex_mesh = vertex_mesh->get_ALPcollider();

			Quaternion offset_quatVF = vertex_shape->world_orientation() * facet_shape->world_orientation().inverse();
			Quaternion offset_quatFV = facet_shape->world_orientation() * vertex_shape->world_orientation().inverse();
			Vector3 offset_posVF = vertex_shape->world_position() - facet_shape->world_position();
			Vector3 offset_posFV = facet_shape->world_position() - vertex_shape->world_position();

			assert(smallest_facetID[1] == -1);
			const Facet& nerest_facet = facet_shape->get_mesh_data()->facets.at(smallest_facetID[0]);

			Vector3 axisF = nerest_facet.normal.unit_vect();
			Vector3 axisW = vector3_quatrotate(axisF, facet_shape->world_orientation()).unit_vect();
			if (vector3_dot(axisW, offset_posAB) < 0) {
				axisF = -axisF;
				axisW = -axisW;
			}

			//vertex_mesh�̂ǂ̒��_���ŋߓ_�����߂�
			u_int nearest_pointID;
			Vector3 pB;
			{
				float max_len = -FLT_MAX;
				Vector3 axisV = vector3_quatrotate(axisF, offset_quatFV);

				for (u_int v_num = 0; v_num < vertex_shape->get_mesh_data()->vertex_num; v_num++) {

					if (vector3_dot(vertex_shape->get_mesh_data()->vertices.at(v_num), axisV) > max_len) {
						max_len = vector3_dot(vertex_shape->get_mesh_data()->vertices.at(v_num), axisV);
						nearest_pointID = v_num;
					}
				}
			}
			pB = vertex_shape->get_mesh_data()->vertices.at(nearest_pointID);
			pB *= vertex_shape->world_scale();

			//��L��p0��facet_mesh�̍ŋߖʏ�̂ǂ��ɂ���̂�
			Vector3 pA;
			{

				Vector3 p = vector3_quatrotate(vector3_quatrotate(pB, vertex_shape->world_orientation()) + offset_posVF, facet_shape->world_orientation().inverse());
				p /= facet_shape->world_scale();
				float min_len = FLT_MAX;
				Vector3 n_pos;


				for (u_int f_num = 0; f_num < facet_shape->get_mesh_data()->facet_num; f_num++) {

					if (vector3_dot(facet_shape->get_mesh_data()->facets.at(f_num).normal, -axisF) < 0.5)continue; //�Փ˖@���Ɣ�ׂ�

					//���b�V���Ɠ_�̍ŋߓ_�����߂�
					get_closestP_point_triangle(p,
						facet_shape->get_mesh_data()->vertices.at(facet_shape->get_mesh_data()->facets.at(f_num).vertexID[0]),
						facet_shape->get_mesh_data()->vertices.at(facet_shape->get_mesh_data()->facets.at(f_num).vertexID[1]),
						facet_shape->get_mesh_data()->vertices.at(facet_shape->get_mesh_data()->facets.at(f_num).vertexID[2]),
						facet_shape->get_mesh_data()->facets.at(f_num).normal,
						n_pos
					);

					if ((p - n_pos).norm() < min_len) {
						pA = n_pos;
						min_len = (p - n_pos).norm();
					}
				}

				pA *= facet_shape->world_scale();
			}

			pair->contacts.addcontact(
				smallest_penetration,
				axisW,
				pA,
				pB
			);
		}

		//SA�̒��_��SB�̖ʂƏՓ˂����ꍇ
		if (smallest_case == SAT_TYPE::POINTA_FACETB)
		{
			const Collider_shape* facet_mesh = meshB;
			const Collider_shape* vertex_mesh = meshA;

			//const std::list<ALP_Collider>::iterator& facet_mesh = facet_mesh->get_ALPcollider();
			//const std::list<ALP_Collider>::iterator& vertex_mesh = vertex_mesh->get_ALPcollider();

			Quaternion offset_quatVF = vertex_mesh->world_orientation() * facet_mesh->world_orientation().inverse();
			Quaternion offset_quatFV = facet_mesh->world_orientation() * vertex_mesh->world_orientation().inverse();
			Vector3 offset_posVF = vertex_mesh->world_position() - facet_mesh->world_position();
			Vector3 offset_posFV = facet_mesh->world_position() - vertex_mesh->world_position();

			assert(smallest_facetID[0] == -1);
			const Facet& nerest_facet = facet_mesh->get_mesh_data()->facets.at(smallest_facetID[1]);

			Vector3 axisF = nerest_facet.normal.unit_vect();
			Vector3 axisW = vector3_quatrotate(axisF, facet_mesh->world_orientation()).unit_vect();
			if (vector3_dot(axisW, offset_posBA) < 0) {
				axisF = -axisF;
				axisW = -axisW;
			}

			//vertex_mesh�̂ǂ̒��_���ŋߓ_�����߂�
			u_int nearest_pointID = 0;
			Vector3 pB;
			{
				float max_len = -FLT_MAX;
				Vector3 axisV = vector3_quatrotate(axisF, offset_quatFV);

				for (u_int v_num = 0; v_num < vertex_mesh->get_mesh_data()->vertex_num; v_num++) {

					if (vector3_dot(vertex_mesh->get_mesh_data()->vertices.at(v_num), axisV) > max_len) {
						max_len = vector3_dot(vertex_mesh->get_mesh_data()->vertices.at(v_num), axisV);
						nearest_pointID = v_num;
					}
				}
			}
			pB = vertex_mesh->get_mesh_data()->vertices.at(nearest_pointID);
			pB *= vertex_mesh->world_scale();

			//��L��pB��facet_mesh�̍ŋߖʏ�̂ǂ��ɂ���̂�
			Vector3 pA;
			{

				Vector3 p = vector3_quatrotate(vector3_quatrotate(pB, vertex_mesh->world_orientation()) + offset_posVF, facet_mesh->world_orientation().inverse());
				p /= facet_mesh->world_scale();
				float min_len = FLT_MAX;
				Vector3 n_pos;


				for (u_int f_num = 0; f_num < facet_mesh->get_mesh_data()->facet_num; f_num++) {

					if (vector3_dot(facet_mesh->get_mesh_data()->facets.at(f_num).normal, -axisF) < 0.5)continue; //�Փ˖@���Ɣ�ׂ�

					//���b�V���Ɠ_�̍ŋߓ_�����߂�
					get_closestP_point_triangle(p,
						facet_mesh->get_mesh_data()->vertices.at(facet_mesh->get_mesh_data()->facets.at(f_num).vertexID[0]),
						facet_mesh->get_mesh_data()->vertices.at(facet_mesh->get_mesh_data()->facets.at(f_num).vertexID[1]),
						facet_mesh->get_mesh_data()->vertices.at(facet_mesh->get_mesh_data()->facets.at(f_num).vertexID[2]),
						facet_mesh->get_mesh_data()->facets.at(f_num).normal,
						n_pos
					);

					if ((p - n_pos).norm() < min_len) {
						pA = n_pos;
						min_len = (p - n_pos).norm();
					}
				}

				pA *= facet_mesh->world_scale();
			}

			pair->contacts.addcontact(
				smallest_penetration,
				-axisW,
				pB,
				pA
			);
		}


		//SA��SB�̕ӓ��m���Փ˂����ꍇ
		else if (smallest_case == SAT_TYPE::EDGE_EDGE)
		{
			Quaternion offset_quatAB = meshA->world_orientation() * meshB->world_orientation().inverse();
			Quaternion offset_quatBA = meshB->world_orientation() * meshA->world_orientation().inverse();
			Vector3 offset_posAB = meshA->world_position() - meshB->world_position();
			Vector3 offset_posBA = meshB->world_position() - meshA->world_position();

			const Edge& edgeA = meshA->get_mesh_data()->edges.at(smallest_facetID[0]);
			const Edge& edgeB = meshB->get_mesh_data()->edges.at(smallest_facetID[1]);

			Vector3 edgeA_p[2] = {
				{meshA->get_mesh_data()->vertices.at(edgeA.vertexID[0]) * meshA->world_scale()},
				{meshA->get_mesh_data()->vertices.at(edgeA.vertexID[1]) * meshA->world_scale()}
			};
			Vector3 edgeB_p[2] = {
				{meshB->get_mesh_data()->vertices.at(edgeB.vertexID[0]) * meshB->world_scale()},
				{meshB->get_mesh_data()->vertices.at(edgeB.vertexID[1]) * meshB->world_scale()}
			};

			Vector3 edgeA_vec = (edgeA_p[0] - edgeA_p[1]).unit_vect();
			Vector3 edgeB_vec = (edgeB_p[0] - edgeB_p[1]).unit_vect();

			//SB�̏���SA�̍��W�n�Ɏ����Ă���
			//Vector3 edgeB_p_A = vector3_quatrotate(edgeB_p[0], offset_quatBA) + offset_posBA;
			Vector3 edgeB_p_A = vector3_quatrotate(vector3_quatrotate(edgeB_p[0], meshB->world_orientation()) + offset_posBA, meshA->world_orientation().inverse());
			Vector3 edgeB_v_A = vector3_quatrotate(edgeB_vec, offset_quatBA);

			//SA�̍��W�n��axis�̐���
			Vector3 axisA = vector3_cross(edgeA_vec, edgeB_v_A);
			axisA = axisA.unit_vect();

			//axis��world���W�n��
			Vector3 axisW = vector3_quatrotate(axisA, meshA->world_orientation());

			//axis�̌�����SB->SA�̌�����
			if (vector3_dot(axisW, offset_posAB) < 0)
			{
				axisA = axisA * -1.0f;
				axisW = axisW * -1.0f;
			}

			//SA�̍��W�n�Ɏ����Ă��Ē����ƒ����̍ŋߓ_���l��
			float s, t;
			Closest_func::get_closestP_two_line(
				edgeA_p[0], edgeA_vec,
				edgeB_p_A, edgeB_v_A,
				s, t
			);
			pair->contacts.addcontact(
				smallest_penetration,
				axisW,
				edgeA_p[0] + s * edgeA_vec,
				edgeB_p[0] + t * edgeB_vec
			);
		}

	}
	else {
		return false;
	}




	return true;
}

#pragma endregion

#pragma endregion
//:::::::::::::::::::::::::::

#endif