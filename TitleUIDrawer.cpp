#include "TitleUIDrawer.h"
#include "Texture.h"
#include "sprite.h"
#include "UIInteraction.h"
#include "MouseRenderer.h"
#include "mouse.h"
#include "Key_Logger.h"
#include "Fade.h"
#include <Windows.h>
#include "GameSetting.h"
#include "Audio.h"
static HWND g_hWnd = nullptr;

static int g_PlayButtonTexId = -1;
void  TitleUIHover(int index, XMFLOAT4 color);
static int g_ButtonClickSFX_ID = -1;
static Mouse_State g_mouseState;

struct Button_Draw_Info
{
	int Button_Texid;
	int tx, ty, tw, th;
	XMFLOAT4 buttonColor;
	bool isHover = false;

	UIBox collision;
};
static Button_Draw_Info g_ButtonType[]
{
	{-1,665,625,266,62,{1.0f,1.0f,1.0f,0.5f},false, { { 133.0f,31.0f },133,31 }},
	{-1,690,795,212,65,{1.0f,1.0f,1.0f,0.5f},false, { { 106.0f,32.0f },106,32 }},
	{-1,1365,804,75,75,{1.0f,1.0f,1.0f,0.5f},false, { { 37.5f,37.5f },37.5f,37.5f }},
	{-1,1487,804,75,75,{1.0f,1.0f,1.0f,0.5f},false, { { 37.5f,37.5f },37.5f,37.5f }},
};
void TitleUIDrawer_Initialize(HWND& hWnd)
{
	g_ButtonClickSFX_ID = LoadAudio("SFX-Butten.wav");
	g_ButtonType[0].Button_Texid = Texture_Load(L"UI_Start_L.png");
	g_ButtonType[1].Button_Texid = Texture_Load(L"UI_Exit_L.png");
	g_ButtonType[2].Button_Texid = Texture_Load(L"C.png");
	g_ButtonType[3].Button_Texid = Texture_Load(L"J.png");
	g_hWnd = hWnd;
}

void TitleUIDrawer_Finalize()
{
	g_hWnd = nullptr;
}

void TitleUIDrawer_Update(double time)
{
	Mouse_GetState(&g_mouseState);
	TitleUIInteraction();
	if (MouseLogger_IsTrigger(0) && g_ButtonType[0].isHover)
	{
		Fade_Start(1.0, true);
		PlayAudio(g_ButtonClickSFX_ID);
		SetAudioVolume(g_ButtonClickSFX_ID, 0.35f);
	}
	if (MouseLogger_IsTrigger(0) && g_ButtonType[1].isHover)
	{
		PlayAudio(g_ButtonClickSFX_ID);
		SetAudioVolume(g_ButtonClickSFX_ID, 0.35f);
		DestroyWindow(g_hWnd);
	}
	if (MouseLogger_IsTrigger(0) && g_ButtonType[2].isHover)
	{
		PlayAudio(g_ButtonClickSFX_ID);
		SetAudioVolume(g_ButtonClickSFX_ID, 0.35f);
		GameLanguage_Set(GameLanguage::CHINESE);
	}
	if (MouseLogger_IsTrigger(0) && g_ButtonType[3].isHover)
	{
		PlayAudio(g_ButtonClickSFX_ID);
		SetAudioVolume(g_ButtonClickSFX_ID, 0.35f);
		GameLanguage_Set(GameLanguage::JAPANESE);
	}
}

UIBox TitleUI_GetUICollision(int index)
{
	float cx = g_ButtonType[index].collision.center.x + g_ButtonType[index].tx;
	float cy = g_ButtonType[index].collision.center.y + g_ButtonType[index].ty;



	return { {cx,cy},g_ButtonType[index].collision.half_width, g_ButtonType[index].collision.half_height };
}

void TitleUIDrawer_Draw()
{
	for (int i = 0; i < BUTTON_MAX; i++)
	{
		Sprite_Draw_UV_UI(g_ButtonType[i].Button_Texid, g_ButtonType[i].tx, g_ButtonType[i].ty, g_ButtonType[i].tw, g_ButtonType[i].th, 1, 1, 0, 0, 1.0f, g_ButtonType[i].buttonColor);
#if defined(DEBUG)||defined(_DEBUG)
		UICollision_DebugDraw(TitleUI_GetUICollision(i));
#endif
	}
}

void TitleUIInteraction()
{
	for (int i = 0; i < BUTTON_MAX; i++)
	{
		if (OnUIBoxCollisionEnter(MouseRenderer_GetBoxCollision(), TitleUI_GetUICollision(i)))
		{
			TitleUIHover(i, { 1.0f,1.0f,1.0f,1.0f });
			g_ButtonType[i].isHover = true;
		}
		else
		{
			TitleUIHover(i, { 1.0f,1.0f,1.0f,0.5f });
			g_ButtonType[i].isHover = false;
		}
	}
}

void TitleUIHover(int index, XMFLOAT4 color)
{
	g_ButtonType[index].buttonColor = color;
}