#include "systems.h"

#pragma comment( lib, "d3dcompiler.lib" ) //これがないと shaderのD3DCompileFromFileが使えない

using namespace Adollib;

ComPtr<ID3D11Device>		Systems::Device;
ComPtr<IDXGISwapChain>		Systems::SwapChain;

ComPtr<ID3D11DeviceContext>		Systems::DeviceContext;
ComPtr<ID3D11RenderTargetView>  Systems::RenderTargetView;

ComPtr<ID3D11Texture2D>				Systems::DepthStencilTexture;
ComPtr<ID3D11DepthStencilView>		Systems::DepthStencilView;
ComPtr<ID3D11ShaderResourceView>	Systems::ShaderResourceView;
ComPtr<ID3D11DepthStencilState>		Systems::DepthStencilState[2];
ComPtr<ID3D11RasterizerState>		Systems::RasterizerState[RASTERIZE_TYPE];
ComPtr<ID3D11BlendState>			Systems::BlendState[BLEND_TYPE];

MonoInput* Systems::inputManager = nullptr;
//MonoAudio* Systems::audioManager = nullptr;
//Debug* framework::debug = nullptr;
std::shared_ptr<Time>  Systems::time = nullptr;
Vector4 Systems::clear_color = Vector4(0, 0.5f, 0.5f, 1);

State_manager::DStypes Systems::DS_type = State_manager::DStypes::DS_TRUE;
State_manager::RStypes Systems::RS_type = State_manager::RStypes::RS_CULL_BACK;
State_manager::BStypes Systems::BS_type = State_manager::BStypes::BS_ADD;

#include "../Imgui/imgui_all.h"

#pragma region Initialize

bool Systems::Initialize(HWND hWnd, int width, int height)
{
	CreateDevice(hWnd);
	InitializeRenderTarget();
	CreateRasterizerState();
	CreateBlendState();

	time = std::make_shared<Time>();
	time->initialize();
	time->setFrameRate(60.0f);
	time->isFloatFPS(true);
	/// Debugの生成
	//Debug::create();
	//debug = Debug::getInstancePtr();

	// 入力マネージャーの生成
	MonoInput::create();
	inputManager = MonoInput::getInstancePtr();
	inputManager->initialize(hWnd);

	return false;
}

HRESULT Systems::CreateDevice(HWND hWnd)
{
	BOOL Windowed = TRUE;
	HRESULT hr = S_OK;

	//::: デバイスとスワップチェインンの作成 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	UINT creationFlags/*= D3D11_CREATE_DEVICE_DEBUG;*/ = 0;

	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_UNKNOWN,
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes ARRAYSIZE(driverTypes);

	//GPUのvarsion
	D3D_FEATURE_LEVEL  FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	//デバイスの生成
	for (UINT i = 0; i < numDriverTypes; i++) {
		D3D_DRIVER_TYPE driverType = driverTypes[i];
		hr = D3D11CreateDevice(
			NULL,
			driverType,
			NULL,
			creationFlags,
			featureLevels,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			Device.GetAddressOf(),
			&FeatureLevel,
			DeviceContext.GetAddressOf());
		if (FAILED(hr)) {
			if (i == numDriverTypes - 1) {
				MessageBoxW(hWnd, L"D3D11CreateDevice", L"Err", MB_ICONSTOP);
				return S_FALSE;
			}
			continue;
		}

		break;
	}

	//インターフェース取得
	IDXGIDevice1* hpDXGI = NULL;
	if (FAILED(Device.Get()->QueryInterface(__uuidof(IDXGIDevice1), (void**)&hpDXGI))) {
		MessageBoxW(hWnd, L"QueryInterface", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	//アダプター取得
	IDXGIAdapter* hpAdapter = NULL;
	if (FAILED(hpDXGI->GetAdapter(&hpAdapter))) {
		MessageBoxW(hWnd, L"GetAdapter", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	//ファクトリー取得
	IDXGIFactory* hpDXGIFactory = NULL;
	hpAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&hpDXGIFactory);
	if (hpDXGIFactory == NULL) {
		MessageBoxW(hWnd, L"GetParent", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	//スワップチェイン作成
	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferDesc.Width = Al_Global::SCREEN_WIDTH;
	scd.BufferDesc.Height = Al_Global::SCREEN_HEIGHT;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SampleDesc.Quality = 0;
	scd.SampleDesc.Count = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;

	scd.Windowed = Windowed;

	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if (FAILED(hpDXGIFactory->CreateSwapChain(Device.Get(), &scd, SwapChain.GetAddressOf()))) {
		MessageBoxW(hWnd, L"CreateSwapChain", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}


	return S_OK;
}

bool Systems::InitializeRenderTarget()
{
	// バックバッファ取得
	ID3D11Texture2D* BackBuffer = NULL;
	HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);

	if (FAILED(hr))
	{
		//assert(FAILED(hr));
		return false;
	}

	// レンダーターゲットビュー生成
	hr = Device->CreateRenderTargetView(BackBuffer, NULL, RenderTargetView.GetAddressOf());
	BackBuffer->Release();
	BackBuffer = NULL;

	if (FAILED(hr))
	{
		//assert(FAILED(hr));
		return false;
	}
	//  深度ステンシルバッファ生成
	CreateDepthStencil();

	// レンダーターゲットビュー設定
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());

	// ビューポートの設定
	SetViewPort(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT);

	return true;
}

void Systems::SetViewPort(int width, int height)
{
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	DeviceContext->RSSetViewports(1, &vp);

}

bool Systems::CreateDepthStencil()
{
	// 深度ステンシル設定
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = Al_Global::SCREEN_WIDTH;
	td.Height = Al_Global::SCREEN_HEIGHT;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R24G8_TYPELESS;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	// 深度ステンシルテクスチャ生成
	HRESULT hr = Device->CreateTexture2D(&td, NULL, &DepthStencilTexture);
	//assert(FAILED(hr));

	if (FAILED(hr))
	{
		return false;
	}

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー生成
	hr = Device->CreateDepthStencilView(DepthStencilTexture.Get(), &dsvd, DepthStencilView.GetAddressOf());
	//assert(FAILED(hr));

	if (FAILED(hr))
	{
		return false;
	}
	//デプスステンシルステート
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = FALSE;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(State_manager::DStypes::DS_FALSE)].GetAddressOf());
	//assert(FAILED(hr));

	if (FAILED(hr))
	{
		return false;
	}
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(State_manager::DStypes::DS_TRUE)].GetAddressOf());
	//assert(FAILED(hr));


	if (FAILED(hr))
	{
		return false;
	}


	// シェーダリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	// シェーダリソースビュー生成
	hr = Device->CreateShaderResourceView(DepthStencilTexture.Get(), &srvd, ShaderResourceView.GetAddressOf());
	//assert(FAILED(hr));

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool Systems::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rd;
	//for (int state = 0; state < static_cast<int>(State_manager::RStypes::RS_SIZE) - 1; state++) {
	//	switch (static_cast<State_manager::RStypes>(state)) {

	//	case State_manager::RStypes::RS_CULL_NONE:
	//		ZeroMemory(&rd, sizeof(rd));
	//		rd.FillMode = D3D11_FILL_SOLID;
	//		rd.CullMode = D3D11_CULL_NONE;
	//		rd.FrontCounterClockwise = FALSE;
	//		rd.DepthBias = 0;
	//		rd.DepthBiasClamp = 0;
	//		rd.SlopeScaledDepthBias = 0;
	//		rd.DepthClipEnable = TRUE;
	//		rd.ScissorEnable = FALSE;
	//		rd.MultisampleEnable = FALSE;
	//		rd.AntialiasedLineEnable = FALSE;

	//		break;

	//	case State_manager::RStypes::RS_CULL_FRONT:
	//		ZeroMemory(&rd, sizeof(rd));
	//		rd.FillMode = D3D11_FILL_SOLID;
	//		rd.CullMode = D3D11_CULL_FRONT;
	//		rd.FrontCounterClockwise = FALSE;
	//		rd.DepthBias = 0;
	//		rd.DepthBiasClamp = 0;
	//		rd.SlopeScaledDepthBias = 0;
	//		rd.DepthClipEnable = TRUE;
	//		rd.ScissorEnable = FALSE;
	//		rd.MultisampleEnable = FALSE;
	//		rd.AntialiasedLineEnable = FALSE;

	//		break;

	//	case State_manager::RStypes::RS_CULL_BACK:
	//		ZeroMemory(&rd, sizeof(rd));
	//		rd.FillMode = D3D11_FILL_SOLID;
	//		rd.CullMode = D3D11_CULL_BACK;
	//		rd.FrontCounterClockwise = FALSE;
	//		rd.DepthBias = 0;
	//		rd.DepthBiasClamp = 0;
	//		rd.SlopeScaledDepthBias = 0;
	//		rd.DepthClipEnable = TRUE;
	//		rd.ScissorEnable = FALSE;
	//		rd.MultisampleEnable = FALSE;
	//		rd.AntialiasedLineEnable = FALSE;

	//		break;

	//	case State_manager::RStypes::RS_WIRE_NONE:
	//		ZeroMemory(&rd, sizeof(rd));
	//		rd.FillMode = D3D11_FILL_WIREFRAME;
	//		rd.CullMode = D3D11_CULL_NONE;
	//		rd.FrontCounterClockwise = FALSE;
	//		rd.DepthBias = 0;
	//		rd.DepthBiasClamp = 0;
	//		rd.SlopeScaledDepthBias = 0;
	//		rd.DepthClipEnable = TRUE;
	//		rd.ScissorEnable = FALSE;
	//		rd.MultisampleEnable = FALSE;
	//		rd.AntialiasedLineEnable = FALSE;
	//		break;

	//	case State_manager::RStypes::RS_WIRE_FRONT:
	//		ZeroMemory(&rd, sizeof(rd));
	//		rd.FillMode = D3D11_FILL_WIREFRAME;
	//		rd.CullMode = D3D11_CULL_FRONT;
	//		rd.FrontCounterClockwise = FALSE;
	//		rd.DepthBias = 0;
	//		rd.DepthBiasClamp = 0;
	//		rd.SlopeScaledDepthBias = 0;
	//		rd.DepthClipEnable = TRUE;
	//		rd.ScissorEnable = FALSE;
	//		rd.MultisampleEnable = FALSE;
	//		rd.AntialiasedLineEnable = FALSE;
	//		break;

	//	case State_manager::RStypes::RS_WIRE_BACK:
	//		ZeroMemory(&rd, sizeof(rd));
	//		rd.FillMode = D3D11_FILL_WIREFRAME;
	//		rd.CullMode = D3D11_CULL_BACK;
	//		rd.FrontCounterClockwise = FALSE;
	//		rd.DepthBias = 0;
	//		rd.DepthBiasClamp = 0;
	//		rd.SlopeScaledDepthBias = 0;
	//		rd.DepthClipEnable = TRUE;
	//		rd.ScissorEnable = FALSE;
	//		rd.MultisampleEnable = FALSE;
	//		rd.AntialiasedLineEnable = FALSE;
	//		break;
	//	}
	//	HRESULT hr = Device->CreateRasterizerState(&rd, RasterizerState[state].GetAddressOf());
	//	//assert(FAILED(hr));

	//	if (FAILED(hr))
	//	{
	//		return false;
	//	}
	//}
	for (int state = 0; state < static_cast<int>(State_manager::RStypes::RS_SIZE) - 1; state++) {
		switch (static_cast<State_manager::RStypes>(state)) {
		case State_manager::RStypes::RS_CULL_BACK:
			ZeroMemory(&rd, sizeof(rd));
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_BACK;
			rd.FrontCounterClockwise = FALSE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = TRUE;
			rd.ScissorEnable = FALSE;
			rd.MultisampleEnable = FALSE;
			rd.AntialiasedLineEnable = FALSE;

			break;

		case State_manager::RStypes::RS_WIRE:
			ZeroMemory(&rd, sizeof(rd));
			rd.FillMode = D3D11_FILL_WIREFRAME;
			rd.CullMode = D3D11_CULL_BACK;
			rd.FrontCounterClockwise = FALSE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = TRUE;
			rd.ScissorEnable = FALSE;
			rd.MultisampleEnable = FALSE;
			rd.AntialiasedLineEnable = FALSE;
			break;

		case State_manager::RStypes::RS_CULL_FRONT:
			ZeroMemory(&rd, sizeof(rd));
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_FRONT;
			rd.FrontCounterClockwise = FALSE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = TRUE;
			rd.ScissorEnable = FALSE;
			rd.MultisampleEnable = FALSE;
			rd.AntialiasedLineEnable = FALSE;

			break;

		case State_manager::RStypes::RS_CULL_NONE:
			ZeroMemory(&rd, sizeof(rd));
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_NONE;
			rd.FrontCounterClockwise = FALSE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = TRUE;
			rd.ScissorEnable = FALSE;
			rd.MultisampleEnable = FALSE;
			rd.AntialiasedLineEnable = FALSE;

			break;
		}
		HRESULT hr = Device->CreateRasterizerState(&rd, RasterizerState[state].GetAddressOf());
		//assert(FAILED(hr));

		if (FAILED(hr))
		{
			return false;
		}
	}

	return true;

}

bool Systems::CreateBlendState()
{
	D3D11_BLEND_DESC bd;

	for (int state = 0; state < static_cast<int>(State_manager::BStypes::BS_SIZE) - 1; state++) {
		switch (static_cast<State_manager::BStypes>(state)) {
		case State_manager::BStypes::BS_NONE:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = false;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case State_manager::BStypes::BS_ALPHA:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case State_manager::BStypes::BS_ADD:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case State_manager::BStypes::BS_SUBTRACT:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case State_manager::BStypes::BS_REPLACE:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;
		case State_manager::BStypes::BS_MULTIPLY:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;
		case State_manager::BStypes::BS_LIGHTEN:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case State_manager::BStypes::BS_DARKEN:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;
		case State_manager::BStypes::BS_SCREEN:
			ZeroMemory(&bd, sizeof(bd));
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_DEST_COLOR;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		}
		//ブレンドステートの作成
		HRESULT hr = Device->CreateBlendState(&bd, BlendState[state].GetAddressOf());
		//assert(FAILED(hr));

		if (FAILED(hr))
		{
			return false;
		}
	}
	return true;
}

#pragma endregion

#pragma region Render





#pragma endregion



void Systems::Release()
{

	//delete time;
	//time = nullptr;
	MonoInput::destroy();
	inputManager = nullptr;
}

//------------------------------------------------
//	クリア
//------------------------------------------------
void Systems::Clear(DWORD color)
{


	float clearColor[4] = { clear_color.x,  clear_color.y,  clear_color.z,  clear_color.w };

	DeviceContext->OMSetDepthStencilState(DepthStencilState[static_cast<int>(State_manager::DStypes::DS_TRUE)].Get(), 1);
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), clearColor);
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}

//------------------------------------------------
//	フリップ
//------------------------------------------------
void Systems::Flip(int n)
{
	// フリップ処理
	SwapChain->Present(n, 0);

}
