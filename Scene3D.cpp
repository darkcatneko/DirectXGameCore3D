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
#include "Light_Camera.h"
#include "ShaderField.h"
#include "circle_shadow.h"
#include "NekoTool.h"
#include "GameUI.h"
static Scene3D g_SceneEnum = Scene3D::SCENE_INIT;
static Scene3D g_SceneNextEnum = Scene3D::SCENE_INIT;

static XMFLOAT3 g_cubePosition;
static XMFLOAT3 g_meshPosition;
static XMFLOAT3 g_cubeVelocity;

//Test MODEL
static MODEL* g_pModelTest = nullptr;


static int texid;
void LightRendering();

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
		Enemy_Create({ 0.0f, 12.0f, 15.0f });
		Enemy_Create({ 20.0f, 14.0f, 12.0f });
		Player3D_Initialize({ 0,12,0 }, { 0,0,1 });
		LightCamera_Initialize(GetPlayerFront(), {0,10,-5});
		GameUI_Initialize();
		//g_pModelTest = ModelLoad("KIRBY.fbx",0.1f,false);
		texid = Texture_Load(L"Grass.png");
		CircleShadow_Initialize();
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
	Map_Update(time);
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
	GameUI_Update(time);
}


void Scene3D_Draw()
{
	LightRendering();
	Direct3D_SetBackBuffer();
	Direct3D_ClearBackBuffer();
	XMFLOAT4X4 mtxView = Camera_GetMatrix();
	XMFLOAT4X4 mtxProj = Camera_GetMatrixPerspective();
	XMMATRIX view = XMLoadFloat4x4(&mtxView);
	XMMATRIX proj = XMLoadFloat4x4(&mtxProj);


	//カメラに関する行列をシェーダーに設定
	Shader3D_SetViewMatrix(view);
	Shader3D_Static_SetViewMatrix(view);
	Shader_Billboard_SetViewMatrix(view);
	Shader3DUnilt_SetViewMatrix(view);
	ShaderField_SetViewMatrix(view);

	Shader3D_SetProjectionMatrix(proj);
	Shader3D_Static_SetProjectionMatrix(proj);
	Shader_Billboard_SetProjectionMatrix(proj);
	Shader3DUnilt_SetProjectMatrix(proj);
	ShaderField_SetProjectionMatrix(proj);


	Light_SetAmbient({ 0.3f,0.3f,0.3f });
	Light_SetDirectionalWorld({ 0.0f,-1.0f,0.0f,0.0f }, { 0.4f,0.4f,0.4f,0.3f });
	Light_SetPointLightCount(1);
	Light_SetPointLight(0, { 0.0f,0.0f,0.0f }, 30.0f, { 1.0f,0.0f,0.0f });
	//Light_SetPointLight(1, { 2.0f,10.0f,0.0f}, 0.1f, { 0.0f,1.0f,0.0f });
	Light_SetSpecularWorld({ 0.1f,0.1f,0.1f,1.0f }, 4.0f, Camera_GetCameraPos());
	//Light_SetPointLight(2, { 0.0f,0.0f,2.0f }, 0.1f, { 0.0f,0.0f,1.0f });
	//ModelDraw(g_pModelTest, { 0.0,0.0,0.0 });

	

	Sky_Draw();
	Grid_Draw({ 0,GetMouseToMapLocation().y,0 }, { 0,0,0 }, { 1,1,1 });

	Ray ray = MakeMouseRay(Get_Mouse_Info().x, Get_Mouse_Info().y);
	Debug_DrawRay(ray);
    //Cube_Draw(target, { 0,0,0 }, {0.1f,0.1f,0.1f});


	MeshField_Draw(g_meshPosition);
	//Cube_Draw(g_cubePosition);
	Enemy_Draw();
	Player3D_Draw();
	Light_SetSpecularWorld({ 0.1f,0.1f,0.1f,1.0f }, 50.0f, Camera_GetCameraPos());
	Bullet3D_Draw();
	Map_Draw();
	Billboard_Draw(texid, g_meshPosition, 3, 3);
	MouseRenderer_Draw();
	GameUI_Draw();
	CircleShadow_Draw(GetPlayerPosition());
	//Sprite_Begin();
	Direct3D_SetOffScreenTexture(0);
	Direct3D_SetDepthEnable(false);
	Sprite_Draw_N(0, 0, 128.0f, 128.0f, 1.0f);
	Map_DrawRotatingGizmos();
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

void LightRendering()
{
	Direct3D_SetOffScreen();
	Direct3D_ClearOffscreen();

	XMFLOAT4X4 mtxView = Camera_GetMatrix();
	XMMATRIX view = XMLoadFloat4x4(&mtxView);
	XMMATRIX proj = XMLoadFloat4x4(&Camera_GetMatrixPerspective());
	XMFLOAT3 camera_position = Camera_GetCameraPos();


	//LightCamera_SetFront(GetPlayerFront());
	LightCamera_SetPosition({ GetPlayerPosition().x,20.0f, GetPlayerPosition().z - 5.0f});
	mtxView = LightCamera_GetViewMatrix();
	XMFLOAT4X4 mtxProj = LightCamera_GetProjectionMatrix();
	view = XMLoadFloat4x4(&mtxView);
	proj = XMLoadFloat4x4(&mtxProj);


	//カメラに関する行列をシェーダーに設定
	Shader3D_SetViewMatrix(view);
	Shader3D_Static_SetViewMatrix(view);
	Shader_Billboard_SetViewMatrix(view);
	Shader3DUnilt_SetViewMatrix(view);
	ShaderField_SetViewMatrix(view);

	Shader3D_SetProjectionMatrix(proj);
	Shader3D_Static_SetProjectionMatrix(proj);
	Shader_Billboard_SetProjectionMatrix(proj);
	Shader3DUnilt_SetProjectMatrix(proj);
	ShaderField_SetProjectionMatrix(proj);

	//サンプラー設定
	Sampler_SetFilterAnisotropic();

	Direct3D_SetDepthEnable(false);
	//空表示
	Sky_Draw();

	Direct3D_SetDepthEnable(true);
	Light_SetAmbient({ 1.0f,1.0f,1.0f });
	MeshField_Draw(g_meshPosition);
	Map_Draw();
	Enemy_Draw();
	Player3D_Draw();
	//Direct3D_SetBackBuffer();
	//Direct3D_ClearBackBuffer();
	//Bullet_Draw();

	//Particle3d_Draw();
}