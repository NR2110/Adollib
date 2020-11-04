#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP_collider.h"
#include "ALP_physics.h"
#include "ALP_shape.h"

namespace Adollib {

	namespace physics_function {
		//::: �v�Z�Ɏg����� :::
		struct Collider_data {
			Vector3 local_position;
			Quaternion local_orientation;
			Vector3 local_scale;

			physics_function::ALP_Collider_shape shape;
		};

		struct ColliderPhysicsShape_itrs {
			std::list<Collider*>::iterator coll_itr;

			std::list<ALP_Collider>::iterator ALPcollider_itr;
			std::list<ALP_Physics>::iterator ALPphysics_itr;
			std::list<ALP_Shape>::iterator ALPshape_itr;
		};

	}

	class Collider : public Component {

	public:
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
		std::list<physics_function::ALP_Shape>::iterator ALPshape_itr;

		std::list<Collider*>::iterator coll_itr;//���g�ւ�itr
		
	public:
		//���g�ւ�itr��Ԃ�
		const physics_function::ColliderPhysicsShape_itrs const get_itrs() const {
			physics_function::ColliderPhysicsShape_itrs ret; 

			ret.ALPcollider_itr = ALPcollider_itr;
			ret.ALPphysics_itr = ALPphysics_itr;
			ret.ALPshape_itr = ALPshape_itr;

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

		//�v�Z�p��struct�ɂ܂Ƃ߂�
		virtual physics_function::Collider_data get_data() = 0;

	public:
		void awake() override;

		void finalize() override;

	};
}