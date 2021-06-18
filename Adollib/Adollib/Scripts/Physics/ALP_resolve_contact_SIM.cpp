//#include "ALP_resolve_contact.h"
//
//#include "../Imgui/debug.h"
//#include "../Imgui/work_meter.h"
//
//#include "ALP__physics_manager.h"
//#include "ALP_physics.h"
//
//using namespace Adollib;
//using namespace Physics_function;
//using namespace Contacts;
//
//#ifdef PHYICSE_USED_SIMD
//
////:::::::::::::::::::::::::::
//#pragma region resolve_contact
////:::::::::::::::::::::::::::
//
////衝突解決
//// ソルバーボディ(解決用のなんか)
//// ボールジョイント
//struct Balljoint {
//	float bias; // 拘束の強さの調整値
//	int rigidBodyA; // 剛体Aへのインデックス
//	int rigidBodyB; // 剛体Bへのインデックス
//	Vector3 anchorA; // 剛体Aのローカル座標系における接続点
//	Vector3 anchorB; // 剛体Bのローカル座標系における接続点
//	Constraint constraint; // 拘束
//
//	// 初期化
//	void reset()
//	{
//		bias = 0.1f;
//		constraint.accuminpulse = 0.0f;
//	}
//};
//
//void CalcTangentVector(const Vector3& normal, DirectX::XMVECTOR& tangent1, DirectX::XMVECTOR& tangent2)
//{
//	Vector3 vec(1.0f, 0.0f, 0.0f);
//	Vector3 n(normal);
//	n[0] = 0.0f;
//	if (n.norm() < FLT_EPSILON) {
//		vec = Vector3(0.0f, 1.0f, 0.0f);
//	}
//
//	DirectX::XMVECTOR xmnorm = DirectX::XMLoadFloat3(&normal);
//	tangent1 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(xmnorm, DirectX::XMLoadFloat3(&vec)));
//	tangent2 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(tangent1, xmnorm));
//}
//#endif
//
//#pragma optimize("", off)
////TODO : 最適化されるとなぜか落ちる 要 原因究明
//void calc_no_optimize(ALP_Solverbody* solverbody[2], const DirectX::XMVECTOR& rA, const DirectX::XMVECTOR& rB, const DirectX::XMVECTOR& axis, const float deltaImpulse) {
//	const DirectX::XMVECTOR& sale0 = DirectX::XMVectorScale(axis, deltaImpulse * solverbody[0]->inv_mass);
//	const DirectX::XMVECTOR& sale1 = DirectX::XMVectorScale(axis, deltaImpulse * solverbody[1]->inv_mass);
//
//	//Vector3 a = Vector3(0);
//	//DirectX::XMVECTOR deb0 = DirectX::XMLoadFloat3(&a);
//	//DirectX::XMVECTOR deb1 = DirectX::XMLoadFloat3(&a);
//	//solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(deb0, deb1);
//
//	solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, sale0);
//	solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), deltaImpulse));
//	solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, sale1);
//	solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), deltaImpulse));
//}
//
//#pragma optimize("", on)
//
//void Physics_function::resolve_contact(std::list<ALP_Collider*>& colliders, std::vector<Contacts::Contact_pair>& pairs) {
//
//	//::: 解決用オブジェクトの生成 :::::::::::
//	std::vector<ALP_Solverbody> SBs;
//	{
//		int count = 0;
//		std::vector<ALP_Solverbody>::iterator itr;
//		SBs.resize(colliders.size());
//		for (const auto& coll : colliders) {
//			ALP_Solverbody& SB = SBs[count];
//
//			SB.delta_LinearVelocity = DirectX::XMLoadFloat3(&vector3_zero());
//			SB.delta_AngulaVelocity = DirectX::XMLoadFloat3(&vector3_zero());
//
//			if (coll->get_ALPphysics()->is_kinmatic_anglar) SB.inv_inertia = DirectX::XMLoadFloat3x3(&coll->get_ALPphysics()->inverse_inertial_tensor());
//			else itr->inv_inertia = DirectX::XMLoadFloat3x3(&matrix33_zero());
//
//			SB.inv_mass = coll->get_ALPphysics()->inverse_mass();
//
//			++count;
//		}
//
//		count = 0;
//		for (auto& coll : colliders) {
//			coll->get_ALPphysics()->solve = &SBs[count];
//			count++;
//		}
//	}
//
//	Collider_shape* coll[2];
//	ALP_Physics* ALPphysics[2];
//	ALP_Solverbody* solverbody[2];
//	//std::vector<Balljoint> balljoints; //今回はなし
//
//	for (auto& pair : pairs) {
//
//		coll[0] = pair.body[0];
//		coll[1] = pair.body[1];
//		ALPphysics[0] = coll[0]->get_ALPcollider()->get_ALPphysics();
//		ALPphysics[1] = coll[1]->get_ALPcollider()->get_ALPphysics();
//		solverbody[0] = ALPphysics[0]->solve;
//		solverbody[1] = ALPphysics[1]->solve;
//
//		// 摩擦の獲得
//		pair.contacts.friction = sqrtf(ALPphysics[0]->dynamic_friction * ALPphysics[1]->dynamic_friction);
//
//		//::: 衝突点情報の更新 ::::::::
//		for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
//			Contactpoint& cp = pair.contacts.contactpoints[C_num];
//
//			DirectX::XMVECTOR rA = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&cp.point[0]), DirectX::XMLoadFloat4(&coll[0]->world_orientation()));
//			DirectX::XMVECTOR rB = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&cp.point[1]), DirectX::XMLoadFloat4(&coll[1]->world_orientation()));
//
//			// 反発係数の獲得
//			// 継続の衝突の場合反発係数を0にする
//			const float restitution = (pair.type == Pairtype::new_pair ? 0.5f * (ALPphysics[0]->restitution + ALPphysics[1]->restitution) : 0.0f);
//
//
//			//衝突時のそれぞれの速度
//			DirectX::XMVECTOR pdota;
//			pdota = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&ALPphysics[0]->anglar_velocity), rA);
//			pdota = DirectX::XMVectorAdd(pdota, DirectX::XMLoadFloat3(&ALPphysics[0]->linear_velocity));
//
//			DirectX::XMVECTOR pdotb;
//			pdotb = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&ALPphysics[1]->anglar_velocity), rB);
//			pdotb = DirectX::XMVectorAdd(pdotb, DirectX::XMLoadFloat3(&ALPphysics[1]->linear_velocity));
//
//			//衝突時の衝突平面法線方向の相対速度(結局衝突に使うのは法線方向への速さ)
//			DirectX::XMVECTOR vrel = DirectX::XMVectorSubtract(pdota, pdotb);
//
//			DirectX::XMVECTOR
//				tangent1, //normalに対するz方向
//				tangent2; //normalに対するx方向
//			CalcTangentVector(cp.normal, tangent1, tangent2);
//
//			//Baraff[1997]の式(8-18)の分母(denominator)を求める
//			const float& term1 = ALPphysics[0]->inverse_mass();
//			const float& term2 = ALPphysics[1]->inverse_mass();
//			DirectX::XMVECTOR tA, tB;
//
//			float term3, term4, denominator;
//			DirectX::XMVECTOR axis;
//			// Normal
//			{
//				//Baraff[1997]の式(8-18)の分母(denominator)を求める
//				axis = DirectX::XMLoadFloat3(&cp.normal);
//				tA = DirectX::XMVector3Cross(rA, axis);
//				tB = DirectX::XMVector3Cross(rB, axis);
//				tA = DirectX::XMVector3Transform(tA, ALPphysics[0]->solve->inv_inertia);
//				tB = DirectX::XMVector3Transform(tB, ALPphysics[1]->solve->inv_inertia);
//				tA = DirectX::XMVector3Cross(tA, rA);
//				tB = DirectX::XMVector3Cross(tB, rB);
//				term3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tA));
//				term4 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tB));
//				denominator = term1 + term2 + term3 + term4;
//
//				cp.constraint[0].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)の分母
//				cp.constraint[0].rhs = -(1.0f + restitution) * DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, vrel)); //Baraff1997(8-18)の分子
//
//				if (0.0f < cp.distance - Phyisics_manager::physicsParams.slop)
//					cp.constraint[0].rhs += (Phyisics_manager::physicsParams.bias * (cp.distance - Phyisics_manager::physicsParams.slop)) / Phyisics_manager::physicsParams.timeStep; //めり込みを直す力
//
//				cp.constraint[0].rhs *= cp.constraint[0].jacDiagInv;
//				cp.constraint[0].lowerlimit = 0.0f;
//				cp.constraint[0].upperlimit = FLT_MAX;
//				DirectX::XMStoreFloat3(&cp.constraint[0].axis, axis);
//			}
//
//			// Tangent1
//			{
//				axis = tangent1;
//				tA = DirectX::XMVector3Cross(rA, axis);
//				tB = DirectX::XMVector3Cross(rB, axis);
//				tA = DirectX::XMVector3Transform(tA, ALPphysics[0]->solve->inv_inertia);
//				tB = DirectX::XMVector3Transform(tB, ALPphysics[1]->solve->inv_inertia);
//				tA = DirectX::XMVector3Cross(tA, rA);
//				tB = DirectX::XMVector3Cross(tB, rB);
//				term3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tA));
//				term4 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tB));
//				denominator = term1 + term2 + term3 + term4;
//
//				cp.constraint[1].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)の分母
//				cp.constraint[1].rhs = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, vrel)); //Baraff1997(8-18)の分子
//				cp.constraint[1].rhs *= cp.constraint[1].jacDiagInv;
//				cp.constraint[1].lowerlimit = 0.0f;
//				cp.constraint[1].upperlimit = 0;
//				DirectX::XMStoreFloat3(&cp.constraint[1].axis, axis);
//			}
//
//			// Tangent2
//			{
//				axis = tangent2;
//				tA = DirectX::XMVector3Cross(rA, axis);
//				tB = DirectX::XMVector3Cross(rB, axis);
//				tA = DirectX::XMVector3Transform(tA, ALPphysics[0]->solve->inv_inertia);
//				tB = DirectX::XMVector3Transform(tB, ALPphysics[1]->solve->inv_inertia);
//				tA = DirectX::XMVector3Cross(tA, rA);
//				tB = DirectX::XMVector3Cross(tB, rB);
//				term3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tA));
//				term4 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, tB));
//				denominator = term1 + term2 + term3 + term4;
//
//				cp.constraint[2].jacDiagInv = 1.0f / denominator; //Baraff1997(8-18)の分母?
//				cp.constraint[2].rhs = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, vrel)); //Baraff1997(8-18)の分子
//				cp.constraint[2].rhs *= cp.constraint[2].jacDiagInv;
//				cp.constraint[2].lowerlimit = 0.0f;
//				cp.constraint[2].upperlimit = 0;
//				DirectX::XMStoreFloat3(&cp.constraint[2].axis, axis);
//			}
//
//
//		}
//	}
//
//	//::: 変化量を求める :::::::::::::::
//	for (auto& pair : pairs) {
//
//		coll[0] = pair.body[0];
//		coll[1] = pair.body[1];
//		ALPphysics[0] = coll[0]->get_ALPcollider()->get_ALPphysics();
//		ALPphysics[1] = coll[1]->get_ALPcollider()->get_ALPphysics();
//		solverbody[0] = ALPphysics[0]->solve;
//		solverbody[1] = ALPphysics[1]->solve;
//
//		for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
//			//衝突点の情報
//			const Contactpoint& cp = pair.contacts.contactpoints[C_num];
//			const DirectX::XMVECTOR rA = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&cp.point[0]), DirectX::XMLoadFloat4(&coll[0]->world_orientation()));
//			const DirectX::XMVECTOR rB = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&cp.point[1]), DirectX::XMLoadFloat4(&coll[1]->world_orientation()));
//
//			for (int k = 0; k < 3; k++) {
//				const float& deltaImpulse = cp.constraint[k].accuminpulse;
//				const DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&cp.constraint[k].axis);
//
//				//TODO : 最適化されるとなぜか落ちる 要 原因究明
//
//				//solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVectorScale(axis, deltaImpulse * solverbody[0]->inv_mass));
//				//solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), deltaImpulse));
//				//solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, DirectX::XMVectorScale(axis, deltaImpulse * solverbody[1]->inv_mass));
//				//solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), deltaImpulse));
//
//				//最適化されると落ちるのでoptimizeをいじった別関数を使う
//				calc_no_optimize(solverbody, rA, rB, axis, deltaImpulse);
//			}
//		}
//
//	}
//
//
//	for (int i = 0; i < Phyisics_manager::physicsParams.solver_iterations; i++) {
//		for (auto& pair : pairs) {
//
//			coll[0] = pair.body[0];
//			coll[1] = pair.body[1];
//			ALPphysics[0] = coll[0]->get_ALPcollider()->get_ALPphysics();
//			ALPphysics[1] = coll[1]->get_ALPcollider()->get_ALPphysics();
//			solverbody[0] = ALPphysics[0]->solve;
//			solverbody[1] = ALPphysics[1]->solve;
//
//			for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++) {
//				//衝突点の情報
//				Contactpoint& cp = pair.contacts.contactpoints[C_num];
//				const DirectX::XMVECTOR rA = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&cp.point[0]), DirectX::XMLoadFloat4(&coll[0]->world_orientation()));
//				const DirectX::XMVECTOR rB = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&cp.point[1]), DirectX::XMLoadFloat4(&coll[1]->world_orientation()));
//
//				{
//					Constraint& constraint = cp.constraint[0];
//					const DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&constraint.axis);
//
//					float delta_impulse = constraint.rhs;
//					DirectX::XMVECTOR delta_velocity[2];
//					delta_velocity[0] = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA));
//					delta_velocity[1] = DirectX::XMVectorAdd(solverbody[1]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB));
//					delta_impulse -= constraint.jacDiagInv * DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, DirectX::XMVectorSubtract(delta_velocity[0], delta_velocity[1])));
//					float old_impulse = constraint.accuminpulse;
//					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
//					delta_impulse = constraint.accuminpulse - old_impulse;
//					solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass));
//					solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), delta_impulse));
//					solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass));
//					solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), delta_impulse));
//				}
//
//				float max_friction = pair.contacts.friction * fabsf(cp.constraint[0].accuminpulse);
//				cp.constraint[1].lowerlimit = -max_friction;
//				cp.constraint[1].upperlimit = +max_friction;
//				cp.constraint[2].lowerlimit = -max_friction;
//				cp.constraint[2].upperlimit = +max_friction;
//
//				{
//					Constraint& constraint = cp.constraint[1];
//					DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&constraint.axis);
//
//					float delta_impulse = constraint.rhs;
//					DirectX::XMVECTOR delta_velocity[2];
//					delta_velocity[0] = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA));
//					delta_velocity[1] = DirectX::XMVectorAdd(solverbody[1]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB));
//					delta_impulse -= constraint.jacDiagInv * DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, DirectX::XMVectorSubtract(delta_velocity[0], delta_velocity[1])));
//					float old_impulse = constraint.accuminpulse;
//					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
//					delta_impulse = constraint.accuminpulse - old_impulse;
//					solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass));
//					solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), delta_impulse));
//					solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass));
//					solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), delta_impulse));
//				}
//
//
//				{
//					Constraint& constraint = cp.constraint[2];
//					DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&constraint.axis);
//
//					float delta_impulse = constraint.rhs;
//					DirectX::XMVECTOR delta_velocity[2];
//					delta_velocity[0] = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[0]->delta_AngulaVelocity, rA));
//					delta_velocity[1] = DirectX::XMVectorAdd(solverbody[1]->delta_LinearVelocity, DirectX::XMVector3Cross(solverbody[1]->delta_AngulaVelocity, rB));
//					delta_impulse -= constraint.jacDiagInv * DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, DirectX::XMVectorSubtract(delta_velocity[0], delta_velocity[1])));
//					float old_impulse = constraint.accuminpulse;
//					constraint.accuminpulse = ALClamp(old_impulse + delta_impulse, constraint.lowerlimit, constraint.upperlimit);
//					delta_impulse = constraint.accuminpulse - old_impulse;
//					solverbody[0]->delta_LinearVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[0]->inv_mass));
//					solverbody[0]->delta_AngulaVelocity = DirectX::XMVectorAdd(solverbody[0]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rA, axis), solverbody[0]->inv_inertia), delta_impulse));
//					solverbody[1]->delta_LinearVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_LinearVelocity, DirectX::XMVectorScale(axis, delta_impulse * solverbody[1]->inv_mass));
//					solverbody[1]->delta_AngulaVelocity = DirectX::XMVectorSubtract(solverbody[1]->delta_AngulaVelocity, DirectX::XMVectorScale(DirectX::XMVector3Transform(DirectX::XMVector3Cross(rB, axis), solverbody[1]->inv_inertia), delta_impulse));
//				}
//
//
//			}
//		}
//	}
//
//	// 速度の更新
//	for (auto& coll : colliders) {
//		if (coll->get_ALPphysics()->is_kinmatic_linear) {
//			Vector3 linervec;
//			DirectX::XMStoreFloat3(&linervec, coll->get_ALPphysics()->solve->delta_LinearVelocity);
//			coll->get_ALPphysics()->linear_velocity += linervec;
//		}
//		if (coll->get_ALPphysics()->is_kinmatic_anglar) {
//			Vector3 anglvec;
//			DirectX::XMStoreFloat3(&anglvec, coll->get_ALPphysics()->solve->delta_AngulaVelocity);
//			coll->get_ALPphysics()->anglar_velocity += anglvec;
//		}
//
//	}
//
//}
//
//#pragma endregion
////:::::::::::::::::::::::::::
//
//#endif