#include "Fade.h"
using namespace DirectX;
#include <iostream>
#include "Texture.h"
#include "direct3d.h"
#include "sprite.h"


static double g_FadeTime{ 0.0 };
static XMFLOAT3 g_color{ 0.0f,0.0f,0.0f };
static double g_FadeStartTime{ 0.0 };
static double g_AccumulatedTime{ 0.0 };
static float g_Alpha = 0.0f;
static FadeState g_State = FADE_STATE_NONE;
static int NowFadeTex = 0;
static int g_FadeTexId = -1;
static int g_FadeWinId = -1;
static int g_FadeLoseId = -1;
int g_FadeTex_Data[3];

void Fade_Initialize()
{
	g_FadeTime = 0.0;
	g_FadeStartTime = 0.0;
	g_AccumulatedTime = 0.0;
	g_color = { 0.0f,0.0f,0.0f };
	g_Alpha = 0.0f;
	g_State = FADE_STATE_NONE;
	g_FadeTexId = Texture_Load(L"white.png");
	g_FadeTex_Data[0] = g_FadeTexId;
}

void Fade_Finalize()
{
}

void Fade_Update(double time)
{
	if (g_State <= FADE_STATE_FINISHED_OUT)return;
	g_AccumulatedTime += time;
	double ratio = std::min((g_AccumulatedTime - g_FadeStartTime) / g_FadeTime, 1.0);
	if (ratio >= 1.0)
	{
		g_State = g_State == FADE_STATE_IN ? FADE_STATE_FINISHED_IN : FADE_STATE_FINISHED_OUT;
	}
	g_Alpha = (float)(g_State == FADE_STATE_IN ? 1.0 - ratio : ratio);
}

void Fade_Draw()
{
	if (g_State == FADE_STATE_FINISHED_IN)return;
	XMFLOAT4 color(g_color.x, g_color.y, g_color.z, g_Alpha);
	Sprite_Draw_UV_UI(g_FadeTex_Data[NowFadeTex], 0.0f, 0.0f, Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight(), 1.0f, 1.0f, 0, 0, 1.0f, color);
}

void Fade_Start(double time, bool isFadeOut, DirectX::XMFLOAT3 color, int fadeTex)
{
	g_FadeStartTime = g_AccumulatedTime;
	g_FadeTime = time;
	g_State = isFadeOut ? FADE_STATE_OUT : FADE_STATE_IN;
	g_color = color;
	g_Alpha = isFadeOut ? 0.0f : 1.0f;
	NowFadeTex = fadeTex;
}

FadeState Fade_GetState()
{
	return g_State;
}
