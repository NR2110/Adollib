#pragma once

#include <vector>
#include <string>
#include "renderer_base.h"
#include "mesh.h"

namespace Adollib {
	class Gameobject;
	class Material;
	class Texture;

	class UI_renderer : public Renderer_base {
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
		//ComPtr<ID3D11DepthStencilState> DepthStencilState;

	public:
		struct UI_data {
		public:
			//float dx = 0;
			//float dy = 0;
			//float dw = 0;
			//float dh = 0;

			float sx = 0;
			float sy = 0;
			float sw = 1;
			float sh = 1;

			float angle = 0.0f;
			//float alpha = 1.0f;
		};
		UI_data ui_data;

	public:
		void load_texture(const wchar_t* filename = nullptr);

		bool check_frustum(const Frustum_data& data) override { return true; };

	public:

		void init() override;

		void render(const Frustum_data& frustum_data) override;

		void render_instancing(Microsoft::WRL::ComPtr<ID3D11Buffer>& instance_buffer, int bufferStart, int bufferCount) override;

	};
}