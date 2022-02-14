#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "Shader/shader.h"


namespace Adollib {
	class Texture;

	class UI_data {
	public:
		UI_data() {};
		UI_data(float dx_, float dy_, float dw_, float dh_, float sx_, float sy_, float sw_, float sh_, float angle_ = 0, float alpha_ = 1) :
			dx(dx_), dy(dy_), dw(dw_), dh(dh_), sx(sx_), sy(sy_), sw(sw_), sh(sh_), angle(angle_), alpha(alpha_)
		{};

	public:
		float dx = 0; //頂点座標
		float dy = 0; //頂点座標
		float dw = 0; //頂点座標
		float dh = 0; //頂点座標
		float sx = 0; //texture座標
		float sy = 0; //texture座標
		float sw = 0; //texture座標
		float sh = 0; //texture座標

		float angle = 0.0f;
		float alpha = 1.0f;
	};

	class UI {

	private:
		ComPtr<ID3D11Buffer> VertexBuffer;
		ComPtr<ID3D11DepthStencilState> DepthStencilState;
		std::shared_ptr<Texture> texture = nullptr;

	public:
		UI_data ui_data;

		Shader shader;

	public:

		UI();

		void set_texture(std::shared_ptr<Texture>& texture);

		void set_default();

		void render();

		//void Set_texture(u_int slot) { texture.Set(slot); }

	};



}