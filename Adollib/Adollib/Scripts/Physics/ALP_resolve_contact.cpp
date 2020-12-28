#include "ALP_resolve_contact.h"

#include "../Imgui/debug.h"
#include "../Imgui/work_meter.h"

#include "ALP__physics_manager.h"
#include "ALP_physics.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

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
void CalcTangentVector(const Vector3& normal, Vector3& tangent1, Vector3& tangent2)
{
	Vector3 vec(1.0f, 0.0f, 0.0f);
	Vector3 n(normal);
	n[0] = 0.0f;
	if (n.norm() < FLT_EPSILON) {
		vec = Vector3(0.0f, 1.0f, 0.0f);
	}
	tangent1 = (vector3_cross(normal, vec)).unit_vect();
	tangent2 = (vector3_cross(tangent1, normal)).unit_vect();
}

#if 1 || _DEBUG
void Physics_function::resolve_contact(std::list<ALP_Collider>& colliders, std::vector<Contacts::Contact_pair>& pairs) {

	//::: �����p�I�u�W�F�N�g�̐��� :::::::::::
	std::vector<ALP_Solverbody> SBs;
	{
		int count = 0;
		for (const auto& coll : colliders) {
			ALP_Solverbody SB;
			SB.orientation = coll.world_orientation;
			SB.delta_LinearVelocity = Vector3(0.0f);
			SB.delta_AngulaVelocity = Vector3(0.0f);

			if(coll.ALPphysics->is_kinmatic_anglar) SB.inv_inertia = coll.ALPphysics->inverse_inertial_tensor();
			else SB.inv_inertia = matrix_zero();

			SB.inv_mass = coll.ALPphysics->inverse_mass();

			SB.num = count;

			SBs.emplace_back(SB);
			count++;
		}
		count = 0;
		for (auto& coll : colliders) {
			coll.ALPphysics->solve = &SBs[count];
			count++;
		}
	}

	std::vector<ALP_Collider_mesh>::iterator coll[2];
	std::list<ALP_Physics>::iterator ALPphysics[2];
	ALP_Solverbody* solverbody[2];
	//std::vector<Balljoint> balljoints; //����͂Ȃ�

	for(auto& pair : pairs){

		coll[0] = pair.body[0];
		coll[1] = pair.body[1];
		ALPphysics[0] = coll[0]->ALPcollider->ALPphysics;
		ALPphysics[1] = coll[1]->ALPcollider->ALPphysics;
		solverbody[0] = ALPphysics[0]->solve;
		solverbody[1] = ALPphysics[1]->solve;

		// ���C�̊l��
		pair.contacts.friction = sqrtf(ALPphysics[0]->dynamic_friction * ALPphysics[1]->dynamic_friction);

		//::: �Փ˓_���̍X�V ::::::::
		for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
			Contactpoint& cp = pair.contacts.contactpoints[C_num];

			Vector3 rA = vector3_quatrotate(cp.point[0], solverbody[0]->orientation);
			Vector3 rB = vector3_quatrotate(cp.point[1], solverbody[1]->orientation);

			// �����W���̊l��
			// �p���̏Փ˂̏ꍇ�����W����0�ɂ���
			float restitution = (pair.type == Pairtype::new_pair ? 0.5f * (ALPphysics[0]->restitution + ALPphysics[1]->restitution) : 0.0f);

#if 1
			//�Փˎ��̂��ꂼ��̑��x
			Vector3 pdota;
			pdota = vector3_cross(ALPphysics[0]->anglar_velocity, rA);
			pdota += ALPphysics[0]->linear_velocity;

			Vector3 pdotb;
			pdotb = vector3_cross(ALPphysics[1]->anglar_velocity, rB);
			pdotb += ALPphysics[1]->linear_velocity;

			//�Փˎ��̏Փ˕��ʖ@�������̑��Α��x(���ǏՓ˂Ɏg���͖̂@�������ւ̑���)
			Vector3 vrel = pdota - pdotb;

			Vector3
				tangent1, //normal�ɑ΂���z����
				tangent2; //normal�ɑ΂���x����
			CalcTangentVector(cp.normal, tangent1, tangent2);

			//Baraff[1997]�̎�(8-18)�̕���(denominator)�����߂�
			float term1 = ALPphysics[0]->inverse_mass();
			float term2 = ALPphysics[1]->inverse_mass();
			Vector3 tA, tB;

			float term3, term4, denominator;
			// Normal
			{
				//Baraff[1997]�̎�(8-18)�̕���(denominator)�����߂�
				Vector3 axis = cp.normal;
				tA = vector3_cross(rA, axis);
				tB = vector3_cross(rB, axis);
				tA = vector3_trans(tA, ALPphysics[0]->solve->inv_inertia);
				tB = vector3_trans(tB, ALPphysics[1]->solve->inv_inertia);
				tA = vector3_cross(tA, rA);
				tB = vector3_cross(tB, rB);
				term3 = vector3_dot(axis, tA);
				term4 = vector3_dot(axis, tB);
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[0].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���
				cp.constraint[0].rhs = -(1.0f + restitution) * vector3_dot(axis, vrel); //Baraff1997(8-18)�̕��q

				if (0.0f < cp.distance - Phyisics_manager::slop)
					cp.constraint[0].rhs += (Phyisics_manager::bias * (cp.distance - Phyisics_manager::slop)) / Phyisics_manager::timeStep; //�߂荞�݂𒼂���

				cp.constraint[0].rhs *= cp.constraint[0].jacDiagInv;
				cp.constraint[0].lowerlimit = 0.0f;
				cp.constraint[0].upperlimit = FLT_MAX;
				cp.constraint[0].axis = axis;
			}

			// Tangent1
			{
				Vector3 axis = tangent1;
				tA = vector3_cross(rA, axis);
				tB = vector3_cross(rB, axis);
				tA = vector3_trans(tA, ALPphysics[0]->solve->inv_inertia);
				tB = vector3_trans(tB, ALPphysics[1]->solve->inv_inertia);
				tA = vector3_cross(tA, rA);
				tB = vector3_cross(tB, rB);
				term3 = vector3_dot(axis, tA);
				term4 = vector3_dot(axis, tB);
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[1].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���
				cp.constraint[1].rhs = -vector3_dot(axis, vrel); //Baraff1997(8-18)�̕��q
				cp.constraint[1].rhs *= cp.constraint[1].jacDiagInv;
				cp.constraint[1].lowerlimit = 0.0f;
				cp.constraint[1].upperlimit = 0;
				cp.constraint[1].axis = axis;
			}

			// Tangent2
			{
				Vector3 axis = tangent2;
				tA = vector3_cross(rA, axis);
				tB = vector3_cross(rB, axis);
				tA = vector3_trans(tA, ALPphysics[0]->solve->inv_inertia);
				tB = vector3_trans(tB, ALPphysics[1]->solve->inv_inertia);
				tA = vector3_cross(tA, rA);
				tB = vector3_cross(tB, rB);
				term3 = vector3_dot(axis, tA);
				term4 = vector3_dot(axis, tB);
				denominator = term1 + term2 + term3 + term4;
				cp.constraint[2].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���?
				cp.constraint[2].rhs = -vector3_dot(axis, vrel); //Baraff1997(8-18)�̕��q
				cp.constraint[2].rhs *= cp.constraint[2].jacDiagInv;
				cp.constraint[2].lowerlimit = 0.0f;
				cp.constraint[2].upperlimit = 0;
				cp.constraint[2].axis = axis;
			}
#endif
#if 0
			float L = solverbody[0]->inv_mass + solverbody[1]->inv_mass;
			Matrix K = matrix_scale(Vector3(L))-(
				matrix_cross(rA) * solverbody[0]->inv_inertia * matrix_cross(rA) +
				matrix_cross(rB) * solverbody[1]->inv_inertia * matrix_cross(rB)
				);

			Vector3 velocityA = ALPphysics[0]->linear_velocity + vector3_cross(ALPphysics[0]->anglar_velocity, rA);
			Vector3 velocityB = ALPphysics[1]->linear_velocity + vector3_cross(ALPphysics[1]->anglar_velocity, rB);
			Vector3 relativeVelocity = velocityA - velocityB; //���Α��x

			//Vector3
			//	tangent1, //normal�ɑ΂���z����
			//	tangent2; //normal�ɑ΂���x����

			CalcTangentVector(cp.normal, tangent1, tangent2);

			// Normal
			{
				Vector3 axis = cp.normal;
				float denom =  + vector3_dot(vector3_trans(axis, K), axis);
				if (fabsf(1 - denom * cp.constraint[2].jacDiagInv) > FLT_EPSILON) {
					int dafsgdf = 0;
				}
				cp.constraint[0].jacDiagInv = 1.0f / denom;
				cp.constraint[0].rhs = -(1.0f + restitution) * vector3_dot(relativeVelocity, axis);

				if (0.0f < cp.distance - Phyisics_manager::slop) {
					cp.constraint[0].rhs += (Phyisics_manager::bias * (cp.distance - Phyisics_manager::slop)) / Phyisics_manager::timeStep; //�߂荞�݂𒼂���
				}

				cp.constraint[0].rhs *= cp.constraint[0].jacDiagInv;
				cp.constraint[0].lowerlimit = 0.0f;
				cp.constraint[0].upperlimit = FLT_MAX;
				cp.constraint[0].axis = axis;
			}

			// Tangent1
			{
				Vector3 axis = tangent1;
				float denom =  + vector3_dot(vector3_trans(axis, K), axis);
				if (fabsf(1 - denom * cp.constraint[2].jacDiagInv) > FLT_EPSILON) {
					int dafsgdf = 0;
				}
				cp.constraint[1].jacDiagInv = 1.0f / denom;
				cp.constraint[1].rhs = -vector3_dot(relativeVelocity, axis);
				cp.constraint[1].rhs *= cp.constraint[1].jacDiagInv;
				cp.constraint[1].lowerlimit = 0.0f;
				cp.constraint[1].upperlimit = 0.0f;
				cp.constraint[1].axis = axis;
			}

			// Tangent2
			{
				Vector3 axis = tangent2;
				float denom =  + vector3_dot(vector3_trans(axis, K), axis);
				if (fabsf(1 - denom * cp.constraint[2].jacDiagInv) > FLT_EPSILON) {
					int dafsgdf = 0;
				}
				cp.constraint[2].jacDiagInv = 1.0f / denom;
				cp.constraint[2].rhs = -vector3_dot(relativeVelocity, axis);
				cp.constraint[2].rhs *= cp.constraint[2].jacDiagInv;
				cp.constraint[2].lowerlimit = 0.0f;
				cp.constraint[2].upperlimit = 0.0f;
				cp.constraint[2].axis = axis;
			}
#endif

		}
	}

	//::: �ω��ʂ����߂� :::::::::::::::
	for (auto& pair : pairs) {

		coll[0] = pair.body[0];
		coll[1] = pair.body[1];
		ALPphysics[0] = coll[0]->ALPcollider->ALPphysics;
		ALPphysics[1] = coll[1]->ALPcollider->ALPphysics;
		solverbody[0] = ALPphysics[0]->solve;
		solverbody[1] = ALPphysics[1]->solve;

		for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
			//�Փ˓_�̏��
			Contactpoint& cp = pair.contacts.contactpoints[C_num];
			Vector3 rA = vector3_quatrotate(cp.point[0], solverbody[0]->orientation);
			Vector3 rB = vector3_quatrotate(cp.point[1], solverbody[1]->orientation);

			for (int k = 0; k < 3; k++) {
				float deltaImpulse = cp.constraint[k].accuminpulse;
				solverbody[0]->delta_LinearVelocity += deltaImpulse * solverbody[0]->inv_mass * cp.constraint[k].axis;
				solverbody[0]->delta_AngulaVelocity += deltaImpulse * vector3_trans(vector3_cross(rA, cp.constraint[k].axis), solverbody[0]->inv_inertia);
				solverbody[1]->delta_LinearVelocity -= deltaImpulse * solverbody[1]->inv_mass * cp.constraint[k].axis;
				solverbody[1]->delta_AngulaVelocity -= deltaImpulse * vector3_trans(vector3_cross(rB, cp.constraint[k].axis), solverbody[1]->inv_inertia);

			}
		}
	}


	for (int i = 0; i < Phyisics_manager::solver_iterations; i++) {
		for (auto& pair : pairs) {

			coll[0] = pair.body[0];
			coll[1] = pair.body[1];
			ALPphysics[0] = coll[0]->ALPcollider->ALPphysics;
			ALPphysics[1] = coll[1]->ALPcollider->ALPphysics;
			solverbody[0] = ALPphysics[0]->solve;
			solverbody[1] = ALPphysics[1]->solve;

			for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
				//�Փ˓_�̏��
				Contactpoint& cp = pair.contacts.contactpoints[C_num];
				Vector3 rA = vector3_quatrotate(cp.point[0], solverbody[0]->orientation);
				Vector3 rB = vector3_quatrotate(cp.point[1], solverbody[1]->orientation);

				{
					Constraint& constraint = cp.constraint[0];
					float delta_impulse = constraint.rhs;
					Vector3 delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
					solverbody[0]->delta_AngulaVelocity += delta_impulse * vector3_trans(vector3_cross(rA, constraint.axis), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngulaVelocity -= delta_impulse * vector3_trans(vector3_cross(rB, constraint.axis), solverbody[1]->inv_inertia);
				}

				float max_friction = pair.contacts.friction * fabsf(cp.constraint[0].accuminpulse);
				cp.constraint[1].lowerlimit = -max_friction;
				cp.constraint[1].upperlimit = +max_friction;
				cp.constraint[2].lowerlimit = -max_friction;
				cp.constraint[2].upperlimit = +max_friction;

				{
					Constraint& constraint = cp.constraint[1];
					float delta_impulse = constraint.rhs;
					Vector3 delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
					solverbody[0]->delta_AngulaVelocity += delta_impulse * vector3_trans(vector3_cross(rA, constraint.axis), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngulaVelocity -= delta_impulse * vector3_trans(vector3_cross(rB, constraint.axis), solverbody[1]->inv_inertia);
				}


				{
					Constraint& constraint = cp.constraint[2];
					float delta_impulse = constraint.rhs;
					Vector3 delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
					solverbody[0]->delta_AngulaVelocity += delta_impulse * vector3_trans(vector3_cross(rA, constraint.axis), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngulaVelocity -= delta_impulse * vector3_trans(vector3_cross(rB, constraint.axis), solverbody[1]->inv_inertia);
				}


			}
		}
	}

	// ���x�̍X�V
	for (auto& coll : colliders) {
		if (coll.ALPphysics->is_kinematic) {
			if (coll.ALPphysics->is_kinmatic_linear) coll.ALPphysics->linear_velocity += coll.ALPphysics->solve->delta_LinearVelocity;
			if (coll.ALPphysics->is_kinmatic_anglar) coll.ALPphysics->anglar_velocity += coll.ALPphysics->solve->delta_AngulaVelocity;
		}
	}

}
#else
void Physics_function::resolve_contact(std::list<ALP_Collider>& colliders, std::vector<Contacts::Contact_pair>& pairs) {

	//::: �����p�I�u�W�F�N�g�̐��� :::::::::::
	std::vector<ALP_Solverbody> SBs;
	{
		int count = 0;
		for (const auto& coll : colliders) {
			ALP_Solverbody SB;
			SB.orientation = DirectX::XMLoadFloat4(&coll.world_orientation);
			SB.delta_LinearVelocity = DirectX::XMVECTOR();
			SB.delta_AngulaVelocity = DirectX::XMVECTOR();
			SB.inv_inertia = DirectX::XMLoadFloat4x4(&coll.ALPphysics->inverse_inertial_tensor());
			SB.inv_mass = coll.ALPphysics->inverse_mass();
			SB.num = count;

			SBs.emplace_back(SB);
			count++;
		}
		count = 0;
		for (auto& coll : colliders) {
			coll.ALPphysics->solve = &SBs[count];
			count++;
		}
	}

	std::vector<ALP_Collider_mesh>::iterator coll[2];
	std::list<ALP_Physics>::iterator ALPphysics[2];
	ALP_Solverbody* solverbody[2];
	//std::vector<Balljoint> balljoints; //����͂Ȃ�

	for (auto& pair : pairs) {

		coll[0] = pair.body[0];
		coll[1] = pair.body[1];
		ALPphysics[0] = coll[0]->ALPcollider->ALPphysics;
		ALPphysics[1] = coll[1]->ALPcollider->ALPphysics;
		solverbody[0] = ALPphysics[0]->solve;
		solverbody[1] = ALPphysics[1]->solve;

		// ���C�̊l��
		pair.contacts.friction = sqrtf(ALPphysics[0]->dynamic_friction * ALPphysics[1]->dynamic_friction);

		//::: �Փ˓_���̍X�V ::::::::
		for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
			Contactpoint& cp = pair.contacts.contactpoints[C_num];

			Vector3 rA = vector3_quatrotate(cp.point[0], solverbody[0]->orientation);
			Vector3 rB = vector3_quatrotate(cp.point[1], solverbody[1]->orientation);

			// �����W���̊l��
			// �p���̏Փ˂̏ꍇ�����W����0�ɂ���
			float restitution = (pair.type == Pairtype::new_pair ? 0.5f * (ALPphysics[0]->restitution + ALPphysics[1]->restitution) : 0.0f);

#if 1
			//�Փˎ��̂��ꂼ��̑��x
			Vector3 pdota;
			pdota = vector3_cross(ALPphysics[0]->anglar_velocity, rA);
			pdota += ALPphysics[0]->linear_velocity;

			Vector3 pdotb;
			pdotb = vector3_cross(ALPphysics[1]->anglar_velocity, rB);
			pdotb += ALPphysics[1]->linear_velocity;

			//�Փˎ��̏Փ˕��ʖ@�������̑��Α��x(���ǏՓ˂Ɏg���͖̂@�������ւ̑���)
			Vector3 vrel = pdota - pdotb;

			Vector3
				tangent1, //normal�ɑ΂���z����
				tangent2; //normal�ɑ΂���x����
			CalcTangentVector(cp.normal, tangent1, tangent2);

			//Baraff[1997]�̎�(8-18)�̕���(denominator)�����߂�
			float term1 = ALPphysics[0]->inverse_mass();
			float term2 = ALPphysics[1]->inverse_mass();
			Vector3 tA, tB;

			float term3, term4, denominator;
			// Normal
			{
				//Baraff[1997]�̎�(8-18)�̕���(denominator)�����߂�
				Vector3 axis = cp.normal;
				tA = vector3_cross(rA, axis);
				tB = vector3_cross(rB, axis);
				tA = vector3_trans(tA, ALPphysics[0]->solve->inv_inertia);
				tB = vector3_trans(tB, ALPphysics[1]->solve->inv_inertia);
				tA = vector3_cross(tA, rA);
				tB = vector3_cross(tB, rB);
				term3 = vector3_dot(axis, tA);
				term4 = vector3_dot(axis, tB);
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[0].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���
				cp.constraint[0].rhs = -(1.0f + restitution) * vector3_dot(axis, vrel); //Baraff1997(8-18)�̕��q

				if (0.0f < cp.distance - Phyisics_manager::slop)
					cp.constraint[0].rhs += (Phyisics_manager::bias * (cp.distance - Phyisics_manager::slop)) / Phyisics_manager::timeStep; //�߂荞�݂𒼂���

				cp.constraint[0].rhs *= cp.constraint[0].jacDiagInv;
				cp.constraint[0].lowerlimit = 0.0f;
				cp.constraint[0].upperlimit = FLT_MAX;
				cp.constraint[0].axis = axis;
			}

			// Tangent1
			{
				Vector3 axis = tangent1;
				tA = vector3_cross(rA, axis);
				tB = vector3_cross(rB, axis);
				tA = vector3_trans(tA, ALPphysics[0]->solve->inv_inertia);
				tB = vector3_trans(tB, ALPphysics[1]->solve->inv_inertia);
				tA = vector3_cross(tA, rA);
				tB = vector3_cross(tB, rB);
				term3 = vector3_dot(axis, tA);
				term4 = vector3_dot(axis, tB);
				denominator = term1 + term2 + term3 + term4;

				cp.constraint[1].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���
				cp.constraint[1].rhs = -vector3_dot(axis, vrel); //Baraff1997(8-18)�̕��q
				cp.constraint[1].rhs *= cp.constraint[1].jacDiagInv;
				cp.constraint[1].lowerlimit = 0.0f;
				cp.constraint[1].upperlimit = 0;
				cp.constraint[1].axis = axis;
			}

			// Tangent2
			{
				Vector3 axis = tangent2;
				tA = vector3_cross(rA, axis);
				tB = vector3_cross(rB, axis);
				tA = vector3_trans(tA, ALPphysics[0]->solve->inv_inertia);
				tB = vector3_trans(tB, ALPphysics[1]->solve->inv_inertia);
				tA = vector3_cross(tA, rA);
				tB = vector3_cross(tB, rB);
				term3 = vector3_dot(axis, tA);
				term4 = vector3_dot(axis, tB);
				denominator = term1 + term2 + term3 + term4;
				cp.constraint[2].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)�̕���?
				cp.constraint[2].rhs = -vector3_dot(axis, vrel); //Baraff1997(8-18)�̕��q
				cp.constraint[2].rhs *= cp.constraint[2].jacDiagInv;
				cp.constraint[2].lowerlimit = 0.0f;
				cp.constraint[2].upperlimit = 0;
				cp.constraint[2].axis = axis;
			}
#endif
#if 0
			float L = solverbody[0]->inv_mass + solverbody[1]->inv_mass;
			Matrix K = matrix_scale(Vector3(L)) - (
				matrix_cross(rA) * solverbody[0]->inv_inertia * matrix_cross(rA) +
				matrix_cross(rB) * solverbody[1]->inv_inertia * matrix_cross(rB)
				);

			Vector3 velocityA = ALPphysics[0]->linear_velocity + vector3_cross(ALPphysics[0]->anglar_velocity, rA);
			Vector3 velocityB = ALPphysics[1]->linear_velocity + vector3_cross(ALPphysics[1]->anglar_velocity, rB);
			Vector3 relativeVelocity = velocityA - velocityB; //���Α��x

			//Vector3
			//	tangent1, //normal�ɑ΂���z����
			//	tangent2; //normal�ɑ΂���x����

			CalcTangentVector(cp.normal, tangent1, tangent2);

			// Normal
			{
				Vector3 axis = cp.normal;
				float denom = +vector3_dot(vector3_trans(axis, K), axis);
				if (fabsf(1 - denom * cp.constraint[2].jacDiagInv) > FLT_EPSILON) {
					int dafsgdf = 0;
				}
				cp.constraint[0].jacDiagInv = 1.0f / denom;
				cp.constraint[0].rhs = -(1.0f + restitution) * vector3_dot(relativeVelocity, axis);

				if (0.0f < cp.distance - Phyisics_manager::slop) {
					cp.constraint[0].rhs += (Phyisics_manager::bias * (cp.distance - Phyisics_manager::slop)) / Phyisics_manager::timeStep; //�߂荞�݂𒼂���
				}

				cp.constraint[0].rhs *= cp.constraint[0].jacDiagInv;
				cp.constraint[0].lowerlimit = 0.0f;
				cp.constraint[0].upperlimit = FLT_MAX;
				cp.constraint[0].axis = axis;
			}

			// Tangent1
			{
				Vector3 axis = tangent1;
				float denom = +vector3_dot(vector3_trans(axis, K), axis);
				if (fabsf(1 - denom * cp.constraint[2].jacDiagInv) > FLT_EPSILON) {
					int dafsgdf = 0;
				}
				cp.constraint[1].jacDiagInv = 1.0f / denom;
				cp.constraint[1].rhs = -vector3_dot(relativeVelocity, axis);
				cp.constraint[1].rhs *= cp.constraint[1].jacDiagInv;
				cp.constraint[1].lowerlimit = 0.0f;
				cp.constraint[1].upperlimit = 0.0f;
				cp.constraint[1].axis = axis;
			}

			// Tangent2
			{
				Vector3 axis = tangent2;
				float denom = +vector3_dot(vector3_trans(axis, K), axis);
				if (fabsf(1 - denom * cp.constraint[2].jacDiagInv) > FLT_EPSILON) {
					int dafsgdf = 0;
				}
				cp.constraint[2].jacDiagInv = 1.0f / denom;
				cp.constraint[2].rhs = -vector3_dot(relativeVelocity, axis);
				cp.constraint[2].rhs *= cp.constraint[2].jacDiagInv;
				cp.constraint[2].lowerlimit = 0.0f;
				cp.constraint[2].upperlimit = 0.0f;
				cp.constraint[2].axis = axis;
			}
#endif

		}
	}

	//::: �ω��ʂ����߂� :::::::::::::::
	for (auto& pair : pairs) {

		coll[0] = pair.body[0];
		coll[1] = pair.body[1];
		ALPphysics[0] = coll[0]->ALPcollider->ALPphysics;
		ALPphysics[1] = coll[1]->ALPcollider->ALPphysics;
		solverbody[0] = ALPphysics[0]->solve;
		solverbody[1] = ALPphysics[1]->solve;

		for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
			//�Փ˓_�̏��
			Contactpoint& cp = pair.contacts.contactpoints[C_num];
			Vector3 rA = vector3_quatrotate(cp.point[0], solverbody[0]->orientation);
			Vector3 rB = vector3_quatrotate(cp.point[1], solverbody[1]->orientation);

			for (int k = 0; k < 3; k++) {
				float deltaImpulse = cp.constraint[k].accuminpulse;
				solverbody[0]->delta_LinearVelocity += deltaImpulse * solverbody[0]->inv_mass * cp.constraint[k].axis;
				solverbody[0]->delta_AngulaVelocity += deltaImpulse * vector3_trans(vector3_cross(rA, cp.constraint[k].axis), solverbody[0]->inv_inertia);
				solverbody[1]->delta_LinearVelocity -= deltaImpulse * solverbody[1]->inv_mass * cp.constraint[k].axis;
				solverbody[1]->delta_AngulaVelocity -= deltaImpulse * vector3_trans(vector3_cross(rB, cp.constraint[k].axis), solverbody[1]->inv_inertia);

			}
		}
	}


	for (int i = 0; i < Phyisics_manager::solver_iterations; i++) {
		for (auto& pair : pairs) {

			coll[0] = pair.body[0];
			coll[1] = pair.body[1];
			ALPphysics[0] = coll[0]->ALPcollider->ALPphysics;
			ALPphysics[1] = coll[1]->ALPcollider->ALPphysics;
			solverbody[0] = ALPphysics[0]->solve;
			solverbody[1] = ALPphysics[1]->solve;

			for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
				//�Փ˓_�̏��
				Contactpoint& cp = pair.contacts.contactpoints[C_num];
				Vector3 rA = vector3_quatrotate(cp.point[0], solverbody[0]->orientation);
				Vector3 rB = vector3_quatrotate(cp.point[1], solverbody[1]->orientation);

				{
					Constraint& constraint = cp.constraint[0];
					float delta_impulse = constraint.rhs;
					Vector3 delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
					solverbody[0]->delta_AngulaVelocity += delta_impulse * vector3_trans(vector3_cross(rA, constraint.axis), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngulaVelocity -= delta_impulse * vector3_trans(vector3_cross(rB, constraint.axis), solverbody[1]->inv_inertia);
				}

				float max_friction = pair.contacts.friction * fabsf(cp.constraint[0].accuminpulse);
				cp.constraint[1].lowerlimit = -max_friction;
				cp.constraint[1].upperlimit = +max_friction;
				cp.constraint[2].lowerlimit = -max_friction;
				cp.constraint[2].upperlimit = +max_friction;

				{
					Constraint& constraint = cp.constraint[1];
					float delta_impulse = constraint.rhs;
					Vector3 delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
					solverbody[0]->delta_AngulaVelocity += delta_impulse * vector3_trans(vector3_cross(rA, constraint.axis), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngulaVelocity -= delta_impulse * vector3_trans(vector3_cross(rB, constraint.axis), solverbody[1]->inv_inertia);
				}


				{
					Constraint& constraint = cp.constraint[2];
					float delta_impulse = constraint.rhs;
					Vector3 delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
					delta_impulse = constraint.accuminpulse - old_impulse;
					solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
					solverbody[0]->delta_AngulaVelocity += delta_impulse * vector3_trans(vector3_cross(rA, constraint.axis), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngulaVelocity -= delta_impulse * vector3_trans(vector3_cross(rB, constraint.axis), solverbody[1]->inv_inertia);
				}


			}
		}
	}

	// ���x�̍X�V
	for (auto& coll : colliders) {
		if (coll.ALPphysics->is_kinematic == true) {
			coll.ALPphysics->linear_velocity += coll.ALPphysics->solve->delta_LinearVelocity;
			coll.ALPphysics->anglar_velocity += coll.ALPphysics->solve->delta_AngulaVelocity;
		}
	}

}
#endif
#pragma endregion
//:::::::::::::::::::::::::::