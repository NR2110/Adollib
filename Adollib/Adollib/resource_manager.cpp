
#include "resource_manager.h"


//--------------------------------
//  変数宣言
//--------------------------------
ResourceManager::ResourceShaderResourceViews    ResourceManager::SRViews[RESOURCE_MAX];
ResourceManager::ResourceVertexShaders          ResourceManager::vertexShaders[RESOURCE_MAX];
ResourceManager::ResourcePixelShaders           ResourceManager::pixelShaders[RESOURCE_MAX];

//--------------------------------
//  バイナリファイルの読み込み
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
//  テクスチャ読み込み
//--------------------------------
bool ResourceManager::loadShaderResourceView(
	ID3D11Device* device, const wchar_t* fileName,
	ID3D11ShaderResourceView** SRView, D3D11_TEXTURE2D_DESC* texDesc)
{
	using DirectX::CreateWICTextureFromFile;

	int no = -1;
	ResourceShaderResourceViews* find = nullptr;
	ID3D11Resource* resource = nullptr;

	// データ探索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourceShaderResourceViews* p = &SRViews[n];//	エイリアス

													 // データが無いなら無視
													 // 但し、最初に見つけた領域ならセット用に確保
		if (p->refNum == 0) //データがなかったら
		{
			if (no == -1) no = n; //最初に見つけたところを保存

			continue;
		}

		// 同じファイルじゃないのならcontinue
		if (wcscmp(p->path, fileName) != 0) continue; //wcscmp:ファイルが同じかどうかを比べる

		// 同名ファイルが存在した
		find = p;
		p->SRView->GetResource(&resource);
		break;
	}

	// データが見つからなかった→新規読み込み
	if (!find)
	{
		ResourceShaderResourceViews* p = &SRViews[no];
		if (FAILED(DirectX::CreateWICTextureFromFile(device, fileName, &resource, &(p->SRView))))
			return false;

		find = p;
		wcscpy_s(p->path, 256, fileName);
	}

	// 最終処理
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
//  頂点シェーダのロード
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

	// データ検索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourceVertexShaders* p = &vertexShaders[n];

		if (p->refNum == 0)
		{
			if (no == -1) no = n;
			continue;
		}

		if (wcscmp(p->path, fileName) != 0) continue;

		// 同名ファイルが存在した
		find = p;
		break;
	}

	// データが見つからなかった
	if (!find)
	{
		ResourceVertexShaders* p = &vertexShaders[no];
		char* blob;
		unsigned int size;

		// コンパイル済みピクセルシェーダーオブジェクトの読み込み
		if (!readBinaryFile(csoFileName, &blob, size)) return false;

		// 頂点シェーダーオブジェクトの生成
		if (FAILED(device->CreateVertexShader(blob, size, nullptr, &p->vertexShader)))
			return false;

		// 入力レイアウトの作成
		HRESULT hr = device->CreateInputLayout(inputElementDesc, numElements, blob, size, &p->layout);
		delete[] blob;
		if (FAILED(hr)) return false;

		// 新規データの保存
		find = p;
		wcscpy_s(find->path, 256, fileName);
	}

	// 最終処理
	*vertexShader = find->vertexShader;
	*inputLayout = find->layout;
	find->refNum++;

	return true;
}

//--------------------------------
//  ピクセルシェーダのロード
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

	// データ検索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourcePixelShaders* p = &pixelShaders[n];
		if (p->refNum == 0)
		{
			if (no == -1) no = n;
			continue;
		}
		if (wcscmp(p->path, fileName) != 0) continue;

		// 同名ファイルが存在した
		find = p;
		break;
	}

	// 新規作成
	if (!find)
	{
		ResourcePixelShaders* p = &pixelShaders[no];
		char* blob;
		unsigned int size;

		// コンパイル済みピクセルシェーダーオブジェクトの読み込み
		if (!readBinaryFile(csoFileName, &blob, size)) return false;

		HRESULT hr = device->CreatePixelShader(blob, size, nullptr, &p->pixelShader);
		delete[] blob;
		if (FAILED(hr)) return false;

		find = p;
		wcscpy_s(find->path, 256, fileName);
	}

	// 最終処理（参照渡しでデータを渡す）
	*pixelShader = find->pixelShader;
	find->refNum++;

	return true;
}

//--------------------------------
//  テクスチャ解放処理
//--------------------------------
void ResourceManager::releaseShaderResourceView(ID3D11ShaderResourceView* SRView)
{
	if (!SRView) return;
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourceShaderResourceViews* p = &SRViews[n];

		// データが無いなら無視GetSRViewFromDepthStencil
		if (p->refNum == 0) continue;

		// データが違うなら無視
		if (SRView != p->SRView) continue;

		// データが存在した
		p->release();
		break;
	}
}

//--------------------------------
//  頂点シェーダ解放処理
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

		//	データが存在した
		p->release();
		break;
	}
}

//--------------------------------
//  ピクセルシェーダ解放処理
//--------------------------------
void ResourceManager::releasePixelShader(ID3D11PixelShader* pixelShader)
{
	if (!pixelShader) return;
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourcePixelShaders* p = &pixelShaders[n];

		if (p->refNum == 0) continue;

		if (pixelShader != p->pixelShader) continue;

		//	データが存在した
		p->release();
		break;
	}
}

//--------------------------------
//  解放処理
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
