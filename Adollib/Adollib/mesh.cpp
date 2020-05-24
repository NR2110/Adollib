#include <fbxsdk.h>
#include <vector>
#include <functional>
//#include <WICTextureLoader.h>
#include <sstream>
#include <iostream>

#include "resource_manager.h"
#include "mesh.h"

using namespace fbxsdk;

#pragma comment(lib,"libfbxsdk-md.lib")

namespace Adollib
{
	Mesh::Mesh(ID3D11Device* device, const char* fileName, const char* filePath)
	{
		HRESULT hr = S_OK;

		// FBX�̓ǂݍ���
		ResourceManager::CreateModelFromFBX(device, meshes, fileName, filePath);

		// ���_�V�F�[�_�[,���̓��C�A�E�g�I�u�W�F�N�g�̐��� 
#pragma region CreateVertexShader
		if (meshes[0].skeletalAnimation.size() > 0)
		{
			D3D11_INPUT_ELEMENT_DESC inputElemntDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONES"  ,  0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			ResourceManager::CreateVsFromCso(device, "skinned_mesh_vs.cso", vertexShader.GetAddressOf(), vertexLayout.GetAddressOf(), inputElemntDesc, ARRAYSIZE(inputElemntDesc));
		}
		else
		{
			D3D11_INPUT_ELEMENT_DESC inputElemntDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			ResourceManager::CreateVsFromCso(device, "skinned_mesh2_vs.cso", vertexShader.GetAddressOf(), vertexLayout.GetAddressOf(), inputElemntDesc, ARRAYSIZE(inputElemntDesc));
		}

	}

	// FBX���璸�_�C���f�b�N�X���擾����
	// FbxMesh         �擾����FBX
	// vector<u_int>*  �ۑ����������_�C���f�b�N�X
	void Mesh::CreateIndices(FbxMesh* mesh, vector<u_int>* indices)
	{
		// ���|���S����
		int polygonNum = mesh->GetPolygonCount();

		//indices->assign(vertexCnt, -1);
		static int index = 0;

		for (int p = 0; p < polygonNum; p++) {
			for (int n = 0; n < 3; n++)
			{
				indices->at(p * 3 + n) = mesh->GetPolygonVertex(p, n);
				index++;
			}
		}
	}
}