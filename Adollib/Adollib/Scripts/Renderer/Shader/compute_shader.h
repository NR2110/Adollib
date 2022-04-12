#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <assert.h>
#include <string>

namespace Adollib {
	namespace Compute_S {

		typedef ID3D11Buffer StructureBuffer;
		typedef ID3D11Buffer UAVBuffer;

		class ComputeShader {
		private:
			void activate();
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> get_context();
			Microsoft::WRL::ComPtr<ID3D11Device>        get_device();

		public:
			Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader = nullptr;
			ComputeShader() {};

			void Load(std::string csname);

			//::::::::::
			// Compute_shaderをはしらせる
			// pSRVs : 入力
			// pUAV  : 出力
			// numViews : 入力が配列の時要素数を入力
			// x : スレッド数
			// y : スレッド数
			// z : スレッド数
			//::::::::::
			void run(ID3D11ShaderResourceView** pSRVs, UINT numSRV, ID3D11UnorderedAccessView** pUAVs, UINT numUAV, UINT x, UINT y, UINT z);
			void run(UINT x, UINT y, UINT z);

			// UAVを任意のクラスに直す
			template<class T>
			T* Read_UAV(Microsoft::WRL::ComPtr<UAVBuffer>& pBuffer) {

				// バッファを生成とコピー.
				Microsoft::WRL::ComPtr<ID3D11Buffer> pBufDbg = nullptr;
				{
					D3D11_BUFFER_DESC desc;
					memset(&desc, 0, sizeof(desc));

					pBuffer->GetDesc(&desc);
					desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
					desc.Usage = D3D11_USAGE_STAGING;
					desc.BindFlags = 0;
					desc.MiscFlags = 0;

					if (SUCCEEDED(get_device()->CreateBuffer(&desc, nullptr, pBufDbg.GetAddressOf())))
					{
						get_context()->CopyResource(pBufDbg.Get(), pBuffer.Get());
					}
				}

				D3D11_MAPPED_SUBRESOURCE subRes;
				get_context()->Map(pBufDbg.Get(), 0, D3D11_MAP_READ, 0, &subRes);

				get_context()->Unmap(pBufDbg.Get(), 0);

				return static_cast<T*>(subRes.pData);
			}

		};

		class ComputeShader_function {
		public:
			// StructureBufferの作成
			static HRESULT create_StructureBuffer(UINT elementSize, UINT count, void* InitData, Microsoft::WRL::ComPtr<StructureBuffer>& ppBufferOut, bool is_upu_access);

			// StructureBufferからSRVの作成
			static HRESULT createSRV_fromSB(Microsoft::WRL::ComPtr<StructureBuffer>& pBuffer, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& ppSRVOut);

			// StructureBufferからUAVの作成
			static HRESULT createUAV_fromSB(Microsoft::WRL::ComPtr<StructureBuffer>& pBuffer, Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& ppUAVOut);
		};
	}
}