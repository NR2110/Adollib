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

		struct Meshcollider_mesh{
			u_int index_num = 0;
			u_int vertex_num = 0; //���_��
			u_int edge_num = 0; //�G�b�W��
			u_int facet_num = 0; //�ʐ�

			std::vector<int> indexes; //���_���
			std::vector<Vector3>* vertices; //���_���
			std::vector<Edge> edges; //�G�b�W�z��
			std::vector<Facet> facets; //�ʔz��

			bool is_Convex = true; //convex�ȃI�u�W�F�N�g���ǂ��� ��ł�����edge�������false
			DOP::DOP_14	dopbase; //����k-dop��data
			Vector3 base_pos[24];//����k-dop�̒��_�f�[�^
			Vector3 half_size = Vector3(1, 1, 1);
		};

		struct Meshcoll_data {
			std::list<Meshcollider_mesh>::iterator collider_mesh;
		};

	}
}