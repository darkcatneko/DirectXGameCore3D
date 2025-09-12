#include "Scene.h"
#include "Game.h"
#include "Title.h"
#include "MouseRenderer.h"
#include "Fade.h"
#include "SecondScene.h"
#include "BattleSystem.h"
#include <Windows.h>
#include "Camera.h"


static Scene g_Scene;
static Scene g_SceneNext;
void Scene_Initialize(HWND& hWnd)
{
	switch (g_Scene)
	{
	case SCENE_TITLE:
		Title_Initialize(hWnd);
		break;
	case SCENE_GAME:
		Game_Initialize();
		break;
	case SCENE_SECOND:
		SecondScene_Initialize();
		break;
	case SCENE_BATTLE:
		BattleSystem_Initialize();
		break;
	case SCENE_RESULT:
		break;
	}

}

void Scene_Finalize()
{
	switch (g_Scene)
	{
	case SCENE_TITLE:
		Title_Finalize();
		break;
	case SCENE_GAME:
		Game_Finitialize();
		break;
	case SCENE_SECOND:
		SecondScene_Finitialize();
		break;
	case SCENE_BATTLE:
		BattleSystem_Finitialize();
		break;
	case SCENE_RESULT:
		break;
	}
}

void Scene_Update(double time)
{
	MouseRenderer_Update(time);
	CameraUpdate(time);
	switch (g_Scene)
	{
	case SCENE_TITLE:
		Title_Update(time);
		break;
	case SCENE_GAME:
		GameUpdate(time);
		if (Fade_GetState() == FADE_STATE_FINISHED_OUT)
		{
			Scene_Change(SCENE_SECOND);
		}
		break;
	case SCENE_SECOND:
		GameUpdate(time);
		SecondSceneUpdate(time);
		break;
	case SCENE_BATTLE:
		BattleSystemUpdate(time);
		break;
	case SCENE_RESULT:
		break;
	}
}

void Scene_Draw()
{
	switch (g_Scene)
	{
	case SCENE_TITLE:
		Title_Draw();
		break;
	case SCENE_GAME:
		GameDraw();
		break;
	case SCENE_SECOND:
		GameDraw();
		SecondSceneDraw();
		break;
	case SCENE_BATTLE:
		BattleSystemDraw();
		break;
	case SCENE_RESULT:
		break;
	}
	MouseRenderer_Draw();
}

void Scene_Refresh(HWND& hWnd)
{
	if (g_Scene != g_SceneNext)
	{
		Scene_Finalize();
		g_Scene = g_SceneNext;
		Scene_Initialize(hWnd);
	}
}

void Scene_Change(Scene scene)
{
	g_SceneNext = scene;
}

Scene GetScene()
{
	return g_Scene;
}
