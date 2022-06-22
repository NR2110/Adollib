#include "ALP_generate_contact.h"

#include "collider_shape.h"
#include "ALP__meshcoll_data.h"
#include "ALP_contact.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

using namespace Closest_func;

#ifdef USE_SIM

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
			//if (shapeB->get_shape_tag() == ALPCollider_shape_type::Plane)	generate_contact_sphere_plane(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Mesh)	generate_contact_sphere_mesh(shapeA, shapeB, pair, is_crossing);
		}
		if (shapeA->get_shape_tag() == ALPCollider_shape_type::BOX) {

			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Sphere)generate_contact_sphere_box(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::BOX)	generate_contact_box_box(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Capsule)generate_contact_box_capsule(shapeA, shapeB, pair, is_crossing);
			//if (shapeB->get_shape_tag() == ALPCollider_shape_type::Plane)	generate_contact_box_plane(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Mesh)	generate_contact_box_mesh(shapeA, shapeB, pair, is_crossing);
		}
		if (shapeA->get_shape_tag() == ALPCollider_shape_type::Capsule) {

			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Sphere)generate_contact_sphere_capsule(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::BOX)generate_contact_box_capsule(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Capsule)generate_contact_capsule_capsule(shapeA, shapeB, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Mesh)generate_contact_capsule_mesh(shapeA, shapeB, pair, is_crossing);
		}
		//if (shapeA->get_shape_tag() == ALPCollider_shape_type::Plane) {

		//	if (shapeB->get_shape_tag() == ALPCollider_shape_type::Sphere)generate_contact_sphere_plane(shapeB, shapeA, pair, is_crossing);
		//	if (shapeB->get_shape_tag() == ALPCollider_shape_type::BOX)	generate_contact_box_plane(shapeB, shapeA, pair, is_crossing);
		//	if (shapeB->get_shape_tag() == ALPCollider_shape_type::Plane) {}
		//}
		if (shapeA->get_shape_tag() == ALPCollider_shape_type::Mesh) {

			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Sphere)generate_contact_sphere_mesh(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::Capsule)generate_contact_capsule_mesh(shapeB, shapeA, pair, is_crossing);
			if (shapeB->get_shape_tag() == ALPCollider_shape_type::BOX)	generate_contact_box_mesh(shapeB, shapeA, pair, is_crossing);
			//	if (shapeB->get_shape_tag() == ALPCollider_shape_type::Plane)	generate_contact_mesh_plane(shapeA, shapeB, pair, is_crossing);
			//	if (shapeB->get_shape_tag() == ALPCollider_shape_type::Mesh)	generate_contact_mesh_mesh(shapeA, shapeB, pair, is_crossing);
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
	DirectX::XMVECTOR world_position; //���S���W
	DirectX::XMVECTOR u_axes[3]; //���̌���
	DirectX::XMVECTOR world_orientation;
	float half_width[4]; //�����Ƃ̕ӂ̒���
};

struct Capsule_struct {
	DirectX::XMVECTOR world_position;
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR y_axis;
	float hight = 0;
	float r = 0;
};

struct Triangle_struct {
	DirectX::XMVECTOR world_position; //triangle�̒��S���W
	DirectX::XMVECTOR world_orientation;
	DirectX::XMVECTOR world_scale;
	DirectX::XMVECTOR normal; //triangle�̖@��(����)
	DirectX::XMVECTOR world_normal;
	DirectX::XMVECTOR vertex_position[3]; //triangle��local�Ȓ��_���W
	DirectX::XMVECTOR world_vertex_position[3]; //triangle��world�Ȓ��_���W

	int Edge_num[3] = { 0,0,0 };
};

//���ɓ��e��������
float sum_of_projected_radii(const OBB_struct& obb, const DirectX::XMVECTOR& vec) {
	using namespace DirectX;

	return
		XMVectorGetX(
			XMVectorAbs(XMVector3Dot(vec, XMVectorScale(obb.u_axes[0], obb.half_width[0]))) +
			XMVectorAbs(XMVector3Dot(vec, XMVectorScale(obb.u_axes[1], obb.half_width[1]))) +
			XMVectorAbs(XMVector3Dot(vec, XMVectorScale(obb.u_axes[2], obb.half_width[2])))
		);
}

float sum_of_projected_radii(const Triangle_struct& tri, const DirectX::XMVECTOR& Wvec, float& min, float& max) {

	float A = DirectX::XMVector3Dot(Wvec, tri.world_vertex_position[0]).m128_f32[0];
	float B = DirectX::XMVector3Dot(Wvec, tri.world_vertex_position[1]).m128_f32[0];
	float C = DirectX::XMVector3Dot(Wvec, tri.world_vertex_position[2]).m128_f32[0];

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
	using namespace DirectX;

	smallest_penetration = FLT_MAX;

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;


	float penetration = 0; //�ђʗ�
	float ra, rb; //obbA,obbB��L�ɓ��e���ꂽ����
	XMVECTOR axis; //���e���鎲
	XMVECTOR distBtoA = obbA.world_position - obbB.world_position; //2obb�̒��S���W�̋���

	//::: obbA�̎���obbB�𓊉e :::::::::::::::::::
	for (int i = 0; i < 3; i++)
	{
		axis = obbA.u_axes[i];
		ra = fabsf(obbA.half_width[i]);
		rb = fabsf(sum_of_projected_radii(obbB, axis));

		assert(ra > 0 && rb > 0);

		penetration = ra + rb - fabsf(XMVectorGetX((XMVector3Dot(axis, distBtoA))));
		if (penetration < 0) return false;

		if (fabsf(XMVectorGetX(XMVector3Dot(axis, distBtoA))) + rb < ra) {
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

			penetration = ra + rb - fabsf(XMVectorGetX((XMVector3Dot(axis, distBtoA))));
			if (penetration < 0) return false;

			if (fabsf(XMVectorGetX(XMVector3Dot(axis, distBtoA))) + ra < rb) {
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

			axis = XMVector3Cross(obbA.u_axes[OB1], obbB.u_axes[OB2]);
			if (XMVectorGetX(XMVector3LengthSq(axis)) <= FLT_EPSILON * FLT_EPSILON)continue;//�O�ς� 0 = ���s

			axis = XMVector3Normalize(axis);
			if (XMVectorGetX(XMVector3LengthSq(axis)) < 1) axis *= (1 + FLT_EPSILON);

			ra = fabsf(sum_of_projected_radii(obbA, axis));
			rb = fabsf(sum_of_projected_radii(obbB, axis));
			//if (vector3_dot(axis, distBtoA) + rb < ra)continue;

			penetration = ra + rb - fabsf(XMVectorGetX(XMVector3Dot(axis, distBtoA)));
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
	using namespace DirectX;
	smallest_penetration = FLT_MAX;

	Ret_S PB_FA, PA_FB;

	float penetration = 0; //�ђʗ�
	float ra; //obb,triangle��L�ɓ��e���ꂽ����
	XMVECTOR axis; //���e���鎲
	const XMVECTOR offset_tri_obb = triangle.world_position - obb.world_position; //triangle����obb�ւ�offset

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
		const float off = XMVector3Dot(axis, offset_tri_obb).m128_f32[0];

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

		XMVECTOR triangle_world_normal = XMVector3Rotate(triangle.normal, triangle.world_orientation);
		for (int i = 0; i < 3; i++)
		{
			//�ӂƖ@���̊O�ςŕ��������s��
			axis = XMVector3Cross(triangle_world_normal, XMVector3Normalize((triangle.world_vertex_position[i] - triangle.world_vertex_position[(i + 1) % 3])));
			if (XMVector3Greater(XMVector3Dot(axis, triangle.world_position), XMVector3Dot(axis, triangle.world_vertex_position[i])))
				axis *= -1;

			if (XMVector3Greater(XMVector3Dot(axis, triangle.world_vertex_position[i]), XMVector3Dot(axis, triangle.world_vertex_position[(i + 2) % 3]))) axis *= -1; //axis��triangle�ӂ��璆�S�Ɍ�����

			//obb�̎������̒���
			ra = fabsf(sum_of_projected_radii(obb, axis));

			//triangle�̎������̊e����
			float trimin, trimax;
			sum_of_projected_radii(triangle, axis, trimin, trimax);

			//�����̎������ւ̒���
			const float off = XMVector3Dot(axis, offset_tri_obb).m128_f32[0];

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

			const float off = XMVector3Dot(axis, offset_tri_obb).m128_f32[0];

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

			const XMVECTOR& tri_vertex_p0 = XMLoadFloat3(&mesh->vertices[mesh->edges[triangle.Edge_num[OB2]].vertexID[0]]);
			const XMVECTOR& tri_vertex_p1 = XMLoadFloat3(&mesh->vertices[mesh->edges[triangle.Edge_num[OB2]].vertexID[1]]);

			const XMVECTOR& world_tri_vertex_p0 = XMVector3Rotate(tri_vertex_p0 * triangle.world_scale, triangle.world_orientation);
			const XMVECTOR& world_tri_vertex_p1 = XMVector3Rotate(tri_vertex_p1 * triangle.world_scale, triangle.world_orientation);


			axis = XMVector3Cross(obb.u_axes[OB1], XMVector3Normalize(world_tri_vertex_p0 - world_tri_vertex_p1));
			if (XMVector2LengthSq(axis).m128_f32[0] <= FLT_EPSILON * FLT_EPSILON)continue;//�O�ς� 0 = ���s

			//mesh����obb�̌�����axis��ύX(������Փ˂��Ȃ��悤�ɂ��邽��)
			if (XMVector3Dot(axis, obb.world_position - (world_tri_vertex_p0 + triangle.world_position)).m128_f32[0] < 0)//�߂荞��ł��邱�Ƃ�Y�ꂸ��!!!!
			{
				axis = axis * -1.0f;
			}
			axis = XMVector3Normalize(axis);
			if (XMVector3LengthSq(axis).m128_f32[0] <= 1) axis *= (1 - FLT_EPSILON);



			//Vector3 obb_vertex_pos;
			//if (vector3_dot(triangle.world_vertex_position[OB2],axis) < vector3_dot(obb.))

			ra = fabsf(sum_of_projected_radii(obb, axis));

			//triangle�̎������̊e����
			float trimin, trimax;
			sum_of_projected_radii(triangle, axis, trimin, trimax);

			//�����̎������ւ̒���
			const float off = XMVector3Dot(axis, offset_tri_obb).m128_f32[0];

			//���ɓ��e�����ђʗʂ����߂�
			penetration = FLT_MAX;
			//penetration = ALmin(penetration, ra - (trimin + off));  //������͏Փ˂��Ȃ�
			penetration = ALmin(penetration, (trimax + off) - (-ra));

			if (penetration < 0) return false; //�������Ă�

			if (XMVector3Dot(axis, triangle.world_normal).m128_f32[0] < 0)continue; //������͏Փ˂��Ȃ�
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
	using namespace DirectX;

	smallest_penetration = FLT_MAX;

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;


	float penetration = 0; //�ђʗ�
	XMVECTOR axis; //���e���鎲
	XMVECTOR distCapsuleToObb = capsule.position; //2obb�̒��S���W�̋���

	//box�̎���cupsule�𓊉e
	for (int i = 0; i < 3; i++) {

		const XMVECTOR axis = XMLoadFloat3(&DOP::AABB_axis[i]);

		float ra = obb.half_width[i];
		float rb = fabsf(XMVectorGetX(XMVector3Dot(axis, capsule.y_axis))) * capsule.hight + capsule.r;

		assert(ra > 0 && rb > 0);

		float penetration = ra + rb - abs(XMVectorGetX(XMVector3Dot(axis, distCapsuleToObb)));
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
		XMVECTOR nearext_vertex = XMVectorSet(obb.half_width[0], obb.half_width[1], obb.half_width[2], 0);
		XMVECTOR capsule_nearest_pos;
		//obb�̒��S����capsule�֐����ȃx�N�g�����Ƃ�Aobb�̍ŋߒ��_�����߂�
		Closest_func_SIM::get_closestP_point_segment(obb.world_position, capsule.position - capsule.y_axis, capsule.position + capsule.y_axis, capsule_nearest_pos);
		if (capsule_nearest_pos.m128_f32[0] < 0)nearext_vertex.m128_f32[0] *= -1;
		if (capsule_nearest_pos.m128_f32[1] < 0)nearext_vertex.m128_f32[1] *= -1;
		if (capsule_nearest_pos.m128_f32[2] < 0)nearext_vertex.m128_f32[2] *= -1;

		//box�̕ӂ�capusle��y���Ő����̍ŋߓ_�����
		for (int i = 0; i < 3; i++) {
			XMVECTOR boxseg_g = nearext_vertex; //box����(nearext_vertex ~ boxseg_g)
			boxseg_g.m128_f32[i] *= -1;

			//box�̕�,capsuleY���̍ŋߓ_�����߁A�����������߂�
			XMVECTOR box_pos, capsule_pos;
			Closest_func_SIM::get_closestP_two_segment(
				nearext_vertex, boxseg_g,
				capsule.position - capsule.y_axis * capsule.hight, capsule.position + capsule.y_axis * capsule.hight,
				box_pos, capsule_pos
			);

			const XMVECTOR axis = XMVector3Normalize(capsule_pos - box_pos);

			float ra =
				fabsf(obb.half_width[0] * XMVector3Dot(axis, XMLoadFloat3(&DOP::AABB_axis[0])).m128_f32[0]) +
				fabsf(obb.half_width[1] * XMVector3Dot(axis, XMLoadFloat3(&DOP::AABB_axis[1])).m128_f32[0]) +
				fabsf(obb.half_width[2] * XMVector3Dot(axis, XMLoadFloat3(&DOP::AABB_axis[2])).m128_f32[0]);
			float rb = fabsf(XMVector3Dot(axis, capsule.y_axis).m128_f32[0] * capsule.hight) + capsule.r;

			float penetration = ra + rb - fabsf(XMVector3Dot(axis, distCapsuleToObb).m128_f32[0]);
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

/*
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

*/
#pragma endregion

//�Փː���
#pragma region SPHERE-SPHERE
bool Physics_function::generate_contact_sphere_sphere(const Collider_shape* SA, const Collider_shape* SB, Contacts::Contact_pair*& pair, bool& is_crossing) {
	using namespace DirectX;

	DirectX::XMVECTOR pA = DirectX::XMLoadFloat3(&SA->world_position());
	DirectX::XMVECTOR pB = DirectX::XMLoadFloat3(&SB->world_position());

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	DirectX::XMVECTOR ACnormal;
	DirectX::XMVECTOR ACcontact_pointA;
	DirectX::XMVECTOR ACcontact_pointB;

	//p1 ���� p0�@�����ւ̖@��
	DirectX::XMVECTOR n = pA - pB;
	float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(n));
	n = DirectX::XMVector3Normalize(n);

	if (length < SA->world_scale().x + SB->world_scale().x) {
		DirectX::XMVECTOR quat_A = DirectX::XMLoadFloat4(&SA->world_orientation().inverse());
		DirectX::XMVECTOR quat_B = DirectX::XMLoadFloat4(&SB->world_orientation().inverse());

		//DirectX::XMVector3Rotate
		//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐��p�ɏ�������
		is_AC = true;
		ACpenetration = SA->world_scale().x + SB->world_scale().x - length;
		ACnormal = n;
		ACcontact_pointA = DirectX::XMVectorScale(DirectX::XMVector3Rotate(n, quat_A), -SA->world_scale().x);
		ACcontact_pointB = DirectX::XMVectorScale(DirectX::XMVector3Rotate(n, quat_B), +SB->world_scale().x);
	}

	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇaddcontact������return
		if (pair->check_oncoll_only == true) return false;

		Vector3 ACnormal_;
		Vector3 ACcontact_pointA_;
		Vector3 ACcontact_pointB_;

		DirectX::XMStoreFloat3(&ACnormal_, ACnormal);
		DirectX::XMStoreFloat3(&ACcontact_pointA_, ACcontact_pointA);
		DirectX::XMStoreFloat3(&ACcontact_pointB_, ACcontact_pointB);

		pair->contacts.addcontact(
			ACpenetration,
			ACnormal_,
			ACcontact_pointA_,
			ACcontact_pointB_
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
	Quaternion box_rotate, box_inverse_rotate;
	Vector3 box_position;
	box_rotate = box->world_orientation();
	box_inverse_rotate = box_rotate.inverse();
	box_position = box->world_position();
	//rotate = matrix_world(Vector3(1, 1, 1), box->world_orientation().get_rotate_matrix(), box->world_position());
	//inverse_rotate = matrix_inverse(rotate);

	Vector3 center;
	center = vector3_quatrotate(sphere->world_position() - box_position, box_inverse_rotate); //box��local���W�n�ł̋��̒��S���W

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
			if (box_halfsize[i] - center[i] < min_value) {
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


	Vector3 n = (sphere->world_position() - (vector3_quatrotate(closest_point, box_rotate) + box_position)).unit_vect(); //box����sphere�ւ̃x�N�g��
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
	using namespace DirectX;

	const XMVECTOR sphere_wpos = XMLoadFloat3(&sphere->world_position());
	const XMVECTOR capsule_wpos = XMLoadFloat3(&capsule->world_position());

	const XMVECTOR capsule_worient = XMLoadFloat4(&capsule->world_orientation());
	const XMVECTOR capsule_worient_inv = XMLoadFloat4(&capsule->world_orientation().inverse());
	const XMVECTOR sphere_worient_inv = XMLoadFloat4(&sphere->world_orientation().inverse());



	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	XMVECTOR ACnormal;
	XMVECTOR ACcontact_pointA;
	XMVECTOR ACcontact_pointB;

	//capsule�̍��W�n�Ōv�Z����(scale�͕ύX���Ȃ�)

	//capsule coord
	const DirectX::XMVECTOR sphere_pos_capcoord = XMVector3Rotate(sphere_wpos - capsule_wpos, capsule_worient_inv);

	// sphere�̒��S�_��capsule�̒����̍ŋߓ_�����߂�
	float s;
	Vector3 y1 = Vector3(0, 1, 0);
	XMVECTOR y1_SIM = XMLoadFloat3(&y1);
	Closest_func_SIM::get_closestP_point_line(sphere_pos_capcoord, XMVectorZero(), y1_SIM, s);
	//s = XMVectorGetX(XMVector3Dot(y1_SIM, sphere_pos_capcoord - XMVectorZero()) / XMVector3Dot(y1_SIM, y1_SIM));
	s = ALmax(ALmin(s, +capsule->world_scale().y), -capsule->world_scale().y); //capsule�̒����ɃN�����v


	//capsule�̍��W�n��sphere-sphere�̔�����s��
	XMVECTOR pA = sphere_pos_capcoord;
	XMVECTOR pB = XMVectorScale(y1_SIM, s);

	//p1 ���� p0�@�����ւ̖@��
	XMVECTOR n = pA - pB;
	float length = XMVectorGetX(XMVector3Length(n));
	n = n / length;

	if (length < sphere->world_scale().x + capsule->world_scale().x) {
		XMVECTOR Wn = XMVector3Rotate(n, capsule_worient); //n��capsule���烏�[���h��

		//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐��p�ɓ���
		is_AC = true;
		ACpenetration = sphere->world_scale().x + capsule->world_scale().x - length;
		ACnormal = +Wn;
		ACcontact_pointA = XMVectorScale(XMVector3Rotate(-Wn, sphere_worient_inv), sphere->world_scale().x);
		ACcontact_pointB = XMVectorScale(n, capsule->world_scale().x) + pB;

	}

	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		Vector3 ACnormal_;
		Vector3 ACcontact_pointA_;
		Vector3 ACcontact_pointB_;

		DirectX::XMStoreFloat3(&ACnormal_, ACnormal);
		DirectX::XMStoreFloat3(&ACcontact_pointA_, ACcontact_pointA);
		DirectX::XMStoreFloat3(&ACcontact_pointB_, ACcontact_pointB);

		if (pair->body[0]->get_shape_tag() == sphere->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal_,
				ACcontact_pointA_,
				ACcontact_pointB_
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal_,
				ACcontact_pointB_,
				ACcontact_pointA_
			);
	}
	return is_AC;
}
#pragma endregion

#pragma region SPHERE-MESH
bool Physics_function::generate_contact_sphere_mesh(const Collider_shape* sphere, const Collider_shape* mesh, Contacts::Contact_pair*& pair, bool& is_crossing) {
	using namespace DirectX;

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	XMVECTOR ACnormal;
	XMVECTOR ACcontact_pointA;
	XMVECTOR ACcontact_pointB;

	if (false && mesh->get_mesh_data()->is_Convex == true) {
		////����mesh�̏Փ˔�����s��
		//Matrix44 rotate, inverse_rotate;
		//rotate = matrix_world(Vector3(1, 1, 1), mesh->world_orientation().get_rotate_matrix(), mesh->world_position());
		//inverse_rotate = matrix_inverse(rotate);

		//Vector3 center;
		//center = vector3_trans(sphere->world_position(), inverse_rotate); //mesh��local���W�n�ł̋��̒��S���W

		////mesh��̍ŋߓ_
		//Vector3 closest_point;

		//closest_point = center;
		////�e�ʂ̊O�ɂ���Ζʕ��ʂɎ����Ă���
		//for (u_int i = 0; i < mesh->get_mesh_data()->facet_num; i++) {
		//	const Vector3& nor = mesh->get_mesh_data()->facets.at(i).normal.unit_vect();
		//	const Vector3& pos = mesh->get_mesh_data()->vertices.at(mesh->get_mesh_data()->facets.at(i).vertexID[0]) * mesh->world_scale();
		//	float d = vector3_dot(nor, pos) - vector3_dot(nor, closest_point);
		//	if (d < 0)
		//		closest_point += d * nor;
		//}

		//float distance = (closest_point - center).norm_sqr(); //�ŋߓ_�Ƌ����S�̋���
		//if (sphere->world_scale().x - distance > FLT_EPSILON) { //float�덷������
		//	Vector3 n = (sphere->world_position() - vector3_trans(closest_point, rotate)).unit_vect(); //mesh����sphere�ւ̃x�N�g��

		//	//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐��p�ɓ���
		//	is_AC = true;
		//	ACpenetration = sphere->world_scale().x - distance;
		//	ACnormal = -n;
		//	ACcontact_pointA = closest_point;
		//	ACcontact_pointB = sphere->world_scale().x * vector3_quatrotate(n, sphere->world_orientation().inverse());
		//}

	}
	else {
		//����mesh�̏Փ˔�����s��

		const XMVECTOR& mesh_Wscale = XMLoadFloat3(&mesh->world_scale());
		const XMVECTOR& mesh_Wpos = XMLoadFloat3(&mesh->world_position());
		const XMVECTOR& mesh_Worient = XMLoadFloat4(&mesh->world_orientation());

		XMVECTOR sphere_pos_meshcoord = XMVector3Rotate(XMLoadFloat3(&(sphere->world_position() - mesh->world_position())), XMLoadFloat4(&mesh->world_orientation().inverse())); //mesh���W�n�ł�sphere��pos

		float min_dis = sphere->world_scale().x;//�Œ዗����sphere�̔��a��
		XMVECTOR closest_point; //mesh��̍ŋߓ_
		bool is_hit = false; //�Փ˂��Ă�����true
		XMVECTOR nor_meshcoord;

		//����mesh�̔���
		for (const auto& faset : mesh->get_mesh_data()->facets) {
			const XMVECTOR& faset_normal = XMLoadFloat3(&faset.normal);
			const XMVECTOR& faset_vertex0 = XMLoadFloat3(&mesh->get_mesh_data()->vertices.at(faset.vertexID[0]));
			const XMVECTOR& faset_vertex1 = XMLoadFloat3(&mesh->get_mesh_data()->vertices.at(faset.vertexID[1]));
			const XMVECTOR& faset_vertex2 = XMLoadFloat3(&mesh->get_mesh_data()->vertices.at(faset.vertexID[2]));

			const XMVECTOR& nor = XMVector3Normalize(XMVectorMultiply(faset_normal, mesh_Wscale));
			const XMVECTOR& mesh_pos0 = XMVectorMultiply(faset_vertex0, mesh_Wscale);

			//mesh���ʂ�"d"
			const float dis = XMVector3Dot(nor, mesh_pos0).m128_f32[0];
			const float dis_sp = XMVector3Dot(nor, sphere_pos_meshcoord).m128_f32[0];

			//mesh���ʂ�sphere�̋�����min_dis���傫����ΏՓ˂��Ȃ�
			if (fabsf(dis_sp - dis) > min_dis) continue;

			const XMVECTOR& mesh_pos1 = XMVectorMultiply(faset_vertex1, mesh_Wscale);
			const XMVECTOR& mesh_pos2 = XMVectorMultiply(faset_vertex2, mesh_Wscale);

			XMVECTOR closest_p;
			Closest_func_SIM::get_closestP_point_triangle(
				sphere_pos_meshcoord,
				mesh_pos0,
				mesh_pos1,
				mesh_pos2,
				nor,
				closest_p
			);

			if (XMVector3LengthSq(sphere_pos_meshcoord - closest_p).m128_f32[0] > min_dis * min_dis)continue; //�ۑ�����Ă��鋗�����傫�����continue

			//min_dis,�ŋߓ_�̍X�V
			closest_point = closest_p;
			min_dis = XMVector3Length(sphere_pos_meshcoord - closest_point).m128_f32[0];
			is_hit = true;
			nor_meshcoord = nor;
		}
		//�������Ă��Ȃ����false��Ԃ�
		if (is_hit == false)return false;

		XMVECTOR Wn = XMVector3Normalize(XMVector3Rotate((sphere_pos_meshcoord - closest_point), mesh_Worient));//mesh����sphere�ւ̃x�N�g��
		if (XMVector3Dot(sphere_pos_meshcoord - closest_point, nor_meshcoord).m128_f32[0] < 0)Wn *= -1;

		is_AC = true;
		ACpenetration = sphere->world_scale().x - min_dis;
		ACnormal = +Wn;
		ACcontact_pointA = XMVectorScale(XMVector3Rotate(-Wn, XMLoadFloat4(&sphere->world_orientation().inverse())), sphere->world_scale().x);
		ACcontact_pointB = closest_point;
	}

	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		Vector3 ACnormal_;
		Vector3 ACcontact_pointA_;
		Vector3 ACcontact_pointB_;

		DirectX::XMStoreFloat3(&ACnormal_, ACnormal);
		DirectX::XMStoreFloat3(&ACcontact_pointA_, ACcontact_pointA);
		DirectX::XMStoreFloat3(&ACcontact_pointB_, ACcontact_pointB);

		if (pair->body[0]->get_shape_tag() == sphere->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal_,
				ACcontact_pointA_,
				ACcontact_pointB_
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal_,
				ACcontact_pointB_,
				ACcontact_pointA_
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
	using namespace DirectX;

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	XMVECTOR ACnormal;
	XMVECTOR ACcontact_pointA;
	XMVECTOR ACcontact_pointB;

	Matrix33 m;
	m = boxA->world_orientation().get_rotate_matrix();
	OBB_struct obbA;
	obbA.world_position = XMLoadFloat3(&boxA->world_position());
	obbA.u_axes[0] = XMVectorSet(m._11, m._12, m._13, 0);
	obbA.u_axes[1] = XMVectorSet(m._21, m._22, m._23, 0);
	obbA.u_axes[2] = XMVectorSet(m._31, m._32, m._33, 0);
	obbA.half_width[0] = boxA->world_scale()[0]; obbA.half_width[1] = boxA->world_scale()[1]; obbA.half_width[2] = boxA->world_scale()[2];
	obbA.world_orientation = XMLoadFloat4(&boxA->world_orientation());

	m = boxB->world_orientation().get_rotate_matrix();
	OBB_struct obbB;
	obbB.world_position = XMLoadFloat3(&boxB->world_position());
	obbB.u_axes[0] = XMVectorSet(m._11, m._12, m._13, 0);
	obbB.u_axes[1] = XMVectorSet(m._21, m._22, m._23, 0);
	obbB.u_axes[2] = XMVectorSet(m._31, m._32, m._33, 0);
	obbB.half_width[0] = boxB->world_scale()[0]; obbB.half_width[1] = boxB->world_scale()[1]; obbB.half_width[2] = boxB->world_scale()[2];
	obbB.world_orientation = XMLoadFloat4(&boxB->world_orientation());

	float smallest_penetration = FLT_MAX;	//�ŏ��߂荞�ݗ�
	int smallest_axis[2];	//�ŏ��߂荞�ݗʂ𓾂��������̍쐬�Ɏg�p�����eOBB�̃��[�J�����ԍ� �Ӂ~�ӗp��2��
	SAT_TYPE smallest_case;	//�Փ˂̎��
	if (!sat_obb_obb(obbA, obbB, smallest_penetration, smallest_axis, smallest_case)) return false;


	//const XMVECTOR boxA_Wpos    = XMLoadFloat3(&boxA->world_position());
	//const XMVECTOR boxA_Worient = XMLoadFloat4(&boxA->world_orientation());
	//const XMVECTOR boxA_Wscale  = XMLoadFloat3(&boxA->world_scale());
	//const XMVECTOR boxB_Wpos    = XMLoadFloat3(&boxB->world_position());
	//const XMVECTOR boxB_Worient = XMLoadFloat4(&boxB->world_orientation());
	//const XMVECTOR boxB_Wscale  = XMLoadFloat3(&boxB->world_scale());

	//obbB�̒��_��obbA�̖ʂƏՓ˂����ꍇ
	if (smallest_case == SAT_TYPE::POINTB_FACETA)
	{
		XMVECTOR d = obbA.world_position - obbB.world_position;	//B����A����
		XMVECTOR Wn = obbA.u_axes[smallest_axis[0]];	//obbA�̏Փ˖ʂ̖@���ƕ��s��obbA�̃��[�J�����x�N�g��
		if (XMVector3Less(XMVector3Dot(Wn, d), XMVectorZero()))	//obbA��obbB�̈ʒu�֌W���Փ˖ʂ̖@���x�N�g�������肷��
		{
			Wn = -Wn;
		}
		Wn = XMVector3Normalize(Wn);

		//boxB�̃��[�J�����W�n�ł̍ŋߓ_(p1) obb1��8���_�̂����̂ǂꂩ
		float* p1f = obbB.half_width;	//obb1�̊e�ӂ̒����́Aobb1�̏d�S����ڐG�_(p)�ւ̑��Έʒu�̎肪����ɂȂ�
		//obb0��obb1�̈ʒu�֌W(d)���ڐG�_(p)�����߂�
		if (XMVector3Greater(XMVector3Dot(obbB.u_axes[0], -Wn), XMVectorZero())) p1f[0] = -p1f[0];
		if (XMVector3Greater(XMVector3Dot(obbB.u_axes[1], -Wn), XMVectorZero())) p1f[1] = -p1f[1];
		if (XMVector3Greater(XMVector3Dot(obbB.u_axes[2], -Wn), XMVectorZero())) p1f[2] = -p1f[2];
		XMVECTOR p1 = XMVectorSet(p1f[0], p1f[1], p1f[2], 0);

		//p1��obbA�̃��[�J�����W�n��
		XMVECTOR P = XMVector3Rotate(p1, obbB.world_orientation) + obbB.world_position;
		XMVECTOR c = XMVector3Rotate(P - obbA.world_position, XMQuaternionInverse(obbA.world_orientation));

		//obbA�̍ŋߓ_�����߂�
		Vector3 p0f;
		Vector3 cf;
		XMStoreFloat3(&p0f, c);
		XMStoreFloat3(&cf, c);
		Vector3 box_halfsize = boxA->world_scale();
		if (cf.x > +box_halfsize.x)p0f.x = +box_halfsize.x;
		if (cf.x < -box_halfsize.x)p0f.x = -box_halfsize.x;

		if (cf.y > +box_halfsize.y)p0f.y = +box_halfsize.y;
		if (cf.y < -box_halfsize.y)p0f.y = -box_halfsize.y;

		if (cf.z > +box_halfsize.z)p0f.z = +box_halfsize.z;
		if (cf.z < -box_halfsize.z)p0f.z = -box_halfsize.z;

		XMVECTOR p0 = XMLoadFloat3(&p0f);
		p0 = p0 + XMVectorScale(XMVector3Rotate(-Wn, XMQuaternionInverse(obbA.world_orientation)), smallest_penetration);

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = Wn;
		ACcontact_pointA = p0;
		ACcontact_pointB = p1;
	}
	//obbA�̒��_��obbB�̖ʂƏՓ˂����ꍇ
	else if (smallest_case == SAT_TYPE::POINTA_FACETB)
	{
		XMVECTOR d = obbA.world_position - obbB.world_position; //B����A����
		XMVECTOR Wn = obbB.u_axes[smallest_axis[1]];
		if (XMVector3Less(XMVector3Dot(Wn, d), XMVectorZero())) //B����A�����łȂ����-1��������
		{
			Wn = -Wn;
		}
		Wn = XMVector3Normalize(Wn);

		float* p0f = obbA.half_width;
		//obb0��obb1�̈ʒu�֌W(d)���ڐG�_(p)�����߂�
		if (XMVector3Greater(XMVector3Dot(obbA.u_axes[0], Wn), XMVectorZero())) p0f[0] = -p0f[0];
		if (XMVector3Greater(XMVector3Dot(obbA.u_axes[1], Wn), XMVectorZero())) p0f[1] = -p0f[1];
		if (XMVector3Greater(XMVector3Dot(obbA.u_axes[2], Wn), XMVectorZero())) p0f[2] = -p0f[2];
		XMVECTOR p0 = XMVectorSet(p0f[0], p0f[1], p0f[2], 0);

		//box0�̋t�s��̍쐬
		Matrix44 rotate, inverse_rotate;
		rotate = boxB->world_orientation().get_rotate_matrix();
		rotate._41 = boxB->world_position().x; //transpse�̓���
		rotate._42 = boxB->world_position().y;
		rotate._43 = boxB->world_position().z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p0��obb0�̃��[�J�����W�n��
		XMVECTOR P = XMVector3Rotate(p0, obbA.world_orientation) + obbA.world_position;
		XMVECTOR c = XMVector3Rotate(P - obbB.world_position, XMQuaternionInverse(obbB.world_orientation));

		//obb0�̍ŋߓ_�����߂�
		Vector3 p1f;
		Vector3 cf;
		XMStoreFloat3(&p1f, c);
		XMStoreFloat3(&cf, c);
		Vector3 box_halfsize = boxB->world_scale();
		if (cf.x > +box_halfsize.x)p1f.x = +box_halfsize.x;
		if (cf.x < -box_halfsize.x)p1f.x = -box_halfsize.x;

		if (cf.y > +box_halfsize.y)p1f.y = +box_halfsize.y;
		if (cf.y < -box_halfsize.y)p1f.y = -box_halfsize.y;

		if (cf.z > +box_halfsize.z)p1f.z = +box_halfsize.z;
		if (cf.z < -box_halfsize.z)p1f.z = -box_halfsize.z;

		XMVECTOR p1 = XMLoadFloat3(&p1f);
		p1 = p1 + XMVectorScale(XMVector3Rotate(Wn, XMQuaternionInverse(obbB.world_orientation)), smallest_penetration);

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = Wn;
		ACcontact_pointA = p0;
		ACcontact_pointB = p1;
	}
	//�Bobb0�̕ӂ�obb1�̕ӂƏՓ˂����ꍇ
	else if (smallest_case == SAT_TYPE::EDGE_EDGE)
	{

		XMVECTOR d = obbB.world_position - obbA.world_position;
		XMVECTOR n;
		n = XMVector3Cross(obbA.u_axes[smallest_axis[0]], obbB.u_axes[smallest_axis[1]]);
		n = XMVector3Normalize(n);
		if (XMVector3Less(XMVector3Dot(n, d), XMVectorZero()))
		{
			n = -n;
		}

		XMVECTOR p[2];
		{
			float* pf[2] = { obbA.half_width, obbB.half_width };
			if (XMVector3Greater(XMVector3Dot(obbA.u_axes[0], -n), XMVectorZero())) pf[0][0] = -pf[0][0];
			if (XMVector3Greater(XMVector3Dot(obbA.u_axes[1], -n), XMVectorZero())) pf[0][1] = -pf[0][1];
			if (XMVector3Greater(XMVector3Dot(obbA.u_axes[2], -n), XMVectorZero())) pf[0][2] = -pf[0][2];

			if (XMVector3Greater(XMVector3Dot(obbB.u_axes[0], +n), XMVectorZero())) pf[1][0] = -pf[1][0];
			if (XMVector3Greater(XMVector3Dot(obbB.u_axes[1], +n), XMVectorZero())) pf[1][1] = -pf[1][1];
			if (XMVector3Greater(XMVector3Dot(obbB.u_axes[2], +n), XMVectorZero())) pf[1][2] = -pf[1][2];

			p[0] = XMVectorSet(pf[0][0], pf[0][1], pf[0][2], 0);
			p[1] = XMVectorSet(pf[1][0], pf[1][1], pf[1][2], 0);
		}

		XMVECTOR P0a = XMVector3Rotate(p[0], obbA.world_orientation) + obbA.world_position;
		XMVECTOR P1a = XMVector3Rotate(p[1], obbB.world_orientation) + obbB.world_position;

		float s, t;
		Closest_func_SIM::get_closestP_two_line(
			P0a, obbA.u_axes[smallest_axis[0]],
			P1a, obbB.u_axes[smallest_axis[1]],
			s, t
		);
		XMVECTOR b_axis[3]{
			XMVectorSet(1,0,0,0),
			XMVectorSet(0,1,0,0),
			XMVectorSet(0,0,1,0)
		};

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = -n;
		ACcontact_pointA = p[0] + XMVectorScale(b_axis[smallest_axis[0]], s);
		ACcontact_pointB = p[1] + XMVectorScale(b_axis[smallest_axis[1]], t);
	}
	else assert(0);

	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		Vector3 ACnormal_;
		Vector3 ACcontact_pointA_;
		Vector3 ACcontact_pointB_;

		DirectX::XMStoreFloat3(&ACnormal_, ACnormal);
		DirectX::XMStoreFloat3(&ACcontact_pointA_, ACcontact_pointA);
		DirectX::XMStoreFloat3(&ACcontact_pointB_, ACcontact_pointB);

		pair->contacts.addcontact(
			ACpenetration,
			ACnormal_,
			ACcontact_pointA_,
			ACcontact_pointB_
		);
	}
	return is_AC;
}
#pragma endregion

#pragma region BOX-Capsule
bool Physics_function::generate_contact_box_capsule(const Collider_shape* box, const Collider_shape* capsule, Contacts::Contact_pair*& pair, bool& is_crossing) {
	using namespace DirectX;

#if 1
	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	XMVECTOR ACnormal;
	XMVECTOR ACcontact_pointA;
	XMVECTOR ACcontact_pointB;

	//box�̏����g���₷���悤�ɕύX
	Matrix33 m;
	m = box->world_orientation().get_rotate_matrix();
	OBB_struct obb;
	obb.world_position = XMLoadFloat3(&box->world_position());
	obb.u_axes[0] = XMVectorSet(m._11, m._12, m._13, 0);
	obb.u_axes[1] = XMVectorSet(m._21, m._22, m._23, 0);
	obb.u_axes[2] = XMVectorSet(m._31, m._32, m._33, 0);
	obb.half_width[0] = box->world_scale()[0];	obb.half_width[1] = box->world_scale()[1];	obb.half_width[2] = box->world_scale()[2];
	obb.world_orientation = XMLoadFloat4(&box->world_orientation());

	//capsule�̏���
	Capsule_struct capsule_s;
	capsule_s.world_position = XMLoadFloat3(&capsule->world_position());
	capsule_s.r = capsule->world_scale().x;
	capsule_s.hight = capsule->world_scale().y;

	XMVECTOR capsule_Worient = XMLoadFloat4(&capsule->world_orientation());
	const XMVECTOR cuppos_boxcoord = XMVector3Rotate(capsule_s.world_position - obb.world_position, XMQuaternionInverse(obb.world_orientation));
	const XMVECTOR cupYaxis_boxcoord = XMVector3Rotate(XMLoadFloat3(&Vector3(0, capsule->world_scale().y, 0)), XMQuaternionMultiply(capsule_Worient, XMQuaternionInverse(obb.world_orientation)));

	capsule_s.position = cuppos_boxcoord;
	capsule_s.y_axis = XMVector3Normalize(cupYaxis_boxcoord);


	float smallest_penetration = FLT_MAX;	//�ŏ��߂荞�ݗ�
	int smallest_axis[2];	//�ŏ��߂荞�ݗʂ𓾂��������̍쐬�Ɏg�p�����eOBB�̃��[�J�����ԍ� �Ӂ~�ӗp��2��
	SAT_TYPE smallest_case;	//�Փ˂̎��
	if (!sat_obb_capsule(obb, capsule_s, smallest_penetration, smallest_axis, smallest_case))return false;

	//capsule��obb�̖ʂƏՓ˂����ꍇ
	if (smallest_case == SAT_TYPE::POINTB_FACETA) {

		const XMVECTOR d = obb.world_position - capsule_s.world_position;	//capsule����obb����
		XMVECTOR Wn = obb.u_axes[smallest_axis[0]];	//box�Փ˖ʂ̖@�� = �Փ˖@��
		if (XMVector3Dot(Wn, d).m128_f32[0] < 0)	//obbA��obbB�̈ʒu�֌W���Փ˖ʂ̖@���x�N�g�������肷��
		{
			Wn = -Wn;
		}
		Wn = XMVector3Normalize(Wn);
		const XMVECTOR boxN = XMVector3Rotate(Wn, XMQuaternionInverse(obb.world_orientation));

		//capsule�̃��[�J�����W�n�ł̍ŋߓ_(p1)
		XMVECTOR p1 = XMVectorSet(0, capsule_s.hight, 0, 0);
		if (XMVector3Dot(capsule_s.y_axis, -boxN).m128_f32[0] > 0) p1 = -p1;
		p1 += XMVectorScale(XMVector3Rotate(Wn, XMQuaternionInverse(capsule_Worient)), capsule_s.r);


		//box���W�n�ł�capsule�̏Փ˓_�����߂�
		XMVECTOR c = cuppos_boxcoord + cupYaxis_boxcoord;
		if (XMVector3Dot(capsule_s.y_axis, -boxN).m128_f32[0] > 0) c = cuppos_boxcoord - cupYaxis_boxcoord;
		c += XMVectorScale(boxN, capsule_s.r);

		//box�̍ŋߓ_�����߂�
		XMVECTOR p0 = c;
		Vector3 box_halfsize = box->world_scale();
		if (c.m128_f32[0] > +box_halfsize.x)p0.m128_f32[0] = +box_halfsize.x;
		if (c.m128_f32[0] < -box_halfsize.x)p0.m128_f32[0] = -box_halfsize.x;

		if (c.m128_f32[1] > +box_halfsize.y)p0.m128_f32[1] = +box_halfsize.y;
		if (c.m128_f32[1] < -box_halfsize.y)p0.m128_f32[1] = -box_halfsize.y;

		if (c.m128_f32[2] > +box_halfsize.z)p0.m128_f32[2] = +box_halfsize.z;
		if (c.m128_f32[2] < -box_halfsize.z)p0.m128_f32[2] = -box_halfsize.z;

		p0 -= XMVectorScale(boxN, smallest_penetration);

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = Wn;
		ACcontact_pointA = p0;
		ACcontact_pointB = p1;
	}

	//obb�̒��_��capsule�ƏՓ˂����ꍇ
	else if (smallest_case == SAT_TYPE::POINTA_FACETB) {
		Vector3 nearext_vertex = Vector3(obb.half_width[0], obb.half_width[1], obb.half_width[2]);
		XMVECTOR capsule_nearest_pos;
		const XMVECTOR yaxis_hight = XMVectorScale(capsule_s.y_axis, capsule_s.hight);

		//obb�̒��S����capsule�֐����ȃx�N�g�����Ƃ�Aobb�̍ŋߒ��_�����߂�
		Closest_func_SIM::get_closestP_point_segment(XMVectorZero(), capsule_s.position - yaxis_hight, capsule_s.position + yaxis_hight, capsule_nearest_pos);
		if (capsule_nearest_pos.m128_f32[0] < 0)nearext_vertex.x *= -1;
		if (capsule_nearest_pos.m128_f32[1] < 0)nearext_vertex.y *= -1;
		if (capsule_nearest_pos.m128_f32[2] < 0)nearext_vertex.z *= -1;

		//TODO : nearext_vertex��

		//box�̕ӂ�capusle��y���Ő����̍ŋߓ_�����
		XMVECTOR nearext_vertex_g = XMLoadFloat3(&nearext_vertex); //box����(nearext_vertex ~ boxseg_g)
		nearext_vertex[smallest_axis[0]] *= -1;
		XMVECTOR boxseg_g = XMLoadFloat3(&nearext_vertex); //box����(nearext_vertex ~ boxseg_g)

		//box�̕�,capsuleY���̍ŋߓ_�����߁A�����������߂�
		XMVECTOR box_pos, capsule_pos;
		float s, t;
		Closest_func_SIM::get_closestP_two_segment(
			nearext_vertex_g, boxseg_g,
			capsule_s.position - yaxis_hight, capsule_s.position + yaxis_hight,
			box_pos, capsule_pos,
			s, t
		);

		const XMVECTOR BoxN = XMVector3Normalize(capsule_pos - box_pos);


		//obb����capsule�ւ̏Փ˖@��
		const XMVECTOR Wn = XMVector3Rotate(BoxN, obb.world_orientation);

		XMVECTOR p0 = XMVectorScale(XMVectorSet(0, capsule_s.hight, 0, 0), (t * 2 - 1)) + XMVectorScale(XMVector3Rotate(-Wn, XMQuaternionInverse(capsule_Worient)), capsule->world_scale().x);


		{
			XMVECTOR box_halfsize = XMLoadFloat3(&box->world_scale());

			//box�̍��W�n�Ōv�Z
			XMVECTOR closest_box, closest_cap;
			float capsule_t = FLT_MAX;

			//box���W�n�ł�capsule�̏��
			//Vector3 cuppos_boxcoord = vector3_quatrotate(capsule->world_position() - box->world_position(), box->world_orientation().inverse());
			XMVECTOR cupsca_boxcoord = XMVector3Rotate(XMVectorSet(0, capsule->world_scale().y, 0, 0), XMQuaternionMultiply(capsule_Worient, XMQuaternionInverse(obb.world_orientation)));

			{
				float tmax;
				is_crossing = Crossing_func_SIM::getCrossingP_AABB_ray(
					XMVectorZero(), box_halfsize,
					cuppos_boxcoord, cupsca_boxcoord,
					capsule_t, tmax
				);
				if (is_crossing) {
					capsule_t = ALClamp(capsule_t, -1, 1);
					closest_cap = cuppos_boxcoord + XMVectorScale(cupsca_boxcoord, capsule_t);
				}
			}
			//TODO:�ʂƐ����̍ŋߓ_�ɕύX����!!!

			//������Box�ƌ������Ă��Ȃ�������Box��Capsule�̍ŋߓ_�����߂�
			if (is_crossing == false)
			{
				float dis_save = FLT_MAX;

				//box�̂��ׂĂ̕ӂƃJ�v�Z���̐����̑�������ōŋߓ_�����߂�
				XMVECTOR vertex[4] = {
					{+XMVectorGetX(box_halfsize), +XMVectorGetY(box_halfsize), +XMVectorGetZ(box_halfsize), 0},
					{-XMVectorGetX(box_halfsize), +XMVectorGetY(box_halfsize), -XMVectorGetZ(box_halfsize), 0},
					{-XMVectorGetX(box_halfsize), -XMVectorGetY(box_halfsize), +XMVectorGetZ(box_halfsize), 0},
					{+XMVectorGetX(box_halfsize), -XMVectorGetY(box_halfsize), -XMVectorGetZ(box_halfsize), 0}
				};

				for (int i = 0; i < 4; i++) {
					XMVECTOR closest_P, closest_Q;
					float s, t;
					Closest_func_SIM::get_closestP_two_segment(vertex[i], XMVectorSet(-vertex[i].m128_f32[0], +vertex[i].m128_f32[1], +vertex[i].m128_f32[2], 0), cuppos_boxcoord - cupsca_boxcoord, cuppos_boxcoord + cupsca_boxcoord, closest_P, closest_Q, s, t);
					if (XMVectorGetX(XMVector3LengthSq(closest_P - closest_Q)) < dis_save) {
						dis_save = XMVectorGetX(XMVector3LengthSq(closest_P - closest_Q));
						closest_box = closest_P;
						closest_cap = closest_Q;
						capsule_t = t * 2 - 1;
					}
					Closest_func_SIM::get_closestP_two_segment(vertex[i], XMVectorSet(+vertex[i].m128_f32[0], -vertex[i].m128_f32[1], +vertex[i].m128_f32[2], 0), cuppos_boxcoord - cupsca_boxcoord, cuppos_boxcoord + cupsca_boxcoord, closest_P, closest_Q, s, t);
					if (XMVectorGetX(XMVector3LengthSq(closest_P - closest_Q)) < dis_save) {
						dis_save = XMVectorGetX(XMVector3LengthSq(closest_P - closest_Q));
						closest_box = closest_P;
						closest_cap = closest_Q;
						capsule_t = t * 2 - 1;
					}
					Closest_func_SIM::get_closestP_two_segment(vertex[i], XMVectorSet(+vertex[i].m128_f32[0], +vertex[i].m128_f32[1], -vertex[i].m128_f32[2], 0), cuppos_boxcoord - cupsca_boxcoord, cuppos_boxcoord + cupsca_boxcoord, closest_P, closest_Q, s, t);
					if (XMVectorGetX(XMVector3LengthSq(closest_P - closest_Q)) < dis_save) {
						dis_save = XMVectorGetX(XMVector3LengthSq(closest_P - closest_Q));
						closest_box = closest_P;
						closest_cap = closest_Q;
						capsule_t = t * 2 - 1;
					}
				}
			}

			//�Փ˂��Ă��Ȃ����false��Ԃ�
			//if (
			//	abs(closest_cap.x) - capsule->world_scale().x > box_halfsize.x ||
			//	abs(closest_cap.y) - capsule->world_scale().x > box_halfsize.y ||
			//	abs(closest_cap.z) - capsule->world_scale().x > box_halfsize.z
			//	) return false;
			if (!XMVector3Less(closest_cap - box_halfsize, XMVectorSet(capsule->world_scale().x, capsule->world_scale().x, capsule->world_scale().x, 0)))return false;

			//box��̍ŋߓ_
			closest_box = closest_cap;
			if (XMVectorGetX(closest_cap) > +XMVectorGetX(box_halfsize))closest_box.m128_f32[0] = +XMVectorGetX(box_halfsize);
			if (XMVectorGetX(closest_cap) < -XMVectorGetX(box_halfsize))closest_box.m128_f32[0] = -XMVectorGetX(box_halfsize);

			if (XMVectorGetY(closest_cap) > +XMVectorGetY(box_halfsize))closest_box.m128_f32[1] = +XMVectorGetY(box_halfsize);
			if (XMVectorGetY(closest_cap) < -XMVectorGetY(box_halfsize))closest_box.m128_f32[1] = -XMVectorGetY(box_halfsize);

			if (XMVectorGetZ(closest_cap) > +XMVectorGetZ(box_halfsize))closest_box.m128_f32[2] = +XMVectorGetZ(box_halfsize);
			if (XMVectorGetZ(closest_cap) < -XMVectorGetZ(box_halfsize))closest_box.m128_f32[2] = -XMVectorGetZ(box_halfsize);

			const float leng = capsule->world_scale().x - XMVectorGetX(XMVector3Length(closest_box - closest_cap)); //�ђʗ�
			if (leng < FLT_EPSILON)return false;

			const XMVECTOR ns = XMVector3Normalize(closest_cap - closest_box); //box����sphere�ւ̃x�N�g�� boxcoord
			const XMVECTOR Wns = XMVector3Rotate(ns, obb.world_orientation); //worldcoord

			//
			is_AC = true;
			ACpenetration = leng;
			ACnormal = -Wns;
			ACcontact_pointA = closest_box;
			ACcontact_pointB = XMVectorScale(XMVectorSet(0, capsule->world_scale().y, 0, 0), capsule_t) + XMVectorScale(XMVector3Rotate(-Wn, XMQuaternionInverse(capsule_Worient)), capsule->world_scale().x);

		}


		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = -Wn;
		ACcontact_pointA = box_pos;
		ACcontact_pointB = p0;


	}

	//obb�̕ӂ�capsule���Փ˂����ꍇ
	else if (smallest_case == SAT_TYPE::EDGE_EDGE) {
		XMVECTOR boxN = -XMVector3Cross(obb.u_axes[smallest_axis[0]], capsule_s.y_axis);	//box���W�n�ł̏Փ˖@�� obx����capsule
		if (XMVector3Dot(capsule_s.position, boxN).m128_f32[0] < 0)boxN *= -1;

		//box�̏Փː���
		XMVECTOR box_seg[2];
		box_seg[0] = XMVectorSet(obb.half_width[0], obb.half_width[1], obb.half_width[2], 0);
		{
			if (boxN.m128_f32[0] < 0) box_seg[0].m128_f32[0] = -box_seg[0].m128_f32[0];
			if (boxN.m128_f32[1] < 0) box_seg[0].m128_f32[1] = -box_seg[0].m128_f32[1];
			if (boxN.m128_f32[2] < 0) box_seg[0].m128_f32[2] = -box_seg[0].m128_f32[2];
		}
		box_seg[1] = box_seg[0];
		box_seg[1].m128_f32[smallest_axis[0]] *= -1;

		float s, t;
		Closest_func_SIM::get_closestP_two_segment(
			box_seg[0], box_seg[1],
			cuppos_boxcoord - cupYaxis_boxcoord, cuppos_boxcoord + cupYaxis_boxcoord,
			s, t
		);

		const XMVECTOR Wn = XMVector3Rotate(boxN, obb.world_orientation);
		const XMVECTOR cupsuleN = XMVector3Rotate(Wn, XMQuaternionInverse(capsule_Worient));

		is_AC = true;
		ACpenetration = smallest_penetration;
		ACnormal = -Wn;
		ACcontact_pointA = box_seg[0] + XMVectorScale((box_seg[1] - box_seg[0]), s);
		ACcontact_pointB = XMVectorScale(XMVectorSet(0, capsule->world_scale().y, 0, 0), (t - 0.5f) * 2) + XMVectorScale(-cupsuleN, capsule->world_scale().x);

	}
	else assert(0);

	if (is_AC)
	{

		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		Vector3 ACnormal_;
		Vector3 ACcontact_pointA_;
		Vector3 ACcontact_pointB_;

		DirectX::XMStoreFloat3(&ACnormal_, ACnormal);
		DirectX::XMStoreFloat3(&ACcontact_pointA_, ACcontact_pointA);
		DirectX::XMStoreFloat3(&ACcontact_pointB_, ACcontact_pointB);

		if (pair->body[0]->get_shape_tag() == box->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal_,
				ACcontact_pointA_,
				ACcontact_pointB_
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal_,
				ACcontact_pointB_,
				ACcontact_pointA_
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
#pragma endregion

#pragma region BOX-MESH

struct Crossing_struct {
	float ACpenetration = 0;
	DirectX::XMVECTOR ACnormal = DirectX::XMVectorZero();
	DirectX::XMVECTOR ACcontact_pointA = DirectX::XMVectorZero();
	DirectX::XMVECTOR ACcontact_pointB = DirectX::XMVectorZero();
};

bool Physics_function::generate_contact_box_mesh(const Collider_shape* box, const Collider_shape* mesh, Contacts::Contact_pair*& pair, bool& is_crossing) {
	using namespace DirectX;

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;

	if (0 && mesh->get_mesh_data()->is_Convex == true) {}
	else {
		//box��mesh�̔���
		Matrix33 m;
		m = box->world_orientation().get_rotate_matrix();
		OBB_struct obb;
		obb.world_position = XMLoadFloat3(&box->world_position());
		obb.u_axes[0] = XMVectorSet(m._11, m._12, m._13, 0);
		obb.u_axes[1] = XMVectorSet(m._21, m._22, m._23, 0);
		obb.u_axes[2] = XMVectorSet(m._31, m._32, m._33, 0);
		obb.half_width[0] = box->world_scale()[0];	obb.half_width[1] = box->world_scale()[1];	obb.half_width[2] = box->world_scale()[2]; obb.half_width[3] = 0;
		obb.world_orientation = XMLoadFloat4(&box->world_orientation());

		Triangle_struct smallest_triangle;
		float smallest_penetration = FLT_MAX;
		int smallest_axis[2] = { -1,-1 };
		SAT_TYPE smallest_case = SAT_TYPE::EDGE_EDGE;


		Triangle_struct triangle;
		triangle.world_position = XMLoadFloat3(&mesh->world_position());
		triangle.world_orientation = XMLoadFloat4(&mesh->world_orientation());
		triangle.world_scale = XMLoadFloat3(&mesh->world_scale());

		std::vector<Crossing_struct> crossing_structs;
		for (const auto& faset : mesh->get_mesh_data()->facets) {

			auto& vertices = mesh->get_mesh_data()->vertices;
			triangle.vertex_position[0] = XMLoadFloat3(&(vertices)[faset.vertexID[0]]);
			triangle.vertex_position[1] = XMLoadFloat3(&(vertices)[faset.vertexID[1]]);
			triangle.vertex_position[2] = XMLoadFloat3(&(vertices)[faset.vertexID[2]]);

			triangle.world_vertex_position[0] = XMVector3Rotate(XMVectorMultiply(triangle.vertex_position[0], triangle.world_scale), triangle.world_orientation);
			triangle.world_vertex_position[1] = XMVector3Rotate(XMVectorMultiply(triangle.vertex_position[1], triangle.world_scale), triangle.world_orientation);
			triangle.world_vertex_position[2] = XMVector3Rotate(XMVectorMultiply(triangle.vertex_position[2], triangle.world_scale), triangle.world_orientation);

			triangle.Edge_num[0] = faset.edgeID[0];
			triangle.Edge_num[1] = faset.edgeID[1];
			triangle.Edge_num[2] = faset.edgeID[2];

			triangle.normal = XMLoadFloat3(&faset.normal);
			triangle.world_normal = XMVector3Rotate(triangle.normal, triangle.world_orientation);

			assert(!isnan(triangle.normal.m128_f32[0]));

			float penetration = FLT_MAX;	//�ŏ��߂荞�ݗ�
			int axis[2];	//�ŏ��߂荞�ݗʂ𓾂��������̍쐬�Ɏg�p�����eOBB�̃��[�J�����ԍ� �Ӂ~�ӗp��2��
			SAT_TYPE s_case;	//�Փ˂̎��
			//assert(!(isnan((obb.u_axes[0] + obb.u_axes[1] + obb.u_axes[2]).norm())));
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

			//obbB�̒��_��obbA�̖ʂƏՓ˂����ꍇ
			if (smallest_case == SAT_TYPE::POINTB_FACETA)
			{
				XMVECTOR& Wn = obb.u_axes[smallest_axis[0]]; //�Փ˖@��
				Wn *= smallest_axis[1]; //�Փ˖@����mesh����obb�̕�����
				Wn = XMVector3Normalize(Wn);

				//triangle�̃��[�J�����W�n�ł̍ŋߓ_(p1) triangle��3���_�̂����̂ǂꂩ
				float save_dis = -FLT_MAX;
				XMVECTOR p1; //mesh�̍��W�n

				if (XMVector3Dot(smallest_triangle.world_vertex_position[0], +Wn).m128_f32[0] > save_dis) { save_dis = XMVector3Dot(smallest_triangle.world_vertex_position[0], +Wn).m128_f32[0];  p1 = smallest_triangle.vertex_position[0]; };
				if (XMVector3Dot(smallest_triangle.world_vertex_position[1], +Wn).m128_f32[0] > save_dis) { save_dis = XMVector3Dot(smallest_triangle.world_vertex_position[1], +Wn).m128_f32[0];  p1 = smallest_triangle.vertex_position[1]; };
				if (XMVector3Dot(smallest_triangle.world_vertex_position[2], +Wn).m128_f32[0] > save_dis) { save_dis = XMVector3Dot(smallest_triangle.world_vertex_position[2], +Wn).m128_f32[0];  p1 = smallest_triangle.vertex_position[2]; };
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
				XMVECTOR Wp1 = XMVector3Rotate(p1, smallest_triangle.world_orientation) + smallest_triangle.world_position;
				//Vector3 P = vector3_quatrotate(Wp1, box->world_orientation()) + box->world_position();
				//XMVECTOR c = vector3_trans(Wp1- obb.world_position, inverse_rotate);
				XMVECTOR c = XMVector3Rotate(Wp1 - obb.world_position, XMQuaternionInverse(obb.world_orientation));

				//obbA�̍ŋߓ_�����߂�
				XMVECTOR p0 = c;
				const Vector3& box_halfsize = box->world_scale();
				if (c.m128_f32[0] > +box_halfsize.x)p0.m128_f32[0] = +box_halfsize.x;
				if (c.m128_f32[0] < -box_halfsize.x)p0.m128_f32[0] = -box_halfsize.x;

				if (c.m128_f32[1] > +box_halfsize.y)p0.m128_f32[1] = +box_halfsize.y;
				if (c.m128_f32[1] < -box_halfsize.y)p0.m128_f32[1] = -box_halfsize.y;

				if (c.m128_f32[2] > +box_halfsize.z)p0.m128_f32[2] = +box_halfsize.z;
				if (c.m128_f32[2] < -box_halfsize.z)p0.m128_f32[2] = -box_halfsize.z;

				p0 += XMVectorScale(XMVector3Rotate(-Wn, XMQuaternionInverse(obb.world_orientation)), smallest_penetration);

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
				XMVECTOR Wn;
				if (smallest_axis[1] == 3)
				{
					Wn = XMVector3Rotate(smallest_triangle.normal, smallest_triangle.world_orientation); //mesh����box�Ɍ����Ă̖@��(B����A)
				}
				else
				{
					Wn = XMVector3Rotate(XMVector3Cross(smallest_triangle.vertex_position[smallest_axis[1]], smallest_triangle.vertex_position[(smallest_axis[1] + 1) % 3]), smallest_triangle.world_orientation);

					if (XMVector3Dot(Wn, smallest_triangle.vertex_position[smallest_axis[1]]).m128_f32[0] < XMVector3Dot(Wn, smallest_triangle.vertex_position[(smallest_axis[1] + 2) % 3]).m128_f32[0])Wn *= -1;
				}
				Wn = XMVector3Normalize(Wn);

				XMVECTOR p0 = XMVectorSet(obb.half_width[0], obb.half_width[1], obb.half_width[2], 0);
				//obb��triangle�̈ʒu�֌W(d)���ڐG�_(p)�����߂�
				if (XMVector3Dot(obb.u_axes[0], Wn).m128_f32[0] > 0) p0.m128_f32[0] = -p0.m128_f32[0];
				if (XMVector3Dot(obb.u_axes[1], Wn).m128_f32[0] > 0) p0.m128_f32[1] = -p0.m128_f32[1];
				if (XMVector3Dot(obb.u_axes[2], Wn).m128_f32[0] > 0) p0.m128_f32[2] = -p0.m128_f32[2];

				//p0��obbB�̃��[�J�����W�n��
				const XMVECTOR WP = XMVector3Rotate(p0, obb.world_orientation) + obb.world_position;
				XMVECTOR c = XMVector3Rotate(WP - triangle.world_position, XMQuaternionInverse(triangle.world_orientation));

				//obbB�̍ŋߓ_�����߂�
				XMVECTOR p1;
				if (0) {
					//mesh�Ɠ_�̍ŋߓ_�����߂���@
					c /= triangle.world_scale;
					Closest_func_SIM::get_closestP_point_triangle(
						c,
						smallest_triangle.vertex_position[0],
						smallest_triangle.vertex_position[1],
						smallest_triangle.vertex_position[2],
						smallest_triangle.normal,
						p1
					);

					p1 *= triangle.world_scale;
				}
				else {
					//�@���Ƃ߂荞�ݗʂ��狁�߂���@ //�������̂ق������|�I�ɑ��� ���Ǐ����s���m
					p1 = c;
					p1 += XMVectorScale(Wn, smallest_penetration);
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

				const XMVECTOR& tri_vertex_p0 = XMLoadFloat3(&mesh->get_mesh_data()->vertices[mesh->get_mesh_data()->edges[smallest_triangle.Edge_num[smallest_axis[1]]].vertexID[0]]);
				const XMVECTOR& tri_vertex_p1 = XMLoadFloat3(&mesh->get_mesh_data()->vertices[mesh->get_mesh_data()->edges[smallest_triangle.Edge_num[smallest_axis[1]]].vertexID[1]]);

				const XMVECTOR& world_tri_vertex_p0 = XMVector3Rotate(XMVectorMultiply(tri_vertex_p0, triangle.world_scale), triangle.world_orientation);
				const XMVECTOR& world_tri_vertex_p1 = XMVector3Rotate(XMVectorMultiply(tri_vertex_p1, triangle.world_scale), triangle.world_orientation);

				XMVECTOR triangle_edge_dir = XMVector3Normalize(tri_vertex_p0 - tri_vertex_p1);
				XMVECTOR world_triangle_edge_dir = XMVector3Rotate(triangle_edge_dir, smallest_triangle.world_orientation);

				XMVECTOR Wn;
				Wn = XMVector3Cross(obb.u_axes[smallest_axis[0]], world_triangle_edge_dir);
				if (XMVector3Dot(Wn, obb.world_position - (world_tri_vertex_p0 + smallest_triangle.world_position)).m128_f32[0] < 0)	//
				{
					Wn = -Wn;
				}
				Wn = XMVector3Normalize(Wn);

				XMVECTOR p[2] = { XMLoadFloat(obb.half_width) ,tri_vertex_p0 };
				{
					if (XMVector3Dot(obb.u_axes[0], Wn).m128_f32[0] > 0) p[0].m128_f32[0] = -p[0].m128_f32[0];
					if (XMVector3Dot(obb.u_axes[1], Wn).m128_f32[0] > 0) p[0].m128_f32[1] = -p[0].m128_f32[1];
					if (XMVector3Dot(obb.u_axes[2], Wn).m128_f32[0] > 0) p[0].m128_f32[2] = -p[0].m128_f32[2];
				}

				//world���W�n��edge��edge�̍ŋߓ_�����߂�
				XMVECTOR P0a = XMVector3Rotate(p[0], obb.world_orientation) + obb.world_position;
				XMVECTOR P1a = XMVector3Rotate(XMVectorMultiply(p[1], smallest_triangle.world_scale), smallest_triangle.world_orientation) + smallest_triangle.world_position;

				float s, t; //�ŋߓ_�����߂邱�Ƃ̂ł���s,t�����߂�
				Closest_func_SIM::get_closestP_two_line(
					P0a, obb.u_axes[smallest_axis[0]],
					P1a, XMVectorMultiply(world_triangle_edge_dir, smallest_triangle.world_scale),
					s, t
				);

				//obb��local���W�n�ł̎��̌���
				XMVECTOR b_axis[3]{
					XMVectorSet(1,0,0,0),
					XMVectorSet(0,1,0,0),
					XMVectorSet(0,0,1,0)
				};

				Crossing_struct crossing;
				is_AC = true;
				crossing.ACpenetration = smallest_penetration;
				crossing.ACnormal = Wn;
				crossing.ACcontact_pointA = p[0] + XMVectorScale(b_axis[smallest_axis[0]], s);
				crossing.ACcontact_pointB = XMVectorMultiply((p[1] + XMVectorScale(triangle_edge_dir, t)), smallest_triangle.world_scale);
				crossing_structs.emplace_back(crossing);
			}

			else assert(0);

		}

		if (is_AC)
		{
			is_crossing = true;
			if (pair->check_oncoll_only == true) return false;

			float smallest_penetrate = FLT_MAX;
			XMVECTOR smallest_axis = XMVectorZero();
			for (const auto& crossing : crossing_structs) {
				if (smallest_penetrate > crossing.ACpenetration) {
					smallest_penetrate = crossing.ACpenetration;
					smallest_axis = crossing.ACnormal;
				}
			}

			for (const auto& crossing : crossing_structs) {
				if (XMVector3Dot(crossing.ACnormal, smallest_axis).m128_f32[0] < 0)continue;

				//oncoll_enter�݂̂̏ꍇ������return

				Vector3 ACnormal_;
				Vector3 ACcontact_pointA_;
				Vector3 ACcontact_pointB_;

				DirectX::XMStoreFloat3(&ACnormal_, crossing.ACnormal);
				DirectX::XMStoreFloat3(&ACcontact_pointA_, crossing.ACcontact_pointA);
				DirectX::XMStoreFloat3(&ACcontact_pointB_, crossing.ACcontact_pointB);


				if (pair->body[0]->get_shape_tag() == box->get_shape_tag())
					pair->contacts.addcontact(
						crossing.ACpenetration,
						ACnormal_,
						ACcontact_pointA_,
						ACcontact_pointB_
					);
				else
					pair->contacts.addcontact(
						crossing.ACpenetration,
						-ACnormal_,
						ACcontact_pointB_,
						ACcontact_pointA_
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
	using namespace DirectX;

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	XMVECTOR ACnormal;
	XMVECTOR ACcontact_pointA;
	XMVECTOR ACcontact_pointB;

	const XMVECTOR capsule_base_axis = XMVectorSet(0, 1, 0, 0);
	const XMVECTOR capsuleA_Worient = XMLoadFloat4(&capsuleA->world_orientation());
	const XMVECTOR capsuleB_Worient = XMLoadFloat4(&capsuleB->world_orientation());

	const XMVECTOR capsuleA_Wpos = XMLoadFloat3(&capsuleA->world_position());
	const XMVECTOR capsuleB_Wpos = XMLoadFloat3(&capsuleB->world_position());

	const XMVECTOR Avec_Wco = XMVectorScale(XMVector3Rotate(capsule_base_axis, capsuleA_Worient), capsuleA->world_scale().y);
	const XMVECTOR Bvec_Wco = XMVectorScale(XMVector3Rotate(capsule_base_axis, capsuleB_Worient), capsuleB->world_scale().y);

	float t, s;
	Closest_func_SIM::get_closestP_two_segment(
		capsuleA_Wpos - Avec_Wco,
		capsuleA_Wpos + Avec_Wco,
		capsuleB_Wpos - Bvec_Wco,
		capsuleB_Wpos + Bvec_Wco,
		s, t
	);

	XMVECTOR pA_Wco = capsuleA_Wpos + XMVectorScale(Avec_Wco, (s * 2 - 1));
	XMVECTOR pB_Wco = capsuleB_Wpos + XMVectorScale(Bvec_Wco, (t * 2 - 1));

	//pB ���� pA�@�����ւ̖@�� Acoord
	XMVECTOR Wn = pA_Wco - pB_Wco;
	float length = XMVector3Length(Wn).m128_f32[0];
	if (length == 0)return false; //�S���ꏏ�̏ꍇ �Փ˓_���킩��Ȃ��̂�return

	Wn = Wn / length;

	if (length < capsuleA->world_scale().x + capsuleB->world_scale().x) {

		//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐��p�ɏ�������
		is_AC = true;
		ACpenetration = capsuleA->world_scale().x + capsuleB->world_scale().x - length;
		ACnormal = Wn;
		ACcontact_pointA = XMVectorScale(capsule_base_axis, capsuleA->world_scale().y * (s * 2 - 1)) + XMVectorScale(XMVector3Rotate(-Wn, XMQuaternionInverse(capsuleA_Worient)), capsuleA->world_scale().x);
		ACcontact_pointB = XMVectorScale(capsule_base_axis, capsuleB->world_scale().y * (t * 2 - 1)) + XMVectorScale(XMVector3Rotate(+Wn, XMQuaternionInverse(capsuleB_Worient)), capsuleB->world_scale().x);
	}


	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		Vector3 ACnormal_;
		Vector3 ACcontact_pointA_;
		Vector3 ACcontact_pointB_;

		DirectX::XMStoreFloat3(&ACnormal_, ACnormal);
		DirectX::XMStoreFloat3(&ACcontact_pointA_, ACcontact_pointA);
		DirectX::XMStoreFloat3(&ACcontact_pointB_, ACcontact_pointB);

		if (pair->body[0]->get_shape_tag() == capsuleA->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal_,
				ACcontact_pointA_,
				ACcontact_pointB_
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal_,
				ACcontact_pointB_,
				ACcontact_pointA_
			);
	}
	return is_AC;
}
#pragma endregion

#pragma region Capsule-Mesh
bool Physics_function::generate_contact_capsule_mesh(const Collider_shape* capsule, const Collider_shape* mesh, Contacts::Contact_pair*& pair, bool& is_crossing) {
	using namespace DirectX;

	//AddContact�p�̕ϐ�
	bool is_AC = false;
	float ACpenetration = 0;
	XMVECTOR ACnormal;
	XMVECTOR ACcontact_pointA;
	XMVECTOR ACcontact_pointB;

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
		const XMVECTOR capusle_Wpos = XMLoadFloat3(&capsule->world_position());
		const XMVECTOR capusle_Worient = XMLoadFloat4(&capsule->world_orientation());

		const XMVECTOR mesh_Wpos = XMLoadFloat3(&mesh->world_position());
		const XMVECTOR mesh_Worient = XMLoadFloat4(&mesh->world_orientation());
		const XMVECTOR mesh_Wscale = XMLoadFloat3(&mesh->world_scale());

		//����mesh�̏Փ˔�����s�� �����䂪�ނƖʓ|�Ȃ̂�scale��world�̂܂܂�
		const XMVECTOR capsule_pos_meshcoord = XMVector3Rotate(capusle_Wpos - mesh_Wpos, XMQuaternionInverse(mesh_Worient)); //mesh���W�n�ł�sphere��pos
		const XMVECTOR capsule_dir_meshcoord = XMVectorScale(XMVector3Rotate(XMVectorSet(0, 1, 0, 0), XMQuaternionMultiply(capusle_Worient, XMQuaternionInverse(mesh_Worient))), capsule->world_scale().y); //mesh���W�n�ł�sphere��pos

		bool is_hit = false; //�Փ˂��Ă�����true
		float min_dis = capsule->world_scale().x;//�Œ዗����sphere�̔��a��
		float min_t; //�Փ˓_��t
		XMVECTOR closest_point; //mesh��̍ŋߓ_
		XMVECTOR nor_meshcoord; //�Փ˓_�ł̖@��
		XMVECTOR min_sphere_pos_meshcoord; //�Փ˓_��capsule��sphere��pos
		//capsule��mesh�̔���
		for (const auto& faset : mesh->get_mesh_data()->facets) {

			const XMVECTOR faset_normal = XMLoadFloat3(&faset.normal);

			const XMVECTOR nor = XMVector3Normalize(XMVectorMultiply(faset_normal, mesh_Wscale));
			const XMVECTOR mesh_pos0 = XMVectorMultiply(XMLoadFloat3(&mesh->get_mesh_data()->vertices.at(faset.vertexID[0])), mesh_Wscale);
			//mesh���ʂ�"d"
			float dis = XMVector3Dot(nor, mesh_pos0).m128_f32[0];

			float Crossing_t;

			//capsule������̍ŋߓ_�������������߂� ���m�ł͂Ȃ����ߋ��߂Ȃ���
			Crossing_func_SIM::getCrossingP_plane_line(
				nor,
				dis,
				capsule_pos_meshcoord,
				capsule_dir_meshcoord,
				Crossing_t, false
			);

			Crossing_t = ALClamp(Crossing_t, -1, 1);

			XMVECTOR sphere_pos_meshcoord = capsule_pos_meshcoord + XMVectorScale(capsule_dir_meshcoord, Crossing_t);
			float dis_sp = XMVector3Dot(nor, sphere_pos_meshcoord).m128_f32[0];

			//sphere��Mesh�̗��ɂ��� �܂��� mesh���ʂ�sphere�̋�����min_dis���傫����ΏՓ˂��Ȃ�
			if (dis_sp - dis < 0 || dis_sp - dis > min_dis) continue;

			const XMVECTOR mesh_pos1 = XMVectorMultiply(XMLoadFloat3(&mesh->get_mesh_data()->vertices.at(faset.vertexID[1])), mesh_Wscale);
			const XMVECTOR mesh_pos2 = XMVectorMultiply(XMLoadFloat3(&mesh->get_mesh_data()->vertices.at(faset.vertexID[2])), mesh_Wscale);

			XMVECTOR closest_p;
			Closest_func_SIM::get_closestP_segment_triangle(
				capsule_pos_meshcoord - capsule_dir_meshcoord, capsule_pos_meshcoord + capsule_dir_meshcoord,
				mesh_pos0, mesh_pos1, mesh_pos2, nor,
				Crossing_t, closest_p
			);

			Crossing_t = Crossing_t * 2 - 1;
			Crossing_t = ALClamp(Crossing_t, -1, 1);

			sphere_pos_meshcoord = capsule_pos_meshcoord + XMVectorScale(capsule_dir_meshcoord, Crossing_t);

			if (XMVector3LengthSq(sphere_pos_meshcoord - closest_p).m128_f32[0] > min_dis * min_dis) continue;

			//min_dis,�ŋߓ_�̍X�V
			closest_point = closest_p;
			min_dis = XMVector3Length(sphere_pos_meshcoord - closest_point).m128_f32[0];
			nor_meshcoord = nor;
			min_t = Crossing_t;
			min_sphere_pos_meshcoord = sphere_pos_meshcoord;
			is_hit = true;

		}
		//�������Ă��Ȃ����false��Ԃ�
		if (is_hit == false)return false;

		const XMVECTOR Wn = XMVector3Normalize(XMVector3Rotate((min_sphere_pos_meshcoord - closest_point), mesh_Worient));//mesh����sphere�ւ̃x�N�g��
		//if (vector3_dot(min_sphere_pos_meshcoord - closest_point, nor_meshcoord) < 0)Wn *= -1; //�o�O�̌���

		is_AC = true;
		ACpenetration = capsule->world_scale().x - min_dis;
		ACnormal = -Wn;
		ACcontact_pointA = closest_point;
		ACcontact_pointB = XMVectorSet(0, capsule->world_scale().y * min_t, 0, 0) + XMVectorScale(XMVector3Rotate(-Wn, XMQuaternionInverse(capusle_Worient)), capsule->world_scale().x);
	}

	if (is_AC)
	{
		is_crossing = true;

		//oncoll_enter�݂̂̏ꍇ������return
		if (pair->check_oncoll_only == true) return false;

		Vector3 ACnormal_;
		Vector3 ACcontact_pointA_;
		Vector3 ACcontact_pointB_;

		DirectX::XMStoreFloat3(&ACnormal_, ACnormal);
		DirectX::XMStoreFloat3(&ACcontact_pointA_, ACcontact_pointA);
		DirectX::XMStoreFloat3(&ACcontact_pointB_, ACcontact_pointB);


		if (pair->body[0]->get_shape_tag() == mesh->get_shape_tag())
			pair->contacts.addcontact(
				ACpenetration,
				ACnormal_,
				ACcontact_pointA_,
				ACcontact_pointB_
			);
		else
			pair->contacts.addcontact(
				ACpenetration,
				-ACnormal_,
				ACcontact_pointB_,
				ACcontact_pointA_
			);
	}
	return is_AC;
}
#pragma endregion
//
//
//#pragma region MESH-PLANE
//bool Physics_function::generate_contact_mesh_plane(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair*& pair, bool& is_crossing) {
//	return true;
//}
//#pragma endregion
//
//#pragma region MESH-MESH
//
//bool GC_concave_mesh_mesh(const ALP_Collider& S1, const Collider_shape* S2, int concave_num, Contacts::Contact_pair*& pair, bool& is_crossing) {
//	return false;
//}
//
//bool Physics_function::generate_contact_mesh_mesh(const Collider_shape* meshA, const Collider_shape* meshB, Contacts::Contact_pair*& pair, bool& is_crossing) {
//	//const std::list<ALP_Collider>::iterator& meshA = mA->get_ALPcollider();
//	//const std::list<ALP_Collider>::iterator& meshB = mB->get_ALPcollider();
//
//	float smallest_penetration = FLT_MAX;	//�ŏ��߂荞�ݗ�
//	int smallest_facetID[2];	//�ŏ��߂荞�ݗʂ𓾂��������̍쐬�Ɏg�p�����eOBB�̃��[�J�����ԍ� �Ӂ~�ӗp��2��
//	SAT_TYPE smallest_case;		//�Փ˂̎��
//
//	//�ǂ����convex��object�̏ꍇ
//	if (meshA->get_mesh_data()->is_Convex == true && meshB->get_mesh_data()->is_Convex == true) {
//		if (!sat_convex_mesh_mesh(meshA, meshB, smallest_penetration, smallest_facetID, smallest_case))return false;
//
//		Quaternion offset_quatBA = meshB->world_orientation() * meshA->world_orientation().inverse();
//		Quaternion offset_quatAB = meshA->world_orientation() * meshB->world_orientation().inverse();
//		Vector3 offset_posBA = meshB->world_position() - meshA->world_position();
//		Vector3 offset_posAB = meshA->world_position() - meshB->world_position();
//
//		//SB�̒��_��SA�̖ʂƏՓ˂����ꍇ
//		if (smallest_case == SAT_TYPE::POINTB_FACETA)
//		{
//			const Collider_shape* facet_shape = meshA;
//			const Collider_shape* vertex_shape = meshB;
//
//			//const std::list<ALP_Collider>::iterator& facet_mesh = facet_mesh->get_ALPcollider();
//			//const std::list<ALP_Collider>::iterator& vertex_mesh = vertex_mesh->get_ALPcollider();
//
//			Quaternion offset_quatVF = vertex_shape->world_orientation() * facet_shape->world_orientation().inverse();
//			Quaternion offset_quatFV = facet_shape->world_orientation() * vertex_shape->world_orientation().inverse();
//			Vector3 offset_posVF = vertex_shape->world_position() - facet_shape->world_position();
//			Vector3 offset_posFV = facet_shape->world_position() - vertex_shape->world_position();
//
//			assert(smallest_facetID[1] == -1);
//			const Facet& nerest_facet = facet_shape->get_mesh_data()->facets.at(smallest_facetID[0]);
//
//			Vector3 axisF = nerest_facet.normal.unit_vect();
//			Vector3 axisW = vector3_quatrotate(axisF, facet_shape->world_orientation()).unit_vect();
//			if (vector3_dot(axisW, offset_posAB) < 0) {
//				axisF = -axisF;
//				axisW = -axisW;
//			}
//
//			//vertex_mesh�̂ǂ̒��_���ŋߓ_�����߂�
//			u_int nearest_pointID;
//			Vector3 pB;
//			{
//				float max_len = -FLT_MAX;
//				Vector3 axisV = vector3_quatrotate(axisF, offset_quatFV);
//
//				for (u_int v_num = 0; v_num < vertex_shape->get_mesh_data()->vertex_num; v_num++) {
//
//					if (vector3_dot(vertex_shape->get_mesh_data()->vertices.at(v_num), axisV) > max_len) {
//						max_len = vector3_dot(vertex_shape->get_mesh_data()->vertices.at(v_num), axisV);
//						nearest_pointID = v_num;
//					}
//				}
//			}
//			pB = vertex_shape->get_mesh_data()->vertices.at(nearest_pointID);
//			pB *= vertex_shape->world_scale();
//
//			//��L��p0��facet_mesh�̍ŋߖʏ�̂ǂ��ɂ���̂�
//			Vector3 pA;
//			{
//
//				Vector3 p = vector3_quatrotate(vector3_quatrotate(pB, vertex_shape->world_orientation()) + offset_posVF, facet_shape->world_orientation().inverse());
//				p /= facet_shape->world_scale();
//				float min_len = FLT_MAX;
//				Vector3 n_pos;
//
//
//				for (u_int f_num = 0; f_num < facet_shape->get_mesh_data()->facet_num; f_num++) {
//
//					if (vector3_dot(facet_shape->get_mesh_data()->facets.at(f_num).normal, -axisF) < 0.5)continue; //�Փ˖@���Ɣ�ׂ�
//
//					//���b�V���Ɠ_�̍ŋߓ_�����߂�
//					Closest_func::get_closestP_point_triangle(p,
//						facet_shape->get_mesh_data()->vertices.at(facet_shape->get_mesh_data()->facets.at(f_num).vertexID[0]),
//						facet_shape->get_mesh_data()->vertices.at(facet_shape->get_mesh_data()->facets.at(f_num).vertexID[1]),
//						facet_shape->get_mesh_data()->vertices.at(facet_shape->get_mesh_data()->facets.at(f_num).vertexID[2]),
//						facet_shape->get_mesh_data()->facets.at(f_num).normal,
//						n_pos
//					);
//
//					if ((p - n_pos).norm() < min_len) {
//						pA = n_pos;
//						min_len = (p - n_pos).norm();
//					}
//				}
//
//				pA *= facet_shape->world_scale();
//			}
//
//			pair->contacts.addcontact(
//				smallest_penetration,
//				axisW,
//				pA,
//				pB
//			);
//		}
//
//		//SA�̒��_��SB�̖ʂƏՓ˂����ꍇ
//		if (smallest_case == SAT_TYPE::POINTA_FACETB)
//		{
//			const Collider_shape* facet_mesh = meshB;
//			const Collider_shape* vertex_mesh = meshA;
//
//			//const std::list<ALP_Collider>::iterator& facet_mesh = facet_mesh->get_ALPcollider();
//			//const std::list<ALP_Collider>::iterator& vertex_mesh = vertex_mesh->get_ALPcollider();
//
//			Quaternion offset_quatVF = vertex_mesh->world_orientation() * facet_mesh->world_orientation().inverse();
//			Quaternion offset_quatFV = facet_mesh->world_orientation() * vertex_mesh->world_orientation().inverse();
//			Vector3 offset_posVF = vertex_mesh->world_position() - facet_mesh->world_position();
//			Vector3 offset_posFV = facet_mesh->world_position() - vertex_mesh->world_position();
//
//			assert(smallest_facetID[0] == -1);
//			const Facet& nerest_facet = facet_mesh->get_mesh_data()->facets.at(smallest_facetID[1]);
//
//			Vector3 axisF = nerest_facet.normal.unit_vect();
//			Vector3 axisW = vector3_quatrotate(axisF, facet_mesh->world_orientation()).unit_vect();
//			if (vector3_dot(axisW, offset_posBA) < 0) {
//				axisF = -axisF;
//				axisW = -axisW;
//			}
//
//			//vertex_mesh�̂ǂ̒��_���ŋߓ_�����߂�
//			u_int nearest_pointID = 0;
//			Vector3 pB;
//			{
//				float max_len = -FLT_MAX;
//				Vector3 axisV = vector3_quatrotate(axisF, offset_quatFV);
//
//				for (u_int v_num = 0; v_num < vertex_mesh->get_mesh_data()->vertex_num; v_num++) {
//
//					if (vector3_dot(vertex_mesh->get_mesh_data()->vertices.at(v_num), axisV) > max_len) {
//						max_len = vector3_dot(vertex_mesh->get_mesh_data()->vertices.at(v_num), axisV);
//						nearest_pointID = v_num;
//					}
//				}
//			}
//			pB = vertex_mesh->get_mesh_data()->vertices.at(nearest_pointID);
//			pB *= vertex_mesh->world_scale();
//
//			//��L��pB��facet_mesh�̍ŋߖʏ�̂ǂ��ɂ���̂�
//			Vector3 pA;
//			{
//
//				Vector3 p = vector3_quatrotate(vector3_quatrotate(pB, vertex_mesh->world_orientation()) + offset_posVF, facet_mesh->world_orientation().inverse());
//				p /= facet_mesh->world_scale();
//				float min_len = FLT_MAX;
//				Vector3 n_pos;
//
//
//				for (u_int f_num = 0; f_num < facet_mesh->get_mesh_data()->facet_num; f_num++) {
//
//					if (vector3_dot(facet_mesh->get_mesh_data()->facets.at(f_num).normal, -axisF) < 0.5)continue; //�Փ˖@���Ɣ�ׂ�
//
//					//���b�V���Ɠ_�̍ŋߓ_�����߂�
//					Closest_func::get_closestP_point_triangle(p,
//						facet_mesh->get_mesh_data()->vertices.at(facet_mesh->get_mesh_data()->facets.at(f_num).vertexID[0]),
//						facet_mesh->get_mesh_data()->vertices.at(facet_mesh->get_mesh_data()->facets.at(f_num).vertexID[1]),
//						facet_mesh->get_mesh_data()->vertices.at(facet_mesh->get_mesh_data()->facets.at(f_num).vertexID[2]),
//						facet_mesh->get_mesh_data()->facets.at(f_num).normal,
//						n_pos
//					);
//
//					if ((p - n_pos).norm() < min_len) {
//						pA = n_pos;
//						min_len = (p - n_pos).norm();
//					}
//				}
//
//				pA *= facet_mesh->world_scale();
//			}
//
//			pair->contacts.addcontact(
//				smallest_penetration,
//				-axisW,
//				pB,
//				pA
//			);
//		}
//
//
//		//SA��SB�̕ӓ��m���Փ˂����ꍇ
//		else if (smallest_case == SAT_TYPE::EDGE_EDGE)
//		{
//			Quaternion offset_quatAB = meshA->world_orientation() * meshB->world_orientation().inverse();
//			Quaternion offset_quatBA = meshB->world_orientation() * meshA->world_orientation().inverse();
//			Vector3 offset_posAB = meshA->world_position() - meshB->world_position();
//			Vector3 offset_posBA = meshB->world_position() - meshA->world_position();
//
//			const Edge& edgeA = meshA->get_mesh_data()->edges.at(smallest_facetID[0]);
//			const Edge& edgeB = meshB->get_mesh_data()->edges.at(smallest_facetID[1]);
//
//			Vector3 edgeA_p[2] = {
//				{meshA->get_mesh_data()->vertices.at(edgeA.vertexID[0]) * meshA->world_scale()},
//				{meshA->get_mesh_data()->vertices.at(edgeA.vertexID[1]) * meshA->world_scale()}
//			};
//			Vector3 edgeB_p[2] = {
//				{meshB->get_mesh_data()->vertices.at(edgeB.vertexID[0]) * meshB->world_scale()},
//				{meshB->get_mesh_data()->vertices.at(edgeB.vertexID[1]) * meshB->world_scale()}
//			};
//
//			Vector3 edgeA_vec = (edgeA_p[0] - edgeA_p[1]).unit_vect();
//			Vector3 edgeB_vec = (edgeB_p[0] - edgeB_p[1]).unit_vect();
//
//			//SB�̏���SA�̍��W�n�Ɏ����Ă���
//			//Vector3 edgeB_p_A = vector3_quatrotate(edgeB_p[0], offset_quatBA) + offset_posBA;
//			Vector3 edgeB_p_A = vector3_quatrotate(vector3_quatrotate(edgeB_p[0], meshB->world_orientation()) + offset_posBA, meshA->world_orientation().inverse());
//			Vector3 edgeB_v_A = vector3_quatrotate(edgeB_vec, offset_quatBA);
//
//			//SA�̍��W�n��axis�̐���
//			Vector3 axisA = vector3_cross(edgeA_vec, edgeB_v_A);
//			axisA = axisA.unit_vect();
//
//			//axis��world���W�n��
//			Vector3 axisW = vector3_quatrotate(axisA, meshA->world_orientation());
//
//			//axis�̌�����SB->SA�̌�����
//			if (vector3_dot(axisW, offset_posAB) < 0)
//			{
//				axisA = axisA * -1.0f;
//				axisW = axisW * -1.0f;
//			}
//
//			//SA�̍��W�n�Ɏ����Ă��Ē����ƒ����̍ŋߓ_���l��
//			float s, t;
//			Closest_func::get_closestP_two_line(
//				edgeA_p[0], edgeA_vec,
//				edgeB_p_A, edgeB_v_A,
//				s, t
//			);
//			pair->contacts.addcontact(
//				smallest_penetration,
//				axisW,
//				edgeA_p[0] + s * edgeA_vec,
//				edgeB_p[0] + t * edgeB_vec
//			);
//		}
//
//	}
//	else {
//		return false;
//	}
//
//
//
//
//	return true;
//}
//
//#pragma endregion

#pragma endregion
//:::::::::::::::::::::::::::

#endif