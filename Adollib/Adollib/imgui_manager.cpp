#include "imgui_manager.h"

#include "systems.h"

#include "imgui_all.h"

#include "work_meter.h"
#include "debug.h"

using namespace Adollib;

bool Imgui_manager::init(HWND hwnd) {

	// https://qiita.com/mizuma/items/73218dab2f6b022b0227#%E3%83%9C%E3%82%BF%E3%83%B3%E3%81%A8%E3%83%81%E3%82%A7%E3%83%83%E3%82%AF%E3%83%9C%E3%83%83%E3%82%AF%E3%82%B9
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;

#if 1
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI
#endif

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

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

	static bool draw_flags[2] = { true };

	ImGui::Begin("Imgui_managerÅ@"); 

	ImGui::Checkbox("Work_meter", &(draw_flags[0]));
	ImGui::Checkbox("Debug", &(draw_flags[1]));
	
	ImGui::End();

	if(draw_flags[0])
	Work_meter::render();

	if(draw_flags[1])
	Debug::render();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	return true;
}

bool Imgui_manager::destroy() {

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return true;
}
