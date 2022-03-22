
#include "vignette_effect.h"

#include "../Main/systems.h"

#include "texture.h"

using namespace Adollib;

void Vignette_effect::awake() {

	ui.ui_data.dx = 0;
	ui.ui_data.dy = 0;
	ui.ui_data.sx = 0;
	ui.ui_data.sy = 0;
	ui.ui_data.dh = Al_Global::SCREEN_HEIGHT;
	ui.ui_data.dw = Al_Global::SCREEN_WIDTH;
	ui.ui_data.sh = Al_Global::SCREEN_HEIGHT;
	ui.ui_data.sw = Al_Global::SCREEN_WIDTH;

	ui.shader.Load_VS("./DefaultShader/vignette_effect_vs.cso");
	ui.shader.Load_PS("./DefaultShader/vignette_effect_ps.cso");

}

void Vignette_effect::render(std::shared_ptr<Texture> output_tex, std::shared_ptr<Texture> color_tex, std::shared_ptr<Texture> normal_tex, std::shared_ptr<Texture> depth_tex, const UI_data& ui_data) {

	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11RenderTargetView* rtv = output_tex->GetRenderTargetView();
	ID3D11DepthStencilView* dsv = output_tex->GetDepthStencilView();
	Systems::DeviceContext->OMSetRenderTargets(1, &rtv, dsv);

	Systems::SetBlendState(State_manager::BStypes::BS_NONE);
	Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_NONE);
	Systems::SetDephtStencilState(State_manager::DStypes::DS_FALSE);


	//ui.ui_data = ui_data;
	ui.set_texture(color_tex);
	ui.render();

}