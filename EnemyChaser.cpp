#include "EnemyChaser.h"
#include "sprite_anim.h"
#include "DirectXMath.h"
#include "Game.h"
#include "Collision.h"
#include "Player.h"
#include "Fade.h"
#include "SecondScene.h"
#include "Audio.h"
using namespace DirectX;

void CheckPlayerCollision();

static XMFLOAT2 g_ChaseEnemyPosition{};

static int g_EnemyAnimePattern_Front = -1;
static int g_EnemyAnimePattern_Left = -1;
static int g_EnemyAnimePattern_Right = -1;
static int g_EnemyAnimePattern_Stop = -1;

static int g_EnemyAnimePlayer_Front = -1;
static int g_EnemyAnimePlayer_Left = -1;
static int g_EnemyAnimePlayer_Right = -1;
static int g_EnemyAnimePlayer_Stop = -1;
static int g_Now_Command = -1;

static bool g_enemyEnable = false;
static int g_EnemyChaseBGM = -1;

static ChaseEnemyAnimationState chaseAnimationState = ChaseEnemyAnimationState::STOPPED;
static Box g_EnemyBoxCollision{ {g_Enemy_size / 2.0f, g_Enemy_size / 2.0f},g_Enemy_size / 2.0f,g_Enemy_size / 2.0f };

struct Enemy_MoveCommand
{
	XMFLOAT2 destination;
	ChaseEnemyAnimationState next_state;
};

static Enemy_MoveCommand commands[]
{
	{{1000,550}, ChaseEnemyAnimationState::RIGHT },
	{{2092,550}, ChaseEnemyAnimationState::FRONT },
	{{2092,1073}, ChaseEnemyAnimationState::LEFT },
	{{1342,1073}, ChaseEnemyAnimationState::STOPPED },
};



void EnemyChaser_Initialize(const XMFLOAT2& position) // GAME
{
	g_EnemyAnimePattern_Front = AnimDataInitialize(L"Scavenger_Front.png", 4, 4, 0, 0, { 0,0 }, { 0.25f,1 }, { g_Enemy_size,g_Enemy_size }, true, 0.1f, 0.55f);
	g_EnemyAnimePattern_Left = AnimDataInitialize(L"Scavenger_Left.png", 4, 4, 0, 0, { 0,0 }, { 0.25f,1 }, { g_Enemy_size,g_Enemy_size }, true, 0.1f, 0.55f);
	g_EnemyAnimePattern_Right = AnimDataInitialize(L"Scavenger_Right.png", 4, 4, 0, 0, { 0,0 }, { 0.25f,1 }, { g_Enemy_size,g_Enemy_size }, true, 0.1f, 0.55f);
	g_EnemyAnimePattern_Stop = AnimDataInitialize(L"Scavenger_Front.png", 1, 1, 0, 0, { 0,0 }, { 0.25f,1 }, { g_Enemy_size,g_Enemy_size }, true, 0.1f, 0.55f);

	g_EnemyAnimePlayer_Front = SpriteAnim_CreatePlayer(g_EnemyAnimePattern_Front, { 0.0f,0.0f });
	g_EnemyAnimePlayer_Left = SpriteAnim_CreatePlayer(g_EnemyAnimePattern_Left, { 0.0f,0.0f });
	g_EnemyAnimePlayer_Right = SpriteAnim_CreatePlayer(g_EnemyAnimePattern_Right, { 0.0f,0.0f });
	g_EnemyAnimePlayer_Stop = SpriteAnim_CreatePlayer(g_EnemyAnimePattern_Stop, { 0.0f,0.0f });

	g_enemyEnable = false;
	g_ChaseEnemyPosition = position;
	g_Now_Command = -1;
	chaseAnimationState = ChaseEnemyAnimationState::STOPPED;
	g_EnemyChaseBGM = LoadAudio("BGM-ScavengerChase.wav");
}
void EnemyChaser_Finitialize()
{
	UnloadAudio(g_EnemyChaseBGM);
}

void EnemyChaserUpdate(float time) // SECOND_SCENE
{
	if (g_enemyEnable != false)
	{
		CheckPlayerCollision();
	}
	if (chaseAnimationState == ChaseEnemyAnimationState::STOPPED || g_enemyEnable == false)return;
	if (chaseAnimationState == ChaseEnemyAnimationState::FRONT)
	{
		g_ChaseEnemyPosition.y += time * g_eEnemySpeed;
	}
	if (chaseAnimationState == ChaseEnemyAnimationState::RIGHT)
	{
		g_ChaseEnemyPosition.x += time * g_eEnemySpeed;
	}
	if (chaseAnimationState == ChaseEnemyAnimationState::LEFT)
	{
		g_ChaseEnemyPosition.x -= time * g_eEnemySpeed;
	}
	if (g_Now_Command < 0)return;
	switch (chaseAnimationState)
	{
	case ChaseEnemyAnimationState::FRONT:
		if (g_ChaseEnemyPosition.y >= commands[g_Now_Command].destination.y)
		{
			chaseAnimationState = commands[g_Now_Command].next_state;
			g_Now_Command += 1;
		}
		break;
	case ChaseEnemyAnimationState::LEFT:
		if (g_ChaseEnemyPosition.x <= commands[g_Now_Command].destination.x)
		{
			chaseAnimationState = commands[g_Now_Command].next_state;
			g_Now_Command += 1;
		}
		break;
	case ChaseEnemyAnimationState::RIGHT:
		if (g_ChaseEnemyPosition.x >= commands[g_Now_Command].destination.x)
		{
			chaseAnimationState = commands[g_Now_Command].next_state;
			g_Now_Command += 1;
		}
		break;
	case ChaseEnemyAnimationState::STOPPED:
		break;
	default:
		break;
	}
}

void EnemyChaserDraw() // GAME
{
	if (Get_GameScene() != 1)return;
	switch (chaseAnimationState)
	{
	case ChaseEnemyAnimationState::FRONT:
		SpriteAnim_Draw(g_EnemyAnimePlayer_Front, g_ChaseEnemyPosition.x, g_ChaseEnemyPosition.y);
		break;
	case ChaseEnemyAnimationState::LEFT:
		SpriteAnim_Draw(g_EnemyAnimePlayer_Left, g_ChaseEnemyPosition.x, g_ChaseEnemyPosition.y);
		break;
	case ChaseEnemyAnimationState::RIGHT:
		SpriteAnim_Draw(g_EnemyAnimePlayer_Right, g_ChaseEnemyPosition.x, g_ChaseEnemyPosition.y);
		break;
	case ChaseEnemyAnimationState::STOPPED:
		SpriteAnim_Draw(g_EnemyAnimePlayer_Stop, g_ChaseEnemyPosition.x, g_ChaseEnemyPosition.y);
		break;
	default:
		break;
	}
#if defined(DEBUG)||defined(_DEBUG)
	Collision_DebugDraw_Box(EnemyChaser_GetCollision());
#endif
}
Box EnemyChaser_GetCollision()
{
	float cx = g_EnemyBoxCollision.center.x + g_ChaseEnemyPosition.x;
	float cy = g_EnemyBoxCollision.center.y + g_ChaseEnemyPosition.y;



	return { {cx,cy},g_EnemyBoxCollision.half_width, g_EnemyBoxCollision.half_height };
}
void EnableChaser()
{
	chaseAnimationState = ChaseEnemyAnimationState::FRONT;
	g_enemyEnable = true;
	g_Now_Command = 0;
	PlayEnemyChaseBGM();
}

void CheckPlayerCollision()
{
	if (OnBoxCollisionEnter(Player_GetBoxCollision(), EnemyChaser_GetCollision()))
	{
		chaseAnimationState = ChaseEnemyAnimationState::STOPPED;
		if (Fade_GetState() != FADE_STATE_OUT)
		{
			g_enemyEnable = false;
			Fade_Start(1.0, true);
			SecondScene_Restart_Enable();
		}
	}
}
void EnemyChaserReset(const XMFLOAT2& position)
{
	g_enemyEnable = false;
	g_ChaseEnemyPosition = position;
	g_Now_Command = -1;
	chaseAnimationState = ChaseEnemyAnimationState::STOPPED;
}
void PlayEnemyChaseBGM()
{
	PlayAudio(g_EnemyChaseBGM,true);
	SetAudioVolume(g_EnemyChaseBGM,0.05f);
}
void StopEnemyChaseBGM()
{
	UnloadAudio(g_EnemyChaseBGM);
}
