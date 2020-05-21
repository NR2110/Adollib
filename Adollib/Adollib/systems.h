#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>
#include <memory>
#include <map>
#include <vector>

#include <assert.h>

#pragma comment(lib,"d3d11.lib")

#include "scene.h"
#include "camera.h"

using Microsoft::WRL::ComPtr;

namespace Adollib {

	class Systems {

	private:
		static ComPtr<IDXGISwapChain>			SwapChain;
		static ComPtr<ID3D11RenderTargetView>	RenderTargetView;
		static ComPtr<ID3D11Texture2D>			DepthStencilTexture;
		static ComPtr<ID3D11DepthStencilView>	DepthStencilView;
		static ComPtr<ID3D11ShaderResourceView>	ShaderResourceView;
		static ComPtr<ID3D11DepthStencilState>	DepthStencilState[2];


		static HRESULT CreateDevice(HWND hWnd);
		static bool CreateDepthStencil();
		static bool InitializeRenderTarget();

		static const int RASTERIZE_TYPE = 4;
		static ComPtr<ID3D11RasterizerState> RasterizerState[RASTERIZE_TYPE];
		static bool CreateRasterizerState();

		static const int BLEND_TYPE = 9;
		static ComPtr<ID3D11BlendState>		BlendState[BLEND_TYPE];
		static bool CreateBlendState();

		static bool CreateCamera();


	public:
		static int SCREEN_WIDTH;
		static int SCREEN_HEIGHT;

		static ComPtr<ID3D11Device>			Device;
		static ComPtr<ID3D11DeviceContext>	DeviceContext;

		static std::vector<cameras> Camera;

		static float elapsed_time;

		static bool Initialize(HWND hWnd, int width, int height);
		static void Release();
		static void Clear(DWORD color = 0x00000000);
		static void Flip(int n = 0);

		static int GetScreenWidth() { return SCREEN_WIDTH; }
		static int GetScreenHeight() { return SCREEN_HEIGHT; }
		static ID3D11DepthStencilView* GetDepthStencilView() { return DepthStencilView.Get(); }
		static ID3D11RenderTargetView* GetRenderTargetView() { return RenderTargetView.Get(); }
		static ID3D11DepthStencilState* GetDephtStencilState(int state) { return DepthStencilState[state].Get(); }
		static ID3D11RasterizerState* GetRasterizerState(int state) { return RasterizerState[state].Get(); }
		static ID3D11BlendState* GetBlendState(int state) { return BlendState[state].Get(); }

		static void SetViewPort(int width, int height);

		//DepthStencilState
		enum { DS_FALSE, DS_TRUE };

		//RasterizerState
		enum { RS_CULL_BACK, RS_WIRE, RS_CULL_FRONT, RS_CULL_NONE };

		//BlendState
		enum { BS_NONE, BS_ALPHA, BS_ADD, BS_SUBTRACT, BS_REPLACE, BS_MULTIPLY, BS_LIGHTEN, BS_DARKEN, BS_SCREEN };


	};



}