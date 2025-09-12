#ifndef BULLET_H
#define BULLET_h
#include "DirectXMath.h"
#include "Collision.h"

static constexpr unsigned int BULLET_MAX = 1024;

void Bullet_Initialize();
void Bullet_Finitialize();

void Bullet_Update(double elapsed_time);
void Bullet_Draw();

void Bullet_Shooter(DirectX::XMFLOAT2& position, DirectX::XMFLOAT2& aimPoint);//主要改造點

bool Bullet_IsEnable(int index);
Circle Bullet_GetCollision(int index);
Box Bullet_GetCollision_Box(int index);
void Bullet_Destroy(int index);
#endif
