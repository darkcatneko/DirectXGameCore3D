#ifndef PLAYER_H
#define PLAYER_H
#include <DirectXMath.h>
#include "Collision.h"

enum class Player_Status
{
	Gaming,
	Dialogue,
};

void Player_Initialize(const DirectX::XMFLOAT2& position, Player_Status status);
void Player_Finitialize();

void PlayerUpdate(float time);
void PlayerDraw();

bool Player_IsEnable();
Circle Player_GetCollision();
Box Player_GetBoxCollision();


void Player_Destroy();

void Player_State_In(Player_Status status);
void Player_State_Out(Player_Status status);

void PlayerStateChange(Player_Status status);

Player_Status GetPlayerGameStatus();

#endif
