
#include "resource_manager.h"


//--------------------------------
//  �ϐ��錾
//--------------------------------
ResourceManager::ResourceShaderResourceViews    ResourceManager::SRViews[RESOURCE_MAX];
ResourceManager::ResourceVertexShaders          ResourceManager::vertexShaders[RESOURCE_MAX];
ResourceManager::ResourcePixelShaders           ResourceManager::pixelShaders[RESOURCE_MAX];

//--------------------------------
//  �o�C�i���t�@�C���̓ǂݍ���
//--------------------------------
bool readBinaryFile(const char* filename, char** blob, unsigned int& size)
{
	using namespace std;

	ifstream inputFile(filename, ifstream::binary);
	inputFile.seekg(0, ifstream::end);
	size = static_cast<int>(inputFile.tellg());
	inputFile.seekg(0, ifstream::beg);
	*blob = new char[size];
	inputFile.read(*blob, size);
	return true;

}

//--------------------------------
//  �e�N�X�`���ǂݍ���
//--------------------------------
bool ResourceManager::loadShaderResourceView(
	ID3D11Device* device, const wchar_t* fileName,
	ID3D11ShaderResourceView** SRView, D3D11_TEXTURE2D_DESC* texDesc)
{
	using DirectX::CreateWICTextureFromFile;

	int no = -1;
	ResourceShaderResourceViews* find = nullptr;
	ID3D11Resource* resource = nullptr;

	// �f�[�^�T��
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourceShaderResourceViews* p = &SRViews[n];//	�G�C���A�X

													 // �f�[�^�������Ȃ疳��
													 // �A���A�ŏ��Ɍ������̈�Ȃ�Z�b�g�p�Ɋm��
		if (p->refNum == 0) //�f�[�^���Ȃ�������
		{
			if (no == -1) no = n; //�ŏ��Ɍ������Ƃ����ۑ�

			continue;
		}

		// �����t�@�C������Ȃ��̂Ȃ�continue
		if (wcscmp(p->path, fileName) != 0) continue; //wcscmp:�t�@�C�����������ǂ������ׂ�

		// �����t�@�C�������݂���
		find = p;
		p->SRView->GetResource(&resource);
		break;
	}

	// �f�[�^��������Ȃ��������V�K�ǂݍ���
	if (!find)
	{
		ResourceShaderResourceViews* p = &SRViews[no];
		if (FAILED(DirectX::CreateWICTextureFromFile(device, fileName, &resource, &(p->SRView))))
			return false;

		find = p;
		wcscpy_s(p->path, 256, fileName);
	}

	// �ŏI����
	ID3D11Texture2D* tex2D;
	resource->QueryInterface(&tex2D);
	*SRView = find->SRView;
	tex2D->GetDesc(texDesc);
	find->refNum++;
	tex2D->Release();
	resource->Release();
	return true;
}

//--------------------------------
//  ���_�V�F�[�_�̃��[�h
//--------------------------------
bool ResourceManager::loadVertexShader(
	ID3D11Device* device, const char* csoFileName,
	D3D11_INPUT_ELEMENT_DESC* inputElementDesc, int numElements,
	ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout)
{
	*vertexShader = nullptr;
	*inputLayout = nullptr;

	ResourceVertexShaders* find = nullptr;
	int no = -1;

	// const char * -> wchar_t *
	wchar_t	fileName[256];
	size_t	stringSize;
	mbstowcs_s(&stringSize, fileName, csoFileName, strlen(csoFileName));

	// �f�[�^����
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourceVertexShaders* p = &vertexShaders[n];

		if (p->refNum == 0)
		{
			if (no == -1) no = n;
			continue;
		}

		if (wcscmp(p->path, fileName) != 0) continue;

		// �����t�@�C�������݂���
		find = p;
		break;
	}

	// �f�[�^��������Ȃ�����
	if (!find)
	{
		ResourceVertexShaders* p = &vertexShaders[no];
		char* blob;
		unsigned int size;

		// �R���p�C���ς݃s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̓ǂݍ���
		if (!readBinaryFile(csoFileName, &blob, size)) return false;

		// ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
		if (FAILED(device->CreateVertexShader(blob, size, nullptr, &p->vertexShader)))
			return false;

		// ���̓��C�A�E�g�̍쐬
		HRESULT hr = device->CreateInputLayout(inputElementDesc, numElements, blob, size, &p->layout);
		delete[] blob;
		if (FAILED(hr)) return false;

		// �V�K�f�[�^�̕ۑ�
		find = p;
		wcscpy_s(find->path, 256, fileName);
	}

	// �ŏI����
	*vertexShader = find->vertexShader;
	*inputLayout = find->layout;
	find->refNum++;

	return true;
}

//--------------------------------
//  �s�N�Z���V�F�[�_�̃��[�h
//--------------------------------
bool ResourceManager::loadPixelShader(
	ID3D11Device* device, const char* csoFileName,
	ID3D11PixelShader** pixelShader)
{
	*pixelShader = nullptr;
	ResourcePixelShaders* find = nullptr;
	int no = -1;

	// const char * -> wchar_t *
	wchar_t	fileName[256];
	size_t stringSize = 0;
	mbstowcs_s(&stringSize, fileName, csoFileName, strlen(csoFileName));

	// �f�[�^����
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourcePixelShaders* p = &pixelShaders[n];
		if (p->refNum == 0)
		{
			if (no == -1) no = n;
			continue;
		}
		if (wcscmp(p->path, fileName) != 0) continue;

		// �����t�@�C�������݂���
		find = p;
		break;
	}

	// �V�K�쐬
	if (!find)
	{
		ResourcePixelShaders* p = &pixelShaders[no];
		char* blob;
		unsigned int size;

		// �R���p�C���ς݃s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̓ǂݍ���
		if (!readBinaryFile(csoFileName, &blob, size)) return false;

		HRESULT hr = device->CreatePixelShader(blob, size, nullptr, &p->pixelShader);
		delete[] blob;
		if (FAILED(hr)) return false;

		find = p;
		wcscpy_s(find->path, 256, fileName);
	}

	// �ŏI�����i�Q�Ɠn���Ńf�[�^��n���j
	*pixelShader = find->pixelShader;
	find->refNum++;

	return true;
}

//--------------------------------
//  �e�N�X�`���������
//--------------------------------
void ResourceManager::releaseShaderResourceView(ID3D11ShaderResourceView* SRView)
{
	if (!SRView) return;
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourceShaderResourceViews* p = &SRViews[n];

		// �f�[�^�������Ȃ疳��GetSRViewFromDepthStencil
		if (p->refNum == 0) continue;

		// �f�[�^���Ⴄ�Ȃ疳��
		if (SRView != p->SRView) continue;

		// �f�[�^�����݂���
		p->release();
		break;
	}
}

//--------------------------------
//  ���_�V�F�[�_�������
//--------------------------------
void ResourceManager::releaseVertexShader(
	ID3D11VertexShader* vertexShader, ID3D11InputLayout* inputLayout)
{
	if (!vertexShader) return;
	if (!inputLayout) return;

	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourceVertexShaders* p = &vertexShaders[n];
		if (p->refNum == 0) continue;
		if (vertexShader != p->vertexShader) continue;
		if (inputLayout != p->layout) continue;

		//	�f�[�^�����݂���
		p->release();
		break;
	}
}

//--------------------------------
//  �s�N�Z���V�F�[�_�������
//--------------------------------
void ResourceManager::releasePixelShader(ID3D11PixelShader* pixelShader)
{
	if (!pixelShader) return;
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourcePixelShaders* p = &pixelShaders[n];

		if (p->refNum == 0) continue;

		if (pixelShader != p->pixelShader) continue;

		//	�f�[�^�����݂���
		p->release();
		break;
	}
}

//--------------------------------
//  �������
//--------------------------------
void ResourceManager::release()
{
	for (int i = 0; i < RESOURCE_MAX; i++)
	{
		SRViews[i].release(true);
		vertexShaders[i].release(true);
		pixelShaders[i].release(true);
	}
}



//******************************************************************************
