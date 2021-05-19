//#pragma once
//
//#include <vector>
//#include <assert.h>
//#include <string>
//#include <unordered_map>
//#include "../Math/math.h"
//#include "../Object/component.h"
//
//#include "ALP__meshcoll_data.h"
//#include "ALP_collider.h"
//#include "ALP_physics.h"
//
//#include "../Imgui/imgui_all.h"
//
//namespace Adollib {
//
//	namespace Physics_function {
//
//		//�\���p��physics_data ���[�U�[���ȒP�ɕύX�ł���悤��
//		struct Physics_data {
//			float inertial_mass = 0; //����
//			float drag = 0; //��C��R
//			float anglar_drag = 0; //��C��R
//			float dynamic_friction = 0; //�����C
//			float static_friction = 0; //�Ö��C
//			float restitution = 0;	 //�����W��
//
//			bool is_fallable = 0; //�����Ȃ�
//			bool is_kinematic = 0;//�e�������Ȃ�(fall�͂���)
//			bool is_kinmatic_anglar = 0; //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
//			bool is_kinmatic_linear = 0; //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
//			bool is_moveable = 0; //�����Ȃ�
//			bool is_hitable = 0;  //�Փ˂��Ȃ�
//		};
//		//:::::::::::::::::::::::::
//
//		//physics_manager��add_collider�����Ƃ��̕Ԃ�l
//		struct ColliderPhysics_itrs {
//			std::list<ALP_Collider>::iterator ALPcollider_itr;
//			std::list<ALP_Physics>::iterator ALPphysics_itr;
//
//			std::list<Collider*>::iterator coll_itr;
//
//		};
//
//	}
//
//	class Collider : public Component {
//
//	public:
//
//		//::: tag�̏����l ::::::::
//		u_int tag = 0; //���g��tag(bit)
//		u_int ignore_tags = 0; //�Փ˂��Ȃ�tags
//
//		//::: unity��physics���� ������K�v�Ȃ�ĂȂ���� ::::
//		Physics_function::Physics_data physics_data;
//
//		//static���m�ŏՓ˔�����s��Ȃ�
//		bool is_static = false;
//
//	protected:
//		//physics_manager�����Ԃ����Bremove���������邽��list�̔z�� ����������itr��ۑ��B�������̂ŗv�C��
//		//���Ԃ�collider�����ׂ�? ->manager���|�C���^�z�������? ������?
//		std::list<Physics_function::ALP_Collider>::iterator ALPcollider_itr;
//		std::list<Physics_function::ALP_Physics>::iterator ALPphysics_itr;
//
//		std::list<Collider*>::iterator coll_itr;//���g�ւ�itr
//
//	private:
//		bool removed = false; //��d��remove������̂�h��
//
//		//����collider��manager����폜
//		void remove_collider();
//
//	public:
//		//::: ��ŕύX���� :::
//		const Vector3 linear_velocity() const { return ALPphysics_itr->linear_velocity; }
//		const Vector3 linear_velocity(Vector3 v) {
//			ALPphysics_itr->linear_velocity = v;
//			return ALPphysics_itr->linear_velocity;
//		}
//		const Vector3 linear_anglar_velocity() const { return ALPphysics_itr->anglar_velocity; }
//		const Vector3 linear_anglar_velocity(Vector3 v) {
//			ALPphysics_itr->anglar_velocity = v;
//			return ALPphysics_itr->anglar_velocity;
//		}
//
//	public:
//		//on collision enter
//		const bool concoll_enter(const unsigned int tag_name) const;
//
//		//���i�ړ��ɗ͂�������
//		void add_force(const Vector3& force);
//
//		//�p��]�ɗ͂�������
//		void add_torque(const Vector3& force);
//
//
//	public:
//		void Update_hierarchy();
//
//		void awake() override;
//		void finalize() override;
//
//	};
//}