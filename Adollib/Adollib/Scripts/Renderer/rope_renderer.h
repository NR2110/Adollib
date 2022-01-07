#pragma once

#include <vector>
#include <string>
#include "renderer_base.h"
#include "mesh.h"

namespace Adollib {

	class Rope_renderer : public Renderer_base {
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Buffer> cb_per_rope = nullptr;

		std::shared_ptr<std::vector<std::pair<Vector3, Vector3>>> vertex_offset;

	private:
		// constantbuffer (b9)
		static const int MAX_JOINTS = 200;
		struct ConstantBufferPerRope
		{
			Vector4 joint_position[MAX_JOINTS];
			Matrix44 joint_rotate[MAX_JOINTS];
		};

	public:
		// ï™äÑêî
		int split_count = 4;

		// îºåa
		float radius = 1;

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