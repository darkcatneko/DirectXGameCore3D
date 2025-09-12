#ifndef BATTLESYSTEM_H
#define BATTLESYSTEM_H

enum class BattleSystemState
{
	DIALOGUE,
	PLAYER_MOVE,
	PLAYER_FIGHT,
	PLAYER_SHOOT,
	BOSS_DESIDE,
	PLAYER_DEAD,
	CLEANERDEAD,
	PLAYERWIN,
};
enum class BattleBossState
{
	CLEANER,
	BOOMBER,
};
void Battle_Stage_ChangeBoss();
void BattleSystem_Initialize();
void BattleSystem_Finitialize();
void BattleSystemUpdate(float time);
void BattleSystemDraw();

void Battle_State_Initialize(BattleSystemState state);
void Battle_State_Finitialize(BattleSystemState state);
void Battle_State_Update(BattleSystemState state, float time);
void Battle_State_ChangeState(BattleSystemState state);
BattleSystemState GetBattleState();
BattleBossState GetBattleBoss();
#endif // !BATTLESYSTEM_H

