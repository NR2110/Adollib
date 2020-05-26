#include "systems.h"
#include "Shader.h"
#include "resource_manager.h"

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
	//	�V�F�[�_�[�P�̃R���p�C��
	//------------------------------------------------
	HRESULT Shader::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
	{
		DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
		ShaderFlags |= D3DCOMPILE_DEBUG;
		ShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

		ComPtr<ID3DBlob> BlobError = NULL;
		// �R���p�C��
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

		// �G���[�o��
		if (BlobError != NULL)
		{
			MessageBoxA(0, (char*)BlobError->GetBufferPointer(), NULL, MB_OK);
		}

		return hr;
	}

	void Shader::Load_VS(const char* cso_name) {
		// ���̓��C�A�E�g
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE(layout);

		ResourceManager::CreateVsFromCso(cso_name, VS.GetAddressOf(), VertexLayout.GetAddressOf(), layout, numElements);
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
	//	�L����
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