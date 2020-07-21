#include "ALP_resolve_contact.h"

using namespace Adollib;
using namespace Contacts;

//:::::::::::::::::::::::::::
#pragma region resolve_contact
//:::::::::::::::::::::::::::

//衝突解決
// ソルバーボディ(解決用のなんか)
// ボールジョイント
struct Balljoint {
	float bias; // 拘束の強さの調整値
	int rigidBodyA; // 剛体Aへのインデックス
	int rigidBodyB; // 剛体Bへのインデックス
	vector3 anchorA; // 剛体Aのローカル座標系における接続点
	vector3 anchorB; // 剛体Bのローカル座標系における接続点
	Constraint constraint; // 拘束

	// 初期化
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
//GPUに渡す情報
struct cs_Pair
{
	int solv_num[2]; //solverbody配列の何番目に情報が入っているか
	Contact contacts;
};
struct  Pair_max
{
	int num;
};
struct  com_Out
{
	physics_function::Solverbody solvs[2];
	float accumes[4][3];
};

void physics_function::resolve_contact(std::vector<Collider*> colliders, std::vector<Contacts::Contact_pair>& pairs) {

	//::: 解決用オブジェクトの生成 :::::::::::
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

	//std::vector<Balljoint> balljoints; //今回はなし
	Collider* coll[2];
	Solverbody* solverbody[2];

	for (int P_num = 0; P_num < pairs.size(); P_num++) {
		Contact_pair& pair = pairs[P_num];

		coll[0] = pairs[P_num].body[0];
		coll[1] = pairs[P_num].body[1];
		solverbody[0] = coll[0]->solve;
		solverbody[1] = coll[1]->solve;

		// 摩擦の獲得
		pair.contacts.friction = sqrtf(coll[0]->friction * coll[1]->friction);

		//::: 衝突点情報の更新 ::::::::
		for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
			Contactpoint& cp = pair.contacts.contactpoints[C_num];

			vector3 rA = vector3_be_rotated_by_quaternion(cp.point[0], solverbody[0]->orientation);
			vector3 rB = vector3_be_rotated_by_quaternion(cp.point[1], solverbody[1]->orientation);

			// 反発係数の獲得
			// 継続の衝突の場合反発係数を0にする
			float restitution = pair.type == Pairtype::new_pair ? 0.5f * (coll[0]->restitution + coll[1]->restitution) : 0.0f;

			//衝突時のそれぞれの速度
			vector3 pdota;
			pdota = vector3_cross(coll[0]->angular_velocity, rA);
			pdota += coll[0]->linear_velocity;

			vector3 pdotb;
			pdotb = vector3_cross(coll[1]->angular_velocity, rB);
			pdotb += coll[1]->linear_velocity;

			//衝突時の衝突平面法線方向の相対速度(結局衝突に使うのは法線方向への速さ)
			vector3 vrel = pdota - pdotb;

			vector3
				tangent1, //normalに対するz方向
				tangent2; //normalに対するx方向
			CalcTangentVector(cp.normal, tangent1, tangent2);

			//Baraff[1997]の式(8-18)の分母(denominator)を求める
			float term1 = coll[0]->inverse_mass();
			float term2 = coll[1]->inverse_mass();
			vector3 tA, tB;

			float term3, term4, denominator;
			// Normal
			{
				//Baraff[1997]の式(8-18)の分母(denominator)を求める
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

				cp.constraint[0].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)の分母
				cp.constraint[0].rhs = -(1.0f + restitution) * vector3_dot(axis, vrel); //Baraff1997(8-18)の分子

				if (0.0f < cp.distance - slop) {
					cp.constraint[0].rhs += (bias * (cp.distance - slop)) / timeStep; //めり込みを直す値
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

				cp.constraint[1].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)の分母
				cp.constraint[1].rhs = -(1.0f + restitution) * vector3_dot(axis, vrel); //Baraff1997(8-18)の分子
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

				cp.constraint[2].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)の分母?
				cp.constraint[2].rhs = -(1.0f + restitution) * vector3_dot(axis, vrel); //Baraff1997(8-18)の分子
				cp.constraint[2].rhs *= cp.constraint[2].jacDiagInv;
				cp.constraint[2].lowerlimit = 0.0f;
				cp.constraint[2].upperlimit = FLT_MAX;
				cp.constraint[2].axis = axis;
			}

		}
	}

	//::: 変化量を求める :::::::::::::::
	for (int P_num = 0; P_num < pairs.size(); P_num++) {
		Contact_pair& pair = pairs[P_num];

		coll[0] = pair.body[0];
		coll[1] = pair.body[1];
		solverbody[0] = coll[0]->solve;
		solverbody[1] = coll[1]->solve;

		for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
			//衝突点の情報　
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

	//ここからGPUに任せる
#if 1
	for (int i = 0; i < 10; i++) {
		for (int P_num = 0; P_num < pairs.size(); P_num++) {
			Contact_pair& pair = pairs[P_num];

			coll[0] = pair.body[0];
			coll[1] = pair.body[1];
			solverbody[0] = coll[0]->solve;
			solverbody[1] = coll[1]->solve;

			for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
				//衝突点の情報　
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

	//QUESTION : ここのstatic外すとすぐ落ちるresourceanager見ても原因わからず　謎
	static Compute_S::ComputeShader compute_shader;
	compute_shader.Load("./DefaultShader/physics_resolve.cso");



	for (int i = 0; i < 10; i++) {
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
		for (int pair_num = 0; pair_num < pairs.size(); pair_num++) {

			if (SBs[S[pair_num].solvs[0].num].delta_AngularVelocity != S[pair_num].solvs[0].delta_AngularVelocity ||
				SBs[S[pair_num].solvs[1].num].delta_AngularVelocity != S[pair_num].solvs[1].delta_AngularVelocity
				) {
				int dasfgdf = 0;
			}

			SBs[S[pair_num].solvs[0].num].delta_AngularVelocity += S[pair_num].solvs[0].delta_AngularVelocity;
			SBs[S[pair_num].solvs[0].num].delta_LinearVelocity += S[pair_num].solvs[0].delta_LinearVelocity;

			SBs[S[pair_num].solvs[1].num].delta_AngularVelocity += S[pair_num].solvs[1].delta_AngularVelocity;
			SBs[S[pair_num].solvs[1].num].delta_LinearVelocity += S[pair_num].solvs[1].delta_LinearVelocity;

			for (int C_num = 0; C_num < pairs[pair_num].contacts.contact_num; C_num++) {
				pairs[pair_num].contacts.contactpoints[C_num].constraint[0].accuminpulse += S[pair_num].accumes[C_num][0];
				pairs[pair_num].contacts.contactpoints[C_num].constraint[1].accuminpulse += S[pair_num].accumes[C_num][1];
				pairs[pair_num].contacts.contactpoints[C_num].constraint[2].accuminpulse += S[pair_num].accumes[C_num][2];
			}

		}

	}
#elif 0
//QUESTION : ここのstatic外すとすぐ落ちるresourceanager見ても原因わからず　謎
	static Compute_S::ComputeShader compute_shader;
	compute_shader.Load("./DefaultShader/physics_resolve.cso");



	for (int i = 0; i < 10; i++) {
		for (int pair_num = 0; pair_num < pairs.size(); pair_num++) {

			cs_Pair cs_pair;
			Pair_max pair_max;
			pair_max.num = pairs.size();

			cs_pair.contacts = pairs[pair_num].contacts;
			cs_pair.solv_num[0] = pairs[pair_num].body[0]->solve->num;
			cs_pair.solv_num[1] = pairs[pair_num].body[1]->solve->num;


			static Microsoft::WRL::ComPtr<StructureBuffer> pair_SB = nullptr;
			static Microsoft::WRL::ComPtr<StructureBuffer> max_SB = nullptr;
			static Microsoft::WRL::ComPtr<StructureBuffer> solve_SB = nullptr;
			static Microsoft::WRL::ComPtr<UAVBuffer> solve_out = nullptr;

			assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(cs_Pair), 1, &cs_pair, pair_SB)));
			assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Pair_max), 1, &pair_max, max_SB)));
			assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Solverbody), SBs.size(), SBs.data(), solve_SB)));
			assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Solverbody) * 2, 1, nullptr, solve_out)));

			//assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(cs_Pair), 5, cs_pair.data(), pair_SB)));
			//assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Pair_max), 1, &pair_max, max_SB)));
			//assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Solverbody), SBs.size(), SBs.data(), solve_SB)));
			//assert(SUCCEEDED(compute_shader.create_StructureBuffer(sizeof(Solverbody) * 2, 5, nullptr, solve_out)));

			static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView > pair_SRV = nullptr;
			static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView > max_SRV = nullptr;
			static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  solve_SRV = nullptr;
			static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> solve_OUT = nullptr;

			assert(SUCCEEDED(compute_shader.createSRV_fromSB(pair_SB, &pair_SRV)));
			assert(SUCCEEDED(compute_shader.createSRV_fromSB(max_SB, &max_SRV)));
			assert(SUCCEEDED(compute_shader.createSRV_fromSB(solve_SB, &solve_SRV)));
			assert(SUCCEEDED(compute_shader.createUAV_fromSB(solve_out, &solve_OUT)));

			ID3D11ShaderResourceView* SRVs[3] = { pair_SRV.Get() ,max_SRV.Get(), solve_SRV.Get() };
			compute_shader.run(SRVs, 3, solve_OUT.Get(), 16, 1, 1);

			Solverbody* S = compute_shader.Read_UAV<Solverbody>(solve_out);
			SBs[S[0].num].delta_AngularVelocity += S[0].delta_AngularVelocity;
			SBs[S[0].num].delta_LinearVelocity += S[0].delta_LinearVelocity;
			SBs[S[1].num].delta_AngularVelocity += S[1].delta_AngularVelocity;
			SBs[S[1].num].delta_LinearVelocity += S[1].delta_LinearVelocity;

			//if (
			//	SBs[S[i].num].delta_AngularVelocity == S[i].delta_AngularVelocity &&
			//	SBs[S[i].num].delta_LinearVelocity == S[i].delta_LinearVelocity &&
			//	SBs[S[i].num].inv_inertia == S[i].inv_inertia &&
			//	SBs[S[i].num].inv_mass == S[i].inv_mass &&
			//	SBs[S[i].num].num == S[i].num &&
			//	SBs[S[i].num].orientation == S[i].orientation
			//	) {
			//	int dafsdgf = 0;
			//}
			//else {
			//	int dafsgdhfjg = 0;
			//}

		}
	}


#endif

	// 速度の更新
	for (int i = 0; i < colliders.size(); i++) {
		colliders[i]->linear_velocity += colliders[i]->solve->delta_LinearVelocity;
		colliders[i]->angular_velocity += colliders[i]->solve->delta_AngularVelocity;
	}

}
#pragma endregion
//:::::::::::::::::::::::::::