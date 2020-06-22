#pragma once
#include "contact.h"

namespace Adollib {
	namespace physics_function {
		struct Solverbody {
			vector3 delta_LinearVelocity; // ���i���x����
			vector3 delta_AngularVelocity; // ��]���x����
			quaternion    orientation; // �p��
			matrix inv_inertia; // �����e���\���̋t�s��
			float   inv_mass; // ���ʂ̋t��
		};

		//�Փ˂̊m�F
		//bool check_contact_sphere_sphere(Sphere& S1, Sphere& S2);
		//bool check_contact_sphere_plane(Sphere& S1, Plane& S2);
		//bool check_contact_sphere_box(Sphere& S1, Box& S2);
		//bool check_contact_box_plane(Box& S1, Plane& S2);
		//bool check_contact_box_box(Box& S1, Box& S2);

		//�O�͂ɂ�鑬�x�Ȃǂ̍X�V
		void applyexternalforce(std::vector<Collider*>& coll);

		//��G�c�ȓ����蔻��(Boardphase)
		void Boardphase(const std::vector<Collider*>& coll, std::vector<Contacts::Contact_pair>& pairs);

		//�Փː���(Narrowphase)
		void generate_contact(std::vector<Contacts::Contact_pair*>& pairs);
		//�Փː���
		bool generate_contact_sphere_sphere(const Sphere& S1, const Sphere& S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_plane(const Sphere& S1, const Plane& S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_box(const Sphere& S1, const  Box& S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_plane(const Box& S1, const Plane& S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_box(const Box& S1, const  Box& S2, Contacts::Contact_pair& pair);

		//�Փˉ���
		void resolve_contact(std::vector<Collider*> colliders, std::vector<Contacts::Contact_pair>& pairs);

		void integrate(std::vector<Collider*>& coll);

		
	}
}