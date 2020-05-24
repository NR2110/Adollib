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

		State_manager::DStypes DS_state;

		State_manager::RStypes RS_state;

		State_manager::BStypes BS_state;

		Material() {
			color = vector4(1, 1, 1, 1);
			DS_state = State_manager::DStypes::DS_TRUE;
			RS_state = State_manager::RStypes::RS_CULL_BACK;
			BS_state = State_manager::BStypes::BS_ADD;
		};

	private:
		//コンスタントバッファ構造体
		struct ConstantBufferForPerFrame
		{
			DirectX::XMFLOAT4	LightColor;		//ライトの色
			DirectX::XMFLOAT4	LightDir;		//ライトの方向
			DirectX::XMFLOAT4	AmbientColor;	//環境光
			DirectX::XMFLOAT4	EyePos;			//カメラ位置
			float	param1;			//環境マッピングブレンド率
			float	param2;
			float	param3;
			float	param4;
			DirectX::XMFLOAT4X4 LightViewProjection;
		};
		DirectX::XMFLOAT4X4 LightViewProjection;



	};

}