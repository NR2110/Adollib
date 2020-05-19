#pragma once

//******************************************************************************
//
//
//      プリミティブ描画用
//
//
//******************************************************************************

//------< include >-------------------------------------------------------------
#include <fstream>
#include <d3d11.h>
#include "DirectXTK-master/Inc/WICTextureLoader.h"


	//==========================================================================
	//
	//      ResourceManagerクラス
	//
	//==========================================================================
class ResourceManager
{
	static const int RESOURCE_MAX = 64;

	//--------------------------------
	//  構造体定義
	//--------------------------------
	struct ResourceShaderResourceViews
	{
		int refNum;
		wchar_t path[256];
		ID3D11ShaderResourceView* SRView;
		ResourceShaderResourceViews() : refNum(0), SRView(nullptr) { path[0] = '\0'; }
		void release(bool force = (false))
		{
			if (refNum == 0) return;
			if (--refNum <= 0) force = true;
			if (force)
			{
				if (SRView) SRView->Release();
				SRView = nullptr;
				refNum = 0;
				path[0] = '\0';
			}
		}
	};
	struct ResourceVertexShaders
	{
		int refNum;
		wchar_t path[256];
		ID3D11VertexShader* vertexShader;
		ID3D11InputLayout* layout;
		ResourceVertexShaders() : refNum(0), vertexShader(nullptr), layout(nullptr) { path[0] = '\0'; }
		void release(bool force = (false))
		{
			if (refNum == 0) return;
			if (--refNum <= 0) force = true;
			if (force)
			{
				if (layout) layout->Release();
				if (vertexShader) vertexShader->Release();
				layout = nullptr;
				vertexShader = nullptr;
				refNum = 0;
				path[0] = '\0';
			}
		}
	};
	struct ResourcePixelShaders
	{
		int refNum;
		wchar_t path[256];
		ID3D11PixelShader* pixelShader;
		ResourcePixelShaders() : refNum(0), pixelShader(nullptr) { path[0] = '\0'; }
		void release(bool force = (false))
		{
			if (refNum == 0) return;
			if (--refNum <= 0) force = true;
			if (force)
			{
				if (pixelShader) pixelShader->Release();
				pixelShader = nullptr;
				refNum = 0;
				path[0] = '\0';
			}
		}
	};

	static ResourceShaderResourceViews	SRViews[RESOURCE_MAX];
	static ResourceVertexShaders		vertexShaders[RESOURCE_MAX];
	static ResourcePixelShaders			pixelShaders[RESOURCE_MAX];

private:
	ResourceManager() {};
	~ResourceManager() {};

public:
	static void release();
	static bool loadShaderResourceView(ID3D11Device*, const wchar_t*,
		ID3D11ShaderResourceView**, D3D11_TEXTURE2D_DESC*);
	static bool loadVertexShader(ID3D11Device*, const char*,
		D3D11_INPUT_ELEMENT_DESC*, int, ID3D11VertexShader**, ID3D11InputLayout**);
	static bool loadPixelShader(ID3D11Device*, const char*, ID3D11PixelShader**);

	static void releaseShaderResourceView(ID3D11ShaderResourceView*);
	static void releaseVertexShader(ID3D11VertexShader*, ID3D11InputLayout*);
	static void releasePixelShader(ID3D11PixelShader*);
};



//******************************************************************************
