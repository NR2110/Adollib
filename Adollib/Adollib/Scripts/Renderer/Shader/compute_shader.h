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
			// Compute_shader���͂��点��
			// pSRVs : ����
			// pUAV  : �o��
			// numViews : ���͂��z��̎��v�f�������
			// x : �X���b�h��
			// y : �X���b�h��
			// z : �X���b�h��
			//::::::::::
			void run(ID3D11ShaderResourceView** pSRVs, UINT numViews, Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pUAV, UINT x = 32, UINT y = 1, UINT z = 1);

			// StructureBuffer�̍쐬
			HRESULT create_StructureBuffer(UINT elementSize, UINT count, void* InitData, Microsoft::WRL::ComPtr<StructureBuffer>& ppBufferOut);

			// StructureBuffer����SRV�̍쐬
			HRESULT createSRV_fromSB(Microsoft::WRL::ComPtr<StructureBuffer> pBuffer, ID3D11ShaderResourceView** ppSRVOut);

			// StructureBuffer����UAV�̍쐬
			HRESULT createUAV_fromSB(Microsoft::WRL::ComPtr<StructureBuffer> pBuffer, ID3D11UnorderedAccessView** ppUAVOut);

			// UAV��C�ӂ̃N���X�ɒ���
			template<class T>
			T* Read_UAV(Microsoft::WRL::ComPtr<UAVBuffer> pBuffer) {

				// �o�b�t�@�𐶐��ƃR�s�[.
				ID3D11Buffer* pBufDbg = nullptr;
				{
					D3D11_BUFFER_DESC desc;
					memset(&desc, 0, sizeof(desc));

					pBuffer->GetDesc(&desc);
					desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
					desc.Usage = D3D11_USAGE_STAGING;
					desc.BindFlags = 0;
					desc.MiscFlags = 0;

					if (SUCCEEDED(get_device()->CreateBuffer(&desc, nullptr, &pBufDbg)))
					{
						get_context()->CopyResource(pBufDbg, pBuffer.Get());
					}
				}

				D3D11_MAPPED_SUBRESOURCE subRes;
				get_context()->Map(pBufDbg, 0, D3D11_MAP_READ, 0, &subRes);
				get_context()->Unmap(pBufDbg, 0);

				return static_cast<T*>(subRes.pData);

				/*

				bool isSuccess = true;

				for (int i = 0; i < NUM_ELEMENTS; ++i)
				{
					// CPU�ŉ��Z.
					int   value0 = g_Buf0[i].s32 + g_Buf1[i].s32;
					float value1 = g_Buf0[i].f32 * g_Buf1[i].f32;

					// GPU�̉��Z���ʂ�CPU�̉��Z���ʂ��r.
					if ((pBufType[i].s32 != value0)
						|| (pBufType[i].f32 != value1))
					{
						ILOG("Failure.");
						ILOG("  index = %d", i);
						ILOG("  cpu value0 = %d, value1 = %f", value0, value1);
						ILOG("  gpu value0 = %d, value1 = %f", pBufType[i].s32, pBufType[i].f32);
						isSuccess = false;
						break;
					}
				}

				// CPU��GPU�̌��ʂ����ׂĈ�v�����琬���̃��O���o��.
				if (isSuccess)
				{
					ILOG("Succeded!!");
				}

				// �A���}�b�v.
				g_pContext->Unmap(pBufDbg, 0);

				// �������.
				ASDX_RELEASE(pBufDbg);

				/*HRESULT hr;
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
				if (pCloneBuf == nullptr) {
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
				}

				D3D11_MAPPED_SUBRESOURCE subRes;
				hr = get_context()->Map(pCloneBuf, 0, D3D11_MAP_READ, 0, &subRes);
				assert(!FAILED(hr) && "convert UAV to templete<T> is failed ");
				get_context().Get()->Unmap(pCloneBuf, 0);

				return static_cast<T*>(subRes.pData);*/
			}

		};


	}
}