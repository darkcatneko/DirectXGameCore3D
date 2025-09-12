#include "Title.h"
#include "Texture.h"
#include "sprite.h"
#include "Key_Logger.h"
#include "direct3d.h"
#include "Fade.h"
#include "Scene.h"
#include "TitleUIDrawer.h"
#include "Audio.h"

static int g_TitlebgTexId = -1;
static int g_TitleIconTexId = -1;
static int g_TitleBGM = -1;
static bool bgmPlaying = false;

void Title_Initialize(HWND &hWnd)
{
	InitAudio();
	g_TitlebgTexId = Texture_Load(L"DOT.png");
	g_TitleIconTexId = Texture_Load(L"MainTitle.JPG");
	//g_TitleIconTexId = Texture_Load(L"8.png");
	TitleUIDrawer_Initialize(hWnd);
	g_TitleBGM = LoadAudio("BGM-Home screen.wav");
	PlayAudio(g_TitleBGM, true);
	SetAudioVolume(g_TitleBGM, 0.05f);
}

void Title_Finalize()
{
	UnloadAudio(g_TitleBGM);
	//Texture_AllRelease();
}

void Title_Update(double time)
{
	/*if (bgmPlaying == false)
	{
		bgmPlaying = true;
		PlayAudio(g_TitleBGM, true);
		SetAudioVolume(g_TitleBGM, 0.05f);
	}*/
	TitleUIDrawer_Update(time);
	if (Fade_GetState() == FADE_STATE_FINISHED_OUT)
	{
		Scene_Change(SCENE_GAME);
	}

}

void Title_Draw()
{
	Sprite_Draw_UV_UI(g_TitlebgTexId, 0, 0, (float)Direct3D_GetBackBufferWidth(), (float)Direct3D_GetBackBufferHeight(), 1, 1, 0, 0, 1.0f, { 0,0,0,1.0f });
	Sprite_Draw_UV_UI(g_TitleIconTexId, 0, 0, (float)Direct3D_GetBackBufferWidth(), (float)Direct3D_GetBackBufferHeight(), 1, 1, 0, 0, (1.1f));
	TitleUIDrawer_Draw();
}
