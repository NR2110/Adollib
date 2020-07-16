#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <assert.h>
#include <string>

namespace Adollib {
	namespace Compute_S {

		class ComputeShader {
		private:
			void activate();
			std::string cso_name;
			Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader = nullptr;

		public:
			ComputeShader(std::string csname) : cso_name(csname) {};
			void compile();
			void run(ID3D11ShaderResourceView** pSRVs, ID3D11UnorderedAccessView* pUAV, UINT numViews = 1, UINT x = 32, UINT y = 1, UINT z = 1);
		};



		typedef ID3D11Buffer StructureBuffer;
		typedef ID3D11Buffer UAVBuffer;

		// StructureBuffer�̍쐬
		bool create_StructureBuffer(UINT elementSize, UINT count, void* InitData, Microsoft::WRL::ComPtr<StructureBuffer> ppBufferOut);

		// StructureBuffer����SRV�̍쐬
		bool createSRV_fromSB(Microsoft::WRL::ComPtr<StructureBuffer> pBuffer, ID3D11ShaderResourceView** ppSRVOut);

		// StructureBuffer����UAV�̍쐬
		bool createUAV_fromSB(Microsoft::WRL::ComPtr<StructureBuffer> pBuffer, ID3D11UnorderedAccessView** ppUAVOut);

		//::::::::::
		// Compute_shader���͂��点��
		// pSRVs : ����
		// pUAV  : �o��
		// numViews : ���͂��z��̎��v�f�������
		// x : �X���b�h��
		// y : �X���b�h��
		// z : �X���b�h��
		//::::::::::
		void run_compute(ID3D11ShaderResourceView** pSRVs, ID3D11UnorderedAccessView* pUAV, UINT numViews = 1, UINT x = 32, UINT y = 1, UINT z = 1);

		// UAV��C�ӂ̃N���X�ɒ���
		template<class T>
		T* Read_UAV(UAVBuffer* pBuffer) {
			HRESULT hr;
			ID3D11Buffer* pCloneBuf = nullptr;

			D3D11_BUFFER_DESC desc;
			memset(&desc, 0, sizeof(desc));

			pBuffer->GetDesc(&desc);
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.Usage = D3D11_USAGE_STAGING;
			desc.BindFlags = 0;
			desc.MiscFlags = 0;

			if (SUCCEEDED(pDevice->CreateBuffer(&desc, nullptr, &pCloneBuf)))
			{
				pContext->CopyResource(pCloneBuf, pBuffer);
			}

			D3D11_MAPPED_SUBRESOURCE subRes;
			hr = g_pContext->Map(pBufDbg, 0, D3D11_MAP_READ, 0, &subRes);

			assert(!FAILED(hr));

			return static_cast<T*>(subRes.pData);
		}


	}
}