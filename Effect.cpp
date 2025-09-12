#include "Effect.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "direct3d.h"
#include "Texture.h"
#include "sprite_anim.h"

struct Effect
{
	XMFLOAT2 position;
	bool isEnable;
	int anim_id;
};

static Effect g_Effects[EFFECT_MAX];
static int g_AmimPatternId[]
{ -1,-1,-1,-1,-1,-1,-1,-1,-1 };

void Effect_Initialize()
{
	for (Effect& e : g_Effects)
	{
		e.isEnable = false;
	}
	g_AmimPatternId[0] = AnimDataInitialize(L"Explosion.png", 16, 4, 0, 0, {0,0}, {0.25f,0.25f}, {64.0f,64.0f}, false, 0.1f, 1.0f);
	g_AmimPatternId[1] = AnimDataInitialize(L"WarningAnim.png", 13, 13, 0, 0, { 0,0 }, { 1.0f/13.0f,1 }, { 150.0f,150.0f }, false, 0.1f, 1.0f);
	g_AmimPatternId[2] = AnimDataInitialize(L"LockOn.png", 12, 12, 0, 0, { 0,0 }, { 1.0f / 12.0f,1 }, { 250.0f,250.0f }, false, 0.05f, 1.0f);
	g_AmimPatternId[3] = AnimDataInitialize(L"Mine.png", 16, 16, 0, 0, { 0,0 }, { 1.0f / 16.0f,1 }, { 250.0f,250.0f }, false, 0.025f, 1.0f);
	g_AmimPatternId[4] = AnimDataInitialize(L"Mine.png", 16, 16, 0, 0, { 0,0 }, { 1.0f / 16.0f,1 }, { 500.0f,500.0f }, false, 0.025f, 1.0f);
	g_AmimPatternId[5] = AnimDataInitialize(L"LockOn.png", 12, 12, 0, 0, { 0,0 }, { 1.0f / 12.0f,1 }, { 50.0f,50.0f }, false, 0.18f, 1.0f);
	g_AmimPatternId[6] = AnimDataInitialize(L"Mine.png", 16, 16, 0, 0, { 0,0 }, { 1.0f / 16.0f,1 }, { 150.0f,150.0f }, false, 0.025f, 1.0f);
	g_AmimPatternId[7] = AnimDataInitialize(L"RocketEffect.png", 16, 16, 0, 0, { 0,0 }, { 1.0f / 16.0f,1 }, { 150.0f,150.0f }, false, 0.025f, 1.0f);
	g_AmimPatternId[8] = AnimDataInitialize(L"RocketEffectB.png", 16, 16, 0, 0, { 0,0 }, { 1.0f / 16.0f,1 }, { 150.0f,150.0f }, false, 0.025f, 1.0f);
	//g_AmimPatternId[3] = AnimDataInitialize(L"Boomber.png", 16, 16, 0, 0, { 0,0 }, { 1.0f / 16.0f,1 }, { 250.0f,250.0f }, false, 0.1f, 1.0f);
}


void Effect_Finitialize()
{
}

void EffectUpdate(float time)
{
	for (Effect& e : g_Effects)
	{
		if (e.isEnable == false)continue;
		if (SpriteAnim_IsStopped(e.anim_id))
		{
			SpriteAnim_DestroyPlayer(e.anim_id);
			e.isEnable = false;
		}
		//這裡畫圖		
	}
}

void EffectDraw()
{
	for (Effect& e : g_Effects)
	{
		if (e.isEnable == false)continue;
		if (e.anim_id > 1)continue;
		SpriteAnim_Draw(e.anim_id, e.position.x, e.position.y);

		//這裡畫圖		
	}
}
void EffectDrawUP()
{
	for (Effect& e : g_Effects)
	{
		if (e.isEnable == false)continue;
		if (e.anim_id <2)continue;
		SpriteAnim_Draw(e.anim_id, e.position.x, e.position.y);

		//這裡畫圖		
	}
}

void Effect_Creater(DirectX::XMFLOAT2& position,int Effect_ID)
{
	for (Effect& e : g_Effects)
	{
		if (e.isEnable)continue;
		e.isEnable = true;
		e.position = position;
		e.anim_id = SpriteAnim_CreatePlayer(g_AmimPatternId[Effect_ID], position);
		break;
	}
}
