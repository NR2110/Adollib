#pragma once

#include <vector>
#include <string>
#include "renderer_base.h"
#include "mesh.h"

namespace Adollib {

	namespace Compute_S { class ComputeShader; }

	// polygon描画用のInstance
	//struct Instance {
	//	DirectX::XMFLOAT4X4 transformMatrix = {};	// world変換やNDC変換を入れる行列
	//	DirectX::XMFLOAT4 texcoordTransform = { 0.0f,0.0f,1.0f,1.0f };
	//	DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
	//};
	//と同じサイズ
	struct Instance_polygon {

		Vector3 position[3] = {};

		Vector3 normal[3] = {};

		Vector2 texcoordTransform = {};

		Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	};
	struct VertexOffset {
		Vector3 position;
		Vector3 normal;
	};

	class Rope_renderer : public Renderer_base {
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;

		// compute_Shader
		std::shared_ptr<Compute_S::ComputeShader> compute_shader = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Buffer> computeBuf_vertexoffset = { nullptr };
		Microsoft::WRL::ComPtr<ID3D11Buffer> computeBuf_color = { nullptr };
		Microsoft::WRL::ComPtr<ID3D11Buffer> computeBuf_result = nullptr;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  computeBufSRV_vertexoffset = { nullptr };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  computeBufSRV_color = { nullptr };
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> computeBufUAV_result = nullptr;

		int instance_count = 0;

		int facet_count = 4;

		std::shared_ptr<std::vector<std::pair<Vector3, Vector3>>> vertex_offset;

	public:
		void load_texture(const wchar_t* filename = nullptr);

		bool check_frustum(const Frustum_data& data) override;

		void set_meshoffset(std::shared_ptr<std::vector<std::pair<Vector3, Vector3>>> off);

	protected:

	public:

		void init() override;

		void render(const Frustum_data& frustum_data) override;

		void render_instancing(Microsoft::WRL::ComPtr<ID3D11Buffer>& instance_buffer, int bufferStart, int bufferCount) override;

	};
}