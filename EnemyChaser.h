#ifndef ENEMYCHASER_H
#define ENEMYCHASER_H

#include "DirectXMath.h"
#include "Collision.h"

constexpr float g_Enemy_size = 100.0f;
constexpr float g_eEnemySpeed = 250.0f;

enum class ChaseEnemyAnimationState
{
	FRONT,
	LEFT,
	RIGHT,
	STOPPED,
};

void EnemyChaser_Initialize(const DirectX::XMFLOAT2& position);
void EnemyChaser_Finitialize();
void EnemyChaserUpdate(float time);
void EnemyChaserDraw();
Box EnemyChaser_GetCollision();
void EnableChaser();
void EnemyChaserReset(const DirectX::XMFLOAT2& position);
void PlayEnemyChaseBGM();
#endif
