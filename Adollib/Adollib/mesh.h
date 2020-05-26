#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <wrl.h>
#include <fbxsdk.h>

using namespace DirectX;
using namespace std;

#define MAX_BONES (4)

namespace Adollib
{

	class Mesh
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexLayout;

	public:

		// ���_�t�H�[�}�b�g
		struct VertexFormat
		{
#define MAX_BONE_INFLUENCES (4)
			XMFLOAT3 position;
			XMFLOAT3 normal;
			XMFLOAT2 texcoord;
			FLOAT bone_weights[MAX_BONE_INFLUENCES] = { 0 };
			INT bone_indices[MAX_BONE_INFLUENCES] = {};
		};

		// �}�e���A��
		struct material
		{
			XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f };
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceVirw = nullptr;
		};

		// �T�u�Z�b�g
		struct subset
		{
			u_int indexStart = 0; // start number of index buffer
			u_int indexCount = 0; // number of vertices (indices)
			material diffuse;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
		};

		// �{�[���E�G�C�g
		struct bone_influence
		{
			int index = 0; // index of bone
			float weight = 0; // weight of bone
		};
		typedef vector<bone_influence> bone_influences_per_control_point;

		// �{�[��
		struct bone
		{
			XMFLOAT4X4 transform;
		};

		// �A�j���[�V�����f�[�^
		typedef vector<bone> skeletal;
		struct skeletal_animation : public vector<skeletal>
		{
			float sampling_time = 1 / 24.0f;
			float animation_tick = 0.0f;
		};

		// ���b�V��
		struct mesh
		{
			Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
			vector<subset> subsets;
			XMFLOAT4X4 globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			vector<skeletal_animation> skeletalAnimation;
		};

	public:
		Mesh() {};

		//Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
		//Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
		//vector<subset> subsets;
		//XMFLOAT4X4 globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		//vector<skeletal_animation> skeletalAnimation;


	};
}