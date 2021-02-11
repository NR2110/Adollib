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

#include "../Imgui/imgui_all.h"

namespace Adollib {

	namespace Physics_function {
		//::: �v�Z�ȂǂɎg����� :::
		struct Collider_data {
			Vector3 local_position = Vector3(0,0,0);
			Quaternion local_orientation = quaternion_identity();
			Vector3 local_scale = Vector3(1, 1, 1);

			Vector3 half_size;

			u_int tag = 0;
			u_int nohit_tag = 0;

			Physics_function::ALP_Collider_shape shape = Physics_function::ALP_Collider_shape::None; //�`���

			Meshcoll_data meshcoll_data; //meshcollider�pdata

		};
		struct Physics_data {
			float inertial_mass = 0; //����
			float drag = 0; //��C��R
			float anglar_drag = 0; //��C��R
			float dynamic_friction = 0; //�����C
			float static_friction = 0; //�Ö��C
			float restitution = 0;	 //�����W��

			bool is_fallable = 0; //�����Ȃ�
			bool is_kinematic = 0;//�e�������Ȃ�(fall�͂���)
			bool is_kinmatic_anglar = 0; //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
			bool is_kinmatic_linear = 0; //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
			bool is_moveable = 0; //�����Ȃ�
			bool is_hitable = 0;  //�Փ˂��Ȃ�
		};
		//:::::::::::::::::::::::::

		struct ColliderPhysics_itrs {
			std::list<ALP_Collider>::iterator ALPcollider_itr;
			std::list<ALP_Physics>::iterator ALPphysics_itr;

			std::list<Collider*>::iterator coll_itr;

		};

	}

	class Collider : public Component {

	public:
		//::: tag�֌W ::::::::
		u_int tag = 0; //���g��tag(bit)
		u_int nohit_tag = 0; //�Փ˂��Ȃ�tags

		//::: unity��physics���� ������K�v�Ȃ�ĂȂ���� ::::
		Physics_function::Physics_data physics_data;

		//static���m�ŏՓ˔�����s��Ȃ�
		bool is_static = false;

	protected:
		std::list<Physics_function::ALP_Collider>::iterator ALPcollider_itr;
		std::list<Physics_function::ALP_Physics>::iterator ALPphysics_itr;

		std::list<Collider*>::iterator coll_itr;//���g�ւ�itr

	private:
		bool removed = false; //��d��remove������̂�h��

	public:
		//::: ��ŕύX���� :::
		const Vector3 linear_velocity() const { return ALPphysics_itr->linear_velocity; }
		const Vector3 linear_velocity(Vector3 v) {
			ALPphysics_itr->linear_velocity = v;
			return ALPphysics_itr->linear_velocity;
		}
		const Vector3 linear_anglar_velocity() const { return ALPphysics_itr->anglar_velocity; }
		const Vector3 linear_anglar_velocity(Vector3 v) {
			ALPphysics_itr->anglar_velocity = v;
			return ALPphysics_itr->anglar_velocity;
		}

	public:
		//on collision enter
		const bool concoll_enter(const unsigned int tag_name) const;

		//���i�ړ��ɗ͂�������
		void add_force(const Vector3& force);

		//�p��]�ɗ͂�������
		void add_torque(const Vector3& force);

		//�v�Z�p��struct
		virtual Physics_function::Collider_data get_Colliderdata() const = 0;
		virtual Physics_function::Meshcoll_data get_Meshdata() { return Physics_function::Meshcoll_data(); };

		const Physics_function::Physics_data get_Physicsdata() const {
			return physics_data;
		};

		//����collider��manager����폜
		void remove_collider();

	public:
		void awake() override;

		void finalize() override;

	};
}