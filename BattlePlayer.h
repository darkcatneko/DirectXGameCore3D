#ifndef BATTLEPLAYER_H
#define BATTLEPLAYER_H
#include <DirectXMath.h>
#include "Collision.h"

void BattlePlayer_Initialize(const DirectX::XMFLOAT2& position);
void BattlePlayer_Finitialize();
void BattlePlayerUpdate(float time);
void BattlePlayerDraw();
Box BattlePlayer_GetCollision();
Circle BattlePlayer_GetCollision_Circle();
void OnPlayerHit();
void PlayerHitEffectUpdate(float time);
int GetPlayerHealth();
DirectX::XMFLOAT2 GetPlayerPos();
DirectX::XMFLOAT2 GetPlayerVel();
#endif
