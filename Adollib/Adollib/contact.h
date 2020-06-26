#pragma once
#include <vector>
#include <assert.h>
#include "quaternion.h"
#include "matrix.h"
#include "rigit_body.h"
#include "physics_global.h"

namespace Adollib {
	//Contact�N���X�p��namespace ���̐l�����܂�G��Ȃ��悤��
	namespace Contacts {

		//�S��
		struct Constraint {
			vector3 axis;		//�S����
			float jacDiagInv;	//�S�����̕���	
			float rhs;			//�����S����
			float tangent_rhs;  //�S�������@���̖ʏ�ł̍S����
			float lowerlimit;	//�S���͂̉���
			float upperlimit;	//�S���͂̏��
			float accuminpulse; //�ݐς����S����
		};


		//�Փ˓_���
		struct Contactpoint {
			float distance;			//�Փː[�x
			vector3 point[2];		//�Փ˓_
			vector3 normal;			//�Փ˓_�̖@���x�N�g��
			Constraint constraint[3];  //�S��

			//������
			void reset() {
				constraint[0].accuminpulse = 0.0f;
				constraint[1].accuminpulse = 0.0f;
				constraint[2].accuminpulse = 0.0f;
			}
		};

		//�Փˏ����p
		class Contact {
		public:
			int contact_num = 0;    //�Փ˓_�̐�
			float friction;		//���̏Փ˂̖��C��
			Contactpoint contactpoints[Contact_max_per_pair]; //�Փ˓_���

			Contact() {
			}

			//::::::::
			// �Փ˓_�̒ǉ�
			// float penetration : �ђʗ�
			// vector3 normal : �Փ˖@�� (���[���h���W�n)
			// vector3 contactpointA : �Փ˓_ (point[0]�̃��[�J�����W�n)
			// vector3 contactpointB : �Փ˓_ (point[1]�̃��[�J�����W�n)
			//::::::::
			void addcontact(
				float penetration,
				const vector3& normal,
				const vector3& contact_pointA,
				const vector3& contact_pointB
			);

			//:::::::
			// �Փ˓_�̏Փ˂��������݂��邩�̊m�F
			// vector3 pointA : Rigitbody[0]�̈ʒu
			// vector3 rotA : Rigitbody[0]�̎p��	
			// vector3 pointB : Rigitbody[1]�̈ʒu	
			// vector3 rotB : Rigitbody[1]�̎p��	
			//:::::::
			void chack_remove_contact_point(
				const vector3& pointA,
				const quaternion& rotA,
				const vector3& pointB,
				const quaternion& rotB
			);
		private:
			//:::::::::
			// addcontact�Ŏg�p
			// ��������ContactPoint��������
			// vector3 newPoint : �ǉ�����CP�̐V�����Փ˓_
			// float newDistance : �ǉ�����CP�̐V�����ђʗ�
			//:::::::::
			int find_exclusion_point(const vector3& newPoint, float newDistance);

			//::::::::::
			// find_exclusion_point�Ŏg�p
			// 4�_�ō����4�p�`�̖ʐ�^2��Ԃ�
			//::::::::::
			float area_4points(const vector3& p1, const vector3& p2, const vector3& p3, const vector3& p4);

			//::::::::
			// i �Ԗڂ̏Փ˓_��j�� ���̌�l�߂�
			//::::::::
			void remove_contactpoint(int i);

			//::::::::
			// ���łɂ��̏Փ˓_������Ă����ꍇcontactpoint�̔ԍ���Ԃ�
			// ����Ă��Ȃ����-1��Ԃ�
			// vector3 contactpointA : �Փ˓_ (point[0]�̃��[�J�����W�n)
			// vector3 contactpointB : �Փ˓_ (point[1]�̃��[�J�����W�n)
			// vector3 normal : �Փ˖@�� (���[���h���W�n)
			//::::::::
			int find_contact_point(
				const vector3& contact_pointA,
				const vector3& contact_pointB,
				const vector3& normal
			);

			//������
			void reset() {
				contact_num = 0;
				for (int i = 0; i < Contact_max_per_pair; i++) {
					contactpoints[i].reset();
				}
			};

			void resolve();
		};

		enum  class Pairtype {
			new_pair, // �V�K
			keep_pair, // �ێ�
		};

		struct Contact_pair {
	
			Pairtype type; //�Փ˂̎��(�O�t���[�����炠��Փ˂��ǂ���)

			Collider* body[2]; //�ڐG����object
			

			Contact contacts; //�Փ˂̏��
		};

	}
}