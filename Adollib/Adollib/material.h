#pragma once

#include <vector>
#include <string>
#include "vector.h"
#include "shader.h"
#include "states_manager.h"

namespace Adollib {

	class Material {

	public:
		std::vector<std::unique_ptr<Shader>> shader;

		vector4 color;

		std::string tex_filename;

		State_manager::DepthStencilStats DS_state;

		State_manager::RasterizerStats RS_state;

		State_manager::BlendStats BS_state;

		Material() {
			color = vector4(1, 1, 1, 1);
			DS_state = State_manager::DepthStencilStats::DS_TRUE;
			RS_state = State_manager::RasterizerStats::RS_CULL_BACK;
			BS_state = State_manager::BlendStats::BS_ADD;
		};

	private:
		//�R���X�^���g�o�b�t�@�\����
		struct ConstantBufferForPerFrame
		{
			DirectX::XMFLOAT4	LightColor;		//���C�g�̐F
			DirectX::XMFLOAT4	LightDir;		//���C�g�̕���
			DirectX::XMFLOAT4	AmbientColor;	//����
			DirectX::XMFLOAT4	EyePos;			//�J�����ʒu
			float	param1;			//���}�b�s���O�u�����h��
			float	param2;
			float	param3;
			float	param4;
			DirectX::XMFLOAT4X4 LightViewProjection;
		};
		DirectX::XMFLOAT4X4 LightViewProjection;



	};

}