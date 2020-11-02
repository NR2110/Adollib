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

	namespace  physics_function {
		class Solverbody;
		class ALP__CollPhysicsShape_data;
	}


	class Collider : public Component {
	public:
		Vector3 local_position;
		Vector3 local_orientation;
		Vector3 local_scale;

		float mass; //����
		float drag; //��C��R
		float anglar_drag; //��C��R
		float dynamic_friction; //�����C
		float static_friction; //�Ö��C
		float restitution;	 //�����W��

		bool is_fallable; //�����Ȃ�
		bool is_kinematic; //�e�������Ȃ�(fall�͂���)
		bool is_moveable; //�����Ȃ�

	private:


	private:
		std::list<physics_function::ALP_Collider>::iterator ALPcollider_itr;
		std::list<physics_function::ALP_Physics>::iterator ALPphysics_itr;
		std::list<physics_function::ALP_Shape>::iterator ALPshape_itr;

		
	public:
		//on collision enter
		bool concoll_enter(std::string tag_name);

		//���i�ړ��ɗ͂�������
		void add_force(const Vector3& force);

		//�p��]�ɗ͂�������
		void add_torque(const Vector3& force);


	public:
		void awake() override;

		void finalize() override;

	};
}