//#define _XM_NO_INTRINSICS_

#include "ALP_resolve_contact.h"


#include "ALP__physics_manager.h"
#include "ALP_physics.h"

#include "ALP_joint.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

using namespace DirectX;

//:::::::::::::::::::::::::::
#pragma region resolve_contact
//:::::::::::::::::::::::::::
//#ifdef PHYICSE_USED_SIMD
#if 1

//�Փˉ���

void CalcTangentVector(const Vector3& normal, const XMVECTOR& vrel, XMVECTOR& tangent1, XMVECTOR& tangent2)
{
#if 0
	Vector3 vec(1.0f, 0.0f, 0.0f);
	Vector3 n(normal);
	n[0] = 0.0f;
	if (n.norm() < FLT_EPSILON) {
		vec = Vector3(0.0f, 1.0f, 0.0f);
	}

	XMVECTOR xmnorm = XMLoadFloat3(&normal);
	tangent1 = XMVector3Normalize(XMVector3Cross(xmnorm, XMLoadFloat3(&vec)));
	tangent2 = XMVector3Normalize(XMVector3Cross(tangent1, xmnorm));
#else
	// ���Α��x�̕�����tangent���Ƃ�(�K���ȕ�����tangent���Ƃ��friction�����܂������Ȃ��Ȃ�)

	// ��̑��Α��x�̌�������tangent�����߂�
	XMVECTOR xmnorm = XMLoadFloat3(&normal);
	XMVECTOR dot = XMVector3Dot(xmnorm, vrel);

	if (XMVectorGetX(dot) < 1 + FLT_EPSILON && XMVectorGetX(dot) > 1 - FLT_EPSILON) {
		// �������Α��x�ƏՓ˖@�������������Ȃ�K����tangent�����߂�
		Vector3 vec(1.0f, 0.0f, 0.0f);
		Vector3 n(normal);
		n[0] = 0.0f;
		if (n.norm() < FLT_EPSILON) {
			vec = Vector3(0.0f, 1.0f, 0.0f);
		}
		tangent1 = XMVector3Normalize(XMVector3Cross(xmnorm, XMLoadFloat3(&vec)));
		tangent2 = XMVector3Normalize(XMVector3Cross(tangent1, xmnorm));

	}
	else {
		tangent1 = XMVector3Normalize(XMVectorSubtract(vrel, XMVectorMultiply(xmnorm, XMVector3Dot(xmnorm, vrel))));
		tangent2 = XMVector3Normalize(XMVector3Cross(tangent1, xmnorm));
	}
#endif

}
bool Calc_joint_effect(ALP_Joint* joint, float inv_duration)
{
	ALP_Physics* ALPphysics[2];
	ALP_Solverbody* solverbody[2];
	world_trans* transform[2];
	XMVECTOR position[2];

	transform[0] = &joint->ALPcollider[0]->transform;
	transform[1] = &joint->ALPcollider[1]->transform;
	ALPphysics[0] = joint->ALPcollider[0]->get_ALPphysics();
	ALPphysics[1] = joint->ALPcollider[1]->get_ALPphysics();
	solverbody[0] = ALPphysics[0]->solve;
	solverbody[1] = ALPphysics[1]->solve;

	float penetrate = 0;

	if (joint->userjoint->limit_effect(joint->limit_constraint_pos[0], joint->limit_constraint_pos[1], penetrate)) {
		if (penetrate == 0)return false;

		//anchor���ꂼ���local���W
		const XMVECTOR rA = XMVector3Rotate(XMLoadFloat3(&joint->limit_constraint_pos[0]), XMLoadFloat4(&transform[0]->orientation));
		const XMVECTOR rB = XMVector3Rotate(XMLoadFloat3(&joint->limit_constraint_pos[1]), XMLoadFloat4(&transform[1]->orientation));

		//anchor���ꂼ���world���W
		position[0] = XMVectorAdd(XMLoadFloat3(&transform[0]->position), rA);
		position[1] = XMVectorAdd(XMLoadFloat3(&transform[1]->position), rB);
		XMVECTOR direction = XMVectorSubtract(position[1], position[0]);
		XMVECTOR distance = XMVector3Length(direction);
		if (XMVectorGetX(distance) == 0)return false;

		direction = XMVectorDivide(direction, distance);

		XMVECTOR velocityA = XMVectorAdd(XMLoadFloat3(&ALPphysics[0]->linear_velocity()), XMVector3Cross(XMLoadFloat3(&ALPphysics[0]->angula_velocity()), rA));
		XMVECTOR velocityB = XMVectorAdd(XMLoadFloat3(&ALPphysics[1]->linear_velocity()), XMVector3Cross(XMLoadFloat3(&ALPphysics[1]->angula_velocity()), rB));
		XMVECTOR relativeVelocity = XMVectorSubtract(velocityA, velocityB);

		const float& term1 = ALPphysics[0]->inverse_mass();
		const float& term2 = ALPphysics[1]->inverse_mass();
		XMVECTOR tA, tB;

		float term3, term4, denominator;
		XMVECTOR axis;
		axis = direction;
		tA = XMVector3Cross(rA, axis);
		tB = XMVector3Cross(rB, axis);
		tA = XMVector3Transform(tA, ALPphysics[0]->solve->inv_tensor);
		tB = XMVector3Transform(tB, ALPphysics[1]->solve->inv_tensor);
		tA = XMVector3Cross(tA, rA);
		tB = XMVector3Cross(tB, rB);
		term3 = XMVectorGetX(XMVector3Dot(axis, tA));
		term4 = XMVectorGetX(XMVector3Dot(axis, tB));
		denominator = term1 + term2 + term3 + term4;

		joint->constraint_limit.jacDiagInv = 1.0f / denominator;

		joint->constraint_limit.rhs = -XMVectorGetX(XMVector3Dot(relativeVelocity, direction)); // velocity error
		joint->constraint_limit.rhs += joint->limit_bias * penetrate * inv_duration; // position error
		joint->constraint_limit.rhs *= joint->constraint_limit.jacDiagInv;
		joint->constraint_limit.lowerlimit = -FLT_MAX;
		joint->constraint_limit.upperlimit = +FLT_MAX;
		XMStoreFloat3(&joint->constraint_limit.axis, direction);

		Vector3 linervec, anglvec;
		XMStoreFloat3(&linervec, direction);

		joint->constraint_limit.accuminpulse = 0.0f;
	}
	else {
		return false;
	}


	return true;
}


void Physics_function::resolve_contact(std::list<Physics_function::ALP_Collider*>& ALP_colliders, std::vector<Contacts::Contact_pair*>& pairs, std::list<Physics_function::ALP_Joint*> l_joints, const float timescale) {

	const float inv_duration = 1 / (Physics_manager::physicsParams.timeStep / timescale);

	//::: �����p�I�u�W�F�N�g�̐��� :::::::::::
	std::vector<ALP_Solverbody> SBs;
	SBs.reserve(ALP_colliders.size());
	// SBs.resize(colliders.size()); //�A���C�����g��SIMD�ƃR���e�i����������??? reserve�ɂ����玡����
	{
		int count = 0;
		ALP_Solverbody SB;

		for (const auto& coll : ALP_colliders) {

			SB.delta_LinearVelocity = XMVectorZero();
			SB.delta_AngulaVelocity = XMVectorZero();

			if (coll->get_ALPphysics()->is_kinmatic_anglar) SB.inv_tensor = XMLoadFloat3x3(&coll->get_ALPphysics()->inverse_inertial_tensor());
			else SB.inv_tensor = XMLoadFloat3x3(&matrix33_zero());

			if (coll->get_ALPphysics()->is_kinmatic_linear) SB.inv_mass = coll->get_ALPphysics()->inverse_mass();
			else SB.inv_mass = 0;

			SB.Worient = XMLoadFloat4(&coll->transform.orientation);
			SB.Wposition = XMLoadFloat3(&coll->transform.position);

			SBs.emplace_back(SB);
			++count;
		}

		count = 0;

		for (auto& coll : ALP_colliders) {
			coll->get_ALPphysics()->solve = &SBs[count];
			count++;
		}

	}

	// joint��scene���܂����Őڑ��ł��邪�A�o����collider��scene��active�łȂ���Γ��삵�Ȃ��悤�ɂ���K�v�����邽��
	// �o����scene��active�Ȏ� �z��ɓ���A�v�Z���s��
	std::vector<Physics_function::ALP_Joint*> joints;
	for (auto& joint : l_joints) {
		if (joint->ALPcollider[0]->get_ALPphysics()->solve == nullptr) continue;
		if (joint->ALPcollider[1]->get_ALPphysics()->solve == nullptr) continue;
		joints.emplace_back(joint);
	}

	Collider_shape* coll[2];
	ALP_Collider* ALPcollider[2];
	ALP_Physics* ALPphysics[2];
	ALP_Solverbody* solverbody[2];


	//::: limit�̉e�����v�Z
	{
		for (auto& joint : joints) {


			if (Calc_joint_effect(joint, inv_duration) == false) {
				//limit�Ɉ����������Ă��Ȃ� -> �����N���Ȃ�
				joint->constraint_limit.jacDiagInv = 0.0f;
				joint->constraint_limit.rhs = 0.0f;
				joint->constraint_limit.lowerlimit = -FLT_MAX;
				joint->constraint_limit.upperlimit = +FLT_MAX;
				joint->constraint_limit.axis = Vector3(0, 0, 0);
			}
		}
	}

	// �S���̃Z�b�g�A�b�v
	{
		XMVECTOR position[2];
		for (auto& joint : joints) {

			ALPphysics[0] = joint->ALPcollider[0]->get_ALPphysics();
			ALPphysics[1] = joint->ALPcollider[1]->get_ALPphysics();
			solverbody[0] = ALPphysics[0]->solve;
			solverbody[1] = ALPphysics[1]->solve;

			//::: anchor�̉e�����v�Z
			for (int i = 0; i < joint->anchor_count; i++) {
				const Vector3& joint_posA = joint->anchor[i].posA;
				const Vector3& joint_posB = joint->anchor[i].posB;
				auto& constraint = joint->constraint[i];

				//anchor���ꂼ���local���W
				const XMVECTOR rA = XMVector3Rotate(XMLoadFloat3(&joint_posA), solverbody[0]->Worient);
				const XMVECTOR rB = XMVector3Rotate(XMLoadFloat3(&joint_posB), solverbody[1]->Worient);

				//anchor���ꂼ���world���W
				position[0] = solverbody[0]->Wposition + rA;
				position[1] = solverbody[1]->Wposition + rB;
				XMVECTOR direction = position[1] - position[0];
				XMVECTOR distance = XMVector3Length(direction);

				float rhs_pow = 0;
				{
					const float dis = fabsf(XMVectorGetX(distance) - joint->offset);
					if (0.0f < dis - joint->slop) {

						const int sign = (XMVectorGetX(distance) - joint->offset > 0) ? +1 : -1;

						// �S���_��Collider�̍��W�̍����̓��� �� offset���l������stretch��shrink�𔻒f����
						const float& bias = (XMVectorGetX(XMVector3Dot(
							position[0] - position[1],
							solverbody[0]->Wposition - solverbody[1]->Wposition))
							* sign
							< 0) ? joint->stretch_bias : joint->shrink_bias;

						rhs_pow = 0.9f * bias * sign * (dis - joint->slop) * inv_duration; // position error
					}
				}

				if (
					//XMVectorGetX(distance) < FLT_EPSILON || //�������I����Ă��Ȃ���� ������0�ɂȂ�
					fabsf(XMVectorGetX(distance) - joint->offset) < FLT_EPSILON || //offset���l�������l
					rhs_pow == 0 //bias��0�̎��Ȃ�
					) {
					//�ƂĂ��߂��ʒu�ɂ��� -> ��������Ȃ�
					constraint.jacDiagInv = 0.0f;
					constraint.rhs = 0.0f;
					constraint.lowerlimit = -FLT_MAX;
					constraint.upperlimit = +FLT_MAX;
					constraint.axis = Vector3(0, 0, 0);
					continue;
				}

				direction = XMVectorDivide(direction, distance);

				XMVECTOR velocityA = XMLoadFloat3(&ALPphysics[0]->linear_velocity()) + XMVector3Cross(XMLoadFloat3(&ALPphysics[0]->angula_velocity()), rA);
				XMVECTOR velocityB = XMLoadFloat3(&ALPphysics[1]->linear_velocity()) + XMVector3Cross(XMLoadFloat3(&ALPphysics[1]->angula_velocity()), rB);
				XMVECTOR relativeVelocity = velocityA - velocityB;

				const float& term1 = ALPphysics[0]->inverse_mass();
				const float& term2 = ALPphysics[1]->inverse_mass();
				XMVECTOR tA, tB;

				float term3, term4, denominator;
				XMVECTOR axis;
				axis = direction;
				tA = XMVector3Cross(rA, axis);
				tB = XMVector3Cross(rB, axis);
				tA = XMVector3Transform(tA, ALPphysics[0]->solve->inv_tensor);
				tB = XMVector3Transform(tB, ALPphysics[1]->solve->inv_tensor);
				tA = XMVector3Cross(tA, rA);
				tB = XMVector3Cross(tB, rB);
				term3 = XMVectorGetX(XMVector3Dot(axis, tA));
				term4 = XMVectorGetX(XMVector3Dot(axis, tB));
				denominator = term1 + term2 + term3 + term4;


				constraint.jacDiagInv = 1.0f / denominator;

				constraint.rhs = -XMVectorGetX(XMVector3Dot(relativeVelocity, direction)); // velocity error

				constraint.rhs += rhs_pow; //��Ōv�Z���Ă���

				constraint.rhs *= constraint.jacDiagInv;
				constraint.lowerlimit = -FLT_MAX;
				constraint.upperlimit = +FLT_MAX;
				XMStoreFloat3(&constraint.axis, direction);

				constraint.accuminpulse = 0.0f;
			}

		}
	}

	// pair�̃Z�b�g�A�b�v
	for (auto& pair : pairs) {

		coll[0] = pair->body[0];
		coll[1] = pair->body[1];
		ALPcollider[0] = coll[0]->get_ALPcollider();
		ALPcollider[1] = coll[1]->get_ALPcollider();
		ALPphysics[0] = ALPcollider[0]->get_ALPphysics();
		ALPphysics[1] = ALPcollider[1]->get_ALPphysics();
		solverbody[0] = ALPphysics[0]->solve;
		solverbody[1] = ALPphysics[1]->solve;

		// ���C�̊l��
		pair->contacts.friction = sqrtf(ALPphysics[0]->dynamic_friction * ALPphysics[1]->dynamic_friction);

		//::: �Փ˓_���̍X�V ::::::::
		for (int C_num = 0; C_num < pair->contacts.contact_num; C_num++) {
			Contactpoint& cp = pair->contacts.contactpoints[C_num];

			const XMVECTOR rA = XMVector3Rotate(XMLoadFloat3(&coll[0]->local_position) + XMVector3Rotate(XMLoadFloat3(&cp.point[0]), XMLoadFloat4(&coll[0]->local_orientation)), solverbody[0]->Worient);
			const XMVECTOR rB = XMVector3Rotate(XMLoadFloat3(&coll[1]->local_position) + XMVector3Rotate(XMLoadFloat3(&cp.point[1]), XMLoadFloat4(&coll[1]->local_orientation)), solverbody[1]->Worient);

			// �����W���̊l��
			// �p���̏Փ˂̏ꍇ�����W����0�ɂ���
			const float restitution = (pair->type == Pairtype::new_pair ? 0.5f * (ALPphysics[0]->restitution + ALPphysics[1]->restitution) : 0.0f);


			//�Փˎ��̂��ꂼ��̑��x
			XMVECTOR pdota;
			pdota = XMVector3Cross(XMLoadFloat3(&ALPphysics[0]->angula_velocity()), rA);
			pdota = pdota + XMLoadFloat3(&ALPphysics[0]->linear_velocity());

			XMVECTOR pdotb;
			pdotb = XMVector3Cross(XMLoadFloat3(&ALPphysics[1]->angula_velocity()), rB);
			pdotb = pdotb + XMLoadFloat3(&ALPphysics[1]->linear_velocity());

			//�Փˎ��̏Փ˕��ʖ@�������̑��Α��x(���ǏՓ˂Ɏg���͖̂@�������ւ̑���)
			XMVECTOR vrel = pdota - pdotb;

			XMVECTOR
				tangent1, //normal�ɑ΂���z����
				tangent2; //normal�ɑ΂���x����
			CalcTangentVector(cp.normal, vrel, tangent1, tangent2);

			//Baraff[1997]�̎�(8-18)�̕���(denominator)�����߂�
			const float& term1 = ALPphysics[0]->inverse_mass();
			const float& term2 = ALPphysics[1]->inverse_mass();
			XMVECTOR tA, tB;

			float term3, term4, denominator;
			XMVECTOR axis;
			// Normal
			{
				// Baraff[1997]�̎�(8-18)�̕���(denominator)�����߂�
				axis = XMLoadFloat3(&cp.normal);
				tA = XMVector3Cross(rA, axis);
				tB = XMVector3Cross(rB, axis);
				tA = XMVector3Transform(tA, ALPphysics[0]->solve->inv_tensor);
				tB = XMVector3Transform(tB, ALPphysics[1]->solve->inv_tensor);
				tA = XMVector3Cross(tA, rA);
				tB = XMVector3Cross(tB, rB);
				term3 = XMVectorGetX(XMVector3Dot(axis, tA));
				term4 = XMVectorGetX(XMVector3Dot(axis, tB));
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[0].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���
				cp.constraint[0].rhs = -(1.0f + restitution) * XMVectorGetX(XMVector3Dot(axis, vrel)); //Baraff1997(8-18)�̕��q

				if (0.0f < cp.distance - Physics_manager::physicsParams.slop)
					cp.constraint[0].rhs += (Physics_manager::physicsParams.bias * (cp.distance - Physics_manager::physicsParams.slop)) * inv_duration; //�߂荞�݂𒼂���

				cp.constraint[0].rhs *= cp.constraint[0].jacDiagInv;
				cp.constraint[0].lowerlimit = 0.0f;
				cp.constraint[0].upperlimit = FLT_MAX;
				XMStoreFloat3(&cp.constraint[0].axis, axis);
			}

			// Tangent1
			{
				axis = tangent1;
				tA = XMVector3Cross(rA, axis);
				tB = XMVector3Cross(rB, axis);
				tA = XMVector3Transform(tA, ALPphysics[0]->solve->inv_tensor);
				tB = XMVector3Transform(tB, ALPphysics[1]->solve->inv_tensor);
				tA = XMVector3Cross(tA, rA);
				tB = XMVector3Cross(tB, rB);
				term3 = XMVectorGetX(XMVector3Dot(axis, tA));
				term4 = XMVectorGetX(XMVector3Dot(axis, tB));
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[1].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���
				cp.constraint[1].rhs = -XMVectorGetX(XMVector3Dot(axis, vrel)); //Baraff1997(8-18)�̕��q
				cp.constraint[1].rhs *= cp.constraint[1].jacDiagInv;
				cp.constraint[1].lowerlimit = 0.0f;
				cp.constraint[1].upperlimit = 0;
				XMStoreFloat3(&cp.constraint[1].axis, axis);
			}

			// Tangent2
			{
				axis = tangent2;
				tA = XMVector3Cross(rA, axis);
				tB = XMVector3Cross(rB, axis);
				tA = XMVector3Transform(tA, ALPphysics[0]->solve->inv_tensor);
				tB = XMVector3Transform(tB, ALPphysics[1]->solve->inv_tensor);
				tA = XMVector3Cross(tA, rA);
				tB = XMVector3Cross(tB, rB);
				term3 = XMVectorGetX(XMVector3Dot(axis, tA));
				term4 = XMVectorGetX(XMVector3Dot(axis, tB));
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[2].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���?
				cp.constraint[2].rhs = -XMVectorGetX(XMVector3Dot(axis, vrel)); //Baraff1997(8-18)�̕��q
				cp.constraint[2].rhs *= cp.constraint[2].jacDiagInv;
				cp.constraint[2].lowerlimit = 0.0f;
				cp.constraint[2].upperlimit = 0;
				XMStoreFloat3(&cp.constraint[2].axis, axis);
			}


		}
	}


	//::: �ω��ʂ����߂� :::::::::::::::
	for (auto& pair : pairs) {

		coll[0] = pair->body[0];
		coll[1] = pair->body[1];
		ALPcollider[0] = coll[0]->get_ALPcollider();
		ALPcollider[1] = coll[1]->get_ALPcollider();
		ALPphysics[0] = coll[0]->get_ALPcollider()->get_ALPphysics();
		ALPphysics[1] = coll[1]->get_ALPcollider()->get_ALPphysics();
		solverbody[0] = ALPphysics[0]->solve;
		solverbody[1] = ALPphysics[1]->solve;

		for (int C_num = 0; C_num < pair->contacts.contact_num; C_num++) {
			//�Փ˓_�̏��
			const Contactpoint& cp = pair->contacts.contactpoints[C_num];

			const XMVECTOR rA = XMVector3Rotate(XMLoadFloat3(&coll[0]->local_position) + XMVector3Rotate(XMLoadFloat3(&cp.point[0]), XMLoadFloat4(&coll[0]->local_orientation)), solverbody[0]->Worient);
			const XMVECTOR rB = XMVector3Rotate(XMLoadFloat3(&coll[1]->local_position) + XMVector3Rotate(XMLoadFloat3(&cp.point[1]), XMLoadFloat4(&coll[1]->local_orientation)), solverbody[1]->Worient);

			for (int k = 0; k < 3; k++) {
				const float& deltaImpulse = cp.constraint[k].accuminpulse;
				const XMVECTOR axis = XMLoadFloat3(&cp.constraint[k].axis);

				solverbody[0]->delta_LinearVelocity = solverbody[0]->delta_LinearVelocity + XMVectorScale(axis, deltaImpulse * solverbody[0]->inv_mass);
				solverbody[0]->delta_AngulaVelocity = solverbody[0]->delta_AngulaVelocity + XMVectorScale(XMVector3Transform(XMVector3Cross(rA, axis), solverbody[0]->inv_tensor), deltaImpulse);
				solverbody[1]->delta_LinearVelocity = solverbody[1]->delta_LinearVelocity - XMVectorScale(axis, deltaImpulse * solverbody[1]->inv_mass);
				solverbody[1]->delta_AngulaVelocity = solverbody[1]->delta_AngulaVelocity - XMVectorScale(XMVector3Transform(XMVector3Cross(rB, axis), solverbody[1]->inv_tensor), deltaImpulse);
			}
		}

	}

	{
		world_trans* transform[2];
		XMVECTOR position[2];
		for (auto& joint : joints) {
			transform[0] = &joint->ALPcollider[0]->transform;
			transform[1] = &joint->ALPcollider[1]->transform;
			ALPphysics[0] = joint->ALPcollider[0]->get_ALPphysics();
			ALPphysics[1] = joint->ALPcollider[1]->get_ALPphysics();
			solverbody[0] = ALPphysics[0]->solve;
			solverbody[1] = ALPphysics[1]->solve;

			const XMVECTOR obj0_Worient = XMLoadFloat4(&transform[0]->orientation);
			const XMVECTOR obj1_Worient = XMLoadFloat4(&transform[1]->orientation);


			for (int i = 0; i < joint->anchor_count; i++) {
				const Vector3 joint_posA = joint->anchor[i].posA;
				const Vector3 joint_posB = joint->anchor[i].posB;

				const XMVECTOR rA = XMVector3Rotate(XMLoadFloat3(&joint_posA), obj0_Worient);
				const XMVECTOR rB = XMVector3Rotate(XMLoadFloat3(&joint_posB), obj1_Worient);

				//�Փ˓_�̏��
				Constraint& constraint = joint->constraint[i];

				const float& deltaImpulse = constraint.accuminpulse;
				XMVECTOR axis = XMLoadFloat3(&constraint.axis);

				solverbody[0]->delta_LinearVelocity = solverbody[0]->delta_LinearVelocity + XMVectorScale(axis, deltaImpulse * solverbody[0]->inv_mass);
				solverbody[0]->delta_AngulaVelocity = solverbody[0]->delta_AngulaVelocity + XMVectorScale(XMVector3Transform(XMVector3Cross(rA, axis), solverbody[0]->inv_tensor), deltaImpulse);
				solverbody[1]->delta_LinearVelocity = solverbody[1]->delta_LinearVelocity - XMVectorScale(axis, deltaImpulse * solverbody[1]->inv_mass);
				solverbody[1]->delta_AngulaVelocity = solverbody[1]->delta_AngulaVelocity - XMVectorScale(XMVector3Transform(XMVector3Cross(rB, axis), solverbody[1]->inv_tensor), deltaImpulse);
			}

			{
				const Vector3 joint_posA = joint->limit_constraint_pos[0];
				const Vector3 joint_posB = joint->limit_constraint_pos[1];

				const XMVECTOR rA = XMVector3Rotate(XMLoadFloat3(&joint_posA), obj0_Worient);
				const XMVECTOR rB = XMVector3Rotate(XMLoadFloat3(&joint_posB), obj1_Worient);

				//�Փ˓_�̏��
				Constraint& constraint = joint->constraint_limit;

				const float& deltaImpulse = constraint.accuminpulse;
				XMVECTOR axis = XMLoadFloat3(&constraint.axis);

				solverbody[0]->delta_LinearVelocity = solverbody[0]->delta_LinearVelocity + XMVectorScale(axis, deltaImpulse * solverbody[0]->inv_mass);
				solverbody[0]->delta_AngulaVelocity = solverbody[0]->delta_AngulaVelocity + XMVectorScale(XMVector3Transform(XMVector3Cross(rA, axis), solverbody[0]->inv_tensor), deltaImpulse);
				solverbody[1]->delta_LinearVelocity = solverbody[1]->delta_LinearVelocity - XMVectorScale(axis, deltaImpulse * solverbody[1]->inv_mass);
				solverbody[1]->delta_AngulaVelocity = solverbody[1]->delta_AngulaVelocity - XMVectorScale(XMVector3Transform(XMVector3Cross(rB, axis), solverbody[1]->inv_tensor), deltaImpulse);

			}
		}
	}

	//::: �v�Z :::::::::
	for (int solver_iterations_count = 0; solver_iterations_count < Physics_manager::physicsParams.solver_iteration; solver_iterations_count++) {
		// �S���̉��Z
		for (auto& joint : joints) {

			{
				const Vector3 joint_posA = joint->limit_constraint_pos[0];
				const Vector3 joint_posB = joint->limit_constraint_pos[1];

				ALPphysics[0] = joint->ALPcollider[0]->get_ALPphysics();
				ALPphysics[1] = joint->ALPcollider[1]->get_ALPphysics();
				solverbody[0] = ALPphysics[0]->solve;
				solverbody[1] = ALPphysics[1]->solve;

				const XMVECTOR rA = XMVector3Rotate(XMLoadFloat3(&joint_posA), solverbody[0]->Worient);
				const XMVECTOR rB = XMVector3Rotate(XMLoadFloat3(&joint_posB), solverbody[1]->Worient);

				Constraint& constraint = joint->constraint_limit;
				const XMVECTOR axis = XMLoadFloat3(&constraint.axis);

				float delta_impulse = constraint.rhs;
				XMVECTOR delta_velocity[2];
				delta_velocity[0] = solverbody[0]->delta_LinearVelocity + XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA);
				delta_velocity[1] = solverbody[1]->delta_LinearVelocity + XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB);
				delta_impulse -= constraint.jacDiagInv * XMVectorGetX(XMVector3Dot(axis, delta_velocity[0] - delta_velocity[1]));

				float old_impulse = constraint.accuminpulse;
				constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
				delta_impulse = constraint.accuminpulse - old_impulse;

				solverbody[0]->delta_LinearVelocity = solverbody[0]->delta_LinearVelocity + XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass);
				solverbody[0]->delta_AngulaVelocity = solverbody[0]->delta_AngulaVelocity + XMVectorScale(XMVector3Transform(XMVector3Cross(rA, axis), solverbody[0]->inv_tensor), delta_impulse);
				solverbody[1]->delta_LinearVelocity = solverbody[1]->delta_LinearVelocity - XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass);
				solverbody[1]->delta_AngulaVelocity = solverbody[1]->delta_AngulaVelocity - XMVectorScale(XMVector3Transform(XMVector3Cross(rB, axis), solverbody[1]->inv_tensor), delta_impulse);
			}

			for (int i = 0; i < joint->anchor_count; i++) {

				const Vector3 joint_posA = joint->anchor[i].posA;
				const Vector3 joint_posB = joint->anchor[i].posB;

				ALPphysics[0] = joint->ALPcollider[0]->get_ALPphysics();
				ALPphysics[1] = joint->ALPcollider[1]->get_ALPphysics();
				solverbody[0] = ALPphysics[0]->solve;
				solverbody[1] = ALPphysics[1]->solve;

				const XMVECTOR rA = XMVector3Rotate(XMLoadFloat3(&joint_posA), solverbody[0]->Worient);
				const XMVECTOR rB = XMVector3Rotate(XMLoadFloat3(&joint_posB), solverbody[1]->Worient);

				Constraint& constraint = joint->constraint[i];
				const XMVECTOR axis = XMLoadFloat3(&constraint.axis);

				float delta_impulse = constraint.rhs;
				XMVECTOR delta_velocity[2];
				delta_velocity[0] = solverbody[0]->delta_LinearVelocity + XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA);
				delta_velocity[1] = solverbody[1]->delta_LinearVelocity + XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB);
				delta_impulse -= constraint.jacDiagInv * XMVectorGetX(XMVector3Dot(axis, delta_velocity[0] - delta_velocity[1]));

				float old_impulse = constraint.accuminpulse;
				constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
				delta_impulse = constraint.accuminpulse - old_impulse;

				solverbody[0]->delta_LinearVelocity = solverbody[0]->delta_LinearVelocity + XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass);
				solverbody[0]->delta_AngulaVelocity = solverbody[0]->delta_AngulaVelocity + XMVectorScale(XMVector3Transform(XMVector3Cross(rA, axis), solverbody[0]->inv_tensor), delta_impulse);
				solverbody[1]->delta_LinearVelocity = solverbody[1]->delta_LinearVelocity - XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass);
				solverbody[1]->delta_AngulaVelocity = solverbody[1]->delta_AngulaVelocity - XMVectorScale(XMVector3Transform(XMVector3Cross(rB, axis), solverbody[1]->inv_tensor), delta_impulse);
			}

		}

		for (auto& pair : pairs) {

			coll[0] = pair->body[0];
			coll[1] = pair->body[1];
			ALPcollider[0] = coll[0]->get_ALPcollider();
			ALPcollider[1] = coll[1]->get_ALPcollider();
			ALPphysics[0] = coll[0]->get_ALPcollider()->get_ALPphysics();
			ALPphysics[1] = coll[1]->get_ALPcollider()->get_ALPphysics();
			solverbody[0] = ALPphysics[0]->solve;
			solverbody[1] = ALPphysics[1]->solve;

			for (int C_num = 0; C_num < pair->contacts.contact_num; C_num++) {
				//�Փ˓_�̏��
				Contactpoint& cp = pair->contacts.contactpoints[C_num];

				const XMVECTOR rA = XMVector3Rotate(XMLoadFloat3(&coll[0]->local_position) + XMVector3Rotate(XMLoadFloat3(&cp.point[0]), XMLoadFloat4(&coll[0]->local_orientation)), solverbody[0]->Worient);
				const XMVECTOR rB = XMVector3Rotate(XMLoadFloat3(&coll[1]->local_position) + XMVector3Rotate(XMLoadFloat3(&cp.point[1]), XMLoadFloat4(&coll[1]->local_orientation)), solverbody[1]->Worient);

				{
					Constraint& constraint = cp.constraint[0];
					const XMVECTOR axis = XMLoadFloat3(&constraint.axis);

					float delta_impulse = constraint.rhs;
					XMVECTOR delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * XMVectorGetX(XMVector3Dot(axis, delta_velocity[0] - delta_velocity[1]));
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity = solverbody[0]->delta_LinearVelocity + XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass);
					solverbody[0]->delta_AngulaVelocity = solverbody[0]->delta_AngulaVelocity + XMVectorScale(XMVector3Transform(XMVector3Cross(rA, axis), solverbody[0]->inv_tensor), delta_impulse);
					solverbody[1]->delta_LinearVelocity = solverbody[1]->delta_LinearVelocity - XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass);
					solverbody[1]->delta_AngulaVelocity = solverbody[1]->delta_AngulaVelocity - XMVectorScale(XMVector3Transform(XMVector3Cross(rB, axis), solverbody[1]->inv_tensor), delta_impulse);
				}

				float max_friction = pair->contacts.friction * fabsf(cp.constraint[0].accuminpulse);
				cp.constraint[1].lowerlimit = -max_friction;
				cp.constraint[1].upperlimit = +max_friction;
				cp.constraint[2].lowerlimit = -max_friction;
				cp.constraint[2].upperlimit = +max_friction;

				{
					Constraint& constraint = cp.constraint[1];
					XMVECTOR axis = XMLoadFloat3(&constraint.axis);

					float delta_impulse = constraint.rhs;
					XMVECTOR delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * XMVectorGetX(XMVector3Dot(axis, delta_velocity[0] - delta_velocity[1]));
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity = solverbody[0]->delta_LinearVelocity + XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass);
					solverbody[0]->delta_AngulaVelocity = solverbody[0]->delta_AngulaVelocity + XMVectorScale(XMVector3Transform(XMVector3Cross(rA, axis), solverbody[0]->inv_tensor), delta_impulse);
					solverbody[1]->delta_LinearVelocity = solverbody[1]->delta_LinearVelocity - XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass);
					solverbody[1]->delta_AngulaVelocity = solverbody[1]->delta_AngulaVelocity - XMVectorScale(XMVector3Transform(XMVector3Cross(rB, axis), solverbody[1]->inv_tensor), delta_impulse);
				}


				{
					Constraint& constraint = cp.constraint[2];
					XMVECTOR axis = XMLoadFloat3(&constraint.axis);

					float delta_impulse = constraint.rhs;
					XMVECTOR delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * XMVectorGetX(XMVector3Dot(axis, delta_velocity[0] - delta_velocity[1]));
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity = solverbody[0]->delta_LinearVelocity + XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass);
					solverbody[0]->delta_AngulaVelocity = solverbody[0]->delta_AngulaVelocity + XMVectorScale(XMVector3Transform(XMVector3Cross(rA, axis), solverbody[0]->inv_tensor), delta_impulse);
					solverbody[1]->delta_LinearVelocity = solverbody[1]->delta_LinearVelocity - XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass);
					solverbody[1]->delta_AngulaVelocity = solverbody[1]->delta_AngulaVelocity - XMVectorScale(XMVector3Transform(XMVector3Cross(rB, axis), solverbody[1]->inv_tensor), delta_impulse);
				}


			}
		}
	}

	// ���x�̍X�V
	for (auto& coll : ALP_colliders) {

		if (coll->get_ALPphysics()->is_kinmatic_linear) {
			Vector3 linervec;
			XMStoreFloat3(&linervec, coll->get_ALPphysics()->solve->delta_LinearVelocity);
			coll->get_ALPphysics()->set_linear_velocity(coll->get_ALPphysics()->linear_velocity() + linervec);
			coll->get_ALPphysics()->set_old_linear_velocity(coll->get_ALPphysics()->old_linear_velocity() + linervec);
		}
		if (coll->get_ALPphysics()->is_kinmatic_anglar) {
			Vector3 anglvec;
			XMStoreFloat3(&anglvec, coll->get_ALPphysics()->solve->delta_AngulaVelocity);
			coll->get_ALPphysics()->set_angula_velocity(coll->get_ALPphysics()->angula_velocity() + anglvec);
			coll->get_ALPphysics()->set_old_angula_velocity(coll->get_ALPphysics()->old_angula_velocity() + anglvec);
		}

		coll->get_ALPphysics()->solve = nullptr;


	}


}

#endif // PHYICSE_USED_SIMD
#pragma endregion
//:::::::::::::::::::::::::::