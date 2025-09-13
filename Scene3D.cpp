#include "Scene3D.h"
#include <Windows.h>
#include "system_timer.h"
#include "Key_Logger.h"
#include "mouse.h"
#include"shader.h"
#include "Shader3D.h"
#include "Cube.h"
#include "Grid.h"
#include "sprite.h"
#include "polygon.h"
#include "Texture.h"
#include "Fade.h"
#include "MouseRenderer.h"
#include "Camera3D.h"
#include "Audio.h"
static Scene3D g_SceneEnum = Scene3D::SCENE_INIT;
static Scene3D g_SceneNextEnum = Scene3D::SCENE_INIT;
void Scene3D_Initialize(HWND& hWnd)
{
	switch (g_SceneEnum)
	{
	case Scene3D::SCENE_INIT:
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
		Camera3D_Initialize();
		break;
	default:
		break;
	}
}

void Scene3D_Finalize()
{
	Texture_Finalize();
	Sprite_Finalize();
	Cube_Finitialize();
	Polygon_Finalize();
	Mouse_Finalize();
	Shader_Finalize();
	Direct3D_Finalize();
	UninitAudio();
	SpriteAnim_Finitialize();
}

void Scene3D_Update(double time)
{
	MouseRenderer_Update(time);
	Camera3D_Update(time);
	Cube_Update(time);
}

void Scene3D_Draw()
{
	for (int y = 10; y >= 0; y--)
	{
		for (int z = y; z >= 0; z--)
		{
			for (int x = y; x >= 0; x--)
			{
				Cube_Draw({ -5.0f + x * 1.0f + 0.5f * (10.0f - y) ,(10.0f - y) * 1.0f + 0.5f,5.0f - z * 1.0f - 0.5f * (10.0f - y) });
			}
		}

	}
	Grid_Draw();
	MouseRenderer_Draw();
}

void Scene3D_Refresh(HWND& hWnd)
{
	if (g_SceneEnum != g_SceneNextEnum)
	{
		Scene3D_Finalize();
		g_SceneEnum = g_SceneNextEnum;
		Scene3D_Initialize(hWnd);
	}
}

void Scene3D_Change(Scene3D scene)
{
	g_SceneNextEnum = scene;
}

Scene3D GetScene3D()
{
	return g_SceneEnum;
}
