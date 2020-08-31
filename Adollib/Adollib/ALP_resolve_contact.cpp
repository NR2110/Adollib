#include "ALP_resolve_contact.h"
#include "debug.h"
#include "work_meter.h"

using namespace Adollib;
using namespace Contacts;
using namespace Compute_S;

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
struct  com_Out
{
	physics_function::Solverbody solvs0;
	physics_function::Solverbody solvs1;
	float accumes[4][3];
};

struct com_in {
	Contactpoint cp[10];
	int num[10];

	int contact_num;
};

#include "work_meter.h"
void physics_function::resolve_contact(std::vector<Collider*> colliders, std::vector<Contacts::Contact_pair>& pairs) {

	//::: �����p�I�u�W�F�N�g�̐��� :::::::::::
	std::vector<Solverbody> SBs;
	for (int i = 0; i < colliders.size(); i++) {
		Solverbody SB;
		SB.orientation = colliders[i]->world_orientation;
		SB.delta_LinearVelocity = vector3(0.0f);
		SB.delta_AngulaVelocity = vector3(0.0f);
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
			pdota = vector3_cross(coll[0]->angula_velocity, rA);
			pdota += coll[0]->linear_velocity;

			vector3 pdotb;
			pdotb = vector3_cross(coll[1]->angula_velocity, rB);
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

				//if (0.0f < cp.distance - physics_g::slop) {
				//	cp.constraint[0].rhs += (physics_g::bias * (cp.distance - physics_g::slop)) / physics_g::timeStep; //�߂荞�݂𒼂���
				//}

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
				solverbody[0]->delta_AngulaVelocity += deltaImpulse * vector3_trans(vector3_cross(rA, cp.constraint[k].axis), solverbody[0]->inv_inertia);
				solverbody[1]->delta_LinearVelocity -= deltaImpulse * solverbody[1]->inv_mass * cp.constraint[k].axis;
				solverbody[1]->delta_AngulaVelocity -= deltaImpulse * vector3_trans(vector3_cross(rB, cp.constraint[k].axis), solverbody[1]->inv_inertia);
			}
		}			//��������GPU�ɔC����
	}


	int whatnum = 2;
	if (whatnum == 0) {
		for (int i = 0; i < physics_g::accuracy; i++) {
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
						delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
						delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
						delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
						float old_impulse = constraint.accuminpulse;
						constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
						delta_impulse = constraint.accuminpulse - old_impulse;
						solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
						solverbody[0]->delta_AngulaVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
						solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
						solverbody[1]->delta_AngulaVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);
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
						delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
						delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
						delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
						float old_impulse = constraint.accuminpulse;
						constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
						delta_impulse = constraint.accuminpulse - old_impulse;
						solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
						solverbody[0]->delta_AngulaVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
						solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
						solverbody[1]->delta_AngulaVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);
					}
					{
						Constraint& constraint = cp.constraint[2];
						float delta_impulse = constraint.rhs;
						vector3 delta_velocity[2];
						delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
						delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
						delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
						float old_impulse = constraint.accuminpulse;
						constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
						delta_impulse = constraint.accuminpulse - old_impulse;
						solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
						solverbody[0]->delta_AngulaVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
						solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
						solverbody[1]->delta_AngulaVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);
					}

				}
			}
		}

	}
	else if (whatnum == 1) {

		//QUESTION : ������static�O���Ƃ���������resourceanager���Ă������킩�炸�@��
		static Compute_S::ComputeShader compute_shader;
		compute_shader.Load("./DefaultShader/physics_resolve.cso");

		for (int i = 0; i < physics_g::accuracy; i++) {
			if (pairs.size() == 0)break;

			Pair_max pair_max;
			pair_max.num = pairs.size();

			std::vector<cs_Pair> cs_pair;
			{
				cs_Pair cp;
				for (int pair_num = 0; pair_num < pairs.size(); pair_num++) {
					cp.contacts = pairs[pair_num].contacts;
					cp.solv_num[0] = pairs[pair_num].body[0]->solve->num;
					cp.solv_num[1] = pairs[pair_num].body[1]->solve->num;
					cs_pair.emplace_back(cp);
				}
			}

			static Microsoft::WRL::ComPtr<StructureBuffer> pair_SB = nullptr;
			static Microsoft::WRL::ComPtr<StructureBuffer> max_SB = nullptr;
			static Microsoft::WRL::ComPtr<StructureBuffer> solve_SB = nullptr;
			static Microsoft::WRL::ComPtr<UAVBuffer>       solve_out = nullptr;
			pair_SB = nullptr;
			max_SB = nullptr;
			solve_SB = nullptr;
			solve_out = nullptr;

			assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(cs_Pair), pairs.size(), cs_pair.data(), pair_SB)));
			assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Pair_max), 1, &pair_max, max_SB)));
			assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Solverbody), SBs.size(), SBs.data(), solve_SB)));
			assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(com_Out), pairs.size(), nullptr, solve_out)));

			//assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(cs_Pair), 5, cs_pair.data(), pair_SB)));
			//assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Pair_max), 1, &pair_max, max_SB)));
			//assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Solverbody), SBs.size(), SBs.data(), solve_SB)));
			//assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Solverbody) * 2, 5, nullptr, solve_out)));

			static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView > pair_SRV = nullptr;
			static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView > max_SRV = nullptr;
			static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView > solve_SRV = nullptr;
			static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> solve_OUT = nullptr;
			pair_SRV = nullptr;
			max_SRV = nullptr;
			solve_SRV = nullptr;
			solve_OUT = nullptr;

			assert(SUCCEEDED(compute_shader.createSRV_fromSB(pair_SB, &pair_SRV)));
			assert(SUCCEEDED(compute_shader.createSRV_fromSB(max_SB, &max_SRV)));
			assert(SUCCEEDED(compute_shader.createSRV_fromSB(solve_SB, &solve_SRV)));
			assert(SUCCEEDED(compute_shader.createUAV_fromSB(solve_out, &solve_OUT)));

			ID3D11ShaderResourceView* SRVs[3] = { pair_SRV.Get() ,max_SRV.Get(), solve_SRV.Get() };
			compute_shader.run(SRVs, 3, solve_OUT.Get(), 16, 1, 1);


			com_Out* S = compute_shader.Read_UAV<com_Out>(solve_out);

			struct A_E {
				vector3 sum_AngularVelocity = vector3(0);
				vector3 sum_LinearVelocity = vector3(0);
			};
			std::vector<A_E> ae;
			{
				A_E aaee;
				for (int i = 0; i < colliders.size(); i++) {
					ae.emplace_back(aaee);
				}

				for (int pair_num = 0; pair_num < pairs.size(); pair_num++) {
					ae[S[pair_num].solvs0.num].sum_AngularVelocity += S[pair_num].solvs0.delta_AngulaVelocity;
					ae[S[pair_num].solvs0.num].sum_LinearVelocity += S[pair_num].solvs0.delta_LinearVelocity;

					ae[S[pair_num].solvs1.num].sum_AngularVelocity += S[pair_num].solvs1.delta_AngulaVelocity;
					ae[S[pair_num].solvs1.num].sum_LinearVelocity += S[pair_num].solvs1.delta_LinearVelocity;
				}
				for (int i = 0; i < colliders.size(); i++) {

					for (int o = 0; o < 3; o++) {
						if (ae[i].sum_AngularVelocity[o] != 0)
							ae[i].sum_AngularVelocity[o] = 1.0f / ae[i].sum_AngularVelocity[o];

						if (ae[i].sum_LinearVelocity[o] != 0)
							ae[i].sum_LinearVelocity[o] = 1.0f / ae[i].sum_LinearVelocity[o];

					}
				}
			}

			for (int pair_num = 0; pair_num < pairs.size(); pair_num++) {

				SBs[S[pair_num].solvs0.num].delta_AngulaVelocity += S[pair_num].solvs0.delta_AngulaVelocity * ae[S[pair_num].solvs0.num].sum_AngularVelocity;
				SBs[S[pair_num].solvs0.num].delta_LinearVelocity += S[pair_num].solvs0.delta_LinearVelocity * ae[S[pair_num].solvs0.num].sum_LinearVelocity;

				SBs[S[pair_num].solvs1.num].delta_AngulaVelocity += S[pair_num].solvs1.delta_AngulaVelocity * ae[S[pair_num].solvs1.num].sum_AngularVelocity;
				SBs[S[pair_num].solvs1.num].delta_LinearVelocity += S[pair_num].solvs1.delta_LinearVelocity * ae[S[pair_num].solvs1.num].sum_LinearVelocity;

				for (int C_num = 0; C_num < pairs[pair_num].contacts.contact_num; C_num++) {
					pairs[pair_num].contacts.contactpoints[C_num].constraint[0].accuminpulse += S[pair_num].accumes[C_num][0];
					pairs[pair_num].contacts.contactpoints[C_num].constraint[1].accuminpulse += S[pair_num].accumes[C_num][1];
					pairs[pair_num].contacts.contactpoints[C_num].constraint[2].accuminpulse += S[pair_num].accumes[C_num][2];
				}

			}

		}
	}
	else if (whatnum == 2) {
		for (int acc_num = 0; acc_num < physics_g::accuracy; acc_num++) {

			struct cs_out {
				Solverbody solvs0;
				Solverbody solvs1;
				float accumes[4][3];

				vector3 offset_delta_AngulaVelocity0;
				vector3 offset_delta_LinearVelocity0;

				vector3 offset_delta_AngulaVelocity1;
				vector3 offset_delta_LinearVelocity1;
			};
			std::vector<cs_out> cs_outs;
			cs_out cs;

			for (int P_num = 0; P_num < pairs.size(); P_num++) {
				Contact_pair pair = pairs[P_num];

				coll[0] = pair.body[0];
				coll[1] = pair.body[1];
				cs.solvs0 = *pairs[P_num].body[0]->solve;
				cs.solvs1 = *pairs[P_num].body[1]->solve;

				solverbody[0] = &cs.solvs0;
				solverbody[1] = &cs.solvs1;

				for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {

					//�Փ˓_�̏��@
					Contactpoint& cp = pair.contacts.contactpoints[C_num];
					vector3 rA = vector3_be_rotated_by_quaternion(cp.point[0], solverbody[0]->orientation);
					vector3 rB = vector3_be_rotated_by_quaternion(cp.point[1], solverbody[1]->orientation);

					Constraint constraint = cp.constraint[0];
					float delta_impulse = constraint.rhs;
					vector3 delta_velocity[2];
					delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
					delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
					delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
					float old_impulse = constraint.accuminpulse;
					constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
					delta_impulse = constraint.accuminpulse - old_impulse;

					solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
					solverbody[0]->delta_AngulaVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
					solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
					solverbody[1]->delta_AngulaVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);

					cs.accumes[C_num][0] = constraint.accuminpulse;


					float max_friction = pair.contacts.friction * fabsf(cs.accumes[C_num][0]);
					cp.constraint[1].lowerlimit = -max_friction;
					cp.constraint[1].upperlimit = +max_friction;
					cp.constraint[2].lowerlimit = -max_friction;
					cp.constraint[2].upperlimit = +max_friction;

					{
						Constraint constraint = cp.constraint[1];
						float delta_impulse = constraint.rhs;
						vector3 delta_velocity[2];
						delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
						delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
						delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
						float old_impulse = constraint.accuminpulse;
						constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
						delta_impulse = constraint.accuminpulse - old_impulse;

						solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
						solverbody[0]->delta_AngulaVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
						solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
						solverbody[1]->delta_AngulaVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);

						cs.accumes[C_num][1] = constraint.accuminpulse;
					}

					{
						Constraint constraint = cp.constraint[2];
						float delta_impulse = constraint.rhs;
						vector3 delta_velocity[2];
						delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
						delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
						delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
						float old_impulse = constraint.accuminpulse;
						constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
						delta_impulse = constraint.accuminpulse - old_impulse;

						solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
						solverbody[0]->delta_AngulaVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
						solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
						solverbody[1]->delta_AngulaVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);

						cs.accumes[C_num][2] = constraint.accuminpulse;
					}


				}

				// offset�����߂�
				cs.offset_delta_AngulaVelocity0 = cs.solvs0.delta_AngulaVelocity - pairs[P_num].body[0]->solve->delta_AngulaVelocity;
				cs.offset_delta_LinearVelocity0 = cs.solvs0.delta_LinearVelocity - pairs[P_num].body[0]->solve->delta_LinearVelocity;

				cs.offset_delta_AngulaVelocity1 = cs.solvs1.delta_AngulaVelocity - pairs[P_num].body[1]->solve->delta_AngulaVelocity;
				cs.offset_delta_LinearVelocity1 = cs.solvs1.delta_LinearVelocity - pairs[P_num].body[1]->solve->delta_LinearVelocity;


				assert(!isnan((
					solverbody[0]->delta_LinearVelocity +
					solverbody[0]->delta_AngulaVelocity +
					solverbody[1]->delta_LinearVelocity +
					solverbody[1]->delta_AngulaVelocity
					).norm()));

				cs_outs.push_back(cs);
			}


#if 1

			struct A_E {
				vector3 sum_AngulaVelocity_A = vector3(0);
				vector3 sum_LinearVelocity_A = vector3(0);

				vector3 sum_AngulaVelocity_D = vector3(0);
				vector3 sum_LinearVelocity_D = vector3(0);
				//--------------
				vector3 invsum_AngulaVelocity_A = vector3(0);
				vector3 invsum_LinearVelocity_A = vector3(0);

				vector3 invsum_AngulaVelocity_D = vector3(0);
				vector3 invsum_LinearVelocity_D = vector3(0);
			};
			std::vector<A_E> ae;
			{
				ae.resize(colliders.size());

				for (int pair_num = 0; pair_num < pairs.size(); pair_num++) {
					// delta_�̍��v���Ƃ��ċt���ɁB -> �edelta_�̔���Ƃ邽��
					for (int i = 0; i < 3; i++) {
						cs_outs[pair_num].solvs0.delta_AngulaVelocity[i] > 0 ?
							ae[cs_outs[pair_num].solvs0.num].sum_AngulaVelocity_A[i] += cs_outs[pair_num].solvs0.delta_AngulaVelocity[i] :
							ae[cs_outs[pair_num].solvs0.num].sum_AngulaVelocity_D[i] += cs_outs[pair_num].solvs0.delta_AngulaVelocity[i];
						cs_outs[pair_num].solvs0.delta_LinearVelocity[i] > 0 ?
							ae[cs_outs[pair_num].solvs0.num].sum_LinearVelocity_A[i] += cs_outs[pair_num].solvs0.delta_LinearVelocity[i] :
							ae[cs_outs[pair_num].solvs0.num].sum_LinearVelocity_D[i] += cs_outs[pair_num].solvs0.delta_LinearVelocity[i];


						cs_outs[pair_num].solvs1.delta_AngulaVelocity[i] > 0 ?
							ae[cs_outs[pair_num].solvs1.num].sum_AngulaVelocity_A[i] += cs_outs[pair_num].solvs1.delta_AngulaVelocity[i] :
							ae[cs_outs[pair_num].solvs1.num].sum_AngulaVelocity_D[i] += cs_outs[pair_num].solvs1.delta_AngulaVelocity[i];
						cs_outs[pair_num].solvs1.delta_LinearVelocity[i] > 0 ?
							ae[cs_outs[pair_num].solvs1.num].sum_LinearVelocity_A[i] += cs_outs[pair_num].solvs1.delta_LinearVelocity[i] :
							ae[cs_outs[pair_num].solvs1.num].sum_LinearVelocity_D[i] += cs_outs[pair_num].solvs1.delta_LinearVelocity[i];
					}

					for (int C_num = 0; C_num < pairs[pair_num].contacts.contact_num; C_num++) {
						// accuminpulse��offset�͂�����
						cs_outs[pair_num].accumes[C_num][0] -= pairs[pair_num].contacts.contactpoints[C_num].constraint[0].accuminpulse;
						cs_outs[pair_num].accumes[C_num][1] -= pairs[pair_num].contacts.contactpoints[C_num].constraint[1].accuminpulse;
						cs_outs[pair_num].accumes[C_num][2] -= pairs[pair_num].contacts.contactpoints[C_num].constraint[2].accuminpulse;
					}

				}

				//sum �� �t����
				for (int i = 0; i < colliders.size(); i++) {

					for (int o = 0; o < 3; o++) {

						ae[i].invsum_AngulaVelocity_A[o] =
							+ae[i].sum_AngulaVelocity_A[o] > FLT_EPSILON ? 1.0f / ae[i].sum_AngulaVelocity_A[o] : 0;
						ae[i].invsum_LinearVelocity_A[o] =
							+ae[i].sum_LinearVelocity_A[o] > FLT_EPSILON ? 1.0f / ae[i].sum_LinearVelocity_A[o] : 0;

						ae[i].invsum_AngulaVelocity_D[o] =
							-ae[i].sum_AngulaVelocity_D[o] > FLT_EPSILON ? 1.0f / ae[i].sum_AngulaVelocity_D[o] : 0;
						ae[i].invsum_LinearVelocity_D[o] =
							-ae[i].sum_LinearVelocity_D[o] > FLT_EPSILON ? 1.0f / ae[i].sum_LinearVelocity_D[o] : 0;

						assert(!isnan((
							ae[i].invsum_AngulaVelocity_A[o] +
							ae[i].invsum_LinearVelocity_A[o] +
							ae[i].invsum_AngulaVelocity_D[o] +
							ae[i].invsum_LinearVelocity_D[o]
							)));


					}
				}
			}

			//offset�̌v�Z
			for (int pair_num = 0; pair_num < pairs.size(); pair_num++) {
				// offset * delta_ * invsum ��
				vector3 inv_ang;
				vector3 inv_lin;
				for (int o = 0; o < 3; o++) {
					inv_ang[o] = cs_outs[pair_num].solvs0.delta_AngulaVelocity[o] > 0 ? ae[cs_outs[pair_num].solvs0.num].invsum_AngulaVelocity_A[o] : ae[cs_outs[pair_num].solvs0.num].invsum_AngulaVelocity_D[o];
					inv_lin[o] = cs_outs[pair_num].solvs0.delta_LinearVelocity[o] > 0 ? ae[cs_outs[pair_num].solvs0.num].invsum_LinearVelocity_A[o] : ae[cs_outs[pair_num].solvs0.num].invsum_LinearVelocity_D[o];
				}
				SBs[cs_outs[pair_num].solvs0.num].delta_AngulaVelocity += cs_outs[pair_num].offset_delta_AngulaVelocity0 * cs_outs[pair_num].solvs0.delta_AngulaVelocity * inv_ang;
				SBs[cs_outs[pair_num].solvs0.num].delta_LinearVelocity += cs_outs[pair_num].offset_delta_LinearVelocity0 * cs_outs[pair_num].solvs0.delta_LinearVelocity * inv_lin;


				for (int o = 0; o < 3; o++) {
					inv_ang[o] = cs_outs[pair_num].solvs1.delta_AngulaVelocity[o] > 0 ? ae[cs_outs[pair_num].solvs1.num].invsum_AngulaVelocity_A[o] : ae[cs_outs[pair_num].solvs1.num].invsum_AngulaVelocity_D[o];
					inv_lin[o] = cs_outs[pair_num].solvs1.delta_LinearVelocity[o] > 0 ? ae[cs_outs[pair_num].solvs1.num].invsum_LinearVelocity_A[o] : ae[cs_outs[pair_num].solvs1.num].invsum_LinearVelocity_D[o];
				}
				SBs[cs_outs[pair_num].solvs1.num].delta_AngulaVelocity += cs_outs[pair_num].offset_delta_AngulaVelocity1 * cs_outs[pair_num].solvs1.delta_AngulaVelocity * inv_ang;
				SBs[cs_outs[pair_num].solvs1.num].delta_LinearVelocity += cs_outs[pair_num].offset_delta_LinearVelocity1 * cs_outs[pair_num].solvs1.delta_LinearVelocity * inv_lin;


				for (int C_num = 0; C_num < pairs[pair_num].contacts.contact_num; C_num++) {
					pairs[pair_num].contacts.contactpoints[C_num].constraint[0].accuminpulse += cs_outs[pair_num].accumes[C_num][0] * 0.5;
					pairs[pair_num].contacts.contactpoints[C_num].constraint[1].accuminpulse += cs_outs[pair_num].accumes[C_num][1] * 0.5;
					pairs[pair_num].contacts.contactpoints[C_num].constraint[2].accuminpulse += cs_outs[pair_num].accumes[C_num][2] * 0.5;

				}
			}
		}
	}
#else

			struct A_E {
				vector3 sum_AngulaVelocity = vector3(0);
				vector3 sum_LinearVelocity = vector3(0);

				vector3 invsum_AngulaVelocity = vector3(0);
				vector3 invsum_LinearVelocity = vector3(0);
			};

			std::vector<A_E> ae;
			{
				ae.resize(colliders.size());

				for (int pair_num = 0; pair_num < pairs.size(); pair_num++) {
					// delta_�̍��v���Ƃ��ċt���ɁB -> �edelta_�̔���Ƃ邽��
					for (int i = 0; i < 3; i++) {

						ae[cs_outs[pair_num].solvs0.num].sum_AngulaVelocity[i] += fabsf(cs_outs[pair_num].solvs0.delta_AngulaVelocity[i]);
						ae[cs_outs[pair_num].solvs0.num].sum_LinearVelocity[i] += fabsf(cs_outs[pair_num].solvs0.delta_LinearVelocity[i]);

						ae[cs_outs[pair_num].solvs1.num].sum_AngulaVelocity[i] += fabsf(cs_outs[pair_num].solvs1.delta_AngulaVelocity[i]);
						ae[cs_outs[pair_num].solvs1.num].sum_LinearVelocity[i] += fabsf(cs_outs[pair_num].solvs1.delta_LinearVelocity[i]);
					}

					for (int C_num = 0; C_num < pairs[pair_num].contacts.contact_num; C_num++) {
						// accuminpulse��offset�͂�����
						cs_outs[pair_num].accumes[C_num][0] -= pairs[pair_num].contacts.contactpoints[C_num].constraint[0].accuminpulse;
						cs_outs[pair_num].accumes[C_num][1] -= pairs[pair_num].contacts.contactpoints[C_num].constraint[1].accuminpulse;
						cs_outs[pair_num].accumes[C_num][2] -= pairs[pair_num].contacts.contactpoints[C_num].constraint[2].accuminpulse;
					}

				}

				//sum �� �t����
				for (int i = 0; i < colliders.size(); i++) {

					for (int o = 0; o < 3; o++) {

						ae[i].invsum_AngulaVelocity[o] =
							+ae[i].sum_AngulaVelocity[o] > FLT_EPSILON ? 1.0f / ae[i].sum_AngulaVelocity[o] : 0;
						ae[i].invsum_LinearVelocity[o] =
							+ae[i].sum_LinearVelocity[o] > FLT_EPSILON ? 1.0f / ae[i].sum_LinearVelocity[o] : 0;

					}
				}
			}



			//offset�̌v�Z
			for (int pair_num = 0; pair_num < pairs.size(); pair_num++) {
				// offset * delta_ * invsum ��
				vector3 inv_ang;
				vector3 inv_lin;
				SBs[cs_outs[pair_num].solvs0.num].delta_AngulaVelocity += cs_outs[pair_num].offset_delta_AngulaVelocity0 * cs_outs[pair_num].solvs0.delta_AngulaVelocity * ae[cs_outs[pair_num].solvs0.num].invsum_AngulaVelocity;
				SBs[cs_outs[pair_num].solvs0.num].delta_LinearVelocity += cs_outs[pair_num].offset_delta_LinearVelocity0 * cs_outs[pair_num].solvs0.delta_LinearVelocity * ae[cs_outs[pair_num].solvs0.num].invsum_LinearVelocity;

				SBs[cs_outs[pair_num].solvs1.num].delta_AngulaVelocity += cs_outs[pair_num].offset_delta_AngulaVelocity1 * cs_outs[pair_num].solvs1.delta_AngulaVelocity * ae[cs_outs[pair_num].solvs1.num].invsum_AngulaVelocity;
				SBs[cs_outs[pair_num].solvs1.num].delta_LinearVelocity += cs_outs[pair_num].offset_delta_LinearVelocity1 * cs_outs[pair_num].solvs1.delta_LinearVelocity * ae[cs_outs[pair_num].solvs1.num].invsum_LinearVelocity;


				for (int C_num = 0; C_num < pairs[pair_num].contacts.contact_num; C_num++) {
					pairs[pair_num].contacts.contactpoints[C_num].constraint[0].accuminpulse += cs_outs[pair_num].accumes[C_num][0];
					pairs[pair_num].contacts.contactpoints[C_num].constraint[1].accuminpulse += cs_outs[pair_num].accumes[C_num][1];
					pairs[pair_num].contacts.contactpoints[C_num].constraint[2].accuminpulse += cs_outs[pair_num].accumes[C_num][2];

				}
			}
		}
	}
#endif

	else if (whatnum == 3) {
		for (int i = 0; i < physics_g::accuracy; i++) {
			if (physics_g::accuracy % 2 == 0) {
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
							delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
							delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
							delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
							float old_impulse = constraint.accuminpulse;
							constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
							delta_impulse = constraint.accuminpulse - old_impulse;
							solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
							solverbody[0]->delta_AngulaVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
							solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
							solverbody[1]->delta_AngulaVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);
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
							delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
							delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
							delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
							float old_impulse = constraint.accuminpulse;
							constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
							delta_impulse = constraint.accuminpulse - old_impulse;
							solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
							solverbody[0]->delta_AngulaVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
							solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
							solverbody[1]->delta_AngulaVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);
						}
						{
							Constraint& constraint = cp.constraint[2];
							float delta_impulse = constraint.rhs;
							vector3 delta_velocity[2];
							delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
							delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
							delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
							float old_impulse = constraint.accuminpulse;
							constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
							delta_impulse = constraint.accuminpulse - old_impulse;
							solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
							solverbody[0]->delta_AngulaVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
							solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
							solverbody[1]->delta_AngulaVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);
						}

					}
				}
			}
			else {
				for (int P_num = pairs.size() - 1; P_num >= 0; P_num--) {
					Contact_pair& pair = pairs[P_num];

					coll[0] = pair.body[0];
					coll[1] = pair.body[1];
					solverbody[0] = coll[0]->solve;
					solverbody[1] = coll[1]->solve;

					for (int C_num = pair.contacts.contact_num - 1; C_num >= 0; C_num--) {
						//�Փ˓_�̏��@
						Contactpoint& cp = pair.contacts.contactpoints[C_num];
						vector3 rA = vector3_be_rotated_by_quaternion(cp.point[0], solverbody[0]->orientation);
						vector3 rB = vector3_be_rotated_by_quaternion(cp.point[1], solverbody[1]->orientation);

						{
							Constraint& constraint = cp.constraint[0];
							float delta_impulse = constraint.rhs;
							vector3 delta_velocity[2];
							delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
							delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
							delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
							float old_impulse = constraint.accuminpulse;
							constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
							delta_impulse = constraint.accuminpulse - old_impulse;
							solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
							solverbody[0]->delta_AngulaVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
							solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
							solverbody[1]->delta_AngulaVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);
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
							delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
							delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
							delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
							float old_impulse = constraint.accuminpulse;
							constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
							delta_impulse = constraint.accuminpulse - old_impulse;
							solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
							solverbody[0]->delta_AngulaVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
							solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
							solverbody[1]->delta_AngulaVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);
						}
						{
							Constraint& constraint = cp.constraint[2];
							float delta_impulse = constraint.rhs;
							vector3 delta_velocity[2];
							delta_velocity[0] = solverbody[0]->delta_LinearVelocity + vector3_cross(solverbody[0]->delta_AngulaVelocity, rA);
							delta_velocity[1] = solverbody[1]->delta_LinearVelocity + vector3_cross(solverbody[1]->delta_AngulaVelocity, rB);
							delta_impulse -= constraint.jacDiagInv * vector3_dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
							float old_impulse = constraint.accuminpulse;
							constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
							delta_impulse = constraint.accuminpulse - old_impulse;
							solverbody[0]->delta_LinearVelocity += delta_impulse * solverbody[0]->inv_mass * constraint.axis;
							solverbody[0]->delta_AngulaVelocity += vector3_trans(vector3_cross(rA, constraint.axis * delta_impulse), solverbody[0]->inv_inertia);
							solverbody[1]->delta_LinearVelocity -= delta_impulse * solverbody[1]->inv_mass * constraint.axis;
							solverbody[1]->delta_AngulaVelocity -= vector3_trans(vector3_cross(rB, constraint.axis * delta_impulse), solverbody[1]->inv_inertia);
						}

					}
				}
			}

		}

	}

	// ���x�̍X�V
	for (int i = 0; i < colliders.size(); i++) {
		colliders[i]->linear_velocity += colliders[i]->solve->delta_LinearVelocity;
		colliders[i]->angula_velocity += colliders[i]->solve->delta_AngulaVelocity;

	}

}
#pragma endregion
//:::::::::::::::::::::::::::