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

#include "Adollib.h"
#include "../Scene/scene.h"
#include "input.h"
#include "time.h"
#include "states_manager.h"

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

		static const int RASTERIZE_TYPE = 4; //ラスタライザの種類
		static ComPtr<ID3D11RasterizerState> RasterizerState[RASTERIZE_TYPE];
		static bool CreateRasterizerState();

		static const int BLEND_TYPE = 9; //blendstateの種類
		static ComPtr<ID3D11BlendState>		BlendState[BLEND_TYPE];
		static bool CreateBlendState();

	private:
	public:
		static MonoInput* inputManager;
		static Time* time;
		static Vector4 clear_color;
		//static MonoAudio* audioManager;

	public:
		static ComPtr<ID3D11Device>			Device;
		static ComPtr<ID3D11DeviceContext>	DeviceContext;

		static bool Initialize(HWND hWnd, int width, int height);
		static void Release();
		static void Clear(DWORD color = 0x00000000);
		static void Flip(int n = 0);

		//static int GetScreenWidth() { return Adollib::SCREEN_WIDTH; }
		//static int GetScreenHeight() { return Adollib::SCREEN_HEIGHT; }
		static ID3D11DepthStencilView* GetDepthStencilView() { return DepthStencilView.Get(); }
		static ID3D11RenderTargetView* GetRenderTargetView() { return RenderTargetView.Get(); }

		static State_manager::DStypes DS_type;
		static State_manager::RStypes RS_type;
		static State_manager::BStypes BS_type;
		static ID3D11DepthStencilState* GetDephtStencilState(State_manager::DStypes state) { 	return DepthStencilState[static_cast<int>(state)].Get();	}
		static ID3D11RasterizerState* GetRasterizerState(State_manager::RStypes state)	   {	return RasterizerState[static_cast<int>(state)].Get(); 	}
		static ID3D11BlendState* GetBlendState(State_manager::BStypes state)			   { 	return BlendState[static_cast<int>(state)].Get();			}

		static void SetDephtStencilState(State_manager::DStypes state) {
			DeviceContext->OMSetDepthStencilState(GetDephtStencilState(state), 1);
			DS_type = state;
		}
		static void SetRasterizerState(State_manager::RStypes state) {
			DeviceContext->RSSetState(GetRasterizerState(state));
			RS_type = state;
		}
		static void SetBlendState(State_manager::BStypes state) {
			DeviceContext->OMSetBlendState(GetBlendState(state), nullptr, 0xFFFFFFFF);
			BS_type = state;
		}

		static State_manager::DStypes GetDS_type() { return DS_type; }
		static State_manager::RStypes GetRS_type() { return RS_type; }
		static State_manager::BStypes GetBS_type() { return BS_type; }

		static void SetViewPort(int width, int height);
		static void SetRenderTargetView() { DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get()); }

		static void CreateConstantBuffer(ID3D11Buffer** ppCB, u_int size) {

			size += 16 - (1 + (size - 1) % 16);

			// 定数バッファ生成
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
			bd.ByteWidth = size;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;

			HRESULT hr = Device->CreateBuffer(&bd, NULL, ppCB);
		};

	};



}