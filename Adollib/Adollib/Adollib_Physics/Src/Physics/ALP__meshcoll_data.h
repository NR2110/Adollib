														#pragma once
#include "../Math/math.h"

namespace Adollib {
	namespace Physics_function {

		enum class Edgetype {
			EdgeConvex,	// �ʃG�b�W
			EdgeConcave,// ���G�b�W
			EdgeFlat,	// ���R�G�b�W
		};

		//�G�b�W
		struct Edge {
			Edgetype type = Edgetype::EdgeConvex; // �G�b�W�̎��
			u_int vertexID[2] = { 0 }; // �[�_�̒��_�C���f�b�N�X vector�̍Ċm�ۂ����邩���Ȃ̂�ID�ŕۑ�
			u_int facetID[2] = { 0 }; // ���L����ʃC���f�b�N�X
		};

		//��
		struct Facet {
			u_int vertexID[3] = { 0 }; // ���_�C���f�b�N�X
			u_int edgeID[3] = { 0 };   // �G�b�W�C���f�b�N�X
			Vector3 normal;  // �ʖ@���x�N�g��
		};

		constexpr int involved_vertex_max = 8;
		// vertex����edge�ւ̏��փA�N�Z�X�ł���悤��
		class Vertex_involvement {
		public:
			Vertex_involvement() {
				edge_saved_num = 0;
				facet_saved_num = 0;
			}

		private:
			int edge_saved_num = 0;
			int facet_saved_num = 0;

		public:
			struct edge_data {
				int edge_id = 0;
				bool is_edge_longer = false;
			};

			// �e�f�[�^��ID
			edge_data edge_involvements[involved_vertex_max] = { 0 };
			int facet_involvements[involved_vertex_max] = {0};

			void add_edge_involvment(int num, bool is_edge_longer) {
				if (edge_saved_num == involved_vertex_max)return;
				edge_involvements[edge_saved_num].edge_id = num;
				edge_involvements[edge_saved_num].is_edge_longer = is_edge_longer;
				++edge_saved_num;
			}

			void add_facet_involvment(int num) {
				if (facet_saved_num == involved_vertex_max)return;
				facet_involvements[facet_saved_num] = num;
				++facet_saved_num;
			}

			int get_edge_involvment_size() { return edge_saved_num; };
			int get_facet_involvment_size() { return facet_saved_num; };
		};

		struct Meshcollider_data{
			std::string FBX_pass;

			u_int index_num = 0;
			u_int vertex_num = 0; //���_��
			u_int edge_num = 0; //�G�b�W��
			u_int facet_num = 0; //�ʐ�

			std::vector<int> indexes; //���_���
			std::vector<Vector3> vertices; //���_���
			std::vector<Vertex_involvement> vertex_involvements; //���_�̊ւ���Ă�����̂ւ�index
			std::vector<Edge> edges; //�G�b�W�z��
			std::vector<Facet> facets; //�ʔz��

			bool is_Convex = true; //convex�ȃI�u�W�F�N�g���ǂ��� ��ł�����edge�������false
			DOP::DOP_14	dopbase; //����k-dop��data
			Vector3 base_pos[8];//����k-dop�̒��_�f�[�^
		};

		//struct Meshcoll_data {
		//	std::list<Meshcollider_data>::iterator collider_mesh;
		//};

	}
}