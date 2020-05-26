
#include "texture.h"
#include "resource_manager.h"
#include "DirectXTK-master/Inc/WICTextureLoader.h"

using namespace Adollib;

Texture::Texture()
{
	ShaderResourceView = nullptr;
	sampler = nullptr;
}


Texture::~Texture()
{
}

bool Texture::Load(const wchar_t* filename)
{
	HRESULT hr = S_OK;

	hr = ResourceManager::LoadTextureFromFile(filename, ShaderResourceView.GetAddressOf(), &texture2d_desc);

	if(FAILED(hr)){
		assert(!"�e�N�X�`���摜�ǂݍ��ݎ��s");
		return false;
	}

	//	�V�F�[�_�[���\�[�X�r���[�쐬
	ComPtr<ID3D11Resource>resource;
	hr = DirectX::CreateWICTextureFromFile(Systems::Device.Get(),
		filename, resource.GetAddressOf(), ShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return false;
	}

	//�e�N�X�`�����擾
	ComPtr<ID3D11Texture2D> texture2D;
	resource.Get()->QueryInterface(texture2D.GetAddressOf());
	texture2D.Get()->GetDesc(&texture2d_desc);

	//	�T���v���X�e�[�g�쐬
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	hr = Systems::Device->CreateSamplerState(
		&sd, sampler.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}
void Texture::Set(UINT Slot, BOOL flg)
{
	if (flg == FALSE) {
		ID3D11ShaderResourceView* rtv[1] = { NULL };
		ID3D11SamplerState* ss[1] = { NULL };
		Systems::DeviceContext->PSSetShaderResources(Slot, 1, rtv);
		Systems::DeviceContext->PSSetSamplers(Slot, 1, ss);
		Systems::DeviceContext->DSSetShaderResources(Slot, 1, rtv);
		Systems::DeviceContext->DSSetSamplers(Slot, 1, ss);
		return;
	}
	if (ShaderResourceView) {
		Systems::DeviceContext->PSSetShaderResources(Slot, 1, ShaderResourceView.GetAddressOf());
		Systems::DeviceContext->PSSetSamplers(Slot, 1, sampler.GetAddressOf());
		Systems::DeviceContext->DSSetShaderResources(Slot, 1, ShaderResourceView.GetAddressOf());
		Systems::DeviceContext->DSSetSamplers(Slot, 1, sampler.GetAddressOf());
	}
}


bool Texture::Create(u_int width, u_int height, DXGI_FORMAT format)
{
	ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;

	//	�e�N�X�`���쐬
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = Systems::Device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//	�����_�[�^�[�Q�b�g�r���[�쐬
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = Systems::Device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}


	//	�V�F�[�_�[���\�[�X�r���[�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = Systems::Device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}


	//	�T���v���X�e�[�g�쐬
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//�{�[�_�[�J���[
	sd.BorderColor[0] = 1.0f;
	sd.BorderColor[1] = 1.0f;
	sd.BorderColor[2] = 1.0f;
	sd.BorderColor[3] = 1.0f;

	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	hr = Systems::Device->CreateSamplerState(&sd, sampler.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}
