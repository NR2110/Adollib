#pragma once

#include <vector>
#include <string>
#include "renderer_base.h"
#include "mesh.h"

namespace Adollib {

	class Croth_renderer : public Renderer_base {
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;

		void instance_update(const Frustum_data& frustum_data);

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