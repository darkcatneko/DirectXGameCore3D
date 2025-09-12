#include "direct3d.h"
#include "sprite.h"
#include"shader.h"
#include "Texture.h"
#include "vector"
#include "debug_text.h"
#include <sstream>
#include "system_timer.h"
#include "sprite_anim.h"
#include "polygon.h"
#include "keyboard.h"
#include "mouse.h"
#include "Key_Logger.h"
#include "Game.h"
#include "GameWindow.h" 
#include"Player.h"
#include "bullet.h"
#include "Enemy.h"
#include "Effect.h"
#include "Audio.h"
#include "Fade.h"
#include "Score.h"
#include "CollisionGameObject.h"
#include "TriggerCollider.h"
#include "Background.h"
#include "DialogueUIManager.h"
#include "EnemyChaser.h"
#include "Camera.h"
#include "PlayerUpdater.h"
#include "particle.h"
#include "SecondScene.h"
using namespace DirectX;

void Collision_Update_E_B();
void Collision_Update_E_P();
float g_Time;
float g_CreateTime;
static int g_NowScene = 0;
Player_Layer g_playerLayer = Player_Layer::GROUND;
static int g_DockBGM_Id = -1;

void Game_Initialize()
{
	g_NowScene = 0;
	g_playerLayer = Player_Layer::GROUND;
	SpriteAnim_Initialize();
	DialogueUIManager_Initialize();
	ChangeSceneSize(1920, 1080);
	ChangeGameScene(0);
	InitAudio();
	BackGround_Initialize();
	Bullet_Initialize();
	Enemy_Initialize();
	Particle_Initialize();
	Effect_Initialize();
	Score_Initialize(0, 0, 5);
	Player_Initialize({ 700.0f,665.0f }, Player_Status::Dialogue);
	CollisionGameObject_Initialize();
	TriggerCollider_Initialize();
	EnemyChaser_Initialize({ 1000.0f,-100.0f });
	Fade_Start(1.0, false);
	g_DockBGM_Id = LoadAudio("SFX-Sea+bird.wav");
	PlayAudio(g_DockBGM_Id,true);
	SetAudioVolume(g_DockBGM_Id, 0.5f);
}

void Game_Finitialize()
{
	UnloadAudio(g_DockBGM_Id);
}

void GameDraw()
{
	BackGroundDraw();
	BackGroundDraw_Back();
	CollisionGameObject_Draw();
	Bullet_Draw();
	Enemy_Draw();
	EnemyChaserDraw();
	BackGroundDraw_Mid();
	if (g_playerLayer == Player_Layer::GROUND)
	{
		PlayerDraw();
	}
	BackGroundDraw_Front();
	if (g_playerLayer == Player_Layer::ROOF)
	{
		PlayerDraw();
	}
	EffectDraw();
	TriggerCollider_Draw();
	DialogueUIManager_Draw();
	Score_Draw();
}

void Change_Player_Draw_Layer(Player_Layer layer)
{
	g_playerLayer = layer;
}

Player_Layer GetPlayerLayer()
{
	return g_playerLayer;
}

void GameUpdate(float time)
{
	//XMFLOAT2 x = { 1500.0f,450.0f };
	//if (g_Time - g_CreateTime >= 2.0f)
	//{

	//	Enemy_Creater(bruh, x, { 0,0 });
	//	Enemy_Creater(cat, x, { 0,250 });
	//	g_CreateTime = g_Time;
	//}
	g_Time += time;
	BackGroundUpdate(time);
	Enemy_Update(time);
	Bullet_Update(time);
	PlayerUpdate(time);
	TriggerCollider_Update(time);
	DialogueUIManager_Update(time);
	Collision_Update_E_B();
	Collision_Update_E_P();
	EffectUpdate(time);
}

void Collision_Update_E_B()
{
	for (int bi = 0; bi < BULLET_MAX; bi++)
	{
		if (!Bullet_IsEnable(bi))continue;
		for (int ei = 0; ei < ENEMY_MAX; ei++)
		{
			if (!Enemy_IsEnable(ei))continue;
			if (OnCircleCollisionEnter(Bullet_GetCollision(bi), Enemy_GetCollision(ei)))
			{
				//HIT!
				Bullet_Destroy(bi);
				Enemy_Hit(1, ei);
			}
		}
	}
}

void Collision_Update_E_P()
{
	if (!Player_IsEnable())return;
	for (int ei = 0; ei < ENEMY_MAX; ei++)
	{
		if (!Enemy_IsEnable(ei))continue;
		if (OnCircleCollisionEnter(Player_GetCollision(), Enemy_GetCollision(ei)))
		{
			//HIT!
			Player_Destroy();
			Enemy_Destroy(ei);
		}
	}
}
int Get_GameScene()
{
	return g_NowScene;
}

void ChangeGameScene(int scene)
{
	g_NowScene = scene;
}
