#ifndef SCENE_H
#define SCENE_H
#include <Windows.h>

void Scene_Initialize(HWND& hWnd);
void Scene_Finalize();
void Scene_Update(double time);
void Scene_Draw();

enum Scene
{
	SCENE_TITLE,
	SCENE_GAME,
	SCENE_SECOND,
	SCENE_BATTLE,
	SCENE_RESULT,
	SCENE_MAX,
};
void Scene_Refresh(HWND& hWnd);
void Scene_Change(Scene scene);
Scene GetScene();
#endif
