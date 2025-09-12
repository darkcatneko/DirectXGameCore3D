#include "PlayerUpdater.h"
#include "DirectXMath.h"
#include "Camera.h"
#include "Key_Logger.h"
#include "NekoTool.h"
#include "direct3d.h"
#include "bullet.h"
#include "Score.h"
#include "CollisionGameObject.h"
#include "Player.h"
#include "MapCollisionInteraction.h"
#include "TriggerCollider.h"
#include "PlayerAnimationLoader.h"
#include "DialogueUIManager.h"
#include "Fade.h"
#include "Game.h"
using namespace DirectX;

static XMFLOAT2 beforeMove;
static XMFLOAT2 afterMove;
static int Scene_Max_Width = 1920;
static int Scene_Max_Height = 1080;

void PlayerUpdater_Update(float time, bool* playerEnable, XMFLOAT2* playerPos, XMFLOAT2* playerVelocity, Facing_Directing& facingDirection, Player_AnimationState& animationState)
{
	if (!playerEnable)return;
	XMVECTOR position = XMLoadFloat2(playerPos);
	XMVECTOR cam_pos = XMLoadFloat2(&Camera_Vec);
	XMVECTOR velocity = XMLoadFloat2(playerVelocity);

	XMVECTOR direction{};
	if (KeyLogger_IsPressed(KK_W))
	{
		direction += {0.0f, -1.0f};
		PlayerFacingDirectionChanger(facingDirection, FRONT, animationState);
	}
	if (KeyLogger_IsPressed(KK_S))
	{
		direction += {0.0f, 1.0f};
		PlayerFacingDirectionChanger(facingDirection, BACK, animationState);
	}
	if (KeyLogger_IsPressed(KK_A))
	{
		direction += {-1.0f, 0.0f};
		PlayerFacingDirectionChanger(facingDirection, LEFT, animationState);
	}
	if (KeyLogger_IsPressed(KK_D))
	{
		direction += {1.0f, 0.0f};
		PlayerFacingDirectionChanger(facingDirection, RIGHT, animationState);
	}
	switch (Get_GameScene())
	{
	case 0:
		if (checkTriggerHintBlock() == 0 && KeyLogger_IsTrigger(KK_E))
		{
			PlayerStateChange(Player_Status::Dialogue);
		}
		if (checkTriggerHintBlock() == 1 && KeyLogger_IsTrigger(KK_E))
		{
			Fade_Start(1.0, true);
			SetTriggerColliderActive(0, 1, false);
		}
		if (KeyLogger_IsTrigger(KK_O))
		{
			Fade_Start(1.0, true);
		}
		break;
	case 1:
		if (checkTriggerHintBlock() == 4 && KeyLogger_IsTrigger(KK_E))
		{
			SetTriggerColliderActive(1, 4,false);
			playerPos->x = 1400.0f;
			playerPos->y = 1200.0f;
			resetCam(-600, -600);
			Change_Player_Draw_Layer(Player_Layer::ROOF);
			SetTriggerColliderActive(1, 5, true);
		}
		if (checkTriggerHintBlock() == 5 && KeyLogger_IsTrigger(KK_E))
		{
			Fade_Start(1.0, true);
		}
		if ( KeyLogger_IsTrigger(KK_J))
		{
			Fade_Start(1.0, true);
		}
		break;
	}	
	XMVECTOR lengthVec = XMVector4Length(direction);
	float length;
	DirectX::XMStoreFloat(&length, lengthVec);

	if (length == 0)
	{
		animationState = IDLE;
	}
	direction = XMVector2Normalize(direction);
	/*velocity += direction * 6000000.0f / 50000.0f * static_cast<float>(time);
	position += velocity;
	velocity += -velocity * 4.0f * time;*/
	velocity = direction * 5.0f;

	XMVECTOR Before_position = position;//做移動前的位置備分
	position += velocity;//移動
	DirectX::XMStoreFloat2(&afterMove, position);//移動後的位置
	//運算
	if (PlayerGameObjectCollisionCheck() == false)
	{
		DirectX::XMStoreFloat2(playerPos, position);
		playerPos->x = clamp<float>(playerPos->x, 0.0, Scene_Max_Width - g_Player_size);
		playerPos->y = clamp<float>(playerPos->y, 0.0, Scene_Max_Height - g_Player_size);
		if (playerPos->x > (float)Direct3D_GetBackBufferWidth() / 2 && playerPos->x < Scene_Max_Width - (float)Direct3D_GetBackBufferWidth() / 2)
		{
			float cam_x = XMVectorGetX(cam_pos);
			float vel_x = XMVectorGetX(velocity);
			cam_pos = XMVectorSetX(cam_pos, cam_x - vel_x);
		}
		else if (playerPos->x < (float)Direct3D_GetBackBufferWidth() / 2)
		{
			cam_pos = XMVectorSetX(cam_pos, 0);
		}
		else if (playerPos->x > Scene_Max_Width - (float)Direct3D_GetBackBufferWidth() / 2)
		{
			cam_pos = XMVectorSetX(cam_pos, (Scene_Max_Width - (float)Direct3D_GetBackBufferWidth()) * -1);
		}
		if (playerPos->y > (float)Direct3D_GetBackBufferHeight() / 2 && playerPos->y < Scene_Max_Height - (float)Direct3D_GetBackBufferHeight() / 2)
		{
			float cam_y = XMVectorGetY(cam_pos);
			float vel_y = XMVectorGetY(velocity);
			cam_pos = XMVectorSetY(cam_pos, cam_y - vel_y);
		}
		else if ((playerPos->y < (float)Direct3D_GetBackBufferHeight() / 2))
		{
			cam_pos = XMVectorSetY(cam_pos, 0);
		}
		else  if (playerPos->y > Scene_Max_Height - (float)Direct3D_GetBackBufferHeight() / 2)
		{
			cam_pos = XMVectorSetY(cam_pos, (Scene_Max_Height - (float)Direct3D_GetBackBufferHeight()) * -1);
		}

		DirectX::XMStoreFloat2(&Camera_Vec, cam_pos);
		changeCameraPos(Camera_Vec.x, Camera_Vec.y);

	}
	//無撞到則成立

	velocity *= 0.9f;

	DirectX::XMStoreFloat2(playerVelocity, velocity);

	/*if (KeyLogger_IsTrigger(KK_SPACE))
	{
		Bullet_Shooter(*playerPos);
		Score_AddScore(1);
	}*/
}

void PlayerUpdater_Dialogue_Update(float time, bool* playerEnable, DirectX::XMFLOAT2* playerPos, DirectX::XMFLOAT2* playerVelocity, Facing_Directing& facingDirection, Player_AnimationState& animationState)
{
	if (KeyLogger_IsTrigger(KK_SPACE))
	{
		bool to_next = DialogueManager_To_NextLine();
		if (to_next)
		{
			PlayerStateChange(Player_Status::Gaming);
		}
	}
	if (KeyLogger_IsTrigger(KK_J))
	{
		DialogueManager_Skip();
		PlayerStateChange(Player_Status::Gaming);

	}

}

void PlayerFacingDirectionChanger(Facing_Directing& target, Facing_Directing to, Player_AnimationState& state)
{
	state = WALKING;
	target = to;
}

bool PlayerGameObjectCollisionCheck()
{
	Box playerCollider = Player_GetBoxCollision();
	playerCollider.center.x = afterMove.x + playerCollider.half_width;
	playerCollider.center.y = afterMove.y + playerCollider.half_height;
	int target_collider_data = 0;
	if (GetPlayerLayer() == Player_Layer::GROUND)
	{
		target_collider_data = Get_GameScene();
	}
	else if (GetPlayerLayer() == Player_Layer::ROOF)
	{
		target_collider_data = 2;
	}

	for (int i = 0; i < GetCollisionObjectSceneCount(target_collider_data); i++)
	{
		if (OnGameObjectCollisionEnter(playerCollider, CollisionGameObject_GetGameobjectCollision(target_collider_data,i)))
		{
			return true;
		}
	}
	return false;
}

void ChangeSceneSize(int width, int height)
{
	Scene_Max_Width = width;
	Scene_Max_Height = height;
}


