#ifndef BATTLEPLAYERUPDATER_H
#define BATTLEPLAYERUPDATER_H
#include "DirectXMath.h"
#include "Collision.h"
#include "BattleGround.h"
constexpr int g_BattlePlayer_size = 60;
void BattlePlayerUpdater_Update(float time, bool* playerEnable, DirectX::XMFLOAT2* playerPos, DirectX::XMFLOAT2* playerVelocity);
OnCollide BattlePlayerGameObjectCollisionCheck(DirectX::XMFLOAT2* playerPos);
#endif
