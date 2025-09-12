#include "BattlePlayer.h"
#include "BattleSystem.h"
#include "BattlePlayerUpdater.h"
#include "BattleBossController.h"
#include <DirectXMath.h>
#include "Texture.h"
#include "sprite.h"
#include "NekoTool.h"
#include "Fade.h"
#include "BattleBossBoomber.h"
using namespace DirectX;

static XMFLOAT2 g_PlayerPosition{};
static XMFLOAT2 g_PlayerVelocity{};
static bool g_PlayerEnable = true;

static int g_BattlePlayer_TexId = -1;
static Box g_BattlePlayerBoxCollision{ {g_BattlePlayer_size / 2.0f,g_BattlePlayer_size / 2.0f},g_BattlePlayer_size / 2.0f,g_BattlePlayer_size / 2.0f };
static Circle g_BattlePlayerCircleCollision{ {g_BattlePlayer_size / 2.0f,g_BattlePlayer_size / 2.0f},g_BattlePlayer_size / 2.0f };

//player health

static int g_Player_Health_Count = 3;
static int g_PlayerCanBeHit = true;
static float g_PlayerInvisibleCountDown = 1.0f;
static bool     g_PlayerFlashTrigger = false;
static float    g_PlayerFlashTimer = 1.0f;
static XMFLOAT4 g_PlayerBaseColor = { 1.0f,1.0f,1.0f,1.0f };
static XMFLOAT4 g_PlayerCurrentColor = { 1.0f,1.0f,1.0f,1.0f };
void PlayerFlashUpdate(float time);


void BattlePlayer_Initialize(const DirectX::XMFLOAT2& position)
{
	g_PlayerPosition = position;
	g_PlayerVelocity = { 0.0f,0.0f };
	g_PlayerEnable = true;
	g_BattlePlayer_TexId = Texture_Load(L"BattlePlayer.png");
	g_Player_Health_Count = 3;
	g_PlayerCanBeHit = true;
	g_PlayerInvisibleCountDown = 1.0f;
}

void BattlePlayer_Finitialize()
{
}

void BattlePlayerUpdate(float time)
{
	BattlePlayerUpdater_Update(time, &g_PlayerEnable, &g_PlayerPosition, &g_PlayerVelocity);
	OnPlayerHit();
	PlayerHitEffectUpdate(time);
	PlayerFlashUpdate(time);
}

void BattlePlayerDraw()
{
	Sprite_Draw(g_BattlePlayer_TexId, g_PlayerPosition.x, g_PlayerPosition.y, g_BattlePlayer_size, g_BattlePlayer_size, 1.0f, g_PlayerCurrentColor);
	for (int i = 0; i < 50; i++)
	{
		if (GetExplosionColliderEnable(i) == false)continue;
	}
#if defined(DEBUG)||defined(_DEBUG)
	Collision_DebugDraw(BattlePlayer_GetCollision_Circle(), { 1.0f,1.0f,1.0f,1.0f });
#endif
}
Box BattlePlayer_GetCollision()
{
	float cx = g_BattlePlayerBoxCollision.center.x + g_PlayerPosition.x;
	float cy = g_BattlePlayerBoxCollision.center.y + g_PlayerPosition.y;



	return { {cx,cy},g_BattlePlayerBoxCollision.half_width, g_BattlePlayerBoxCollision.half_height };
}

Circle BattlePlayer_GetCollision_Circle()
{
	float cx = g_BattlePlayerCircleCollision.center.x + g_PlayerPosition.x;
	float cy = g_BattlePlayerCircleCollision.center.y + g_PlayerPosition.y;



	return { {cx,cy},g_BattlePlayerCircleCollision.radius };
}

void OnPlayerHit()
{
	switch (GetBattleBoss())
	{
	case BattleBossState::CLEANER:
		for (int i = 0; i < 3; i++)
		{
			if (Get_Cleaner_Bullet_Enable(i) == true)
			{
				if (OnCircleCollisionEnter(GetCleanerBulletCollider(i), BattlePlayer_GetCollision_Circle()))
				{
					if (g_PlayerCanBeHit == true)
					{
						g_PlayerCanBeHit = false;
						g_Player_Health_Count--;
					}
				}

			}
		}
		if (Get_CleanerSweapEnable())
		{
			if (OnCircleCollisionEnter(GetSweapCollider(), BattlePlayer_GetCollision_Circle()))
			{
				if (g_PlayerCanBeHit == true)
				{
					g_PlayerCanBeHit = false;
					g_Player_Health_Count--;
				}
			}
		}
		break;
	case BattleBossState::BOOMBER:
		for (int i = 0; i < 500; i++)
		{
			if (GetExplosionColliderEnable(i) == false)continue;
			if (OnBoxCollisionEnter(GetExplosionCollision(i), BattlePlayer_GetCollision()))
			{
				if (g_PlayerCanBeHit == true)
				{
					g_PlayerCanBeHit = false;
					g_Player_Health_Count--;
				}
			}
		}
		break;
	default:
		break;
	}
	if (g_Player_Health_Count < 0 && Fade_GetState() != FADE_STATE_OUT)
	{
		Fade_Start(0.1, true, {1.0f,1.0f,1.0f,},1);
		Battle_State_ChangeState(BattleSystemState::PLAYER_DEAD);
	}

}
void PlayerHitEffectUpdate(float time)
{
	if (g_PlayerCanBeHit == false)
	{
		g_PlayerInvisibleCountDown = clamp<float>(g_PlayerInvisibleCountDown - time, 0, 10);
	}
	if (g_PlayerInvisibleCountDown == 0)
	{
		g_PlayerCanBeHit = true;
		g_PlayerInvisibleCountDown = 1.0f;
	}

}
void PlayerFlashUpdate(float time)
{
	if (g_PlayerCanBeHit == false)
	{
		g_PlayerFlashTimer = clamp<float>(g_PlayerFlashTimer - time, 0, 10);
		float t = g_PlayerFlashTimer * 20.0f; // çTêßëM‡°ïpó¶ (20.0f = ïpó¶)
		float blink = (sinf(t) * 0.5f + 0.5f); // [0,1] îVä‘
		// Ë¯êFç›å¥énË¯êF òa îíêFîVä‘êÿä∑
		XMFLOAT4 white{ 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT4 red{ 1.0f, 0.0f, 0.0f, 1.0f };

		g_PlayerCurrentColor.x = white.x + (red.x - white.x) * blink;
		g_PlayerCurrentColor.y = white.y + (red.y - white.y) * blink;
		g_PlayerCurrentColor.z = white.z + (red.z - white.z) * blink;
		g_PlayerCurrentColor.w = 1.0f;
		if (g_PlayerFlashTimer == 0)
		{
			g_PlayerFlashTrigger = false;
			g_PlayerFlashTimer = 0.15f;
			g_PlayerCurrentColor = g_PlayerBaseColor;
		}
	}
	if (g_PlayerCanBeHit == true)
	{
		g_PlayerCurrentColor = g_PlayerBaseColor;
	}
}

int GetPlayerHealth()
{
	return g_Player_Health_Count;
}

XMFLOAT2 GetPlayerPos()
{
	return g_PlayerPosition;
}
XMFLOAT2 GetPlayerVel()
{
	return g_PlayerVelocity;
}


