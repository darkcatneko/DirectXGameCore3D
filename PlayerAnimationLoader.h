#ifndef PLAYERANIMATIONLOADER_H
#define PLAYERANIMATIONLOADER_H
#include "DirectXMath.h"

constexpr float g_Player_size = 100.0f;
extern int g_PlayerTexId;
extern int g_PlayerDialogueHintBlock;
extern int g_PlayerAnimPatternId_F_I;
extern int g_PlayerAnimPatternId_F_M;
extern int g_PlayerAnimPatternId_R_I;
extern int g_PlayerAnimPatternId_R_M;
extern int g_PlayerAnimPatternId_L_I;
extern int g_PlayerAnimPatternId_L_M;
extern int g_PlayerAnimPatternId_B_I;
extern int g_PlayerAnimPatternId_B_M;

extern int g_PlayerAnimPlayerId_F_I;
extern int g_PlayerAnimPlayerId_F_M;
extern int g_PlayerAnimPlayerId_R_I;
extern int g_PlayerAnimPlayerId_R_M;
extern int g_PlayerAnimPlayerId_L_I;
extern int g_PlayerAnimPlayerId_L_M;
extern int g_PlayerAnimPlayerId_B_I;
extern int g_PlayerAnimPlayerId_B_M;
void PlayerAnimationLoad_Initialize(const DirectX::XMFLOAT2& position);

#endif
