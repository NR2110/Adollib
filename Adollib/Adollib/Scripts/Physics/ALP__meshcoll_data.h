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
			Edgetype type; // �G�b�W�̎��
			u_int vertexID[2]; // �[�_�̒��_�C���f�b�N�X vector�̍Ċm�ۂ����邩���Ȃ̂�ID�ŕۑ�
			u_int facetID[2]; // ���L����ʃC���f�b�N�X 
		};

		//��
		struct Facet {
			u_int vertexID[3]; // ���_�C���f�b�N�X
			u_int edgeID[3];   // �G�b�W�C���f�b�N�X
			Vector3 normal;  // �ʖ@���x�N�g��
		};

		struct Meshcoll_data {
			u_int vertex_num; //���_��
			u_int edge_num; //�G�b�W��
			u_int facet_num; //�ʐ�

			std::vector<Vector3>* vertices; //���_���
			std::vector<Edge>* edges; //�G�b�W�z��
			std::vector<Facet>* facets; //�ʔz��

			bool is_Convex = true;
		};

	}
}