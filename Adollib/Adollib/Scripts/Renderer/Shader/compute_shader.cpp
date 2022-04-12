#include "compute_shader.h"
#include "../../Main/resource_manager.h"
#include "../../Main/systems.h"

using namespace Adollib;
using namespace Compute_S;

#pragma region  ComputeShader

Microsoft::WRL::ComPtr<ID3D11DeviceContext> ComputeShader::get_context() {
	return Systems::DeviceContext;
}
Microsoft::WRL::ComPtr<ID3D11Device> ComputeShader::get_device() {
	return Systems::Device;
}

void ComputeShader::activate() {
	Systems::DeviceContext->CSSetShader(computeShader.Get(), NULL, 0);
}

void ComputeShader::Load(std::string csname) {
	assert(SUCCEEDED(ResourceManager::CreateCsFromCso(csname.c_str(), computeShader)));
}


void ComputeShader::run(
	ID3D11ShaderResourceView** pSRVs, UINT numSRV,
	ID3D11UnorderedAccessView** pUAVs, UINT numUAV,
	UINT x , UINT y, UINT z
) {
	Systems::DeviceContext->CSSetShader(computeShader.Get(), nullptr, 0);
	Systems::DeviceContext->CSSetShaderResources(0, numSRV, pSRVs);
	Systems::DeviceContext->CSSetUnorderedAccessViews(0, numUAV, pUAVs, nullptr);

	//Systems::DeviceContext->CSSetShaderResources(0, 1, pSRVs[0].GetAddressOf());
	//Systems::DeviceContext->CSSetShaderResources(1, 1, pSRVs[1].GetAddressOf());
	//Systems::DeviceContext->CSSetShaderResources(2, 1, pSRVs[2].GetAddressOf());

	Systems::DeviceContext->Dispatch(x, y, z);


	ID3D11UnorderedAccessView* pNullUAVs[4] = { nullptr, nullptr, nullptr, nullptr };
	ID3D11ShaderResourceView* pNullSRVs[4] = { nullptr, nullptr,nullptr,  nullptr };
	ID3D11Buffer* pNullCBs[1] = { nullptr };

	Systems::DeviceContext->CSSetShader(nullptr, nullptr, 0);
	Systems::DeviceContext->CSSetShaderResources(0, 4, pNullSRVs);
	Systems::DeviceContext->CSSetUnorderedAccessViews(0, 4, pNullUAVs, nullptr);

}

void ComputeShader::run(
	UINT x, UINT y, UINT z
) {
	Systems::DeviceContext->Dispatch(x, y, z);
}


#pragma endregion


HRESULT ComputeShader_function::create_StructureBuffer(UINT elementSize, UINT count, void* pInitData, Microsoft::WRL::ComPtr<StructureBuffer>& ppSBufferOut, bool is_upu_access) {
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = elementSize * count;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = elementSize;

	if (is_upu_access) {
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// リソースに許可されるCPUアクセスの指定
	}

	if (pInitData) {
		D3D11_SUBRESOURCE_DATA init_data;
		init_data.pSysMem = pInitData;

		return Systems::Device->CreateBuffer(&desc, &init_data, ppSBufferOut.ReleaseAndGetAddressOf());
	}

	return Systems::Device->CreateBuffer(&desc, nullptr, ppSBufferOut.ReleaseAndGetAddressOf());
}

// StructureBufferからSRVの作成
HRESULT ComputeShader_function::createSRV_fromSB(Microsoft::WRL::ComPtr<StructureBuffer>& pSBuffer, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& ppSRVOut) {

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	pSBuffer->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;


	if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		srvDesc.BufferEx.NumElements = desc.ByteWidth / 4;
	}
	else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.BufferEx.NumElements = desc.ByteWidth / desc.StructureByteStride;
	}
	else
	{
		return E_INVALIDARG;
	}

	return Systems::Device->CreateShaderResourceView(pSBuffer.Get(), &srvDesc, ppSRVOut.ReleaseAndGetAddressOf());
}

// StructureBufferからUAVの作成
HRESULT ComputeShader_function::createUAV_fromSB(Microsoft::WRL::ComPtr<StructureBuffer>& pSBuffer, Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& ppUAVOut) {
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	pSBuffer->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	memset(&uavDesc, 0, sizeof(uavDesc));

	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;

	if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		uavDesc.Buffer.NumElements = desc.ByteWidth / 4;
	}
	else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
	}
	else
	{
		return E_INVALIDARG;
	}

	return Systems::Device->CreateUnorderedAccessView(pSBuffer.Get(), &uavDesc, ppUAVOut.ReleaseAndGetAddressOf());
}
