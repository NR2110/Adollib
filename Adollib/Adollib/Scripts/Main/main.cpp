#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#include <Windows.h>
#include <tchar.h>

#include "systems.h"
#include "Adollib.h"
#include "../Imgui/imgui_manager.h"
#include "loop.h"

#include <math.h>


#include "../../Imgui/imgui.h"
#include "../../Imgui/imgui_impl_win32.h"
#include "../../Imgui/imgui_impl_dx11.h"

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

	if (Systems::Initialize(hwnd, Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT) == true){

		return 0;
	}

	//メインループ
	MSG hMsg = { 0 };
	float Interval = 1.0f;
	int fps = 0;

	loop loop;

	loop.init(hwnd);
	Systems::time->reset();

	while (hMsg.message != WM_QUIT) {
		if (PeekMessage(&hMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}
		else
		{
			if (Adollib::Imgui_manager::update(hMsg) == false) continue;

			Systems::time->tick();

			Al_Global::second_per_frame = Systems::time->deltaTime();
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
			loop.Render();
#pragma region Imgui_demo


			//ImGui::Begin("main");


			//ImGui::End();



#pragma endregion


		}
	}

	loop.destroy();
	Systems::Release();

	//memory leak 221 ～ 302 は何をしても出るため無視
	//_CrtDumpMemoryLeaks();

	return 0;
}