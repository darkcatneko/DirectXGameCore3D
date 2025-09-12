#include "BattleHint.h"
#include "Texture.h"
#include "sprite.h"
#include "BattleSystem.h"

static int g_DragHint_TexId = -1;
static int g_DodgeHint_TexId = -1;
static int g_ShootHint_TexId = -1;

void BattleHint_Initialize()
{
	g_DragHint_TexId = Texture_Load(L"DragHint.png");
	g_DodgeHint_TexId = Texture_Load(L"DodgeHint.png");
	g_ShootHint_TexId = Texture_Load(L"ShootHint.png");
}

void BattleHint_Finalize()
{
}

void BattleHint_Update(double time)
{
}

void BattleHint_Draw()
{
	
	switch (GetBattleBoss())
	{
	case BattleBossState::CLEANER:
		switch (GetBattleState())
		{
		case BattleSystemState::PLAYER_FIGHT:
			Sprite_Draw(g_DragHint_TexId, 602, 25, 391, 65);
			break;
		case BattleSystemState::PLAYER_SHOOT:
			Sprite_Draw(g_ShootHint_TexId, 602, 25, 391, 65);
			break;
		case BattleSystemState::BOSS_DESIDE:
			Sprite_Draw(g_DodgeHint_TexId, 706, 25, 184, 65);
		default:
			break;
		}
		break;
	case BattleBossState::BOOMBER:
		switch (GetBattleState())
		{
		case BattleSystemState::PLAYER_FIGHT:
			//Sprite_Draw(g_DragHint_TexId, 602, 25, 391, 65);
			break;
		case BattleSystemState::PLAYER_SHOOT:
			Sprite_Draw(g_ShootHint_TexId, 602, 25, 391, 65);
			break;
		case BattleSystemState::BOSS_DESIDE:
			Sprite_Draw(g_DodgeHint_TexId, 706, 25, 184, 65);
		default:
			break;
		}
		break;
		break;
	default:
		break;
	}
}
