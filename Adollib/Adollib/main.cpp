#define _CRTDBG_MAP_ALLOC  

#include <stdlib.h>  
#include <crtdbg.h> 

#include <Windows.h>
#include <tchar.h>

#include "systems.h"
#include "Adollib.h"
#include "imgui_manager.h"
#include "loop.h"

#include <math.h>


#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx11.h"

using namespace Adollib;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return 1;
	}

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
		DirectX::Mouse::ProcessMessage(msg, wparam, lparam);
		break;
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		}
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
		break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(msg, wparam, lparam);
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		Systems::time->stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		Systems::time->start();
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
	//window
	TCHAR szWindowClass[] = TEXT("Adollib_sample");
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = fnWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = 0;
	RegisterClassEx(&wcex);

	RECT rc = { 0, 0, Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hwnd = 
		CreateWindow(
			szWindowClass,
			szWindowClass,
			WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, 
			CW_USEDEFAULT, 
			CW_USEDEFAULT,
			rc.right - rc.left, 
			rc.bottom - rc.top,
			NULL, NULL, 
			instance, 
			NULL
		);
	ShowWindow(hwnd, cmd_show);

	if (FAILED(Systems::Initialize(hwnd, Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT))){

		return 0;
	}

	//メインループ
	MSG hMsg = { 0 };
	float Interval = 1.0f;
	LARGE_INTEGER now, before, freq;
	QueryPerformanceCounter(&before);
	int fps = 0;

	loop loop;

	loop.init(hwnd);

	while (hMsg.message != WM_QUIT) {
		if (PeekMessage(&hMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}
		else
		{
			if (Adollib::Imgui_manager::update(hMsg) == false) continue;

			/*LONGLONG A = GetTickCount64();
			Al_Global::second_per_frame =
				(A - before) * 0.001f;

			if (Al_Global::second_per_frame == 0) {
				int adfsgdhfgj = 0;
			}
			before = GetTickCount64();*/

			QueryPerformanceCounter(&now);
			QueryPerformanceFrequency(&freq);
			Al_Global::second_per_frame = (float)((now.QuadPart - before.QuadPart ) * (double)1000 / freq.QuadPart) * 0.001f;

			QueryPerformanceCounter(&before);

			Al_Global::second_per_game += Al_Global::second_per_frame;
			float mspf = 1000.0f / fps;

			Interval -= Al_Global::second_per_frame;
			fps++;
			if (Interval < 0) {
				std::ostringstream outs;
				outs.precision(6);
				outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
				SetWindowTextA(hwnd, outs.str().c_str());
				Interval += 1.0f;
				fps = 0;

			}

			// 更新・描画
			if (loop.Update(hMsg, hwnd, Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT) == false)continue;

#pragma region Imgui_demo

			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
			{
				//static float f = 0.0f;
				//static int counter = 0;

				//static bool flags[12] = {false};
				//ImGuiWindowFlags flag = 0;
				//if(flags[0]) flag |=  ImGuiWindowFlags_NoTitleBar;			// タイトルバーを非表示にします。
				//if(flags[1]) flag |=  ImGuiWindowFlags_NoResize;				// ウィンドウをリサイズ不可にします。
				//if(flags[2]) flag |=  ImGuiWindowFlags_NoMove;				// ウィンドウを移動不可にします。
				//if(flags[3]) flag |=  ImGuiWindowFlags_NoScrollbar;			// スクロールバーを無効にします。
				//if(flags[4]) flag |=  ImGuiWindowFlags_NoScrollWithMouse;	// マウスホイールでのスクロール操作を無効にしま
				//if(flags[5]) flag |=  ImGuiWindowFlags_NoCollapse;			// タイトルバーをダブルクリックして閉じる挙動を無効に
				//if(flags[6]) flag |=  ImGuiWindowFlags_NoBackground;			// ウィンドウ内の背景を非表示にします。
				//if(flags[7]) flag |=  ImGuiWindowFlags_NoBringToFrontOnFocus;// ウィンドウをクリックしてフォーカスした際
				//if(flags[8]) flag |=  ImGuiWindowFlags_NoNav;				// ゲームパッドやキーボードでのUIの操作を無効にします。
				//if(flags[9]) flag |=  ImGuiWindowFlags_NoSavedSettings;		// imgui.iniでウィンドウの位置などを自動保存/ロー
				//if(flags[10])flag |= ImGuiWindowFlags_AlwaysAutoResize;		// 自動でウィンドウ内のコンテンツに合わせてリサ
				//if(flags[11])flag |= ImGuiWindowFlags_NoFocusOnAppearing;	// 表示/非表示の際のトランジションアニメーショ

				//ImGui::Begin("window setting");                          // Create a window called "Hello, world!" and append into it.

				//ImGui::Text("demo");     
				//static bool is_open = true;
				//ImGui::Checkbox("that box", &is_open);

				//ImGui::Checkbox("Imis_openFlags_NoTitleBar;			", &flags[0]);      // Edit bools storing our window open/close state
				//ImGui::Checkbox("ImGuiWindowFlags_NoResize;				", &flags[1]);      // Edit bools storing our window open/close state
				//ImGui::Checkbox("ImGuiWindowFlags_NoMove;				", &flags[2]);      // Edit bools storing our window open/close state
				//ImGui::Checkbox("ImGuiWindowFlags_NoScrollbar;			", &flags[3]);      // Edit bools storing our window open/close state
				//ImGui::Checkbox("ImGuiWindowFlags_NoScrollWithMouse;	", &flags[4]);      // Edit bools storing our window open/close state
				//ImGui::Checkbox("ImGuiWindowFlags_NoCollapse;			", &flags[5]);      // Edit bools storing our window open/close state
				//ImGui::Checkbox("ImGuiWindowFlags_NoBackground;			", &flags[6]);      // Edit bools storing our window open/close state
				//ImGui::Checkbox("ImGuiWindowFlags_NoBringToFrontOnFocus;", &flags[7]);      // Edit bools storing our window open/close state
				//ImGui::Checkbox("ImGuiWindowFlags_NoNav;				", &flags[8]);      // Edit bools storing our window open/close state
				//ImGui::Checkbox("ImGuiWindowFlags_NoSavedSettings;		", &flags[9]);      // Edit bools storing our window open/close state
				//ImGui::Checkbox("ImGuiWindowFlags_AlwaysAutoResize;		", &flags[10]);      // Edit bools storing our window open/close state
				//ImGui::Checkbox("ImGuiWindowFlags_NoFocusOnAppearing;	", &flags[11]);      // Edit bools storing our window open/close state

				//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				//ImGui::End();

				//if (is_open) {
				//	ImGui::Begin("Another Window", &is_open, flag);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

				//	static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
				//	// 最小値 0, 最大値 1 のグラフを作成します。
				//	// ImVec2(0,50)としていることで、幅は自動で決まり、高さは50になります。
				//		ImGui::PlotHistogram("ヒストグラム", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 100));
				//			static float progress = 0.22f;
				//	// 0 ~ 1 までの中での 0.22 が何％の位置にいるのかを表示します。
				//	ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));


				//	ImGui::End();
				//}
			}


#pragma endregion
			loop.Render();

		}
	}

	loop.destroy();
	Systems::Release();

	//memory leak 221 ～ 302 は何をしても出るため無視
	_CrtDumpMemoryLeaks();

	return 0;
}