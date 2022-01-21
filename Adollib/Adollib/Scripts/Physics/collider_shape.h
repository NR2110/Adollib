#pragma once

#include "../Math/math.h"
#include "collider_types.h"

namespace Adollib {
	namespace Physics_function {

		struct Meshcollider_data;
		class ALP_Collider;
	}

	class Collider_shape {
	public:
		Vector3 local_position;
		Quaternion local_orientation;
		Vector3 local_scale;

	protected:
		Vector3	world_position_;
		Quaternion	world_orientation_;
		Vector3	world_scale_;

		Vector3 old_world_position_;
		Quaternion old_world_orientation_;
		Vector3 old_world_scale_;

	public:
		// bool is_update_world_trans = false;

	public:
		const Vector3	world_position()const { return world_position_; };
		const Quaternion	world_orientation()const { return world_orientation_; };
		const Vector3	world_scale()const { return world_scale_; };

		const Vector3	old_world_position()const { return old_world_position_; };
		const Quaternion	old_world_orientation()const { return old_world_orientation_; };
		const Vector3	old_world_scale()const { return old_world_scale_; };

	public:

		// 質量を考慮しない慣性モーメント  barycenter : 回転の中心
		Matrix33 get_tensor(const Vector3& barycenter = Vector3(0)) {
			Matrix33 tensor;
			//shapeの中心を回転中心とした慣性モーメント
			tensor = local_tensor();

			tensor = local_orientation.get_rotate_matrix() * tensor * matrix_inverse(local_orientation.get_rotate_matrix());

			// barycenterを回転中心として shapeのlocal座標の分を考慮 (平行軸の定理)
			Vector3 dis = local_position - barycenter;
			tensor._11 += (dis.y * dis.y + dis.z * dis.z);
			tensor._22 += (dis.z * dis.z + dis.x * dis.x);
			tensor._33 += (dis.x * dis.x + dis.y * dis.y);

			return tensor;
		};

		// shapeの体積
		virtual const float get_volume() const = 0;

	protected:
		// このshapeがどのような形なのか
		Physics_function::ALPCollider_shape_type shape_tag = Physics_function::ALPCollider_shape_type::None;

		// DOP
		DOP::DOP_14	dop14;	//DOP データ

		// meshcollider用 vertexes,Edges,Facetsなどの情報 resorce_managerの中にある情報へのポインタ
		Physics_function::Meshcollider_data* mesh_data = nullptr;

		// このshapeがアタッチされたcolliderへのイテレータ
		Physics_function::ALP_Collider* ALPcollider_ptr = nullptr;

	public:
		const Physics_function::ALPCollider_shape_type get_shape_tag() const { return shape_tag; };

		const DOP::DOP_14 get_DOP() const { return dop14; };

		const Physics_function::Meshcollider_data* get_mesh_data() const { return mesh_data; };

		// TODO : ALPCollider::oncoll_tagをgenerate_contactで変更する必要があるためconstを外している 用修正
		Physics_function::ALP_Collider* get_ALPcollider() { return ALPcollider_ptr; };

		const u_int get_tag() const; //このspaheがアタッチされた collider : public component のtagを返す

		const u_int get_ignore_tags() const;//このspaheがアタッチされた collider : public component のignore_tagsを返す

	public:

		// world情報の更新
		void update_world_trans(const Vector3& GO_Wposiiton, const Quaternion& GO_Worientation, const Vector3& GO_Wscale);

		// gameobjectのtransformをALPcolliderにコピーする際にshapeが与える影響(基本的にはない)
		virtual void effect_for_copy_transform_to_collider(Vector3& GO_Wposiiton, Quaternion& GO_Worientation, Vector3& GO_Wscale) {};

		// ALPcolliderのtransformをgameobjectにコピーする際にshapeが与える影響(基本的にはない)
		virtual void effect_for_copy_transform_to_gameobject(const Vector3& position_amount_of_change, const Quaternion& orientation_amount_of_change, const Vector3& position_amount_of_change_local, const Quaternion& orientation_amount_of_change_local) {};

	protected:
		// 各shapeのユーザー用の情報(box:center,size)から計算用の情報(position,scale)に治す
		virtual void adapt_Colliderdata() = 0;

		// shapeの中心を回転中心とした慣性モーメント
		virtual const Matrix33 local_tensor() const = 0;

	public:
		// 派生クラスの情報を基底クラスのdataに直す
		void update_Colliderdata() {
			adapt_Colliderdata();
		};

		// ヒエラルキーへの表示
		virtual void Update_hierarchy(int hierarchy_num = 0) = 0;

		// DOPの更新
		virtual void update_dop14() = 0;


	public:
		virtual ~Collider_shape() {}; //派生クラスの持つshaerd_ptrのlefカウントを減らすため
	};



}