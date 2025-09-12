#ifndef PLAYERUPDATER_H
#define PLAYERUPDATER_H
#include "DirectXMath.h"

enum  Player_AnimationState
{
	IDLE,
	WALKING,
};

enum Facing_Directing
{
	FRONT,
	RIGHT,
	LEFT,
	BACK,
};
void PlayerUpdater_Update(float time, bool* playerEnable, DirectX::XMFLOAT2* playerPos, DirectX::XMFLOAT2* playerVelocity, Facing_Directing& facingDirection, Player_AnimationState& animationState);
void PlayerUpdater_Dialogue_Update(float time, bool* playerEnable, DirectX::XMFLOAT2* playerPos, DirectX::XMFLOAT2* playerVelocity, Facing_Directing& facingDirection, Player_AnimationState& animationState);
void PlayerFacingDirectionChanger(Facing_Directing& target, Facing_Directing to, Player_AnimationState& state);
bool PlayerGameObjectCollisionCheck();
void ChangeSceneSize(int width, int height);
#endif
