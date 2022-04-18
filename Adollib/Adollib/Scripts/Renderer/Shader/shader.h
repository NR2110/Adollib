#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <memory>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <assert.h>

namespace Adollib {
	class Shader
	{
	protected:
		ComPtr<ID3D11VertexShader>		VS = nullptr; // 頂点シェーダ
		ComPtr<ID3D11PixelShader>		PS = nullptr; // ピクセルシェーダ
		ComPtr<ID3D11GeometryShader>	GS = nullptr; // ジオメトリシェーダ
		ComPtr<ID3D11HullShader>		HS = nullptr; // ハルシェーダ
		ComPtr<ID3D11DomainShader>		DS = nullptr; // ドメインネームシェーダ

		ComPtr<ID3D11PixelShader>		PS_shadow = nullptr; // 影用ピクセルシェーダ

		HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

	public:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexLayout;


	public:

		Shader() { /*ZeroMemory(this, sizeof(Shader));*/ }
		virtual ~Shader();

		void Load_VS(const char* cso_Name);
		void Load_PS(const char* cso_name);
		void Load_GS(const char* cso_name);
		void Load_HS(const char* cso_name);
		void Load_DS(const char* cso_name);

		void Load_PS_shadow(const char* cso_name);

		void Activate(const bool& is_PSshader_update = true);

	};

}