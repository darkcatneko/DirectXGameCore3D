#include "Scene3D.h"
#include <Windows.h>
#include "system_timer.h"
#include "Key_Logger.h"
#include "mouse.h"
#include"shader.h"
#include "Shader3D_Static.h"
#include "Cube.h"
#include "Grid.h"
#include "sprite.h"
#include "Texture.h"
#include "Fade.h"
#include "MouseRenderer.h"
#include "Camera3D.h"
#include "Audio.h"
#include "Sampler.h"
#include "MeshField.h"
#include "Light.h"
#include "model.h"
#include "Player3D.h"
#include "PlayerCamera.h"
#include "Map.h"
#include "Bullet3D.h"
#include "Billboard.h"
#include "Shader_Billboard.h"
#include "shader3d_unlit.h"
#include "sky.h"
#include "enemy.h"
static Scene3D g_SceneEnum = Scene3D::SCENE_INIT;
static Scene3D g_SceneNextEnum = Scene3D::SCENE_INIT;

static XMFLOAT3 g_cubePosition;
static XMFLOAT3 g_meshPosition;
static XMFLOAT3 g_cubeVelocity;

//Test MODEL
static MODEL* g_pModelTest = nullptr;

static int texid;

void Scene3D_Initialize(HWND& hWnd)
{
	g_cubePosition = { 5.0f,0.0f,0.0f };
	g_meshPosition = { 0.0f,-0.5f,0.0f };
	switch (g_SceneEnum)
	{
	case Scene3D::SCENE_INIT:
		SystemTimer_Initialize();
		KeyLogger_Initialize();
		Mouse_Initialize(hWnd);
		Shader_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
		Shader3D_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
		Shader3D_Static_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
		Shader3DUnilt_Initialize();
		Shader_Billboard_Initialize();
		Sampler_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
		Texture_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());

		Cube_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
		Light_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
		MeshField_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
		Grid_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
		Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
		Map_Initialize();
		Billboard_Initialize();
		Bullet3D_Initialize();
		Fade_Initialize();
		MouseRenderer_Initialize();
		Sky_Initialize();
		Camera3D_Initialize({ 0.0f,5.0f,-10.0f }, { 0.0f,0.0f,1.0f }, { 1.0f,0.0f,0.0f });
		Enemy_Initialize();
		//Enemy_Create({ 5.0f, 0.0f, 0.0f });
		Player3D_Initialize({ 10,0,0 }, { 0,0,1 });
		//g_pModelTest = ModelLoad("KIRBY.fbx",0.1f,false);
		texid = Texture_Load(L"Grass.png");
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
	Mouse_Finalize();
	Shader_Finalize();
	Direct3D_Finalize();
	UninitAudio();
	SpriteAnim_Finitialize();
	Sampler_Finalize();
	Bullet3D_Finitialize();
}

void Scene3D_Update(double time)
{
	MouseRenderer_Update(time);

	Camera3D_Update(time);

	Cube_Update(time);
	Enemy_Update(time);
	Player3D_Update(time);

	Bullet3D_Update(time);
	for (int i = 0; i < Map_GetObjectsCount(); i++)
	{
		for (int j = 0; j < Bullet3D_GetObjectsCount(); j++)
		{
			AABB bullet = Bullet_GetAABB(j);
			AABB object = Map_GetObjects(i)->Collision;
			if (Collision_IsOverlapAABB(bullet, object))
			{
				Bullet3D_Destroy(j);
			}
		}
	}
}

void Scene3D_Draw()
{
	Light_SetAmbient({ 0.3f,0.3f,0.3f });
	Light_SetDirectionalWorld({ 0.0f,-1.0f,0.0f,0.0f }, { 0.4f,0.4f,0.4f,0.3f });
	Light_SetPointLightCount(1);
	Light_SetPointLight(0, { 0.0f,0.0f,0.0f }, 30.0f, { 1.0f,0.0f,0.0f });
	//Light_SetPointLight(1, { 2.0f,10.0f,0.0f}, 0.1f, { 0.0f,1.0f,0.0f });
	Light_SetSpecularWorld({ 0.1f,0.1f,0.1f,1.0f }, 4.0f, Camera_GetCameraPos());
	//Light_SetPointLight(2, { 0.0f,0.0f,2.0f }, 0.1f, { 0.0f,0.0f,1.0f });
	//ModelDraw(g_pModelTest, { 0.0,0.0,0.0 });

	

	Sky_Draw();
	MeshField_Draw(g_meshPosition);
	//Cube_Draw(g_cubePosition);
	Enemy_Draw();
	Player3D_Draw();
	Light_SetSpecularWorld({ 0.1f,0.1f,0.1f,1.0f }, 50.0f, Camera_GetCameraPos());
	Bullet3D_Draw();
	//Grid_Draw();
	Billboard_Draw(texid, g_meshPosition, 3, 3);
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
