#ifndef ENEMY_H
#define ENEMY_h
#include "DirectXMath.h"
#include "Collision.h"

static constexpr unsigned int ENEMY_MAX = 256;
void Enemy_Initialize();
void Enemy_Finitialize();

void Enemy_Update(double elapsed_time);
void Enemy_Draw();

enum EnemyTypeId :int
{
	bruh,
	cat,
};

void Enemy_Creater(EnemyTypeId id, DirectX::XMFLOAT2& position, DirectX::XMFLOAT2 offset);//主要改造點

bool Enemy_IsEnable(int index);
Circle Enemy_GetCollision(int index);

void Enemy_Destroy(int index);

void Enemy_Hit(int damage, int index);
#endif
#pragma once
