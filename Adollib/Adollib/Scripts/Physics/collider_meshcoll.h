#pragma once
#include "collider__base.h"
#include "meshcoll_resorce_manager.h"
#include "ALP__meshcoll_data.h"

namespace Adollib {

	//Mesh collider�N���X
	class Meshcoll : public Collider {
	public:
		Vector3 center;
		Vector3 rotate;
		Vector3 size;

	private:
		u_int vertex_num; //���_��
		u_int edge_num; //�G�b�W��
		u_int facet_num; //�ʐ�
		bool is_Convex = true; //convex�ȃI�u�W�F�N�g���ǂ��� ��ł�����edge�������false

		std::vector<Vector3>* vertices; //���_���
		std::vector<physics_function::Edge> edges; //�G�b�W�z��
		std::vector<physics_function::Facet> facets; //�ʔz��

		DOP::DOP_14	dopbase; //����k-dop��data

		Vector3 half_size = Vector3(1, 1, 1);
	public:
		//�s���I�u�W�F�N�g�Ƃ��Đ���
		Meshcoll() :center(Vector3(0)), rotate(Vector3(0)), size(0) {}

		void load_mesh(const char* filename) {
			////shape�̐ݒ�
			//shape = Collider_shape::shape_mesh;

			vertices = nullptr;
			edges.clear();
			facets.clear();

			std::vector<int>* inde; //���_���

			Collider_ResourceManager::CreateMCFromFBX(filename, &inde, &vertices);

			std::vector<Vector3>& vert = (*vertices);

			vertex_num = vert.size();

			//�ʏ��̕ۑ�
			{
				facet_num = vertex_num / 3;
				physics_function::Facet F;
				for (int i = 0; i < facet_num; i++) {
					F.vertexID[0] = (*inde)[i * 3];
					F.vertexID[1] = (*inde)[i * 3 + 1];
					F.vertexID[2] = (*inde)[i * 3 + 2];

					F.normal = vector3_cross(vert[F.vertexID[1]] - vert[F.vertexID[0]], vert[F.vertexID[2]] - vert[F.vertexID[0]]);
					F.normal = F.normal.unit_vect();

					facets.emplace_back(F);
				}
			}

			//�G�b�W���̕ۑ�
			{
				edge_num = 0;
				std::vector<int>edgeID_Table;
				edgeID_Table.resize(vertex_num * vertex_num / 2);
				for (int& u : edgeID_Table) {
					u = 0xffffff;
				}

				physics_function::Edge E;
				for (int i = 0; i < facet_num; i++) {
					physics_function::Facet& facet = facets[i];

					for (int o = 0; o < 3; o++) {
						u_int vertId0 = ALmin(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);
						u_int vertId1 = ALmax(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);
						int tableId = (int)((int)vertId1 * ((int)vertId1 - 1) / (float)2 + (int)vertId0);

						if (edgeID_Table[tableId] == 0xffffff) {
							// ���񎞂͓o�^�̂�
							E.facetID[0] = i;
							E.facetID[1] = i;
							E.vertexID[0] = vertId0;
							E.vertexID[1] = vertId1;
							E.type = physics_function::Edgetype::EdgeConvex; // �ʃG�b�W�ŏ�����
							edges.emplace_back(E);

							facet.edgeID[o] = edge_num;
							edgeID_Table[tableId] = edge_num;

							edge_num++;
						}
						else {
							// ���łɓo�^����Ă���
							physics_function::Edge& edge = edges[edgeID_Table[tableId]];
							physics_function::Facet& facetB = facets[edge.facetID[0]];

							assert(edge.facetID[0] == edge.facetID[1] && "Don't let the edges have 3�` facet.");

							// �G�b�W�Ɋ܂܂�Ȃ��`�ʂ̒��_��B�ʂ̕\�������Ŕ��f
							Vector3 s = vert[facet.vertexID[(o + 2) % 3]];
							Vector3 q = vert[facetB.vertexID[0]];
							float d = vector3_dot(s - q, facetB.normal);

							//�G�b�W�̎�ނ����
							if (d < -FLT_EPSILON) {
								edge.type = physics_function::Edgetype::EdgeConvex;
							}
							else if (d > FLT_EPSILON) {
								edge.type = physics_function::Edgetype::EdgeConcave;
								is_Convex = false;
							}
							else {
								edge.type = physics_function::Edgetype::EdgeFlat;
							}

							edge.facetID[1] = i;
						}

					}


				}

			}

			for (int i = 0; i < DOP::DOP_size; i++) {
				dopbase.max[i] = -FLT_MAX;
				dopbase.min[i] = +FLT_MAX;
			}

			//������Ԃ�DOP�̕ۑ� ��]�Ȃǂ��l��������DOP�͂���DOP����ɂ���
			for (int i = 0; i < DOP::DOP_size; i++) {
				for (Vector3& v : vert) {

					float dis = vector3_dot(DOP::DOP_14_axis[i], v);
					if (dopbase.max[i] < dis) dopbase.max[i] = +dis * 1.00000001f;//�m���ɂ��邽�߂�����Ƒ傫�߂ɂƂ�
					if (dopbase.min[i] > dis) dopbase.min[i] = +dis * 1.00000001f;
				}
			}

			size = Vector3(dopbase.max[0] - dopbase.min[0], dopbase.max[1] - dopbase.min[1], dopbase.max[2] - dopbase.min[2]) / 2.0f;
		}

		physics_function::Collider_data get_Colliderdata() const override {
			physics_function::Collider_data ret;

			ret.local_position = center;
			ret.local_orientation = quaternion_from_euler(rotate);
			ret.local_scale = size;

			ret.half_size = Vector3(1, 1, 1);

			ret.dopbase = dopbase;

			ret.shape = physics_function::ALP_Collider_shape::Mesh;
			return ret;
		};

		physics_function::Meshcoll_data get_Meshdata() override {
			physics_function::Meshcoll_data ret;
			ret.vertices = vertices;
			ret.edges = &edges;
			ret.facets = &facets;
			   
			ret.vertex_num = vertex_num;
			ret.edge_num = edge_num;
			ret.facet_num = facet_num;

			ret.is_Convex = is_Convex;
			return ret;
		}

	};
}