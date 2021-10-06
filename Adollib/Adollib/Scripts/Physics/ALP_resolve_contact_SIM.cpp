//#define _XM_NO_INTRINSICS_

#include "ALP_resolve_contact.h"

#include "../Imgui/debug.h"
#include "../Imgui/work_meter.h"

#include "ALP__physics_manager.h"
#include "ALP_physics.h"

#include "ALP_joint.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

//:::::::::::::::::::::::::::
#pragma region resolve_contact
//:::::::::::::::::::::::::::
//#ifdef PHYICSE_USED_SIMD
#if 1

//�Փˉ���
// �\���o�[�{�f�B(�����p�̂Ȃ�)
// �{�[���W���C���g
struct Balljoint {
	float bias; // �S���̋����̒����l
	int rigidBodyA; // ����A�ւ̃C���f�b�N�X
	int rigidBodyB; // ����B�ւ̃C���f�b�N�X
	Vector3 anchorA; // ����A�̃��[�J�����W�n�ɂ�����ڑ��_
	Vector3 anchorB; // ����B�̃��[�J�����W�n�ɂ�����ڑ��_
	Constraint constraint; // �S��

	// ������
	void reset()
	{
		bias = 0.1f;
		constraint.accuminpulse = 0.0f;
	}
};

void CalcTangentVector(const Vector3& normal, const DirectX::XMVECTOR& vrel, DirectX::XMVECTOR& tangent1, DirectX::XMVECTOR& tangent2)
{
#if 0
	Vector3 vec(1.0f, 0.0f, 0.0f);
	Vector3 n(normal);
	n[0] = 0.0f;
	if (n.norm() < FLT_EPSILON) {
		vec = Vector3(0.0f, 1.0f, 0.0f);
	}

	DirectX::XMVECTOR xmnorm = DirectX::XMLoadFloat3(&normal);
	tangent1 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(xmnorm, DirectX::XMLoadFloat3(&vec)));
	tangent2 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(tangent1, xmnorm));
#else
	// ���Α��x�̕�����tangent���Ƃ�(�K���ȕ�����tangent���Ƃ��friction�����܂������Ȃ��Ȃ�)

	// ��̑��Α��x�̌�������tangent�����߂�
	DirectX::XMVECTOR xmnorm = DirectX::XMLoadFloat3(&normal);
	DirectX::XMVECTOR dot = DirectX::XMVector3Dot(xmnorm, vrel);

	if (DirectX::XMVectorGetX(dot) < 1 + FLT_EPSILON && DirectX::XMVectorGetX(dot) > 1 - FLT_EPSILON) {
	// �������Α��x�ƏՓ˖@�������������Ȃ�K����tangent�����߂�
		Vector3 vec(1.0f, 0.0f, 0.0f);
		Vector3 n(normal);
		n[0] = 0.0f;
		if (n.norm() < FLT_EPSILON) {
			vec = Vector3(0.0f, 1.0f, 0.0f);
		}
		tangent1 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(xmnorm, DirectX::XMLoadFloat3(&vec)));
		tangent2 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(tangent1, xmnorm));

	}
	else {
		tangent1 = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(vrel, DirectX::XMVectorMultiply(xmnorm, DirectX::XMVector3Dot(xmnorm, vrel))));
		tangent2 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(tangent1, xmnorm));
	}
#endif

}
bool Calc_joint_effect(ALP_Joint* joint)
{
	ALP_Physics* ALPphysics[2];
	ALP_Solverbody* solverbody[2];
	world_trans* transform[2];
	DirectX::XMVECTOR position[2];

	transform[0] = &joint->ALPcollider[0]->transform;
	transform[1] = &joint->ALPcollider[1]->transform;
	ALPphysics[0] = joint->ALPcollider[0]->get_ALPphysics();
	ALPphysics[1] = joint->ALPcollider[1]->get_ALPphysics();
	solverbody[0] = ALPphysics[0]->solve;
	solverbody[1] = ALPphysics[1]->solve;

	float penetrate = 0;

	if (joint->joint->limit_effect(joint->limit_constraint_pos[0], joint->limit_constraint_pos[1], penetrate)) {
		if (penetrate == 0)return false;

		//anchor���ꂼ���local���W
		const DirectX::XMVECTOR rA = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&joint->limit_constraint_pos[0]), DirectX::XMLoadFloat4(&transform[0]->orientation));
		const DirectX::XMVECTOR rB = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&joint->limit_constraint_pos[1]), DirectX::XMLoadFloat4(&transform[1]->orientation));

		//anchor���ꂼ���world���W
		position[0] = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&transform[0]->position), rA);
		position[1] = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&transform[1]->position), rB);
		DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(position[1], position[0]);
		DirectX::XMVECTOR distance = DirectX::XMVector3Length(direction);
		if (DirectX::XMVectorGetX(distance) == 0)return false;

		direction = DirectX::XMVectorDivide(direction, distance);

		DirectX::XMVECTOR velocityA = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&ALPphysics[0]->linear_velocity()), DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&ALPphysics[0]->angula_velocity()), rA));
		DirectX::XMVECTOR velocityB = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&ALPphysics[1]->linear_velocity()), DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&ALPphysics[1]->angula_velocity()), rB));
		DirectX::XMVECTOR relativeVelocity = DirectX::XMVectorSubtract(velocityA, velocityB);

		const float& term1 = ALPphysics[0]->inverse_mass();
		const float& term2 = ALPphysics[1]->inverse_mass();
		DirectX::XMVECTOR tA, tB;

		float term3, term4, denominator;
		DirectX::XMVECTOR axis;
		axis = direction;
		tA = DirectX::XMVector3Cross(rA, axis);
		tB = DirectX::XMVector3Cross(rB, axis);
		tA = DirectX::XMVector3Transform(tA, ALPphysics[0]->solve->inv_inertia);
		tB = DirectX::XMVector3Transform(tB, ALPphysics[1]->solve->inv_inertia);
		tA = DirectX::XMVector3Cross(tA, rA);
		tB = DirectX::XMVector3Cross(tB, rB);
		term3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tA));
		term4 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tB));
		denominator = term1 + term2 + term3 + term4;


		joint->constraint_limit.jacDiagInv = 1.0f / denominator;

		joint->constraint_limit.rhs = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(relativeVelocity, direction)); // velocity error
		joint->constraint_limit.rhs += joint->limit_bias * penetrate / Phyisics_manager::physicsParams.timeStep; // position error
		joint->constraint_limit.rhs *= joint->constraint_limit.jacDiagInv;
		joint->constraint_limit.lowerlimit = -FLT_MAX;
		joint->constraint_limit.upperlimit = +FLT_MAX;
		DirectX::XMStoreFloat3(&joint->constraint_limit.axis, direction);

		Vector3 linervec, anglvec;
		DirectX::XMStoreFloat3(&linervec, direction);

		joint->constraint_limit.accuminpulse = 0.0f;


	}
	else {
		return false;
	}


	return true;
}


void Physics_function::resolve_contact(std::list<ALP_Collider*>& colliders, std::vector<Contacts::Contact_pair*>& pairs, std::list<Physics_function::ALP_Joint*> joints) {


	Work_meter::start("Make_solver");

	//::: �����p�I�u�W�F�N�g�̐��� :::::::::::
	std::vector<ALP_Solverbody> SBs;
	SBs.reserve(sizeof(ALP_Solverbody) * colliders.size());
	//SBs.resize(colliders.size()); //�A���C�����g��SIMD�ƃR���e�i����������??? reserve�ɂ����玡����
	{
		int count = 0;
		ALP_Solverbody SB;
		for (const auto& coll : colliders) {

			SB.delta_LinearVelocity = DirectX::XMLoadFloat3(&vector3_zero());
			SB.delta_AngulaVelocity = DirectX::XMLoadFloat3(&vector3_zero());

			if (coll->get_ALPphysics()->is_kinmatic_anglar) SB.inv_inertia = DirectX::XMLoadFloat3x3(&coll->get_ALPphysics()->inverse_inertial_tensor());
			else SB.inv_inertia = DirectX::XMLoadFloat3x3(&matrix33_zero());

			if (coll->get_ALPphysics()->is_kinmatic_linear) SB.inv_mass = coll->get_ALPphysics()->inverse_mass();
			else SB.inv_mass = 0;

			SBs.emplace_back(SB);
			++count;
		}

		count = 0;
		for (auto& coll : colliders) {
			coll->get_ALPphysics()->solve = &SBs[count];
			count++;
		}
	}

	Collider_shape* coll[2];
	ALP_Collider* ALPcollider[2];
	ALP_Physics* ALPphysics[2];
	ALP_Solverbody* solverbody[2];

	Work_meter::stop("Make_solver");

	Work_meter::start("Setup_solver_joint");
	// �S���̃Z�b�g�A�b�v
	{
		world_trans* transform[2];
		DirectX::XMVECTOR position[2];
		for (auto& joint : joints) {
			joint->adapt_Jointdata();

			transform[0] = &joint->ALPcollider[0]->transform;
			transform[1] = &joint->ALPcollider[1]->transform;
			ALPphysics[0] = joint->ALPcollider[0]->get_ALPphysics();
			ALPphysics[1] = joint->ALPcollider[1]->get_ALPphysics();
			solverbody[0] = ALPphysics[0]->solve;
			solverbody[1] = ALPphysics[1]->solve;

			//::: limit�̉e�����v�Z
			Work_meter::start("Setup_solver_joint_effect");

			if (Calc_joint_effect(joint) == false) {
				//limit�Ɉ����������Ă��Ȃ� -> �����N���Ȃ�
				joint->constraint_limit.jacDiagInv = 0.0f;
				joint->constraint_limit.rhs = 0.0f;
				joint->constraint_limit.lowerlimit = -FLT_MAX;
				joint->constraint_limit.upperlimit = +FLT_MAX;
				joint->constraint_limit.axis = Vector3(0, 0, 0);
			}
			Work_meter::stop("Setup_solver_joint_effect");

			//::: anchor�̉e�����v�Z
			for (int i = 0; i < joint->anchor_count; i++) {
				const Vector3 joint_posA = joint->anchor[i].posA;
				const Vector3 joint_posB = joint->anchor[i].posB;
				auto& constraint = joint->constraint[i];

				//anchor���ꂼ���local���W
				const DirectX::XMVECTOR rA = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&joint_posA), DirectX::XMLoadFloat4(&transform[0]->orientation));
				const DirectX::XMVECTOR rB = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&joint_posB), DirectX::XMLoadFloat4(&transform[1]->orientation));

				//anchor���ꂼ���world���W
				position[0] = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&transform[0]->position), rA);
				position[1] = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&transform[1]->position), rB);
				DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(position[1], position[0]);
				DirectX::XMVECTOR distance = DirectX::XMVector3Length(direction);

				if (DirectX::XMVectorGetX(distance) < FLT_EPSILON) {
					//�ƂĂ��߂��ʒu�ɂ��� -> ��������Ȃ�
					constraint.jacDiagInv = 0.0f;
					constraint.rhs = 0.0f;
					constraint.lowerlimit = -FLT_MAX;
					constraint.upperlimit = +FLT_MAX;
					constraint.axis = Vector3(0, 0, 0);
					continue;
				}

				direction = DirectX::XMVectorDivide(direction, distance);

				DirectX::XMVECTOR velocityA = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&ALPphysics[0]->linear_velocity()), DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&ALPphysics[0]->angula_velocity()), rA));
				DirectX::XMVECTOR velocityB = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&ALPphysics[1]->linear_velocity()), DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&ALPphysics[1]->angula_velocity()), rB));
				DirectX::XMVECTOR relativeVelocity = DirectX::XMVectorSubtract(velocityA, velocityB);

				const float& term1 = ALPphysics[0]->inverse_mass();
				const float& term2 = ALPphysics[1]->inverse_mass();
				DirectX::XMVECTOR tA, tB;

				float term3, term4, denominator;
				DirectX::XMVECTOR axis;
				axis = direction;
				tA = DirectX::XMVector3Cross(rA, axis);
				tB = DirectX::XMVector3Cross(rB, axis);
				tA = DirectX::XMVector3Transform(tA, ALPphysics[0]->solve->inv_inertia);
				tB = DirectX::XMVector3Transform(tB, ALPphysics[1]->solve->inv_inertia);
				tA = DirectX::XMVector3Cross(tA, rA);
				tB = DirectX::XMVector3Cross(tB, rB);
				term3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tA));
				term4 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tB));
				denominator = term1 + term2 + term3 + term4;


				constraint.jacDiagInv = 1.0f / denominator;

				constraint.rhs = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(relativeVelocity, direction)); // velocity error

				if (0.0f < DirectX::XMVectorGetX(distance) - joint->slop)
					constraint.rhs += joint->bias * (DirectX::XMVectorGetX(distance) - joint->slop) / Phyisics_manager::physicsParams.timeStep; // position error
				constraint.rhs *= constraint.jacDiagInv;
				constraint.lowerlimit = -FLT_MAX;
				constraint.upperlimit = +FLT_MAX;
				DirectX::XMStoreFloat3(&constraint.axis, direction);

				constraint.accuminpulse = 0.0f;
			}

		}
	}

	Work_meter::stop("Setup_solver_joint");

	Work_meter::start("Setup_solver_contact");
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

			const DirectX::XMVECTOR rA = DirectX::XMVector3Rotate(DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&coll[0]->local_position), DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&cp.point[0]), DirectX::XMLoadFloat4(&coll[0]->local_orientation))), DirectX::XMLoadFloat4(&ALPcollider[0]->transform.orientation));
			const DirectX::XMVECTOR rB = DirectX::XMVector3Rotate(DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&coll[1]->local_position), DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&cp.point[1]), DirectX::XMLoadFloat4(&coll[1]->local_orientation))), DirectX::XMLoadFloat4(&ALPcollider[1]->transform.orientation));

			// �����W���̊l��
			// �p���̏Փ˂̏ꍇ�����W����0�ɂ���
			const float restitution = (pair->type == Pairtype::new_pair ? 0.5f * (ALPphysics[0]->restitution + ALPphysics[1]->restitution) : 0.0f);


			//�Փˎ��̂��ꂼ��̑��x
			DirectX::XMVECTOR pdota;
			pdota = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&ALPphysics[0]->angula_velocity()), rA);
			pdota = DirectX::XMVectorAdd(pdota, DirectX::XMLoadFloat3(&ALPphysics[0]->linear_velocity()));

			DirectX::XMVECTOR pdotb;
			pdotb = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&ALPphysics[1]->angula_velocity()), rB);
			pdotb = DirectX::XMVectorAdd(pdotb, DirectX::XMLoadFloat3(&ALPphysics[1]->linear_velocity()));

			//�Փˎ��̏Փ˕��ʖ@�������̑��Α��x(���ǏՓ˂Ɏg���͖̂@�������ւ̑���)
			DirectX::XMVECTOR vrel = DirectX::XMVectorSubtract(pdota, pdotb);

			DirectX::XMVECTOR
				tangent1, //normal�ɑ΂���z����
				tangent2; //normal�ɑ΂���x����
			CalcTangentVector(cp.normal, vrel, tangent1, tangent2);

			//Baraff[1997]�̎�(8-18)�̕���(denominator)�����߂�
			const float& term1 = ALPphysics[0]->inverse_mass();
			const float& term2 = ALPphysics[1]->inverse_mass();
			DirectX::XMVECTOR tA, tB;

			float term3, term4, denominator;
			DirectX::XMVECTOR axis;
			// Normal
			{
				//Baraff[1997]�̎�(8-18)�̕���(denominator)�����߂�
				axis = DirectX::XMLoadFloat3(&cp.normal);
				tA = DirectX::XMVector3Cross(rA, axis);
				tB = DirectX::XMVector3Cross(rB, axis);
				tA = DirectX::XMVector3Transform(tA, ALPphysics[0]->solve->inv_inertia);
				tB = DirectX::XMVector3Transform(tB, ALPphysics[1]->solve->inv_inertia);
				tA = DirectX::XMVector3Cross(tA, rA);
				tB = DirectX::XMVector3Cross(tB, rB);
				term3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tA));
				term4 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tB));
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[0].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���
				cp.constraint[0].rhs = -(1.0f + restitution) * DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, vrel)); //Baraff1997(8-18)�̕��q

				if (0.0f < cp.distance - Phyisics_manager::physicsParams.slop)
					cp.constraint[0].rhs += (Phyisics_manager::physicsParams.bias * (cp.distance - Phyisics_manager::physicsParams.slop)) / Phyisics_manager::physicsParams.timeStep; //�߂荞�݂𒼂���

				cp.constraint[0].rhs *= cp.constraint[0].jacDiagInv;
				cp.constraint[0].lowerlimit = 0.0f;
				cp.constraint[0].upperlimit = FLT_MAX;
				DirectX::XMStoreFloat3(&cp.constraint[0].axis, axis);
			}

			// Tangent1
			{
				axis = tangent1;
				tA = DirectX::XMVector3Cross(rA, axis);
				tB = DirectX::XMVector3Cross(rB, axis);
				tA = DirectX::XMVector3Transform(tA, ALPphysics[0]->solve->inv_inertia);
				tB = DirectX::XMVector3Transform(tB, ALPphysics[1]->solve->inv_inertia);
				tA = DirectX::XMVector3Cross(tA, rA);
				tB = DirectX::XMVector3Cross(tB, rB);
				term3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tA));
				term4 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tB));
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[1].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���
				cp.constraint[1].rhs = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, vrel)); //Baraff1997(8-18)�̕��q
				cp.constraint[1].rhs *= cp.constraint[1].jacDiagInv;
				cp.constraint[1].lowerlimit = 0.0f;
				cp.constraint[1].upperlimit = 0;
				DirectX::XMStoreFloat3(&cp.constraint[1].axis, axis);
			}

			// Tangent2
			{
				axis = tangent2;
				tA = DirectX::XMVector3Cross(rA, axis);
				tB = DirectX::XMVector3Cross(rB, axis);
				tA = DirectX::XMVector3Transform(tA, ALPphysics[0]->solve->inv_inertia);
				tB = DirectX::XMVector3Transform(tB, ALPphysics[1]->solve->inv_inertia);
				tA = DirectX::XMVector3Cross(tA, rA);
				tB = DirectX::XMVector3Cross(tB, rB);
				term3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tA));
				term4 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tB));
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[2].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���?
				cp.constraint[2].rhs = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, vrel)); //Baraff1997(8-18)�̕��q
				cp.constraint[2].rhs *= cp.constraint[2].jacDiagInv;
				cp.constraint[2].lowerlimit = 0.0f;
				cp.constraint[2].upperlimit = 0;
				DirectX::XMStoreFloat3(&cp.constraint[2].axis, axis);
			}


		}
	}

	Work_meter::stop("Setup_solver_contact");


	Work_meter::start("adapt_impulse");


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

			const DirectX::XMVECTOR rA = DirectX::XMVector3Rotate(DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&coll[0]->local_position), DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&cp.point[0]), DirectX::XMLoadFloat4(&coll[0]->local_orientation))), DirectX::XMLoadFloat4(&ALPcollider[0]->transform.orientation));
			const DirectX::XMVECTOR rB = DirectX::XMVector3Rotate(DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&coll[1]->local_position), DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&cp.point[1]), DirectX::XMLoadFloat4(&coll[1]->local_orientation))), DirectX::XMLoadFloat4(&ALPcollider[1]->transform.orientation));

			for (int k = 0; k < 3; k++) {
				const float& deltaImpulse = cp.constraint[k].accuminpulse;
				DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&cp.constraint[k].axis);

				solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVectorScale(axis, deltaImpulse * solverbody[0]->inv_mass));
				solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), deltaImpulse));
				solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, DirectX::XMVectorScale(axis, deltaImpulse * solverbody[1]->inv_mass));
				solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), deltaImpulse));
			}
		}

	}

	{
		world_trans* transform[2];
		DirectX::XMVECTOR position[2];
		for (auto& joint : joints) {
			transform[0] = &joint->ALPcollider[0]->transform;
			transform[1] = &joint->ALPcollider[1]->transform;
			ALPphysics[0] = joint->ALPcollider[0]->get_ALPphysics();
			ALPphysics[1] = joint->ALPcollider[1]->get_ALPphysics();
			solverbody[0] = ALPphysics[0]->solve;
			solverbody[1] = ALPphysics[1]->solve;

			for (int i = 0; i < joint->anchor_count; i++) {
				const Vector3 joint_posA = joint->anchor[i].posA;
				const Vector3 joint_posB = joint->anchor[i].posB;

				const DirectX::XMVECTOR rA = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&joint_posA), DirectX::XMLoadFloat4(&transform[0]->orientation));
				const DirectX::XMVECTOR rB = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&joint_posB), DirectX::XMLoadFloat4(&transform[1]->orientation));

				//�Փ˓_�̏��
				Constraint& constraint = joint->constraint[i];

				const float& deltaImpulse = constraint.accuminpulse;
				DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&constraint.axis);

				solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVectorScale(axis, deltaImpulse * solverbody[0]->inv_mass));
				solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), deltaImpulse));
				solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, DirectX::XMVectorScale(axis, deltaImpulse * solverbody[1]->inv_mass));
				solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), deltaImpulse));
			}

			{
				const Vector3 joint_posA = joint->limit_constraint_pos[0];
				const Vector3 joint_posB = joint->limit_constraint_pos[1];

				const DirectX::XMVECTOR rA = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&joint_posA), DirectX::XMLoadFloat4(&transform[0]->orientation));
				const DirectX::XMVECTOR rB = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&joint_posB), DirectX::XMLoadFloat4(&transform[1]->orientation));

				//�Փ˓_�̏��
				Constraint& constraint = joint->constraint_limit;

				const float& deltaImpulse = constraint.accuminpulse;
				DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&constraint.axis);

				solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVectorScale(axis, deltaImpulse * solverbody[0]->inv_mass));
				solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), deltaImpulse));
				solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, DirectX::XMVectorScale(axis, deltaImpulse * solverbody[1]->inv_mass));
				solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), deltaImpulse));

			}
		}
	}

	Work_meter::stop("adapt_impulse");

	Work_meter::start("solver");

	for (int solver_iterations_count = 0; solver_iterations_count < Phyisics_manager::physicsParams.solver_iterations; solver_iterations_count++) {
		// �S���̉��Z
		world_trans* transform[2];
		for (auto& joint : joints) {

			{
				const Vector3 joint_posA = joint->limit_constraint_pos[0];
				const Vector3 joint_posB = joint->limit_constraint_pos[1];

				transform[0] = &joint->ALPcollider[0]->transform;
				transform[1] = &joint->ALPcollider[1]->transform;
				ALPphysics[0] = joint->ALPcollider[0]->get_ALPphysics();
				ALPphysics[1] = joint->ALPcollider[1]->get_ALPphysics();
				solverbody[0] = ALPphysics[0]->solve;
				solverbody[1] = ALPphysics[1]->solve;

				const DirectX::XMVECTOR rA = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&joint_posA), DirectX::XMLoadFloat4(&transform[0]->orientation));
				const DirectX::XMVECTOR rB = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&joint_posB), DirectX::XMLoadFloat4(&transform[1]->orientation));

				Constraint& constraint = joint->constraint_limit;
				const DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&constraint.axis);

				float delta_impulse = constraint.rhs;
				DirectX::XMVECTOR delta_velocity[2];
				delta_velocity[0] = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA));
				delta_velocity[1] = DirectX::XMVectorAdd(solverbody[1]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB));
				delta_impulse -= constraint.jacDiagInv * DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, DirectX::XMVectorSubtract(delta_velocity[0], delta_velocity[1])));

				float old_impulse = constraint.accuminpulse;
				constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
				delta_impulse = constraint.accuminpulse - old_impulse;

				solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass));
				solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), delta_impulse));
				solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass));
				solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), delta_impulse));
			}

			for (int i = 0; i < joint->anchor_count; i++) {
				const Vector3 joint_posA = joint->anchor[i].posA;
				const Vector3 joint_posB = joint->anchor[i].posB;

				transform[0] = &joint->ALPcollider[0]->transform;
				transform[1] = &joint->ALPcollider[1]->transform;
				ALPphysics[0] = joint->ALPcollider[0]->get_ALPphysics();
				ALPphysics[1] = joint->ALPcollider[1]->get_ALPphysics();
				solverbody[0] = ALPphysics[0]->solve;
				solverbody[1] = ALPphysics[1]->solve;

				const DirectX::XMVECTOR rA = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&joint_posA), DirectX::XMLoadFloat4(&transform[0]->orientation));
				const DirectX::XMVECTOR rB = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&joint_posB), DirectX::XMLoadFloat4(&transform[1]->orientation));

				Constraint& constraint = joint->constraint[i];
				const DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&constraint.axis);

				float delta_impulse = constraint.rhs;
				DirectX::XMVECTOR delta_velocity[2];
				delta_velocity[0] = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA));
				delta_velocity[1] = DirectX::XMVectorAdd(solverbody[1]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB));
				delta_impulse -= constraint.jacDiagInv * DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, DirectX::XMVectorSubtract(delta_velocity[0], delta_velocity[1])));

				float old_impulse = constraint.accuminpulse;
				constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
				delta_impulse = constraint.accuminpulse - old_impulse;

				solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass));
				solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), delta_impulse));
				solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass));
				solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), delta_impulse));
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

				const DirectX::XMVECTOR rA = DirectX::XMVector3Rotate(DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&coll[0]->local_position), DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&cp.point[0]), DirectX::XMLoadFloat4(&coll[0]->local_orientation))), DirectX::XMLoadFloat4(&ALPcollider[0]->transform.orientation));
				const DirectX::XMVECTOR rB = DirectX::XMVector3Rotate(DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&coll[1]->local_position), DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&cp.point[1]), DirectX::XMLoadFloat4(&coll[1]->local_orientation))), DirectX::XMLoadFloat4(&ALPcollider[1]->transform.orientation));

				{
					Constraint& constraint = cp.constraint[0];
					const DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&constraint.axis);

					float delta_impulse = constraint.rhs;
					DirectX::XMVECTOR delta_velocity[2];
					delta_velocity[0] = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA));
					delta_velocity[1] = DirectX::XMVectorAdd(solverbody[1]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB));
					delta_impulse -= constraint.jacDiagInv * DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, DirectX::XMVectorSubtract(delta_velocity[0], delta_velocity[1])));
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass));
					solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), delta_impulse));
					solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass));
					solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), delta_impulse));
				}

				float max_friction = pair->contacts.friction * fabsf(cp.constraint[0].accuminpulse);
				cp.constraint[1].lowerlimit = -max_friction;
				cp.constraint[1].upperlimit = +max_friction;
				cp.constraint[2].lowerlimit = -max_friction;
				cp.constraint[2].upperlimit = +max_friction;

				{
					Constraint& constraint = cp.constraint[1];
					DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&constraint.axis);

					float delta_impulse = constraint.rhs;
					DirectX::XMVECTOR delta_velocity[2];
					delta_velocity[0] = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA));
					delta_velocity[1] = DirectX::XMVectorAdd(solverbody[1]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB));
					delta_impulse -= constraint.jacDiagInv * DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, DirectX::XMVectorSubtract(delta_velocity[0], delta_velocity[1])));
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass));
					solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), delta_impulse));
					solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass));
					solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), delta_impulse));
				}


				{
					Constraint& constraint = cp.constraint[2];
					DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&constraint.axis);

					float delta_impulse = constraint.rhs;
					DirectX::XMVECTOR delta_velocity[2];
					delta_velocity[0] = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA));
					delta_velocity[1] = DirectX::XMVectorAdd(solverbody[1]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB));
					delta_impulse -= constraint.jacDiagInv * DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, DirectX::XMVectorSubtract(delta_velocity[0], delta_velocity[1])));
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass));
					solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), delta_impulse));
					solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass));
					solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), delta_impulse));
				}


			}
		}
	}

	Work_meter::stop("solver");

	// ���x�̍X�V
	for (auto& coll : colliders) {

		if (coll->get_ALPphysics()->is_kinmatic_linear) {
			Vector3 linervec;
			DirectX::XMStoreFloat3(&linervec, coll->get_ALPphysics()->solve->delta_LinearVelocity);
			if (isnan(linervec.norm())) {
				int adsfgh = 0;
			}
			coll->get_ALPphysics()->set_linear_velocity(coll->get_ALPphysics()->linear_velocity() + linervec);
			coll->get_ALPphysics()->set_old_linear_velocity(coll->get_ALPphysics()->old_linear_velocity() + linervec);
		}
		if (coll->get_ALPphysics()->is_kinmatic_anglar) {
			Vector3 anglvec;
			DirectX::XMStoreFloat3(&anglvec, coll->get_ALPphysics()->solve->delta_AngulaVelocity);
			if (isnan(anglvec.norm())) {
				int adsfgh = 0;
			}
			coll->get_ALPphysics()->set_angula_velocity(coll->get_ALPphysics()->angula_velocity() + anglvec);
			coll->get_ALPphysics()->set_old_angula_velocity(coll->get_ALPphysics()->old_angula_velocity() + anglvec);
		}

	}


}

#endif // PHYICSE_USED_SIMD
#pragma endregion
//:::::::::::::::::::::::::::