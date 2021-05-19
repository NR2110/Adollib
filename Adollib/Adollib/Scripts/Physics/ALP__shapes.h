#pragma once

#include "../Math/math.h"


namespace Adollib {
	namespace Physics_function {

		//::: 計算などに使う情報 :::
		//struct Collider_data {
		//	Vector3 local_position = Vector3(0, 0, 0);
		//	Quaternion local_orientation = quaternion_identity();
		//	Vector3 local_scale = Vector3(1, 1, 1);

		//	Vector3 half_size;

		//	u_int tag = 0;
		//	u_int ignore_tags = 0;

		//	Physics_function::ALP_Collider_shape_tag shape = Physics_function::ALP_Collider_shape_tag::None; //形情報

		//};

		enum class ALP_Collider_shape_tag {
			None,
			BOX,
			Sphere,
			Plane,
			Capsule,
			Mesh,
			Cylinder,

			shapes_size
		};

		class Meshcollider_data;
		class ALP_Collider;
	}
	class ALP_shape {
	public:

	public:
		Vector3 local_position;
		Quaternion local_orientation;
		Vector3 local_scale;

	private:
		Vector3	world_position_;
		Quaternion	world_orientation_;
		Vector3	world_scale_;

		Vector3 old_world_position_;
		Quaternion old_world_orientation_;
		Vector3 old_world_scale_;

	public:
		const Vector3	world_position()const { return world_position_; };
		const Quaternion	world_orientation()const { return world_orientation_; };
		const Vector3	world_scale()const { return world_scale_; };

		const Vector3	old_world_position()const { return old_world_position_; };
		const Quaternion	old_world_orientation()const { return old_world_orientation_; };
		const Vector3	old_world_scale()const { return old_world_scale_; };

	protected:
		//このshapeがどのような形なのか
		Physics_function::ALP_Collider_shape_tag shape_tag;

		//14-DOP
		DOP::DOP_14	dop14;	//14DOP データ

		//meshcollider用 vertexes,Edges,Facetsなどの情報 resorce_managerの中にある情報へのポインタ
		Physics_function::Meshcollider_data* mesh_data;

		//このshapeがアタッチされたcolliderへのイテレータ
		Physics_function::ALP_Collider* ALPcollider_ptr;

	public:

		const Physics_function::ALP_Collider_shape_tag get_shape_tag() const { return shape_tag; };

		const DOP::DOP_14 get_DOP() const { return dop14; };

		const Physics_function::Meshcollider_data* get_mesh_data() const { return mesh_data; };

		//TODO : ALPCollider::oncoll_tagをgenerate_contactで変更する必要があるためconstを外している 用修正
		Physics_function::ALP_Collider* get_ALPcollider() const { return ALPcollider_ptr; };

	public:
		u_int tag = 0; //自身のtag(bit)
		u_int ignore_tags = 0; //衝突しないtags(bit)



	public:

		//world情報の更新
		void update_world_trans(const Vector3& GO_Wposiiton, const Quaternion& GO_Worientation, const Vector3& GO_Wscale);

	public:
		//各shapeのユーザー用の情報(box:center,size)から計算用の情報(position,scale)に治す
		virtual void adapt_Colliderdata() = 0;

		//ヒエラルキーへの表示
		virtual void Update_hierarchy() = 0;

		//DOPの更新
		virtual void update_dop14() = 0;

	};



}