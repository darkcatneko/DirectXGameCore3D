#include "PlayerAnimationLoader.h"
#include "Texture.h"
#include "sprite_anim.h"



int g_PlayerTexId = -1;
int g_PlayerDialogueHintBlock = -1;
int g_PlayerAnimPatternId_F_I = -1;
int g_PlayerAnimPatternId_F_M = -1;
int g_PlayerAnimPatternId_R_I = -1;
int g_PlayerAnimPatternId_R_M = -1;
int g_PlayerAnimPatternId_L_I = -1;
int g_PlayerAnimPatternId_L_M = -1;
int g_PlayerAnimPatternId_B_I = -1;
int g_PlayerAnimPatternId_B_M = -1;

int g_PlayerAnimPlayerId_F_I = -1;
int g_PlayerAnimPlayerId_F_M = -1;
int g_PlayerAnimPlayerId_R_I = -1;
int g_PlayerAnimPlayerId_R_M = -1;
int g_PlayerAnimPlayerId_L_I = -1;
int g_PlayerAnimPlayerId_L_M = -1;
int g_PlayerAnimPlayerId_B_I = -1;
int g_PlayerAnimPlayerId_B_M = -1;

void PlayerAnimationLoad_Initialize(const DirectX::XMFLOAT2& position)
{
	//g_PlayerTexId = Texture_Load(L"kokosozai.png");
	g_PlayerDialogueHintBlock = Texture_Load(L"DialogueTester.png");
	g_PlayerAnimPatternId_F_I = AnimDataInitialize(L"M_Q1_backwalk_0000.png", 1, 1, 0, 0, { 0,0 }, { 0.25f,1 }, { g_Player_size,g_Player_size }, true, 0.1f, 1.0f);
	g_PlayerAnimPatternId_F_M = AnimDataInitialize(L"M_Q1_backwalk_0000.png", 4, 4, 0, 0, { 0,0 }, { 0.25f,1 }, { g_Player_size,g_Player_size }, true, 0.1f, 1.0f);
	g_PlayerAnimPatternId_L_I = AnimDataInitialize(L"M_Q1_sidewalk_L_0000.png", 1, 1, 0, 0, { 0,0 }, { 0.25f,1 }, { g_Player_size,g_Player_size }, true, 0.1f, 1.0f);
	g_PlayerAnimPatternId_L_M = AnimDataInitialize(L"M_Q1_sidewalk_L_0000.png", 4, 4, 0, 0, { 0,0 }, { 0.25f,1 }, { g_Player_size,g_Player_size }, true, 0.1f, 1.0f);
	g_PlayerAnimPatternId_R_I = AnimDataInitialize(L"M_Q1_sidewalk_0000.png", 1, 1, 0, 0, { 0,0 }, { 0.25f,1 }, { g_Player_size,g_Player_size }, true, 0.1f, 1.0f);
	g_PlayerAnimPatternId_R_M = AnimDataInitialize(L"M_Q1_sidewalk_0000.png", 4, 4, 0, 0, { 0,0 }, { 0.25f,1 }, { g_Player_size,g_Player_size }, true, 0.1f, 1.0f);
	g_PlayerAnimPatternId_B_I = AnimDataInitialize(L"M_Q1_walk_0000.png", 1, 1, 0, 0, { 0,0 }, { 0.25f,1 }, { g_Player_size,g_Player_size }, true, 0.1f, 1.0f);
	g_PlayerAnimPatternId_B_M = AnimDataInitialize(L"M_Q1_walk_0000.png", 4, 4, 0, 0, { 0,0 }, { 0.25f,1 }, { g_Player_size,g_Player_size }, true, 0.1f, 1.0f);

	g_PlayerAnimPlayerId_F_I = SpriteAnim_CreatePlayer(g_PlayerAnimPatternId_F_I, position);
	g_PlayerAnimPlayerId_F_M = SpriteAnim_CreatePlayer(g_PlayerAnimPatternId_F_M, position);
	g_PlayerAnimPlayerId_L_I = SpriteAnim_CreatePlayer(g_PlayerAnimPatternId_L_I, position);
	g_PlayerAnimPlayerId_L_M = SpriteAnim_CreatePlayer(g_PlayerAnimPatternId_L_M, position);
	g_PlayerAnimPlayerId_R_I = SpriteAnim_CreatePlayer(g_PlayerAnimPatternId_R_I, position);
	g_PlayerAnimPlayerId_R_M = SpriteAnim_CreatePlayer(g_PlayerAnimPatternId_R_M, position);
	g_PlayerAnimPlayerId_B_I = SpriteAnim_CreatePlayer(g_PlayerAnimPatternId_B_I, position);
	g_PlayerAnimPlayerId_B_M = SpriteAnim_CreatePlayer(g_PlayerAnimPatternId_B_M, position);
	//g_PlayerAnimPlayerId = SpriteAnim_CreatePlayer(g_PlayerAnimPatternId, position);
}
