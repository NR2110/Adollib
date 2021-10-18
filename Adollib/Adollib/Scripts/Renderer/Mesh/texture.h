#pragma once

#include "../../Main/systems.h"

class Texture
{
protected:

public:
	ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	ComPtr<ID3D11SamplerState> sampler = nullptr;
	ComPtr<ID3D11RenderTargetView> RenderTargetView = nullptr;

	// テクスチャ情報
	D3D11_TEXTURE2D_DESC texture2d_desc;

	Texture();
	virtual ~Texture();
	bool Load(const wchar_t* filename = nullptr);
	void Set(UINT Slot = 0, BOOL flg = TRUE);
	UINT GetWidth() { return texture2d_desc.Width; }
	UINT GetHeight() { return texture2d_desc.Height; }
	bool Create(u_int width, u_int height, DXGI_FORMAT format);
	ID3D11RenderTargetView* GetRenderTarget() { return RenderTargetView.Get(); }

};
