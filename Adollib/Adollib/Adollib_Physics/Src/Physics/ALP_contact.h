#pragma once
#include <vector>
#include <list>
#include <assert.h>
#include "../Math/quaternion.h"
#include "../Math/matrix.h"
#include "ALP_collider.h"

namespace Adollib {
	namespace Physics_function {
		//Contact�N���X�p��namespace
		namespace Contacts {
			static const int contact_max_per_pair = 4; //���pair�ŏՓ˂̉��U���s���ő�̐�

			//�S��
			struct Constraint {
				Vector3 axis;			//�S����
				float jacDiagInv = 0;	//�S�����̕���
				float rhs = 0;			//�����S����
				float tangent_rhs = 0;  //�S�������@���̖ʏ�ł̍S����
				float lowerlimit = 0;	//�S���͂̉���
				float upperlimit = 0;	//�S���͂̏��
				float accuminpulse = 0; //�ݐς����S����
			};


			//�Փ˓_���
			struct Contactpoint {
				float distance = 0;		//�Փː[�x
				Vector3 point[2];		//�Փ˓_
				Vector3 normal;			//�Փ˓_�̖@���x�N�g��
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
				float friction = 0;		//���̏Փ˂̖��C��
				Contactpoint contactpoints[contact_max_per_pair]; //�Փ˓_���

				Contact() {
				}

				//::::::::
				// �Փ˓_�̒ǉ�
				// float penetration : �ђʗ�
				// vector3 normal : point[0]����point[1]�Փ˖@�� (���[���h���W�n)
				// vector3 contactpointA : �Փ˓_ (point[0]�̃��[�J�����W�n)
				// vector3 contactpointB : �Փ˓_ (point[1]�̃��[�J�����W�n)
				//::::::::
				void addcontact(
					float penetration,
					const Vector3& normal,
					const Vector3& contact_pointA,
					const Vector3& contact_pointB
				);

				//:::::::
				// �Փ˓_�̏Փ˂��������݂��邩�̊m�F
				// vector3 pointA : Rigitbody[0]�̈ʒu
				// vector3 rotA : Rigitbody[0]�̎p��
				// vector3 pointB : Rigitbody[1]�̈ʒu
				// vector3 rotB : Rigitbody[1]�̎p��
				// ���݂��Ȃ��Փ˓_�������true��Ԃ�(sleep�����̂��߂�)
				//:::::::
				bool chack_remove_contact_point(
					const Collider_shape* shape0,
					const Collider_shape* shape1
				);

				//������
				void reset() {
					contact_num = 0;
					for (int i = 0; i < contact_max_per_pair; i++) {
						contactpoints[i].reset();
					}
				};

			private:
				//:::::::::
				// addcontact�Ŏg�p
				// ��������ContactPoint��������
				// vector3 newPoint : �ǉ�����CP�̐V�����Փ˓_
				// float newDistance : �ǉ�����CP�̐V�����ђʗ�
				//:::::::::
				int find_exclusion_point(const Vector3& newPoint, float newDistance);

				//::::::::::
				// find_exclusion_point�Ŏg�p
				// 4�_�ō����4�p�`�̖ʐ�^2��Ԃ�
				//::::::::::
				float area_4points(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& p4);

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
					const Vector3& contact_pointA,
					const Vector3& contact_pointB,
					const Vector3& normal
				);
			};

			enum class Pairtype {
				new_pair, // �V�K
				keep_pair, // �ێ�
			};

			struct Contact_pair {

				Pairtype type = Pairtype::new_pair; //�Փ˂̎��(�O�t���[�����炠��Փ˂��ǂ���)

				union {
					struct {
						Collider_shape* body[2]; //�ڐG����object
					};
					double key; //�|�C���^2����Ȃ郆�j�[�N�Ȑ���
				};

				Contact contacts; //�Փ˂̏��

				bool check_oncoll_only = false;
			};


			// �\�[�g
			// param[in,out] d �\�[�g����f�[�^�̔z��
			// param buff �\�[�g�p�̃o�b�t�@�i���̓f�[�^�Ɠ��T�C�Y�j
			// param n �f�[�^�̐�

			static int partition(std::vector<Contacts::Contact_pair*>& array, int l, int r) {
				auto& pivot = array[r];
				int i = (l - 1);

				for (int j = l; j <= r - 1; j++) {
					if (array[j]->key <= pivot->key) {
						i++;

						const auto t = array[i];
						array[i] = array[j];
						array[j] = t;
					}
				}

				{
					const auto t = array[i + 1];
					array[i + 1] = array[r];
					array[r] = t;
				}
				return (i + 1);
			}

			static void Contact_pair_quick_sort(std::vector<Contacts::Contact_pair*>& array, int l, int r) {
				if (l < r) {
					int pivot = partition(array, l, r);
					Contact_pair_quick_sort(array, l, pivot - 1);
					Contact_pair_quick_sort(array, pivot + 1, r);
				}
			}




		}
	}
}