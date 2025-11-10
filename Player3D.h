#ifndef PLAYER3D_H
#define PLAYER3D_H
#include "DirectXMath.h"
#include "Collision3D.h"

void Player3D_Initialize(const DirectX::XMFLOAT3 position, const DirectX::XMFLOAT3 front);
void Player3D_Finitialize();
void Player3D_Update(double elapsed_time);
void Player3D_Draw();

const DirectX::XMFLOAT3& GetPlayerPosition();
const DirectX::XMFLOAT3& GetPlayerFront();
AABB GetPlayer_AABB();
#endif