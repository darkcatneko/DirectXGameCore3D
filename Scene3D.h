#ifndef SCENE3D_H
#define SCENE3D_H
#include <Windows.h>

void Scene3D_Initialize(HWND& hWnd);
void Scene3D_Finalize();
void Scene3D_Update(double time);
void Scene3D_Draw();

enum class Scene3D
{
	SCENE_INIT,
	SCENE_TITLE,
	SCENE_GAME,
	SCENE_RESULT,
	SCENE_MAX,
};
void Scene3D_Refresh(HWND& hWnd);
void Scene3D_Change(Scene3D scene);
Scene3D GetScene3D();

#endif
