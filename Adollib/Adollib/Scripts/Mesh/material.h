#pragma once

#include <vector>
#include <string>
#include "../Math/vector.h"
#include "../Shader/shader.h"
#include "mesh.h"
#include "../Main/states_manager.h"

namespace Adollib {

	class Material {
	private:
		ComPtr<ID3D11Buffer> Mat_cb; 
	public:
		Material();

		int animeIndex = 0;

		Shader shader;

		Vector4 color;

		std::string tex_filename;

		std::vector<Mesh::mesh>* meshes;

		State_manager::DStypes DS_state;

		State_manager::RStypes RS_state;

		State_manager::BStypes BS_state;


		void Load_VS(const char* csoName) {
			// 入力レイアウト
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONES"  ,  0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			UINT numElements = ARRAYSIZE(layout);

			shader.Load_VS(csoName, &vertexLayout, layout, numElements);
		}
		void Load_PS(const char* cso_name) { shader.Load_PS(cso_name); };
		void Load_GS(const char* cso_name) { shader.Load_GS(cso_name); };
		void Load_HS(const char* cso_name) { shader.Load_HS(cso_name); };
		void Load_DS(const char* cso_name) { shader.Load_DS(cso_name); };

		void render();

	private:

		//頂点フォーマット
		struct VertexFormat
		{
#define MAX_BONE_INFLUENCES (4)
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT2 texcoord;
			FLOAT bone_weights[MAX_BONE_INFLUENCES] = { 0 };
			INT bone_indices[MAX_BONE_INFLUENCES] = {};
		};
		Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexLayout;


	};

}