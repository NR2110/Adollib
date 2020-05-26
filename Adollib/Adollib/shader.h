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
		ComPtr<ID3D11VertexShader>		VS = nullptr; // ���_�V�F�[�_
		ComPtr<ID3D11PixelShader>		PS = nullptr; // �s�N�Z���V�F�[�_
		ComPtr<ID3D11GeometryShader>	GS = nullptr; // �W�I���g���V�F�[�_
		ComPtr<ID3D11HullShader>		HS = nullptr; //�@�n���V�F�[�_
		ComPtr<ID3D11DomainShader>		DS = nullptr; //�@�h���C���l�[���V�F�[�_

		HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);


	public:

		Shader() { /*ZeroMemory(this, sizeof(Shader));*/ }
		virtual ~Shader();

		void Load_VS(const char* csoName,ID3D11InputLayout** inputLayout,D3D11_INPUT_ELEMENT_DESC* inputElementDesc,  UINT numElements);
		void Load_PS(const char* cso_name);
		void Load_GS(const char* cso_name);
		void Load_HS(const char* cso_name);
		void Load_DS(const char* cso_name);

		void Activate();
	};


	//----------------------------------------------
	//
	//	���_�f�[�^�\���̒�`
	//
	//----------------------------------------------

	struct VERTEX
	{
		DirectX::XMFLOAT3 Pos;	//�ʒu
		DirectX::XMFLOAT3 Normal;//�@��
		DirectX::XMFLOAT2 Tex;	//UV���W
		DirectX::XMFLOAT4 Color;	//���_�F
	};
}