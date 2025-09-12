#include "BattleSystem.h"
#include "BattlePlayer.h"
#include "Fade.h"
#include "Camera.h"
#include "Background.h"
#include "BattleGround.h"
#include "BattleBossController.h"
#include "BossCleanerController.h"
#include "BattleBossBoomber.h"
#include "Key_Logger.h"
#include "Effect.h"
#include "bullet.h"
#include "DialogueUIManager.h"
#include "Scene.h"
#include "particle.h"
#include "BattleHint.h"
#include "Audio.h"
static BattleSystemState g_NowBattleState = BattleSystemState::DIALOGUE;
static BattleBossState g_NowBoss = BattleBossState::CLEANER;
static int g_NowPlot = 0;
static int g_CleanerBattleBGM_id = -1;

void BattleSystem_Initialize()
{
	resetCam(0, 0);
	Fade_Start(1.0, false);
	DialogueManager_Reset(2);
	g_NowPlot = 0;
	g_NowBattleState = BattleSystemState::DIALOGUE;
	g_NowBoss = BattleBossState::CLEANER;
	BattlePlayer_Initialize({ 800.0,600.0f });
	BattleGround_Initialize();
	CleanerBossInitialize();
	BattleBossController_Initialize();
	BattleHint_Initialize();
	g_CleanerBattleBGM_id = LoadAudio("BGM-Scavenger Battle.wav");
	PlayAudio(g_CleanerBattleBGM_id, true);
	SetAudioVolume(g_CleanerBattleBGM_id, 0.05f);
}

void BattleSystem_Finitialize()
{
	BattleBossController_Finitialize();
	UnloadAudio(g_CleanerBattleBGM_id);
}

void BattleSystemUpdate(float time)
{
	if (KeyLogger_IsTrigger(KK_M))
	{
		Fade_Start(0.1f, false);
		BattleSystem_Finitialize();
		BattleSystem_Initialize();
	}
	Battle_State_Update(g_NowBattleState, time);
	if (g_NowBattleState == BattleSystemState::PLAYER_DEAD)return;
	BattlePlayerUpdate(time);
	BattleGroundUpdate(time);
	BattleBossControllerUpdate(time);
	Bullet_Update(time);
	EffectUpdate(time);
	Particle_Update(time);
	if (KeyLogger_IsTrigger(KK_P))
	{
		BattleSystem_Finitialize();
		BattleSystem_Initialize();
	}
	if (KeyLogger_IsTrigger(KK_O))
	{
		Scene_Change(SCENE_TITLE);
	}
}

void BattleSystemDraw()
{
	BattleBackGroundDraw();
	EffectDraw();
	Particle_Draw();
	DrawRocket();
	BattleBossControllerDraw();
	if (g_NowBattleState == BattleSystemState::PLAYER_FIGHT)
	{
		switch (g_NowBoss)
		{
		case BattleBossState::CLEANER:
			DrawEYES();
			DrawEyeMiniGame_Timer();
			break;
		case BattleBossState::BOOMBER:
			DrawBombLine();
			break;
		}
	}
	BattleGroundDraw();
	Bullet_Draw();
	BattlePlayerDraw();
	if (g_NowBattleState == BattleSystemState::DIALOGUE)
	{
		DialogueUIManagerBattleDraw();
	}
	EffectDrawUP();
	DrawDebugMissileLine();
	DrawMissiles();
	BattleHint_Draw();
}

void Battle_State_Initialize(BattleSystemState state)
{
	switch (state)
	{
	case BattleSystemState::PLAYER_MOVE:
		break;
	case BattleSystemState::PLAYER_FIGHT:
		switch (g_NowBoss)
		{
		case BattleBossState::CLEANER:
			BattleBossEyeMiniGameTimerReset();
			GenerateCleanerEyePos();
			break;
		case BattleBossState::BOOMBER:
			Generate_BombLiner();
			break;
		default:
			break;
		}
		break;
	case BattleSystemState::PLAYER_SHOOT:
		ResetBossHitCount();
		break;
	case BattleSystemState::BOSS_DESIDE:
		switch (g_NowBoss)
		{
		case BattleBossState::CLEANER:
			switch (GetCleanerHealth())
			{
			case 6:
				BossAttack_CreateBullet(0, 5000);
				BossAttack_CreateBullet(1, 5000);
				BossAttack_CreateBullet(2, 5000);
				break;
			case 5:
				BossAttack_CreateBullet(0, 1000);
				break;
			case 4:
				BossAttack_CreateBullet(0, 1000);
				BossAttack_CreateBullet(1, 1000);
				break;
			case 3:
				BossAttack_CreateBullet(0, 1000);
				BossAttack_CreateBullet(1, 1000);
				BossAttack_CreateBullet(2, 2000);
				break;
			case 2:
				BossAttack_CreateSweap();
				BossAttack_CreateBullet(0, 1000);
				break;
			case 1:
				BossAttack_CreateBullet(0, 1000);
				BossAttack_CreateBullet(1, 1000);
				BossAttack_CreateSweap();
				break;
			case 0:
				BossAttack_CreateBullet(0, 1000);
				BossAttack_CreateBullet(1, 2000);
				BossAttack_CreateBullet(2, 3000);
				BossAttack_CreateSweap();
				break;
			default:
				break;
			}
			break;
		case BattleBossState::BOOMBER:
			switch (GetBoomberHealth())
			{
			case 8:
				GenerateMineAttack(30);
				break;
			case 7:
				GenerateMineAttack(30);
				break;
			case 6:
				StartMissilePack();
				break;
			case 5:
				StartMissilePack();
				break;
			case 4:
				StartRocketFullLuncher(5);
				break;
			case 3:
				StartRocketFullLuncher(6);
				break;
			case 2:
				StartRocketFullLuncher(7);
				break;
			case 1:
				StartRocketFullLuncher(8);
				break;
			case 0:
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		break;
	case BattleSystemState::CLEANERDEAD:
		CleanerDeadAnimationStart();
		break;
	case BattleSystemState::PLAYERWIN:
		Fade_Start(0.1, true);
		break;
		break;
	}
}

void Battle_State_Finitialize(BattleSystemState state)
{
}

void Battle_State_Update(BattleSystemState state, float time)
{
	switch (state)
	{
	case BattleSystemState::DIALOGUE:
		if (KeyLogger_IsTrigger(KK_Y))
		{
			Battle_State_ChangeState(BattleSystemState::PLAYER_MOVE);
		}
		if (KeyLogger_IsTrigger(KK_SPACE))
		{
			bool end = DialogueManager_To_NextLine();
			if (end)
			{
				switch (g_NowPlot)
				{
				case 0:
					Battle_State_ChangeState(BattleSystemState::BOSS_DESIDE);
					g_NowPlot++;
					break;
				case 1:
					Battle_State_ChangeState(BattleSystemState::CLEANERDEAD);
					g_NowPlot++;
					break;
				case 2:
					Battle_Stage_ChangeBoss();
					break;
				default:
					break;
				}
			}
		}
		break;
	case BattleSystemState::PLAYER_MOVE:
		ButtonDragUpdate();
		break;
	case BattleSystemState::PLAYER_FIGHT:
		ButtonDragUpdate();
		switch (g_NowBoss)
		{
		case BattleBossState::CLEANER:
			BattleBossEyeMiniGameTimerUpdate(time);
			EyeUpdate();
			break;
		case BattleBossState::BOOMBER:
			BombLinerUpdate(time);
			break;
		}
		break;
	case BattleSystemState::PLAYER_DEAD:
		if (KeyLogger_IsTrigger(KK_R))
		{
			BattleSystem_Finitialize();
			BattleSystem_Initialize();
		}
		break;
	case BattleSystemState::PLAYERWIN:
		if (KeyLogger_IsTrigger(KK_R))
		{
			Fade_Start(0.1f, false);
			Scene_Change(SCENE_TITLE);
		}
		break;
	}
}

void Battle_State_ChangeState(BattleSystemState state)
{
	if (g_NowBattleState != state)
	{
		Battle_State_Finitialize(g_NowBattleState);
		g_NowBattleState = state;
		Battle_State_Initialize(g_NowBattleState);
	}
}

void Battle_Stage_ChangeBoss()
{
	UnloadAudio(g_CleanerBattleBGM_id);
	g_NowBoss = BattleBossState::BOOMBER;
	BattleBoss_BomberInit();
	Battle_State_ChangeState(BattleSystemState::BOSS_DESIDE);
}

BattleSystemState GetBattleState()
{
	return g_NowBattleState;
}

BattleBossState GetBattleBoss()
{
	return g_NowBoss;
}


