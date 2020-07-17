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
			Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> get_context();
			Microsoft::WRL::ComPtr<ID3D11Device>        get_device();

		public:
			ComputeShader() {};

			void Load(std::string csname);

			//::::::::::
			// Compute_shader���͂��点��
			// pSRVs : ����
			// pUAV  : �o��
			// numViews : ���͂��z��̎��v�f�������
			// x : �X���b�h��
			// y : �X���b�h��
			// z : �X���b�h��
			//::::::::::
			void run(ID3D11ShaderResourceView** pSRVs, UINT numViews, ID3D11UnorderedAccessView* pUAV, UINT x = 32, UINT y = 1, UINT z = 1);

			// StructureBuffer�̍쐬
			HRESULT create_StructureBuffer(UINT elementSize, UINT count, void* InitData, Microsoft::WRL::ComPtr<StructureBuffer>& ppBufferOut);

			// StructureBuffer����SRV�̍쐬
			HRESULT createSRV_fromSB(Microsoft::WRL::ComPtr<StructureBuffer> pBuffer, ID3D11ShaderResourceView** ppSRVOut);

			// StructureBuffer����UAV�̍쐬
			HRESULT createUAV_fromSB(Microsoft::WRL::ComPtr<StructureBuffer> pBuffer, ID3D11UnorderedAccessView** ppUAVOut);

			// UAV��C�ӂ̃N���X�ɒ���
			template<class T>
			T* Read_UAV(Microsoft::WRL::ComPtr<UAVBuffer> pBuffer) {
				HRESULT hr;
				ID3D11Buffer* pCloneBuf = nullptr;

				D3D11_BUFFER_DESC desc;
				memset(&desc, 0, sizeof(desc));
				pBuffer->GetDesc(&desc);
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				desc.Usage = D3D11_USAGE_STAGING;
				desc.BindFlags = 0;
				desc.MiscFlags = 0;

				if (SUCCEEDED(get_device()->CreateBuffer(&desc, nullptr, &pCloneBuf)))
				{
					get_context()->CopyResource(pCloneBuf, pBuffer.Get());
				}

				D3D11_MAPPED_SUBRESOURCE subRes;
				hr = get_context()->Map(pCloneBuf, 0, D3D11_MAP_READ, 0, &subRes);
				assert(!FAILED(hr));
				get_context().Get()->Unmap(pCloneBuf, 0);

				return static_cast<T*>(subRes.pData);
			}

		};

		
	}
}