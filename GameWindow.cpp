#include <algorithm>
#include <Windows.h>
#include <SDKDDKVer.h>
#include "GameWindow.h"
#define WIND32_LEAN_AND_MEAN
#include <DirectXMath.h>
#include "keyboard.h"
#include "mouse.h"
using namespace DirectX;
//Window

static constexpr char WINDOW_CLASS[] = "GameWindow";
static constexpr char TITLE[] = "Window表示";
static XMFLOAT2 now_vector = { 0,0 };
static button_input_info input_info;
static bool now_face_right = true;

//Window プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND GameWindow_Create(HINSTANCE hInstance)
{
	//Windowsの登録
	WNDCLASSEX wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc; //重要
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	RegisterClassEx(&wcex);



	//Main Window creation

	const constexpr int SCREEN_WIDTH = 1600;
	const constexpr int SCREEN_HEIGHT = 900;

	RECT window_rect{ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	DWORD style = WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME;
	AdjustWindowRect(&window_rect, style, FALSE);

	const int WINDOW_WIDTH = window_rect.right - window_rect.left;
	const int WINDOW_HEIGHT = window_rect.bottom - window_rect.top;

	//Get Desktop Size
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	const int WINDOW_X = std::max((desktop_width - WINDOW_WIDTH) / 2, 0);
	const int WINDOW_Y = std::max((desktop_height - WINDOW_HEIGHT) / 2, 0);


	HWND hWnd = CreateWindow(
		WINDOW_CLASS,
		TITLE,
		style,
		WINDOW_X,
		WINDOW_Y,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		nullptr, nullptr, hInstance, nullptr);
	return hWnd;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {

	case WM_ACTIVATEAPP:
		Keyboard_ProcessMessage(message, wParam, lParam);
		Mouse_ProcessMessage(message, wParam, lParam);
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
		Mouse_ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:

		if (wParam == VK_ESCAPE) {
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard_ProcessMessage(message, wParam, lParam);
		break;
	case WM_CLOSE:
		if (MessageBox(hWnd, "Are you sure you want to close?", "Warning", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			DestroyWindow(hWnd);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void change_vector(XMFLOAT2& target, int x, int y)
{
	target.x += x;
	target.y += y;
}
void change_vector_to(XMFLOAT2& target, int x, int y)
{
	target.x = x;
	target.y = y;
}

XMFLOAT2 return_vector()
{
	return now_vector;
}
void check_face(bool input_right)
{
	if (input_right)
	{
		if (now_face_right != TRUE)
		{
			now_face_right = TRUE;
		}
	}
	else
	{
		if (now_face_right == TRUE)
		{
			now_face_right = FALSE;
		}
	}
}
bool return_face()
{
	return now_face_right;
}
