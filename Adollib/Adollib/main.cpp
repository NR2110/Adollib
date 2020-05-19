#include <Windows.h>
#include <tchar.h>

#include "systems.h"
#include "loop.h"

#include <math.h>

using namespace Adollib;

LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
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
	case WM_ACTIVATEAPP:
		//	DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
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
		//	DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
		break;
	case WM_ENTERSIZEMOVE:
		break;
	case WM_EXITSIZEMOVE:
		break;
	case WM_SETCURSOR:
		ShowCursor(FALSE);
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
	//window
	TCHAR szWindowClass[] = TEXT("adollib_sample");
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

	RECT rc = { 0, 0, systems::SCREEN_WIDTH, systems::SCREEN_HEIGHT };
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

	if (FAILED(systems::Initialize(hwnd, systems::SCREEN_WIDTH, systems::SCREEN_HEIGHT))){

		return 0;
	}

	//メインループ
	MSG hMsg = { 0 };
	float Interval = 1.0f;
	DWORD before = GetTickCount64();
	int fps = 0;

	loop loop;

	loop.init();

	while (hMsg.message != WM_QUIT) {
		if (PeekMessage(&hMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}
		else
		{
			systems::elapsed_time =
				(GetTickCount64() - before) * 0.001f;

			before = GetTickCount64();
			float mspf = 1000.0f / fps;

			Interval -= systems::elapsed_time;
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
			loop.Update(hwnd, systems::SCREEN_WIDTH, systems::SCREEN_HEIGHT);
			loop.Render();
		}
	}
	//SAFE_DELETE(scenemanager);
	systems::Release();
	return 0;
}