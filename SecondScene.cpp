#include "SecondScene.h"
#include "sprite_anim.h"
#include "Fade.h"
#include "Player.h"
#include "PlayerUpdater.h"
#include "Camera.h"
#include "Game.h"
#include "DialogueUIManager.h"
#include "EnemyChaser.h"
#include "TriggerCollider.h"
#include "Scene.h"

static float g_Time;
static float g_CreateTime;
static bool g_Restart = false;
void SecondScene_Initialize()
{
	Fade_Start(1.0, false);
	g_Restart = false;
	ChangeGameScene(1);
	Change_Player_Draw_Layer(Player_Layer::GROUND);
	DialogueManager_Reset(1);
	Dialogue_ChangeCharacter(2);
	ChangeSceneSize(2230, 1500);
	resetCam(-200, 0);
	RefreshTrigger();
	Player_Initialize({ 1000.0f,100.0f }, Player_Status::Gaming);
	EnemyChaser_Initialize({ 1000.0f,-100.0f });
}

void SecondScene_Finitialize()
{
	EnemyChaser_Finitialize();
}

void SecondSceneUpdate(float time)
{
	EnemyChaserUpdate(time);
	if (g_Restart)
	{
		if (Fade_GetState() == FADE_STATE_FINISHED_OUT)
		{
			EnemyChaserReset({ 1000.0f,-100.0f });
			SecondScene_Finitialize();
			SecondScene_Initialize();

		}
	}
	else
	{
		if (Fade_GetState() == FADE_STATE_FINISHED_OUT)
		{
			Scene_Change(SCENE_BATTLE);
		}
	}
}

void SecondSceneDraw()
{

}

void SecondScene_Restart_Enable()
{
	g_Restart = true;
}