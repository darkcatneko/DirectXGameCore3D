#include <Windows.h>
#include <SDKDDKVer.h>
#include "GameWindow.h" 
#define WIND32_LEAN_AND_MEAN
#include "direct3d.h"
#include "sprite.h"
#include"shader.h"
#include "Texture.h"
#include "vector"
#include "debug_text.h"
#include <sstream>
#include "system_timer.h"
#include "sprite_anim.h"
#include "polygon.h"
#include "keyboard.h"
#include "mouse.h"
#include "Key_Logger.h"
#include "Camera.h"
#include "Audio.h"
#include "Fade.h"
#include "Scene3D.h"
#include "Collision.h"
#include "MouseRenderer.h"
#include "UIInteraction.h"
#include "BattleBossController.h"
#include "GameSetting.h"
#include "Shader3D.h"
#include "Cube.h"
#include "Grid.h"
using namespace DirectX;

XMFLOAT2 scale;
//Main
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	(void)CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	HWND hWnd = GameWindow_Create(hInstance);
	Direct3D_Initialize(hWnd);

	SystemTimer_Initialize();
	KeyLogger_Initialize();
	Mouse_Initialize(hWnd);
	Shader_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Shader3D_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());

	Cube_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Grid_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Polygon_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());

	Texture_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Fade_Initialize();
	MouseRenderer_Initialize();

	hal::DebugText dt(Direct3D_GetDevice(), Direct3D_GetContext(),
		L"consolab_ascii_512.png",
		Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight(),
		0.0f, 0.0f,
		0, 0,
		0.0f, 0.0f);
	//Collision_Debug_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	//UICollision_Debug_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	//GameLanguage_Initialize(GameLanguage::CHINESE);

	//Mouse_SetMode(MOUSE_POSITION_MODE_RELATIVE);//相對座標滑鼠模式
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	Mouse_SetVisible(false);
	double exec_last_time = SystemTimer_GetTime();
	double fps_last_time = exec_last_time;
	double current_time = 0.0f;
	ULONG frame_count = 0;
	double fps = 0.0f;

	float x = 0.0f;

	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				return 0;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			current_time = SystemTimer_GetTime();

			double elapsed_time = current_time - fps_last_time;
			//if (elapsed_time < 1.0f / 60.0)continue;
			if (elapsed_time >= 1.0f)
			{
				fps = frame_count / elapsed_time;
				fps_last_time = current_time;
				frame_count = 0;
			}

			elapsed_time = current_time - exec_last_time;
			//////////////////////   START    //////////////////////

			//////////////////////   UPDATE      //////////////////////

			if (elapsed_time >= 1.0 / 60.0)
			{
				exec_last_time = current_time;

#pragma region 0627Mouse
				KeyLogger_Update();
				MouseLogger_Update();
				Mouse_State mouse_state{};
				Mouse_GetState(&mouse_state);
#pragma endregion

				Scene3D_Update(elapsed_time);
				SpriteAnim_Update(elapsed_time);
				Fade_Update(elapsed_time);
				Direct3D_Clear();
				Sprite_Begin();





				//////////////////////    DRAW      //////////////////////
				Scene3D_Draw();
				Cube_Draw();
				Grid_Draw();
				Fade_Draw();
#if defined(DEBUG)||defined(_DEBUG)
				std::stringstream ss;
				ss << "FPS: " << CameraPos_X << "  " << CameraPos_Y << "MOUSE: " << mouse_state.x << " " << mouse_state.y << " bullet:" << GetBulletCount(0) << "/" << GetBulletCount(1) << "/" << GetBulletCount(2);
				dt.SetText(ss.str().c_str());
				//dt.SetText("YOUHEI", { 0.0f,0.0f,1.0f,1.0f });

				dt.Draw();
				dt.Clear();
#endif // defined(DEBUG)||defined(_DEBUG)
				Direct3D_Present();
				Scene3D_Refresh(hWnd);
				frame_count++;
			}

			/*if (FRAME_TIME % 15 == 0)
			{
				frame++;
			}*/





		}
	}
	Texture_Finalize();
	Sprite_Finalize();
	Cube_Finitialize();
	Polygon_Finalize();
	Mouse_Finalize();
	Shader_Finalize();
	Direct3D_Finalize();
	UninitAudio();
	SpriteAnim_Finitialize();
	Collision_Debug_Finitialize();
	return (int)msg.wParam;
}
//Window Procedure


