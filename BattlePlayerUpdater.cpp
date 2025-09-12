#include "BattlePlayerUpdater.h"
#include "DirectXMath.h"
#include "Key_Logger.h"
#include "NekoTool.h"
#include "BattleGround.h"
#include "BattlePlayer.h"
#include "Background.h"
#include "BattleSystem.h"
#include "bullet.h"
#include "MouseRenderer.h"
using namespace DirectX;

static XMFLOAT2 beforeMove;
static XMFLOAT2 afterMove;
static int Scene_Max_Width = 1600;
static int Scene_Max_Height = 900;


void BattlePlayerUpdater_Update(float time, bool* playerEnable, DirectX::XMFLOAT2* playerPos, DirectX::XMFLOAT2* playerVelocity)
{
	if (!playerEnable)return;
	XMVECTOR position = XMLoadFloat2(playerPos);
	XMVECTOR velocity = XMLoadFloat2(playerVelocity);
	XMVECTOR direction{};
	if (KeyLogger_IsPressed(KK_W))
	{
		direction += {0.0f, -1.0f};
	}
	if (KeyLogger_IsPressed(KK_S))
	{
		direction += {0.0f, 1.0f};
	}
	if (KeyLogger_IsPressed(KK_A))
	{
		direction += {-1.0f, 0.0f};
	}
	if (KeyLogger_IsPressed(KK_D))
	{
		direction += {1.0f, 0.0f};
	}
	if (GetBattleState() == BattleSystemState::PLAYER_SHOOT)
	{
		if (MouseLogger_IsTrigger(0))
		{
			XMFLOAT2 temp = { playerPos->x,playerPos->y };
			Mouse_State mouse;
			Mouse_GetState(&mouse);
			XMFLOAT2 aim = { mouse.x * 1.0f, mouse.y * 1.0f };
			Bullet_Shooter(temp, aim);
		}
	}
	XMVECTOR lengthVec = XMVector4Length(direction);
	float length;
	DirectX::XMStoreFloat(&length, lengthVec);

	direction = XMVector2Normalize(direction);
	velocity += direction * 6000000.0f / 150000.0f * static_cast<float>(time);
	velocity += -velocity * 4.0f * time;
	//velocity = direction * 5.0f;

	XMVECTOR Before_position = position;//做移動前的位置備分
	position += velocity * 2;//移動
	DirectX::XMStoreFloat2(&afterMove, position);//移動後的位置
	if (BattlePlayerGameObjectCollisionCheck(&afterMove).collide == false)
	{
		DirectX::XMStoreFloat2(playerPos, position);
		playerPos->x = clamp<float>(playerPos->x, 0.0, Scene_Max_Width - g_BattlePlayer_size);
		playerPos->y = clamp<float>(playerPos->y, 0.0, Scene_Max_Height - g_BattlePlayer_size);
	}
	else if (BattlePlayerGameObjectCollisionCheck(&afterMove).collide == true)
	{
		velocity *= 0;
	}
	//無撞到則成立
	velocity *= 0.9f;

	DirectX::XMStoreFloat2(playerVelocity, velocity);
}
OnCollide BattlePlayerGameObjectCollisionCheck(DirectX::XMFLOAT2* playerPos)
{
	OnCollide result = { false,{0,0},{0,0} };
	XMFLOAT2 pointA = { 0,0 };
	XMFLOAT2 pointB = { 0,0 };
	/*DirectX::XMFLOAT2 playerColliderBase[] =
	{
		{playerPos->x                      ,playerPos->y},
		{playerPos->x + g_BattlePlayer_size/2,playerPos->y},
		{playerPos->x + g_BattlePlayer_size,playerPos->y},
		{playerPos->x + g_BattlePlayer_size,playerPos->y + g_BattlePlayer_size/2},
		{playerPos->x + g_BattlePlayer_size,playerPos->y + g_BattlePlayer_size},
		{playerPos->x + g_BattlePlayer_size/2,playerPos->y + g_BattlePlayer_size},
		{playerPos->x                      ,playerPos->y + g_BattlePlayer_size},
		{playerPos->x                      ,playerPos->y + g_BattlePlayer_size/2},
	};
	for (int i = 0; i < 8; i++)
	{
		if (PointInQuad(GetBattleGroundColliderData(0), playerColliderBase[i], 1e-6f))
		{
			result = true;
		}
	}*/
	float cx = BattlePlayer_GetCollision_Circle().radius + playerPos->x;
	float cy = BattlePlayer_GetCollision_Circle().radius + playerPos->y;



	Circle targetCircle = { {cx,cy}, BattlePlayer_GetCollision_Circle().radius };
	for (int i = 0; i < MAX_BATTLEGROUND_OBJECT; i++)
	{
		if (CircleCompletelyOutsideQuad(GetBattleGroundColliderData(i), targetCircle).collide == false)
		{
			result.collide = true;
			result.pointA = CircleCompletelyOutsideQuad(GetBattleGroundColliderData(i), targetCircle).pointA;
			result.pointB = CircleCompletelyOutsideQuad(GetBattleGroundColliderData(i), targetCircle).pointB;
		}

	}
	return result;
}




