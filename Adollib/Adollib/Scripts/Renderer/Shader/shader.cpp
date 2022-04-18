#include "../../Main/systems.h"
#include "../../Main/resource_manager.h"
#include "../Shader/Shader.h"

#include <d3dcompiler.h>

namespace Adollib {
	Shader::~Shader()
	{
	}

	//****************************************************************
	//
	//
	//
	//****************************************************************
	//------------------------------------------------
	//	シェーダー単体コンパイル
	//------------------------------------------------
	HRESULT Shader::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
	{
		DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
		ShaderFlags |= D3DCOMPILE_DEBUG;
		ShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

		ComPtr<ID3DBlob> BlobError = NULL;
		// コンパイル
		HRESULT hr = D3DCompileFromFile(
			filename,
			NULL,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			method,
			shaderModel,
			ShaderFlags,
			0,
			ppBlobOut,
			BlobError.GetAddressOf()
		);

		// エラー出力
		if (BlobError != NULL)
		{
			MessageBoxA(0, (char*)BlobError->GetBufferPointer(), NULL, MB_OK);
		}

		return hr;
	}

	void Shader::Load_VS(const char* cso_name) {

		// 入力レイアウト
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONES"  ,  0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{"TRANSFORM_MATRIX", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 1,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TRANSFORM_MATRIX", 1,DXGI_FORMAT_R32G32B32A32_FLOAT, 1,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TRANSFORM_MATRIX", 2,DXGI_FORMAT_R32G32B32A32_FLOAT, 1,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TRANSFORM_MATRIX", 3,DXGI_FORMAT_R32G32B32A32_FLOAT, 1,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TEXCOORD_TRANSFORM", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 1,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"COLOR",			 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 1,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};

		UINT numElements = ARRAYSIZE(layout);

		ResourceManager::CreateVsFromCso(cso_name, VS, vertexLayout, layout, numElements);
	}

	void Shader::Load_PS(const char* cso_name) {
		ResourceManager::CreatePsFromCso(cso_name, PS);
	}
	void Shader::Load_GS(const char* cso_name) {
		ResourceManager::CreateGsFromCso(cso_name, GS);
	}
	void Shader::Load_DS(const char* cso_name) {
		ResourceManager::CreateDsFromCso(cso_name, DS);
	}
	void Shader::Load_HS(const char* cso_name) {
		ResourceManager::CreateHsFromCso(cso_name, HS);
	}

	void Shader::Load_PS_shadow(const char* cso_name) {
		ResourceManager::CreatePsFromCso(cso_name, PS_shadow);
	}

	//****************************************************************
	//
	//
	//
	//****************************************************************
	//------------------------------------------------
	//	有効化
	//------------------------------------------------
	void Shader::Activate(const bool& is_PSshader_update)
	{
		Systems::DeviceContext->IASetInputLayout(vertexLayout.Get());

		Systems::DeviceContext->VSSetShader(VS.Get(), NULL, 0);
		Systems::DeviceContext->GSSetShader(GS.Get(), NULL, 0);
		Systems::DeviceContext->HSSetShader(HS.Get(), NULL, 0);
		Systems::DeviceContext->DSSetShader(DS.Get(), NULL, 0);

		if (is_PSshader_update) Systems::DeviceContext->PSSetShader(PS.Get(), NULL, 0);
		else if(PS_shadow != nullptr) Systems::DeviceContext->PSSetShader(PS_shadow.Get(), NULL, 0);




	}

}