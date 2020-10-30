#include "../Main/systems.h"
#include "../Shader/Shader.h"
#include "../Main/resource_manager.h"

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

	void Shader::Load_VS(const char* cso_name, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements) {
		ResourceManager::CreateVsFromCso(cso_name, VS.GetAddressOf(), inputLayout, inputElementDesc, numElements);
	}
	void Shader::Load_PS(const char* cso_name) {
		ResourceManager::CreatePsFromCso(cso_name, PS.GetAddressOf());
	}
	void Shader::Load_GS(const char* cso_name) {
		ResourceManager::CreateGsFromCso(cso_name, GS.GetAddressOf());
	}
	void Shader::Load_DS(const char* cso_name) {
		ResourceManager::CreateDsFromCso(cso_name, DS.GetAddressOf());
	}
	void Shader::Load_HS(const char* cso_name) {
		ResourceManager::CreateHsFromCso(cso_name, HS.GetAddressOf());
	}

	//****************************************************************
	//
	//
	//
	//****************************************************************
	//------------------------------------------------
	//	有効化
	//------------------------------------------------
	void Shader::Activate()
	{
		Systems::DeviceContext->VSSetShader(VS.Get(), NULL, 0);
		Systems::DeviceContext->PSSetShader(PS.Get(), NULL, 0);
		Systems::DeviceContext->GSSetShader(GS.Get(), NULL, 0);
		Systems::DeviceContext->HSSetShader(HS.Get(), NULL, 0);
		Systems::DeviceContext->DSSetShader(DS.Get(), NULL, 0);

	}

}