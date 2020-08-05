#include "imgui_manager.h"

#include "systems.h"

#include "imgui_all.h"

#include "work_meter.h"

using namespace Adollib;

bool Imgui_manager::init(HWND hwnd) {

	// https://qiita.com/mizuma/items/73218dab2f6b022b0227#%E3%83%9C%E3%82%BF%E3%83%B3%E3%81%A8%E3%83%81%E3%82%A7%E3%83%83%E3%82%AF%E3%83%9C%E3%83%83%E3%82%AF%E3%82%B9
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(Systems::Device.Get(), Systems::DeviceContext.Get());

	//ImFont* font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/meiryob.ttc", 21.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//ImFont* font = io.Fonts->AddFontDefault();
	//ImFont* font = io.Fonts->AddFontFromFileTTF("Imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("Imgui/misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("Imgui/misc/fonts/DroidSans.ttf", 16.0f);
	ImFont* font = io.Fonts->AddFontFromFileTTF("Imgui/misc/fonts/ProggyTiny.ttf", 10.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != NULL);

	return true;
}

bool Imgui_manager::update(MSG hMsg) {
	// ImGui
	if (::PeekMessage(&hMsg, NULL, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&hMsg);
		::DispatchMessage(&hMsg);
		return false;
	}
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return true;
}

bool Imgui_manager::render() {
	Work_meter::render();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return true;
}

bool Imgui_manager::destroy() {

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return true;
}
