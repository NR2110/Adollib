#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

namespace Adollib {

	namespace  physics_function {
		class Solverbody;
	}
	namespace Contacts {
		class Contact;
	}


	class Collider : public Component {
	public:
		Vector3 local_position;
		Vector3 local_orientation;
		Vector3 local_scale;

		float mass; //質量
		float drag; //空気抵抗
		float anglar_drag; //空気抵抗
		float dynamic_friction; //動摩擦
		float static_friction; //静摩擦
		float restitution;	 //反発係数

		bool is_fallable; //落ちない
		bool is_kinematic; //影響うけない(fallはする)
		bool is_moveable; //動かない


	private:
		//itr
		
	public:
		bool concoll_enter(std::string tag_name);

		////外力の更新
		//void apply_external_force(float duration = 1);

		////速度、加速度を0にする
		//void reset_force();

		////座標,姿勢の更新
		//void integrate(float duration = 1);

		////gameobjectへの変化量を求める
		//void solv_resolve();

		////gameobjectへ変化量を渡す
		//void resolve_gameobject();

		////gameobjectのtransformからcolliderのworld空間での情報を更新
		//virtual void update_world_trans() = 0;

		////並進移動に力を加える
		//void add_force(const Vector3& force);

		////角回転に力を加える
		//void add_torque(const Vector3& force);

		////可動オブジェクトかどうか
		//bool is_movable() const;

		////落下オブジェクトかどうか
		//bool is_fallable() const;

		////質量の逆数を返す(不稼働オブジェクトは0を返す)
		//float inverse_mass() const;

		////慣性モーメントの逆行列を返す
		//matrix inverse_inertial_tensor() const;

		////サイズ所得関数
		//virtual Quaternion get_dimension() const = 0;

		////サイズ変更などに対応するため毎フレーム慣性テンソルなどを更新
		//virtual void update_inertial(const Vector3& size, float density = 1) = 0;

		////オブジェクトが動くたびに呼び出す　のが効率よいが適当に毎フレーム呼ぶ
		////DOP_14データの更新
		//virtual void update_dop14() = 0;

		void awake() override;

		void finalize() override;

	};
}