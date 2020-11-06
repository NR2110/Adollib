#pragma once
#include "../Math/math.h"

namespace Adollib {
	namespace physics_function {

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

		struct Meshcoll_data {
			u_int vertex_num = 0; //���_��
			u_int edge_num = 0; //�G�b�W��
			u_int facet_num = 0; //�ʐ�

			std::vector<Vector3>* vertices = nullptr; //���_���
			std::vector<Edge>* edges = nullptr; //�G�b�W�z��
			std::vector<Facet>* facets = nullptr; //�ʔz��

			bool is_Convex = true;
		};

	}
}