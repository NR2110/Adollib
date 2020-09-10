#pragma once
#include "collider__base.h"
#include "meshcoll_resorce_manager.h"

namespace Adollib {

	enum class Edgetype {
		EdgeConvex,	// �ʃG�b�W
		EdgeConcave,// ���G�b�W
		EdgeFlat,	// ���R�G�b�W
	};

	//�G�b�W
	struct Edge {
		Edgetype type; // �G�b�W�̎��
		u_int vertexID[2]; // �[�_�̒��_�C���f�b�N�X vector�̍Ċm�ۂ����邩���Ȃ̂�ID�ŕۑ�
		u_int facetID[2]; // ���L����ʃC���f�b�N�X 
	};

	//��
	struct Facet {
		u_int vertexID[3]; // ���_�C���f�b�N�X
		u_int edgeID[3];   // �G�b�W�C���f�b�N�X
		vector3 normal;  // �ʖ@���x�N�g��
	};


	//Mesh collider�N���X
	class Meshcoll : public Collider {
	public:
		vector3 half_size = vector3();
		vector3 offset = vector3();
		DOP::DOP_14 dopbase;

		u_int vertex_num; //���_��
		u_int edge_num; //�G�b�W��
		u_int facet_num; //�ʐ�
		bool is_Convex = true;

		std::vector<vector3> vertices; //���_���
		std::vector<Edge> edges; //�G�b�W�z��
		std::vector<Facet> facets; //�ʔz��

		//�s���I�u�W�F�N�g�Ƃ��Đ���
		Meshcoll(const char* filename, float density, vector3 pos = vector3(0, 0, 0)) {
			//shape�̐ݒ�
			shape = Collider_shape::shape_mesh;

			std::vector<int> inde; //���_���
			vertices.clear();
			edges.clear();
			facets.clear();

			Collider_ResourceManager::CreateMCFromFBX(filename, &inde, &vertices);

			//�ʏ��̕ۑ�
			{
				facet_num = vertex_num / 3;
				Facet F;
				for (int i = 0; i < facet_num; i++) {
					F.vertexID[0] = inde[i * 3];
					F.vertexID[1] = inde[i * 3 + 1];
					F.vertexID[2] = inde[i * 3 + 2];

					F.normal = vector3_cross(vertices[F.vertexID[1]] - vertices[F.vertexID[0]], vertices[F.vertexID[2]] - vertices[F.vertexID[0]]);
					F.normal = F.normal.unit_vect();

					facets.emplace_back(F);
				}
			}

			//�G�b�W���̕ۑ�
			{
				edge_num = 0;
				std::vector<u_int>edgeID_Table;
				edgeID_Table.resize(vertex_num * vertex_num / 2);
				for (u_int& u : edgeID_Table) {
					u = 0xffffff;
				}

				Edge E;
				for (int i = 0; i < facet_num; i++) {
					Facet& facet = facets[i];

					for (int o = 0; o < 3; o++) {
						u_int vertId0 = ALmax(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);
						u_int vertId1 = ALmin(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);
						u_int tableId = vertId1 * (vertId1 - 1) / 2 + vertId0;

						if (edgeID_Table[tableId] == 0xffffff) {
							// ���񎞂͓o�^�̂�
							E.facetID[0] = i;
							E.facetID[1] = i;
							E.vertexID[0] = vertId0;
							E.vertexID[1] = vertId1;
							E.type = Edgetype::EdgeConvex; // �ʃG�b�W�ŏ�����
							edges.emplace_back(E);

							facet.edgeID[o] = edge_num;
							edgeID_Table[tableId] = edge_num;

							edge_num++;
						}
						else {
							// ���łɓo�^����Ă���
							Edge& edge = edges[edgeID_Table[tableId]];
							Facet& facetB = facets[edge.facetID[0]];

							assert(edge.facetID[0] == edge.facetID[1] && "Don't let the edges have 3�` facet.");

							// �G�b�W�Ɋ܂܂�Ȃ��`�ʂ̒��_��B�ʂ̕\�������Ŕ��f
							vector3 s = vertices[facet.vertexID[(o + 2) % 3]];
							vector3 q = vertices[facetB.vertexID[0]];
							float d = vector3_dot(s - q, facetB.normal);

							//�G�b�W�̎�ނ����
							if (d < -FLT_EPSILON) {
								edge.type = Edgetype::EdgeConvex;
							}
							else if (d > FLT_EPSILON) {
								edge.type = Edgetype::EdgeConcave;
								is_Convex = false;
							}
							else {
								edge.type = Edgetype::EdgeFlat;
							}

							edge.facetID[1] = i;
						}

					}


				}

			}

			memset(dopbase.max, -FLT_MAX, DOP::DOP_size);
			memset(dopbase.min,  FLT_MAX, DOP::DOP_size);

			//������Ԃ�DOP�̕ۑ� ��]�Ȃǂ��l��������DOP�͂���DOP����ɂ���
			for (int i = 0; i < DOP::DOP_size; i++) {
				for (vector3& v : vertices) {

					float dis = fabsf(vector3_dot(DOP::DOP_14_axis[i], v));
					if (dopbase.max[i] < dis) dop14.max[i] = +dis * 1.00000001f;//�m���ɂ��邽�߂�����Ƒ傫�߂ɂƂ�
					if (dopbase.min[i] > dis) dop14.min[i] = +dis * 1.00000001f;
				}
			}

			half_size = vector3(dop14.max[0] - dop14.min[0], dop14.max[1] - dop14.min[1], dop14.max[2] - dop14.min[2]);

			//���x�̕ۑ�
			this->density = density;

			//���W
			local_position = pos;

			//���ʂ̌v�Z
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}

		//�T�C�Y�̏����֐��̃I�[�o�[���C�h
		quaternion get_dimension() const {
			return half_size;
		}
		//world�ϊ��֐��̃I�[�o�[���C�h
		void update_world_trans();

		//size�▧�x���ύX�����Ǝ��ʂ⊮�����[�����g�̕ύX���K�v�ɂȂ邩�炻�̂��߂ɗp��
		void update_inertial(const vector3& half_size, float density = 1) {
			this->density = density;

			//���ʂ̌v�Z
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}

		//dop6�̍X�V
		void update_dop14();

	};
}