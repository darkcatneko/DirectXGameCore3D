#include "Player.h"
#include <DirectXMath.h>
#include "sprite.h"
#include "Texture.h"
#include "Key_Logger.h"
#include "bullet.h"
#include "direct3d.h"
#include "NekoTool.h"
using namespace DirectX;
#include "Collision.h"
#include "Camera.h"
#include "Score.h"
#include "sprite_anim.h"
#include "PlayerUpdater.h"
#include "PlayerAnimationLoader.h"
#include "TriggerCollider.h"


static const XMFLOAT2 g_DialogueHintBlockOffset = { 60,-15 };

static Player_Status g_PlayerStatus = Player_Status::Gaming;
static XMFLOAT2 g_PlayerPosition{};
static XMFLOAT2 g_PlayerVelocity{};
static Player_AnimationState  g_PlayerAnimationState = IDLE;
static Facing_Directing g_PlayerFacingDirection = RIGHT;
//static int g_PlayerTexId = -1;
//static int g_PlayerAnimPatternId = -1;
//static int g_PlayerAnimPlayerId = -1;
static Circle g_PlayerCollision{ {g_Player_size / 2.0f,g_Player_size / 2.0f},g_Player_size / 2.0f };
static Box g_PlayerBoxCollision{ {g_Player_size / 2.0f,g_Player_size / 2.0f},g_Player_size / 2.0f,g_Player_size / 2.0f };
static bool g_PlayerEnable = true;

float sc_w;
float sc_h;

void Player_Initialize(const XMFLOAT2& position, Player_Status status)
{
	g_PlayerStatus = status;
	g_PlayerPosition = position;
	g_PlayerVelocity = { 0.0f,0.0f };
	//playerload圖片
	g_PlayerEnable = true;
	/*g_PlayerTexId = Texture_Load(L"kokosozai.png");
	g_PlayerAnimPatternId = AnimDataInitialize(L"kokosozai.png", 13, 13, 0, 0, { 0,0 }, { 32.0f / 512.0f,32.0f / 512.0f }, { 64.0f,64.0f }, true);
	g_PlayerAnimPlayerId = SpriteAnim_CreatePlayer(g_PlayerAnimPatternId, position);*/
	PlayerAnimationLoad_Initialize(position);
	sc_w = (float)Direct3D_GetBackBufferWidth();
	sc_h = (float)Direct3D_GetBackBufferHeight();
	PlayerUpdater_Update(0, &g_PlayerEnable, &g_PlayerPosition, &g_PlayerVelocity, g_PlayerFacingDirection, g_PlayerAnimationState);//初始化camera位置
}

void Player_Finitialize()
{
}

void PlayerUpdate(float time)
{
	switch (g_PlayerStatus)
	{
	case Player_Status::Gaming:
		PlayerUpdater_Update(time, &g_PlayerEnable, &g_PlayerPosition, &g_PlayerVelocity, g_PlayerFacingDirection, g_PlayerAnimationState);
		break;
	case Player_Status::Dialogue:
		PlayerUpdater_Dialogue_Update(time, &g_PlayerEnable, &g_PlayerPosition, &g_PlayerVelocity, g_PlayerFacingDirection, g_PlayerAnimationState);
		break;
	default:
		break;
	}
}

void PlayerDraw()
{
	if (!g_PlayerEnable)return;
	//Sprite_Draw(g_PlayerTexId, g_PlayerPosition.x, g_PlayerPosition.y, 64, 64);
	switch (g_PlayerStatus)
	{
	case Player_Status::Gaming:
		break;
	case Player_Status::Dialogue:
		break;
	default:
		break;
	}
	switch (g_PlayerAnimationState)
	{
	case IDLE:
		switch (g_PlayerFacingDirection)
		{
		case FRONT:
			SpriteAnim_Draw(g_PlayerAnimPlayerId_F_I, g_PlayerPosition.x, g_PlayerPosition.y);
			break;
		case RIGHT:
			SpriteAnim_Draw(g_PlayerAnimPlayerId_R_I, g_PlayerPosition.x, g_PlayerPosition.y);
			break;
		case LEFT:
			SpriteAnim_Draw(g_PlayerAnimPlayerId_L_I, g_PlayerPosition.x, g_PlayerPosition.y);
			break;
		case BACK:
			SpriteAnim_Draw(g_PlayerAnimPlayerId_B_I, g_PlayerPosition.x, g_PlayerPosition.y);
			break;
		default:
			break;
		}
		break;
	case WALKING:
		switch (g_PlayerFacingDirection)
		{
		case FRONT:
			SpriteAnim_Draw(g_PlayerAnimPlayerId_F_M, g_PlayerPosition.x, g_PlayerPosition.y);
			break;
		case RIGHT:
			SpriteAnim_Draw(g_PlayerAnimPlayerId_R_M, g_PlayerPosition.x, g_PlayerPosition.y);
			break;
		case LEFT:
			SpriteAnim_Draw(g_PlayerAnimPlayerId_L_M, g_PlayerPosition.x, g_PlayerPosition.y);
			break;
		case BACK:
			SpriteAnim_Draw(g_PlayerAnimPlayerId_B_M, g_PlayerPosition.x, g_PlayerPosition.y);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	if (checkTriggerHintBlock() >= 0)
	{
		Sprite_Draw(g_PlayerDialogueHintBlock, g_PlayerPosition.x + g_DialogueHintBlockOffset.x, g_PlayerPosition.y + g_DialogueHintBlockOffset.y, 64, 32);
	}
#if defined(DEBUG)||defined(_DEBUG)
	Collision_DebugDraw(Player_GetCollision(), { 1.0,1.0,1.0,1.0 });
#endif
}

bool Player_IsEnable()
{
	return g_PlayerEnable;
}



Circle Player_GetCollision()
{
	float cx = g_PlayerCollision.center.x + g_PlayerPosition.x;
	float cy = g_PlayerCollision.center.y + g_PlayerPosition.y;



	return { {cx,cy},g_PlayerCollision.radius };
}
Box Player_GetBoxCollision()
{
	float cx = g_PlayerBoxCollision.center.x + g_PlayerPosition.x;
	float cy = g_PlayerBoxCollision.center.y + g_PlayerPosition.y;



	return { {cx,cy},g_PlayerBoxCollision.half_width, g_PlayerBoxCollision.half_height };
}
void Player_Destroy()
{
	g_PlayerEnable = false;
}

void Player_State_In(Player_Status status)
{
	g_PlayerStatus = status;
}

void Player_State_Out(Player_Status status)
{
	switch (status)
	{
	case Player_Status::Gaming:
		g_PlayerAnimationState = IDLE;
		break;
	case Player_Status::Dialogue:
		break;
	default:
		break;
	}
}

void PlayerStateChange(Player_Status status)
{
	Player_Status past_status = g_PlayerStatus;
	Player_State_Out(past_status);
	Player_State_In(status);
}

Player_Status GetPlayerGameStatus()
{
	return g_PlayerStatus;
}


