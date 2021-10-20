#pragma once

#include "../../Main/systems.h"

namespace Adollib {
	class Texture
	{
	protected:

	private:
		ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
		ComPtr<ID3D11SamplerState> sampler = nullptr;
		ComPtr<ID3D11RenderTargetView> RenderTargetView = nullptr;
		ComPtr<ID3D11DepthStencilView> DepthStencilView = nullptr;

		// �e�N�X�`�����
		D3D11_TEXTURE2D_DESC texture2d_desc;
	public:


		Texture();
		virtual ~Texture();
		bool Load(const wchar_t* filename = nullptr);
		void Set(UINT Slot = 0, BOOL flg = TRUE);
		UINT GetWidth() { return texture2d_desc.Width; }
		UINT GetHeight() { return texture2d_desc.Height; }
		bool Create(u_int width, u_int height, DXGI_FORMAT format);
		// �[�x�X�e���V���o�b�t�@�쐬
		bool CreateDepth(u_int width, u_int height, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS);

		ID3D11RenderTargetView* GetRenderTargetView() { return RenderTargetView.Get(); }
		ID3D11DepthStencilView* GetDepthStencilView() { return DepthStencilView.Get(); }
		ID3D11ShaderResourceView* GetShaderResourceView() { return ShaderResourceView.Get();  }
	};

}
