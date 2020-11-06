#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP__meshcoll_data.h"
#include "ALP_collider.h"
#include "ALP_physics.h"

namespace Adollib {

	namespace physics_function {
		//::: �v�Z�ȂǂɎg����� :::
		struct Collider_data {
			Vector3 local_position;
			Quaternion local_orientation;
			Vector3 local_scale;

			Vector3 half_size;

			physics_function::ALP_Collider_shape shape; //�`���

			DOP::DOP_14	dopbase; //MeshCollider�̍ŏ���k-dop
		};
		struct Physics_data {
			float inertial_mass; //����
			float drag; //��C��R
			float anglar_drag; //��C��R
			float dynamic_friction;//�����C
			float static_friction; //�Ö��C
			float restitution;	 //�����W��

			bool is_fallable; //�����Ȃ�
			bool is_kinematic; //�e�������Ȃ�(fall�͂���)
			bool is_moveable;//�����Ȃ�
			bool is_hitable;  //�Փ˂��Ȃ�
		};
		//:::::::::::::::::::::::::

		struct ColliderPhysics_itrs {
			std::list<Collider*>::iterator coll_itr;

			std::list<ALP_Collider>::iterator ALPcollider_itr;
			std::list<ALP_Physics>::iterator ALPphysics_itr;
		};

	}

	class Collider : public Component {

	public:
		//::: tag�֌W ::::::::
		u_int tag; //���g��tag(bit)
		u_int not_hitable_tags; //�Փ˂��Ȃ�tags

		//::: unity��physics���� ������K�v�Ȃ�ĂȂ���� ::::
		float inertial_mass; //����
		float drag; //��C��R
		float anglar_drag; //��C��R
		float dynamic_friction; //�����C
		float static_friction; //�Ö��C
		float restitution;	 //�����W��

		bool is_fallable; //�����Ȃ�
		bool is_kinematic;//�e�������Ȃ�(fall�͂���)
		bool is_moveable; //�����Ȃ�
		bool is_hitable;  //�Փ˂��Ȃ�

	private:
		std::list<physics_function::ALP_Collider>::iterator ALPcollider_itr;
		std::list<physics_function::ALP_Physics>::iterator ALPphysics_itr;

		std::list<Collider*>::iterator coll_itr;//���g�ւ�itr
		
	public:
		//���g�ւ�itr��Ԃ�
		const physics_function::ColliderPhysics_itrs const get_itrs() const {
			physics_function::ColliderPhysics_itrs ret; 

			ret.ALPcollider_itr = ALPcollider_itr;
			ret.ALPphysics_itr = ALPphysics_itr;

			ret.coll_itr = coll_itr;

			return ret;
		};

	public:
		//on collision enter
		const bool concoll_enter(const unsigned int tag_name) const;

		//���i�ړ��ɗ͂�������
		void add_force(const Vector3& force);

		//�p��]�ɗ͂�������
		void add_torque(const Vector3& force);

		//�v�Z�p��struct
		virtual physics_function::Collider_data get_Colliderdata() const = 0;
		virtual physics_function::Meshcoll_data get_Meshdata() { return physics_function::Meshcoll_data(); };
		physics_function::Physics_data  get_Physicsdata() {
			physics_function::Physics_data data;
			data.inertial_mass = inertial_mass;
			data.drag = drag;
			data.anglar_drag = anglar_drag;
			data.dynamic_friction = dynamic_friction;
			data.static_friction = static_friction;
			data.restitution = restitution;

			data.is_fallable = is_fallable;
			data.is_kinematic = is_kinematic;
			data.is_moveable = is_moveable;
			data.is_hitable = is_hitable;
			return data;
		};

	public:
		void awake() override;

		void finalize() override;

	};
}