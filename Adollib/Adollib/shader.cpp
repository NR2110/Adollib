#include "systems.h"
#include "Shader.h"

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

	//------------------------------------------------
	//	�V�F�[�_�[�Z�b�g�R���p�C��
	//------------------------------------------------
	bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc)
	{
		HRESULT hr = S_OK;

		ComPtr<ID3DBlob> VSBlob = NULL;
		// ���_�V�F�[�_
		hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
		assert(SUCCEEDED(hr));


		// ���_�V�F�[�_����
		hr = Systems::Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
		assert(SUCCEEDED(hr));

		// ���̓��C�A�E�g
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE(layout);

		// ���̓��C�A�E�g����
		hr = Systems::Device->CreateInputLayout(
			layout,
			numElements,
			VSBlob->GetBufferPointer(),
			VSBlob->GetBufferSize(),
			VertexLayout.GetAddressOf()
		);
		assert(SUCCEEDED(hr));


		// ���̓��C�A�E�g�ݒ�
		Systems::DeviceContext->IASetInputLayout(VertexLayout.Get());


		// �s�N�Z���V�F�[�_
		ComPtr<ID3DBlob> PSBlob = NULL;
		hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
		if (FAILED(hr))
		{
			return false;
		}

		// �s�N�Z���V�F�[�_����
		hr = Systems::Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
		//PSBlob->Release();
		assert(SUCCEEDED(hr));

		return true;
	}


	//------------------------------------------------
	//	�V�F�[�_�[�Z�b�g�R���p�C��2
	//------------------------------------------------
	bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc)
	{
		HRESULT hr = S_OK;

		ComPtr<ID3DBlob> VSBlob = NULL;
		// ���_�V�F�[�_
		hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
		assert(SUCCEEDED(hr));

		// ���_�V�F�[�_����
		hr = Systems::Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
		assert(SUCCEEDED(hr));

		// ���̓��C�A�E�g
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE(layout);

		// ���̓��C�A�E�g����
		hr = Systems::Device->CreateInputLayout(
			layout,
			numElements,
			VSBlob->GetBufferPointer(),
			VSBlob->GetBufferSize(),
			VertexLayout.GetAddressOf()
		);

		assert(SUCCEEDED(hr));

		// ���̓��C�A�E�g�ݒ�
		Systems::DeviceContext->IASetInputLayout(VertexLayout.Get());

		// �s�N�Z���V�F�[�_
		ComPtr<ID3DBlob> PSBlob = NULL;
		hr = Compile(filename, PSFunc, "ps_5_0", PSBlob.GetAddressOf());
		assert(SUCCEEDED(hr));

		// �s�N�Z���V�F�[�_����
		hr = Systems::Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
		//PSBlob.Get()->Release();
		assert(SUCCEEDED(hr));
		// �W�I���g���V�F�[�_
		ComPtr<ID3DBlob> GSBlob = NULL;
		hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
		assert(SUCCEEDED(hr));

		// �W�I���g���V�F�[�_����
		hr = Systems::Device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, GS.GetAddressOf());
		assert(SUCCEEDED(hr));

		return true;
	}

	//------------------------------------------------
	//	�V�F�[�_�[�Z�b�g�R���p�C��3
	//------------------------------------------------
	bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc, LPCSTR HSFunc, LPCSTR DSFunc)
	{
		HRESULT hr = S_OK;

		ComPtr<ID3DBlob> VSBlob = NULL;
		// ���_�V�F�[�_
		hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
		assert(SUCCEEDED(hr));

		// ���_�V�F�[�_����
		hr = Systems::Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
		assert(SUCCEEDED(hr));

		// ���̓��C�A�E�g
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE(layout);

		// ���̓��C�A�E�g����
		hr = Systems::Device->CreateInputLayout(
			layout,
			numElements,
			VSBlob->GetBufferPointer(),
			VSBlob->GetBufferSize(),
			VertexLayout.GetAddressOf()
		);

		assert(SUCCEEDED(hr));
		VSBlob.Get()->Release();

		// ���̓��C�A�E�g�ݒ�
		Systems::DeviceContext->IASetInputLayout(VertexLayout.Get());

		// �s�N�Z���V�F�[�_
		ComPtr<ID3DBlob> PSBlob = NULL;
		hr = Compile(filename, PSFunc, "ps_5_0", PSBlob.GetAddressOf());
		assert(SUCCEEDED(hr));

		// �s�N�Z���V�F�[�_����
		hr = Systems::Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
		assert(SUCCEEDED(hr));

		// �W�I���g���V�F�[�_
		ComPtr<ID3DBlob> GSBlob = NULL;
		hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
		assert(SUCCEEDED(hr));
		// �W�I���g���V�F�[�_����
		hr = Systems::Device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, GS.GetAddressOf());
		assert(SUCCEEDED(hr));

		// �n���V�F�[�_
		ComPtr<ID3DBlob> HSBlob = NULL;
		hr = Compile(filename, HSFunc, "hs_5_0", HSBlob.GetAddressOf());
		assert(SUCCEEDED(hr));

		// �n���V�F�[�_����
		hr = Systems::Device->CreateHullShader(HSBlob->GetBufferPointer(), HSBlob->GetBufferSize(), NULL, HS.GetAddressOf());
		assert(SUCCEEDED(hr));

		// �h���C���V�F�[�_
		ComPtr<ID3DBlob> DSBlob = NULL;
		hr = Compile(filename, DSFunc, "ds_5_0", DSBlob.GetAddressOf());
		assert(SUCCEEDED(hr));

		// �h���C���V�F�[�_����
		hr = Systems::Device->CreateDomainShader(DSBlob->GetBufferPointer(), DSBlob->GetBufferSize(), NULL, DS.GetAddressOf());
		assert(SUCCEEDED(hr));



		return true;
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