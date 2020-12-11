#include "ALP_generate_contact.h"

#include "../Imgui/work_meter.h"

#include "../Imgui/imgui_all.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

using namespace Closest_func;

//:::::::::::::::::::::::::::
#pragma region generate_contact
//:::::::::::::::::::::::::::

void Physics_function::generate_contact(std::vector<Contacts::Contact_pair>& pairs) {

	for (auto& pair : pairs) {

		const std::vector<ALP_Collider_mesh>::iterator& meshA = pair.body[0];
		const std::vector<ALP_Collider_mesh>::iterator& meshB = pair.body[1];
		const std::list<ALP_Collider>::iterator& shapeA = pair.body[0]->ALPcollider;
		const std::list<ALP_Collider>::iterator& shapeB = pair.body[1]->ALPcollider;

		if (shapeA->shape == ALP_Collider_shape::Sphere) {

			if (shapeB->shape == ALP_Collider_shape::Sphere)generate_contact_sphere_sphere(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::BOX)	generate_contact_sphere_box(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Capsule)generate_contact_sphere_capsule(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Plane)	generate_contact_sphere_plane(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Mesh)	generate_contact_sphere_mesh(meshA, meshB, pair);
		}
		if (shapeA->shape == ALP_Collider_shape::BOX) {

			if (shapeB->shape == ALP_Collider_shape::Sphere)generate_contact_sphere_box(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::BOX)	generate_contact_box_box(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Capsule)generate_contact_box_capsule(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Plane)	generate_contact_box_plane(meshA, meshB, pair);
		}
		if (shapeA->shape == ALP_Collider_shape::Capsule) {
			if (shapeB->shape == ALP_Collider_shape::Sphere)generate_contact_sphere_capsule(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::BOX)generate_contact_box_capsule(meshB, meshA, pair);
		}
		if (shapeA->shape == ALP_Collider_shape::Plane) {

			if (shapeB->shape == ALP_Collider_shape::Sphere)generate_contact_sphere_plane(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::BOX)	generate_contact_box_plane(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::Plane) {}
		}
		if (shapeA->shape == ALP_Collider_shape::Mesh) {

			if (shapeB->shape == ALP_Collider_shape::Sphere)generate_contact_sphere_mesh(meshB, meshA, pair);
			if (shapeB->shape == ALP_Collider_shape::Plane)	generate_contact_mesh_plane(meshA, meshB, pair);
			if (shapeB->shape == ALP_Collider_shape::Mesh)	generate_contact_mesh_mesh(meshA, meshB, pair);
		}


	}
}


#pragma region data

//�����������p����ۂɎg�p����
enum class SAT_TYPE {
	POINTA_FACETB,
	POINTB_FACETA,
	EDGE_EDGE
};
struct Ret_S {
	float penetrate;
	int smallest_axis[2];
	SAT_TYPE smallest_case;
	bool hit_point_to_face;
};

struct OBB {
	Vector3 world_position; //���S���W
	Vector3 u_axes[3]; //���̌���
	Quaternion world_orientation;
	Vector3 half_width; //�����Ƃ̕ӂ̒���
};

//���ɓ��e��������
float sum_of_projected_radii(const OBB& obb, const Vector3& vec) {
	return
		fabsf(vector3_dot(vec, obb.half_width.x * obb.u_axes[0])) +
		fabsf(vector3_dot(vec, obb.half_width.y * obb.u_axes[1])) +
		fabsf(vector3_dot(vec, obb.half_width.z * obb.u_axes[2]));
}

float sum_of_projected_radii(float& max, float& min, const std::vector<ALP_Collider_mesh>::iterator& meshcoll, const Vector3& nor) {
	float value;
	max = -FLT_MAX;
	min = +FLT_MAX;
	for (const Vector3& vertex : *meshcoll->mesh->vertices) {
		value = vector3_dot(vertex * meshcoll->ALPcollider->world_scale, nor);
		if (max < value)max = value;
		if (min > value)min = value;
	}

	return max - min;
}

//OBB��OBB�̕��������� �Փ˂��Ă����true��Ԃ� ���łɐF�X�����ɓ����
bool sat_obb_obb(
	const OBB& obbA, const OBB& obbB,
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

//�ǂ�����ʕ�̏ꍇ
bool sat_convex_mesh_mesh(const std::vector<ALP_Collider_mesh>::iterator& meshA, const std::vector<ALP_Collider_mesh>::iterator& meshB,
	float& smallest_penetration, //�ŏ��̊ђʗ�
	int smallest_axis[2], //�ǂ̎��ōŋ߂ɂȂ�����(edge�~edge�p��2���p��)
	SAT_TYPE& smallest_case //�ǂ̂悤�Ȍ`�ōŋ߂ɂȂ��Ă��邩
) {
	smallest_penetration = FLT_MAX;

	std::list<ALP_Collider>::iterator collA = meshA->ALPcollider;
	std::list<ALP_Collider>::iterator collB = meshB->ALPcollider;

	// Meshcoll_1�̊e�ʖ@���𕪗����Ƃ���
	Quaternion offset_quatBA = collB->world_orientation * collA->world_orientation.conjugate();
	Quaternion offset_quatAB = collA->world_orientation * collB->world_orientation.conjugate();
	Vector3 offset_posBA = collB->world_position - collA->world_position;
	Vector3 offset_posAB = collA->world_position - collB->world_position;

	Ret_S PB_FA, PA_FB;
	PB_FA.hit_point_to_face = PA_FB.hit_point_to_face = true;
	PA_FB.penetrate = PB_FA.penetrate = FLT_MAX;
	float penetration = FLT_MAX;

	////::: obbA�̎���obbB�𓊉e :::::::::::::::::::
		//collA�̍��W�n�Ōv�Z���s��
	float maxA, minA, maxB, minB;
	for (u_int f = 0; f < meshA->mesh->facet_num; f++) {
		const Facet& facet = meshA->mesh->facets.at(f); //meshcoord
		const Vector3& axis = facet.normal; //meshcoord

		// collA�𕪗����ɓ��e
		sum_of_projected_radii(maxA, minA, meshA, axis);
		assert(maxA >= minA);

		// collB�𕪗����ɓ��e
		sum_of_projected_radii(maxB, minB, meshB, vector3_quatrotate(axis, offset_quatAB).unit_vect());
		assert(maxB >= minB);
		float off = vector3_dot(offset_posBA, vector3_quatrotate(axis, collA->world_orientation).unit_vect());
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
		for (u_int f = 0; f < meshB->mesh->facet_num; f++) {
			const Facet& facet = meshB->mesh->facets.at(f);
			const Vector3& axis = facet.normal;

			// collB�𕪗����ɓ��e
			sum_of_projected_radii(maxB, minB, meshB, axis);
			assert(maxB >= minB);

			// collA�𕪗����ɓ��e
			sum_of_projected_radii(maxA, minA, meshA, vector3_quatrotate(axis, offset_quatBA));
			assert(maxA >= minA);
			float off = vector3_dot(offset_posAB, vector3_quatrotate(axis, meshB->ALPcollider->world_orientation).unit_vect());
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

	Work_meter::start("generate_edge_edge");
	//::: �O�ς̎��ɓ��e(�ŋߋ��������߂邽��)
		//collA�̍��W�n�Ōv�Z���s��
	Vector3 save_axisW;
	float save_crossnorm = 0;
	Vector3 axisA, axisB, axisW;
	for (u_int eA = 0; eA < meshA->mesh->edge_num; eA++) {
		const Edge& edgeA = meshA->mesh->edges.at(eA);
		if (edgeA.type != Edgetype::EdgeConvex) continue;

		const Vector3& edgeVecA = meshA->mesh->vertices->at(edgeA.vertexID[1]) - meshA->mesh->vertices->at(edgeA.vertexID[0]);
		for (u_int eB = 0; eB < meshB->mesh->edge_num; eB++) {
			const Edge& edgeB = meshB->mesh->edges.at(eB);
			if (edgeB.type != Edgetype::EdgeConvex) continue;

			const Vector3 edgeVecB = vector3_quatrotate(meshB->mesh->vertices->at(edgeB.vertexID[1]) - meshB->mesh->vertices->at(edgeB.vertexID[0]), offset_quatBA);

			axisA = vector3_cross(edgeVecA, edgeVecB);
			if (axisA.norm() <= FLT_EPSILON * FLT_EPSILON) continue;
			axisA = axisA.unit_vect();
			axisW = vector3_quatrotate(axisA, meshA->ALPcollider->world_orientation);
			//axis�̌�����A->B�ɂ���
			if (vector3_dot(axisW, meshB->ALPcollider->world_position - meshA->ALPcollider->world_position) < 0) {
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
			float CN = fabsf(off) - (vector3_dot(axisA, meshA->mesh->vertices->at(edgeA.vertexID[0])) + -(vector3_dot(axisB, meshB->mesh->vertices->at(edgeB.vertexID[0]))));

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
	Work_meter::stop("generate_edge_edge");
	// ����������������Ȃ��ꍇOBB�͌������Ă���͂�
	return (smallest_penetration < FLT_MAX&& smallest_penetration > FLT_EPSILON) ? true : false;

}

bool sat_obb_convex_mesh(const OBB& obb, const std::vector<ALP_Collider_mesh>::iterator& mesh,
	float& smallest_penetration, //�ŏ��̊ђʗ�
	int smallest_axis[2], //�ǂ̎��ōŋ߂ɂȂ�����(edge�~edge�p��2���p��)
	SAT_TYPE& smallest_case //�ǂ̂悤�Ȍ`�ōŋ߂ɂȂ��Ă��邩
) {
	smallest_penetration = FLT_MAX;

	const std::list<ALP_Collider>::iterator& mesh_coll = mesh->ALPcollider;

	// Meshcoll_1�̊e�ʖ@���𕪗����Ƃ���
	Quaternion offset_quatBA = mesh_coll->world_orientation * obb.world_orientation.conjugate();
	Quaternion offset_quatAB = obb.world_orientation * mesh_coll->world_orientation.conjugate();
	Vector3 offset_posBA = mesh_coll->world_position - obb.world_position;
	Vector3 offset_posAB = obb.world_position - mesh_coll->world_position;

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
		sum_of_projected_radii(maxA, minA, mesh, vector3_quatrotate(axis, mesh_coll->world_orientation.conjugate()).unit_vect());
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
		for (u_int f = 0; f < mesh->mesh->facet_num; f++) {
			const Facet& facet = mesh->mesh->facets.at(f);
			const Vector3& axis = facet.normal; //meshcoord

			// mesh�𕪗����ɓ��e
			sum_of_projected_radii(maxA, minA, mesh, axis);
			assert(maxA >= minA);

			// obb�𕪗����ɓ��e
			float ra = sum_of_projected_radii(obb, vector3_quatrotate(axis, mesh_coll->world_orientation));

			float off = vector3_dot(offset_posAB, vector3_quatrotate(axis, mesh_coll->world_orientation).unit_vect());
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
	Work_meter::stop("generate_edge_edge");
	// ����������������Ȃ��ꍇOBB�͌������Ă���͂�
	return (smallest_penetration < FLT_MAX&& smallest_penetration > FLT_EPSILON) ? true : false;
}
//
#pragma endregion

//�Փː���
#pragma region SPHERE-SPHERE
bool Physics_function::generate_contact_sphere_sphere(const std::vector<ALP_Collider_mesh>::iterator& SA, const std::vector<ALP_Collider_mesh>::iterator& SB, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& collA = SA->ALPcollider;
	const std::list<ALP_Collider>::iterator& collB = SB->ALPcollider;

	Vector3 pA = collA->world_position;
	Vector3 pB = collB->world_position;

	//p1 ���� p0�@�����ւ̖@��
	Vector3 n = pA - pB;
	float length = n.norm_sqr();
	n = n.unit_vect();

	if (length < collA->world_scale.x + collB->world_scale.x) {
		//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐�����
		pair.contacts.addcontact(
			collA->world_scale.x + collB->world_scale.x - length,
			n,
			collA->world_scale.x * vector3_quatrotate(-n, collA->world_orientation.conjugate()),
			collB->world_scale.x * vector3_quatrotate(+n, collB->world_orientation.conjugate())
		);
		return true;
	}

	return false;
}
#pragma endregion

#pragma region SPHERE-PLANE
bool Physics_function::generate_contact_sphere_plane(const std::vector<ALP_Collider_mesh>::iterator& sphere_mesh, const std::vector<ALP_Collider_mesh>::iterator& plane_mesh, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& sphere = sphere_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& plane = plane_mesh->ALPcollider;

	//���ʂƕ��ʂ̏Փ˔�����s��
	Matrix rotate, inverse_rotate;
	rotate = plane->world_orientation.get_rotate_matrix();
	rotate._41 = plane->world_position.x; //transpse�̓���
	rotate._42 = plane->world_position.y;
	rotate._43 = plane->world_position.z;
	rotate._44 = 1;
	Vector3 n(rotate._21, rotate._22, rotate._23);
	inverse_rotate = matrix_inverse(rotate);

	Vector3 p;
	p = vector3_trans(sphere->world_position, inverse_rotate); //���ʂ�"���_��ʂ�"�@����(0,1,0)"�̏�Ԃ̎��̋��̒��S���W

	////���ʂ̗�����̏Փ˔���
	//if (half_space && p.y < 0)return 0;

	if (abs(p.y) < sphere->world_scale.x) {
		n = p.y > 0 ? n : -n;

		if (pair.body[0]->ALPcollider->shape == plane->shape) {
			//body[0]�@���@plane
			//body[1]�@���@sphere
			pair.contacts.addcontact(
				sphere->world_scale.x - abs(p.y),
				n,
				Vector3(p.x, 0, p.z),
				sphere->world_scale.x * vector3_quatrotate(-n, sphere->world_orientation.conjugate())
			);
		}
		else {

			//body[0]�@���@sphere
			//body[1]�@���@plane
			pair.contacts.addcontact(
				sphere->world_scale.x - abs(p.y),
				n,
				sphere->world_scale.x * vector3_quatrotate(-n, sphere->world_orientation.conjugate()),
				Vector3(p.x, 0, p.z)
			);
		}
		return true;
	}

	return false;
}
#pragma endregion

#pragma region SPHERE-BOX
bool Physics_function::generate_contact_sphere_box(const std::vector<ALP_Collider_mesh>::iterator& sphere_mesh, const std::vector<ALP_Collider_mesh>::iterator& box_mesh, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& sphere = sphere_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& box = box_mesh->ALPcollider;

	//����box�̏Փ˔�����s��
	Matrix rotate, inverse_rotate;
	rotate = matrix_world(Vector3(1, 1, 1), box->world_orientation.get_rotate_matrix(), box->world_position);
	inverse_rotate = matrix_inverse(rotate);

	Vector3 center;
	center = vector3_trans(sphere->world_position, inverse_rotate); //box��local���W�n�ł̋��̒��S���W

	Vector3 box_halfsize = box->world_scale * box->half_size;

	if (
		abs(center.x) - sphere->world_scale.x > box_halfsize.x ||
		abs(center.y) - sphere->world_scale.x > box_halfsize.y ||
		abs(center.z) - sphere->world_scale.x > box_halfsize.z
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
	if (sphere->world_scale.x - distance < FLT_EPSILON) return false; //�Փ˂��Ă��Ȃ�������false

	Vector3 n = (sphere->world_position - vector3_trans(closest_point, rotate)).unit_vect(); //box����sphere�ւ̃x�N�g��
	if (vector3_dot(n, sphere->world_position - box->world_position) < 0)n *= -1;

	if (pair.body[0]->ALPcollider->shape == box->shape) {
		//body[0]�@���@box
		//body[1]�@���@sphere
		pair.contacts.addcontact(
			sphere->world_scale.x - distance,
			-n,
			closest_point,
			sphere->world_scale.x * vector3_quatrotate(-n, sphere->world_orientation.conjugate())
		);
	}
	else {
		//body[0]�@���@sphere
		//body[1]�@���@box
		pair.contacts.addcontact(
			sphere->world_scale.x - distance,
			+n,
			sphere->world_scale.x * vector3_quatrotate(-n, sphere->world_orientation.conjugate()),
			closest_point
		);
	}
	return true;
}
#pragma endregion

#pragma region SPHERE-Capsule
bool Physics_function::generate_contact_sphere_capsule(const std::vector<ALP_Collider_mesh>::iterator& sphere_mesh, const std::vector<ALP_Collider_mesh>::iterator& capsule_mesh, Contacts::Contact_pair& pair) {

	const std::list<ALP_Collider>::iterator& sphere = sphere_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& capsule = capsule_mesh->ALPcollider;

	//capsule�̍��W�n�Ōv�Z����(scale�͕ύX���Ȃ�)

	//capsule coord
	Vector3 sphere_pos_capcoord = vector3_quatrotate(sphere->world_position - capsule->world_position, capsule->world_orientation.conjugate());

	float s;
	Closest_func::get_closestP_point_line(sphere_pos_capcoord, Vector3(0), Vector3(0, 1, 0), s);
	s = ALmax(ALmin(s, +capsule->world_scale.y), -capsule->world_scale.y); //capsule�̒����ɃN�����v


	//capsule�̍��W�n��sphere-sphere�̔�����s��
	Vector3 pA = sphere_pos_capcoord;
	Vector3 pB = s * Vector3(0, 1, 0);

	//p1 ���� p0�@�����ւ̖@��
	Vector3 n = pA - pB;
	float length = n.norm_sqr();
	n = n.unit_vect();

	if (length < sphere->world_scale.x + capsule->world_scale.x) {
		Vector3 Wn = vector3_quatrotate(n, capsule->world_orientation); //n��capsule���烏�[���h��

		if (pair.body[0]->ALPcollider->shape == sphere->shape) {
			//�Փ˂��Ă�����Contact�I�u�W�F�N�g�𐶐�����
			pair.contacts.addcontact(
				sphere->world_scale.x + capsule->world_scale.x - length,
				+Wn,
				sphere->world_scale.x * vector3_quatrotate(-Wn, sphere->world_orientation.conjugate()),
				capsule->world_scale.x * n + pB
			);
		}
		else {
			pair.contacts.addcontact(
				sphere->world_scale.x + capsule->world_scale.x - length,
				-Wn,
				capsule->world_scale.x * n + pB,
				sphere->world_scale.x * vector3_quatrotate(-Wn, sphere->world_orientation.conjugate())
			);
		}
		return true;
	}

	return false;
}
#pragma endregion

#pragma region SPHERE-MESH
bool Physics_function::generate_contact_sphere_mesh(const std::vector<ALP_Collider_mesh>::iterator& sphere_mesh, const std::vector<ALP_Collider_mesh>::iterator& mesh_mesh, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& sphere = sphere_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& mesh = mesh_mesh->ALPcollider;

	if (mesh_mesh->mesh->is_Convex == true) {
		//����mesh�̏Փ˔�����s��
		Matrix rotate, inverse_rotate;
		rotate = matrix_world(Vector3(1, 1, 1), mesh->world_orientation.get_rotate_matrix(), mesh->world_position);
		inverse_rotate = matrix_inverse(rotate);

		Vector3 center;
		center = vector3_trans(sphere->world_position, inverse_rotate); //mesh��local���W�n�ł̋��̒��S���W

		//mesh��̍ŋߓ_
		Vector3 closest_point;

		closest_point = center;
		//�e�ʂ̊O�ɂ���Ζʕ��ʂɎ����Ă���
		for (u_int i = 0; i < mesh_mesh->mesh->facet_num; i++) {
			const Vector3& nor = mesh_mesh->mesh->facets.at(i).normal.unit_vect();
			const Vector3& pos = mesh_mesh->mesh->vertices->at(mesh_mesh->mesh->facets.at(i).vertexID[0]) * mesh->world_scale;
			float d = vector3_dot(nor, pos) - vector3_dot(nor, closest_point);
			if (d < 0)
				closest_point += d * nor;
		}

		float distance = (closest_point - center).norm_sqr(); //�ŋߓ_�Ƌ����S�̋���
		if (sphere->world_scale.x - distance > FLT_EPSILON) { //float�덷������
			Vector3 n = (sphere->world_position - vector3_trans(closest_point, rotate)).unit_vect(); //mesh����sphere�ւ̃x�N�g��

			if (pair.body[0]->ALPcollider->shape == mesh->shape) {
				//body[0]�@���@mesh
				//body[1]�@���@sphere
				pair.contacts.addcontact(
					sphere->world_scale.x - distance,
					-n,
					closest_point,
					sphere->world_scale.x * vector3_quatrotate(n, sphere->world_orientation.conjugate())
				);
			}
			else {
				//body[0]�@���@sphere
				//body[1]�@���@mesh
				pair.contacts.addcontact(
					sphere->world_scale.x - distance,
					n,
					sphere->world_scale.x * vector3_quatrotate(n, sphere->world_orientation.conjugate()),
					closest_point
				);
			}
			return true;
		}
	}
	else {
		return false;

	}

	return false;
}
#pragma endregion


#pragma region BOX-PLANE
bool Physics_function::generate_contact_box_plane(const std::vector<ALP_Collider_mesh>::iterator& box_mesh, const std::vector<ALP_Collider_mesh>::iterator& plane_mesh, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& box = box_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& plane = plane_mesh->ALPcollider;

	Vector3 vertices[8] = {
		// obb���W�n�ł̊e���_�̃��[�J�����W
		Vector3(-box->world_scale.x, -box->world_scale.y, -box->world_scale.z),
		Vector3(-box->world_scale.x, -box->world_scale.y, +box->world_scale.z),
		Vector3(-box->world_scale.x, +box->world_scale.y, -box->world_scale.z),
		Vector3(-box->world_scale.x, +box->world_scale.y, +box->world_scale.z),
		Vector3(+box->world_scale.x, -box->world_scale.y, -box->world_scale.z),
		Vector3(+box->world_scale.x, -box->world_scale.y, +box->world_scale.z),
		Vector3(+box->world_scale.x, +box->world_scale.y, -box->world_scale.z),
		Vector3(+box->world_scale.x, +box->world_scale.y, +box->world_scale.z)
	};

	//Box�ƕ��ʂ̏Փ˔�����s��
	Matrix rotate, inverse_rotate;
	rotate = plane->world_orientation.get_rotate_matrix();
	rotate._41 = plane->world_position.x; //transpse�̓���
	rotate._42 = plane->world_position.y;
	rotate._43 = plane->world_position.z;
	rotate._44 = 1;
	Vector3 n(rotate._21, rotate._22, rotate._23);
	inverse_rotate = matrix_inverse(rotate);

	Vector3 p;
	p = vector3_trans(box->world_position, inverse_rotate); //���ʂ�"���_��ʂ�"�@����(0,1,0)"�̏�Ԃ̎���Box�̒��S���W(p.y�������ɂȂ�)
	//n = p.y > 0 ? n : -n; //�{�b�N�X�����ʉ��ɂ�������@���𔽑΂ɂ���

	float max_penetrate = 0.0f;
	Vector3 pointbox, pointplane;
	Vector3 vs;

	for (int i = 0; i < 8; i++) {
		// ���_�̃��[���h���W
		vs = vector3_quatrotate(vertices[i], box->world_orientation);

		// -�ʖ@���̓��ς��Ƃ�(�ʕ����ւ̒���)
		float rA = vector3_dot(-n, vs);

		if (max_penetrate < rA - p.y) {
			max_penetrate = rA - p.y;
			pointbox = vertices[i];

			// ���_��plane��Ԃ�
			vs = vector3_trans(vs + box->world_position, inverse_rotate);
			pointplane = vs;
			pointplane.y = 0;
		}
	}

	if (max_penetrate != 0) {
		if (pair.body[0]->ALPcollider->shape == plane->shape) {
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

bool Physics_function::generate_contact_box_box(const std::vector<ALP_Collider_mesh>::iterator& bA, const std::vector<ALP_Collider_mesh>::iterator& bB, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& boxA = bA->ALPcollider;
	const std::list<ALP_Collider>::iterator& boxB = bB->ALPcollider;

	Matrix m;
	m = boxA->world_orientation.get_rotate_matrix();
	OBB obbA;
	obbA.world_position = boxA->world_position;
	obbA.u_axes[0].x = m._11; obbA.u_axes[0].y = m._12; obbA.u_axes[0].z = m._13;
	obbA.u_axes[1].x = m._21; obbA.u_axes[1].y = m._22; obbA.u_axes[1].z = m._23;
	obbA.u_axes[2].x = m._31; obbA.u_axes[2].y = m._32; obbA.u_axes[2].z = m._33;
	obbA.half_width = boxA->half_size * boxA->world_scale;
	obbA.world_orientation = boxA->world_orientation;
	assert(!isnan(obbA.world_orientation.norm()));

	m = boxB->world_orientation.get_rotate_matrix();
	OBB obbB;
	obbB.world_position = boxB->world_position;
	obbB.u_axes[0].x = m._11; obbB.u_axes[0].y = m._12; obbB.u_axes[0].z = m._13;
	obbB.u_axes[1].x = m._21; obbB.u_axes[1].y = m._22; obbB.u_axes[1].z = m._23;
	obbB.u_axes[2].x = m._31; obbB.u_axes[2].y = m._32; obbB.u_axes[2].z = m._33;
	obbB.half_width = boxB->half_size * boxB->world_scale;
	obbB.world_orientation = boxB->world_orientation;
	assert(!isnan(obbB.world_orientation.norm()));

	float smallest_penetration = FLT_MAX;	//�ŏ��߂荞�ݗ�
	int smallest_axis[2];	//�ŏ��߂荞�ݗʂ𓾂��������̍쐬�Ɏg�p�����eOBB�̃��[�J�����ԍ� �Ӂ~�ӗp��2��
	SAT_TYPE smallest_case;	//�Փ˂̎��
	if (!sat_obb_obb(obbA, obbB, smallest_penetration, smallest_axis, smallest_case)) return false;

	//obbB�̒��_��obbA�̖ʂƏՓ˂����ꍇ
	if (smallest_case == SAT_TYPE::POINTB_FACETA)
	{
		Vector3 d = obbB.world_position - obbA.world_position;	//obbA����obbB�ւ̑��Έʒu
		Vector3 Wn = obbA.u_axes[smallest_axis[0]];	//obbA�̏Փ˖ʂ̖@���ƕ��s��obbA�̃��[�J�����x�N�g��
		if (vector3_dot(Wn, d) < 0)	//obbA��obbB�̈ʒu�֌W���Փ˖ʂ̖@���x�N�g�������肷��
		{
			Wn = Wn * -1.0f;
		}
		Wn = Wn.unit_vect();

		//boxB�̃��[�J�����W�n�ł̍ŋߓ_(p1) obb1��8���_�̂����̂ǂꂩ
		Vector3 p1 = obbB.half_width;	//obb1�̊e�ӂ̒����́Aobb1�̏d�S����ڐG�_(p)�ւ̑��Έʒu�̎肪����ɂȂ�
		//obb0��obb1�̈ʒu�֌W(d)���ڐG�_(p)�����߂�
		if (vector3_dot(obbB.u_axes[0], +Wn) > 0) p1.x = -p1.x;
		if (vector3_dot(obbB.u_axes[1], +Wn) > 0) p1.y = -p1.y;
		if (vector3_dot(obbB.u_axes[2], +Wn) > 0) p1.z = -p1.z;

		//boxA�̋t�s��̍쐬
		Matrix rotate, inverse_rotate;
		rotate = boxA->world_orientation.get_rotate_matrix();
		rotate._41 = boxA->world_position.x; //transpse�̓���
		rotate._42 = boxA->world_position.y;
		rotate._43 = boxA->world_position.z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p1��obbA�̃��[�J�����W�n��
		Vector3 P = vector3_quatrotate(p1, boxB->world_orientation) + boxB->world_position;
		Vector3 c = vector3_trans(P, inverse_rotate);

		//obbA�̍ŋߓ_�����߂�
		Vector3 p0 = c;
		Vector3 box_halfsize = boxA->half_size * boxA->world_scale;
		if (c.x > +box_halfsize.x)p0.x = +box_halfsize.x;
		if (c.x < -box_halfsize.x)p0.x = -box_halfsize.x;

		if (c.y > +box_halfsize.y)p0.y = +box_halfsize.y;
		if (c.y < -box_halfsize.y)p0.y = -box_halfsize.y;

		if (c.z > +box_halfsize.z)p0.z = +box_halfsize.z;
		if (c.z < -box_halfsize.z)p0.z = -box_halfsize.z;

		pair.contacts.addcontact(
			smallest_penetration,
			-Wn,
			p0,
			p1
		);
	}
	//obbA�̒��_��obbB�̖ʂƏՓ˂����ꍇ
	else if (smallest_case == SAT_TYPE::POINTA_FACETB)
	{
		Vector3 d = obbB.world_position - obbA.world_position;
		Vector3 Wn = obbB.u_axes[smallest_axis[1]];
		if (vector3_dot(Wn, d) < 0)
		{
			Wn = Wn * -1.0f;
		}
		Wn = Wn.unit_vect();

		Vector3 p0 = obbA.half_width;
		//obb0��obb1�̈ʒu�֌W(d)���ڐG�_(p)�����߂�
		if (vector3_dot(obbA.u_axes[0], -Wn) > 0) p0.x = -p0.x;
		if (vector3_dot(obbA.u_axes[1], -Wn) > 0) p0.y = -p0.y;
		if (vector3_dot(obbA.u_axes[2], -Wn) > 0) p0.z = -p0.z;

		//box0�̋t�s��̍쐬
		Matrix rotate, inverse_rotate;
		rotate = boxB->world_orientation.get_rotate_matrix();
		rotate._41 = boxB->world_position.x; //transpse�̓���
		rotate._42 = boxB->world_position.y;
		rotate._43 = boxB->world_position.z;
		rotate._44 = 1;
		inverse_rotate = matrix_inverse(rotate);

		//p0��obb0�̃��[�J�����W�n��
		Vector3 P = vector3_quatrotate(p0, boxA->world_orientation) + boxA->world_position;
		Vector3 c = vector3_trans(P, inverse_rotate);

		//obb0�̍ŋߓ_�����߂�
		Vector3 p1 = c;
		Vector3 box_halfsize = boxB->half_size * boxB->world_scale;
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
			-Wn,
			p0,
			p1
		);
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

#pragma region BOX-Capsule
bool Physics_function::generate_contact_box_capsule(const std::vector<ALP_Collider_mesh>::iterator& box_mesh, const std::vector<ALP_Collider_mesh>::iterator& capsule_mesh, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& capsule = capsule_mesh->ALPcollider;
	const std::list<ALP_Collider>::iterator& box = box_mesh->ALPcollider;

	Vector3 box_halfsize = box->world_scale * box->half_size;

	//box�̍��W�n�Ōv�Z
	Vector3 closest_box, closest_cap;
	float capsule_t = FLT_MAX;

	//box���W�n�ł�capsule�̏��
	Vector3 cuppos_boxcoord = vector3_quatrotate(capsule->world_position - box->world_position, box->world_orientation.conjugate());
	Vector3 cupsca_boxcoord = vector3_quatrotate(Vector3(0, capsule->world_scale.y, 0), capsule->world_orientation * box->world_orientation.conjugate());


	static float LL_save = 0;
	if (ImGui::Begin("NR_debug")) {
		ImGui::Text("%f", LL_save);
	}
	ImGui::End();

	bool is_crossing = false;
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
		abs(closest_cap.x) - capsule->world_scale.x > box_halfsize.x ||
		abs(closest_cap.y) - capsule->world_scale.x > box_halfsize.y ||
		abs(closest_cap.z) - capsule->world_scale.x > box_halfsize.z
		) return false;

	//box��̍ŋߓ_
	closest_box = closest_cap;
	if (closest_cap.x > +box_halfsize.x)closest_box.x = +box_halfsize.x;
	if (closest_cap.x < -box_halfsize.x)closest_box.x = -box_halfsize.x;

	if (closest_cap.y > +box_halfsize.y)closest_box.y = +box_halfsize.y;
	if (closest_cap.y < -box_halfsize.y)closest_box.y = -box_halfsize.y;

	if (closest_cap.z > +box_halfsize.z)closest_box.z = +box_halfsize.z;
	if (closest_cap.z < -box_halfsize.z)closest_box.z = -box_halfsize.z;

	float leng = capsule->world_scale.x - (closest_box - closest_cap).norm_sqr(); //�ђʗ�
	if (leng < FLT_EPSILON)return false;

	Vector3 n = (closest_cap - closest_box).unit_vect(); //box����sphere�ւ̃x�N�g�� boxcoord
	Vector3 Wn = vector3_quatrotate(n, box->world_orientation); //worldcoord

	LL_save = leng;
	if (pair.body[0]->ALPcollider->shape == box->shape) {
		//body[0]�@���@box
		//body[1]�@���@sphere
		pair.contacts.addcontact(
			leng,
			-Wn,
			closest_box,
			(Vector3(0, capsule->world_scale.y, 0) * capsule_t) + vector3_quatrotate(-Wn, capsule->world_orientation.conjugate()) * capsule->world_scale.x
			//vector3_quatrotate(vector3_quatrotate(closest_cap - n * capsule->world_scale.x, box->world_orientation.conjugate()) + box->world_position - capsule->world_position, capsule->world_orientation.conjugate())
		);
	}
	else {
		//body[0]�@���@sphere
		//body[1]�@���@box
		pair.contacts.addcontact(
			leng,
			+Wn,
			//vector3_quatrotate(vector3_quatrotate(closest_cap - n * capsule->world_scale.x, box->world_orientation.conjugate()) + box->world_position - capsule->world_position, capsule->world_orientation.conjugate()),
			(Vector3(0, capsule->world_scale.y, 0)* capsule_t) + vector3_quatrotate(-Wn, capsule->world_orientation.conjugate()) * capsule->world_scale.x,
			closest_box
		);
	}
	return true;
}
#pragma endregion

#pragma region BOX-MESH
bool Physics_function::generate_contact_box_mesh(const std::vector<ALP_Collider_mesh>::iterator& S1, const std::vector<ALP_Collider_mesh>::iterator& S2, Contacts::Contact_pair& pair) {
	return true;
}
#pragma endregion


#pragma region MESH-PLANE
bool Physics_function::generate_contact_mesh_plane(const std::vector<ALP_Collider_mesh>::iterator& S1, const std::vector<ALP_Collider_mesh>::iterator& S2, Contacts::Contact_pair& pair) {
	return true;
}
#pragma endregion

#pragma region MESH-MESH

bool GC_concave_mesh_mesh(const ALP_Collider& S1, const std::vector<ALP_Collider_mesh>::iterator& S2, int concave_num, Contacts::Contact_pair& pair) {
	return false;
}

bool Physics_function::generate_contact_mesh_mesh(const std::vector<ALP_Collider_mesh>::iterator& mA, const std::vector<ALP_Collider_mesh>::iterator& mB, Contacts::Contact_pair& pair) {
	const std::list<ALP_Collider>::iterator& meshA = mA->ALPcollider;
	const std::list<ALP_Collider>::iterator& meshB = mB->ALPcollider;

	float smallest_penetration = FLT_MAX;	//�ŏ��߂荞�ݗ�
	int smallest_facetID[2];	//�ŏ��߂荞�ݗʂ𓾂��������̍쐬�Ɏg�p�����eOBB�̃��[�J�����ԍ� �Ӂ~�ӗp��2��
	SAT_TYPE smallest_case;		//�Փ˂̎��

	//�ǂ����convex��object�̏ꍇ
	if (mA->mesh->is_Convex == true && mB->mesh->is_Convex == true) {
		if (!sat_convex_mesh_mesh(mA, mB, smallest_penetration, smallest_facetID, smallest_case))return false;

		Quaternion offset_quatBA = meshB->world_orientation * meshA->world_orientation.conjugate();
		Quaternion offset_quatAB = meshA->world_orientation * meshB->world_orientation.conjugate();
		Vector3 offset_posBA = meshB->world_position - meshA->world_position;
		Vector3 offset_posAB = meshA->world_position - meshB->world_position;

		//SB�̒��_��SA�̖ʂƏՓ˂����ꍇ
		if (smallest_case == SAT_TYPE::POINTB_FACETA)
		{
			const std::vector<ALP_Collider_mesh>::iterator& facet_mesh = mA;
			const std::vector<ALP_Collider_mesh>::iterator& vertex_mesh = mB;

			const std::list<ALP_Collider>::iterator& facet_coll = facet_mesh->ALPcollider;
			const std::list<ALP_Collider>::iterator& vertex_coll = vertex_mesh->ALPcollider;

			Quaternion offset_quatVF = vertex_coll->world_orientation * facet_coll->world_orientation.conjugate();
			Quaternion offset_quatFV = facet_coll->world_orientation * vertex_coll->world_orientation.conjugate();
			Vector3 offset_posVF = vertex_coll->world_position - facet_coll->world_position;
			Vector3 offset_posFV = facet_coll->world_position - vertex_coll->world_position;

			assert(smallest_facetID[1] == -1);
			const Facet& nerest_facet = facet_mesh->mesh->facets.at(smallest_facetID[0]);

			Vector3 axisF = nerest_facet.normal.unit_vect();
			Vector3 axisW = vector3_quatrotate(axisF, facet_coll->world_orientation).unit_vect();
			if (vector3_dot(axisW, offset_posAB) < 0) {
				axisF = -axisF;
				axisW = -axisW;
			}

			//vertex_coll�̂ǂ̒��_���ŋߓ_�����߂�
			u_int nearest_pointID;
			Vector3 pB;
			{
				float max_len = -FLT_MAX;
				Vector3 axisV = vector3_quatrotate(axisF, offset_quatFV);

				for (u_int v_num = 0; v_num < vertex_mesh->mesh->vertex_num; v_num++) {

					if (vector3_dot(vertex_mesh->mesh->vertices->at(v_num), axisV) > max_len) {
						max_len = vector3_dot(vertex_mesh->mesh->vertices->at(v_num), axisV);
						nearest_pointID = v_num;
					}
				}
			}
			pB = vertex_mesh->mesh->vertices->at(nearest_pointID);
			pB *= vertex_coll->world_scale;

			//��L��p0��facet_coll�̍ŋߖʏ�̂ǂ��ɂ���̂�
			Vector3 pA;
			{

				Vector3 p = vector3_quatrotate(vector3_quatrotate(pB, vertex_coll->world_orientation) + offset_posVF, facet_coll->world_orientation.conjugate());
				p /= facet_coll->world_scale;
				float min_len = FLT_MAX;
				Vector3 n_pos;


				for (u_int f_num = 0; f_num < facet_mesh->mesh->facet_num; f_num++) {

					if (vector3_dot(facet_mesh->mesh->facets.at(f_num).normal, -axisF) < 0.5)continue; //�Փ˖@���Ɣ�ׂ�

					//���b�V���Ɠ_�̍ŋߓ_�����߂�
					get_closestP_point_triangle(p,
						facet_mesh->mesh->vertices->at(facet_mesh->mesh->facets.at(f_num).vertexID[0]),
						facet_mesh->mesh->vertices->at(facet_mesh->mesh->facets.at(f_num).vertexID[1]),
						facet_mesh->mesh->vertices->at(facet_mesh->mesh->facets.at(f_num).vertexID[2]),
						facet_mesh->mesh->facets.at(f_num).normal,
						n_pos
					);

					if ((p - n_pos).norm() < min_len) {
						pA = n_pos;
						min_len = (p - n_pos).norm();
					}
				}

				pA *= facet_coll->world_scale;
			}

			pair.contacts.addcontact(
				smallest_penetration,
				axisW,
				pA,
				pB
			);
		}

		//SA�̒��_��SB�̖ʂƏՓ˂����ꍇ
		if (smallest_case == SAT_TYPE::POINTA_FACETB)
		{
			const std::vector<ALP_Collider_mesh>::iterator& facet_mesh = mB;
			const std::vector<ALP_Collider_mesh>::iterator& vertex_mesh = mA;

			const std::list<ALP_Collider>::iterator& facet_coll = facet_mesh->ALPcollider;
			const std::list<ALP_Collider>::iterator& vertex_coll = vertex_mesh->ALPcollider;

			Quaternion offset_quatVF = vertex_coll->world_orientation * facet_coll->world_orientation.conjugate();
			Quaternion offset_quatFV = facet_coll->world_orientation * vertex_coll->world_orientation.conjugate();
			Vector3 offset_posVF = vertex_coll->world_position - facet_coll->world_position;
			Vector3 offset_posFV = facet_coll->world_position - vertex_coll->world_position;

			assert(smallest_facetID[0] == -1);
			const Facet& nerest_facet = facet_mesh->mesh->facets.at(smallest_facetID[1]);

			Vector3 axisF = nerest_facet.normal.unit_vect();
			Vector3 axisW = vector3_quatrotate(axisF, facet_coll->world_orientation).unit_vect();
			if (vector3_dot(axisW, offset_posBA) < 0) {
				axisF = -axisF;
				axisW = -axisW;
			}

			//vertex_coll�̂ǂ̒��_���ŋߓ_�����߂�
			u_int nearest_pointID = 0;
			Vector3 pB;
			{
				float max_len = -FLT_MAX;
				Vector3 axisV = vector3_quatrotate(axisF, offset_quatFV);

				for (u_int v_num = 0; v_num < vertex_mesh->mesh->vertex_num; v_num++) {

					if (vector3_dot(vertex_mesh->mesh->vertices->at(v_num), axisV) > max_len) {
						max_len = vector3_dot(vertex_mesh->mesh->vertices->at(v_num), axisV);
						nearest_pointID = v_num;
					}
				}
			}
			pB = vertex_mesh->mesh->vertices->at(nearest_pointID);
			pB *= vertex_coll->world_scale;

			//��L��pB��facet_coll�̍ŋߖʏ�̂ǂ��ɂ���̂�
			Vector3 pA;
			{

				Vector3 p = vector3_quatrotate(vector3_quatrotate(pB, vertex_coll->world_orientation) + offset_posVF, facet_coll->world_orientation.conjugate());
				p /= facet_coll->world_scale;
				float min_len = FLT_MAX;
				Vector3 n_pos;


				for (u_int f_num = 0; f_num < facet_mesh->mesh->facet_num; f_num++) {

					if (vector3_dot(facet_mesh->mesh->facets.at(f_num).normal, -axisF) < 0.5)continue; //�Փ˖@���Ɣ�ׂ�

					//���b�V���Ɠ_�̍ŋߓ_�����߂�
					get_closestP_point_triangle(p,
						facet_mesh->mesh->vertices->at(facet_mesh->mesh->facets.at(f_num).vertexID[0]),
						facet_mesh->mesh->vertices->at(facet_mesh->mesh->facets.at(f_num).vertexID[1]),
						facet_mesh->mesh->vertices->at(facet_mesh->mesh->facets.at(f_num).vertexID[2]),
						facet_mesh->mesh->facets.at(f_num).normal,
						n_pos
					);

					if ((p - n_pos).norm() < min_len) {
						pA = n_pos;
						min_len = (p - n_pos).norm();
					}
				}

				pA *= facet_coll->world_scale;
			}

			pair.contacts.addcontact(
				smallest_penetration,
				-axisW,
				pB,
				pA
			);
		}


		//SA��SB�̕ӓ��m���Փ˂����ꍇ
		else if (smallest_case == SAT_TYPE::EDGE_EDGE)
		{
			Quaternion offset_quatAB = meshA->world_orientation * meshB->world_orientation.conjugate();
			Quaternion offset_quatBA = meshB->world_orientation * meshA->world_orientation.conjugate();
			Vector3 offset_posAB = meshA->world_position - meshB->world_position;
			Vector3 offset_posBA = meshB->world_position - meshA->world_position;

			const Edge& edgeA = mA->mesh->edges.at(smallest_facetID[0]);
			const Edge& edgeB = mB->mesh->edges.at(smallest_facetID[1]);

			Vector3 edgeA_p[2] = {
				{mA->mesh->vertices->at(edgeA.vertexID[0]) * meshA->world_scale},
				{mA->mesh->vertices->at(edgeA.vertexID[1]) * meshA->world_scale}
			};
			Vector3 edgeB_p[2] = {
				{mB->mesh->vertices->at(edgeB.vertexID[0]) * meshB->world_scale},
				{mB->mesh->vertices->at(edgeB.vertexID[1]) * meshB->world_scale}
			};

			Vector3 edgeA_vec = (edgeA_p[0] - edgeA_p[1]).unit_vect();
			Vector3 edgeB_vec = (edgeB_p[0] - edgeB_p[1]).unit_vect();

			//SB�̏���SA�̍��W�n�Ɏ����Ă���
			//Vector3 edgeB_p_A = vector3_quatrotate(edgeB_p[0], offset_quatBA) + offset_posBA;
			Vector3 edgeB_p_A = vector3_quatrotate(vector3_quatrotate(edgeB_p[0], meshB->world_orientation) + offset_posBA, meshA->world_orientation.conjugate());
			Vector3 edgeB_v_A = vector3_quatrotate(edgeB_vec, offset_quatBA);

			//SA�̍��W�n��axis�̐���
			Vector3 axisA = vector3_cross(edgeA_vec, edgeB_v_A);
			axisA = axisA.unit_vect();

			//axis��world���W�n��
			Vector3 axisW = vector3_quatrotate(axisA, meshA->world_orientation);

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
			pair.contacts.addcontact(
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