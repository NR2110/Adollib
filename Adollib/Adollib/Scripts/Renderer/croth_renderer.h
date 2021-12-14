#pragma once

#include <vector>
#include <string>
#include "renderer_base.h"
#include "mesh.h"

namespace Adollib {

	// polygon描画用のInstance
	//struct Instance
	//{
	//	DirectX::XMFLOAT4X4 transformMatrix = {};	// world変換やNDC変換を入れる行列
	//	DirectX::XMFLOAT4 texcoordTransform = { 0.0f,0.0f,1.0f,1.0f };
	//	DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
	//};
	//と同じサイズ
	struct Instance_polygon
	{
		DirectX::XMFLOAT3 position[3] = {};

		DirectX::XMFLOAT3 normal[3] = {};

		DirectX::XMFLOAT2 texcoordTransform = {};

		DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
	};

	class Croth_renderer : public Renderer_base {
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;

		void instance_update(const Frustum_data& frustum_data);

		int instance_count = 0;

	public:
		void load_texture(const wchar_t* filename = nullptr);

		bool check_frustum(const Frustum_data& data) override;

	protected:

	public:

		void init() override;

		void render(const Frustum_data& frustum_data) override;

		void render_instancing(Microsoft::WRL::ComPtr<ID3D11Buffer>& instance_buffer, int bufferStart, int bufferCount) override;

	};
}