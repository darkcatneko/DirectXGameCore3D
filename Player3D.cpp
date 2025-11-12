#include "Player3D.h"
#include "model.h"
#include "key_logger.h"
#include "Light.h"
#include "camera.h"
#include "PlayerCamera.h"
#include "DirectXMath.h"
#include "Map.h"
#include "Cube.h"
using namespace DirectX;

namespace {
	DirectX::XMFLOAT3 g_PlayerPosition = {};
	DirectX::XMFLOAT3 g_PlayerFront = { 0.0f,0.0f,1.0f };
	DirectX::XMFLOAT3 g_PlayerVelocity = {};
	MODEL* g_pPlayerModel = nullptr;
	bool g_IsJump = false;
}

void Player3D_Initialize(const DirectX::XMFLOAT3 position, const DirectX::XMFLOAT3 front)
{
	g_PlayerPosition = position;
	g_PlayerVelocity = { 0.0f,0.0f,0.0f };
	DirectX::XMStoreFloat3(&g_PlayerFront, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)));
	g_pPlayerModel = ModelLoad("test.fbx", 0.1f,false);
}

void Player3D_Finalize()
{
	ModelRelease(g_pPlayerModel);
}

void  Player3D_Update(double elapsed_time)
{
	XMVECTOR player_pos = DirectX::XMLoadFloat3(&g_PlayerPosition);
	XMVECTOR player_velocity = DirectX::XMLoadFloat3(&g_PlayerVelocity);
	XMVECTOR gravity_velocity = {};

	if (KeyLogger_IsTrigger(KK_SPACE) && !g_IsJump) {
		player_velocity += {0.0f, 10.0f, 0.0f};
		g_IsJump = true;
	}

	//重力落下
	DirectX::XMVECTOR gdir{ 0.0f,1.0f,0.0f };
	player_velocity += gdir * -9.8f * 1.0f * (float)(elapsed_time);
	gravity_velocity = player_velocity * (float)elapsed_time;
	player_pos += gravity_velocity;

	DirectX::XMStoreFloat3(&g_PlayerPosition, player_pos);
	AABB player = GetPlayer_AABB();
	for (int i = 0; i < Map_GetObjectsCount(); i++)
	{

		AABB Object = Map_GetObjects(i)->Collision;

		//被重力拖的物件有被撞到嗎
		Hit hit = Collision_IsHitAABB(Object, player);
		if (hit.isHit)
		{
			if (hit.normal.y > 0.0f)
			{
				//player_pos -= gravity_velocity;
				player_pos = XMVectorSetY(player_pos, Object.max.y+0.5f);
				player_velocity *= { 1.0f, 0.0f, 1.0f};
				g_IsJump = false;
			}
		}
	}
	
	//有撞到地面嗎
	if (DirectX::XMVectorGetY(player_pos) <= 0.0f)
	{
		player_pos = XMVectorSetY(player_pos, 0.0f);
		player_velocity *= { 1.0f, 0.0f, 1.0f};
		g_IsJump = false;
	}

	


	DirectX::XMVECTOR direction{};
	DirectX::XMVECTOR front = DirectX::XMLoadFloat3(&GetPlayerFront()) * XMVECTOR{1,0,1};

	if (KeyLogger_IsPressed(KK_W)) {
		direction += front;
	}

	if (KeyLogger_IsPressed(KK_S)) {
		direction -= front;
	}

	if (KeyLogger_IsPressed(KK_D)) {
		direction += DirectX::XMVector3Cross({ 0.0f,1.0f,0.0f }, front);
	}

	if (KeyLogger_IsPressed(KK_A)) {
		direction -= DirectX::XMVector3Cross({ 0.0f,1.0f,0.0f }, front);
	}


	direction = DirectX::XMVector3Normalize(direction);

	player_velocity += direction * (float)(2000.0 / 50.0 * elapsed_time);

	player_velocity += -player_velocity *(float)(4.0*elapsed_time);
	player_pos += player_velocity * (float)elapsed_time;

	for (int i = 0; i < Map_GetObjectsCount(); i++)
	{
		AABB Object = Cube_GetAABB(Map_GetObjects(i)->Position);
		Hit hit = Collision_IsHitAABB(Object, player);
		//撞擊判定
		if (hit.isHit)
		{
			if (hit.normal.x > 0.0f)
			{
				//player_pos -= gravity_velocity;
				player_pos = XMVectorSetX(player_pos, Object.max.x + 0.5f);
				player_velocity *= { 0.0f, 1.0f, 1.0f};
			}
			else if (hit.normal.x < 0.0f)
			{
				player_pos = XMVectorSetX(player_pos, Object.min.x - 0.5f);
				player_velocity *= { 0.0f, 1.0f, 1.0f};
			}
			else if (hit.normal.y < 0.0f)
			{
				player_pos = XMVectorSetY(player_pos, Object.min.y - 0.5f);
				player_velocity *= { 1.0f, 0.0f, 1.0f};
			}
			else if (hit.normal.z > 0.0f)
			{
				player_pos = XMVectorSetZ(player_pos, Object.max.z + 0.5f);
				player_velocity *= { 1.0f, 1.0f, 0.0f};
			}
			else if (hit.normal.z < 0.0f)
			{
				player_pos = XMVectorSetZ(player_pos, Object.min.z - 0.5f);
				player_velocity *= { 1.0f, 1.0f, 0.0f};
			}
		}

		DirectX::XMStoreFloat3(&g_PlayerPosition, player_pos);
		DirectX::XMStoreFloat3(&g_PlayerVelocity, player_velocity);
	}
}

void Player3D_Draw()
{
	Light_SetSpecularWorld({ 0.1f,0.1f,0.1f,0.1f }, 10.0f, PlayerCamera_GetCameraPos());//镜面反射光
	DirectX::XMMATRIX world = DirectX::XMMatrixTranslation(
		g_PlayerPosition.x,
		g_PlayerPosition.y,
		g_PlayerPosition.z
	);
	ModelDraw(g_pPlayerModel, g_PlayerPosition);
	Map_Draw();
	//Cube_Draw(g_PlayerPosition);
}

const DirectX::XMFLOAT3& GetPlayerPosition()
{
	return g_PlayerPosition;
}

const DirectX::XMFLOAT3 &GetPlayerFront()
{
	return g_PlayerFront;
}

AABB GetPlayer_AABB()
{
	return {
		{g_PlayerPosition.x + 0.5f, g_PlayerPosition.y + 0.5f,g_PlayerPosition.z + 0.5f},
		{g_PlayerPosition.x - 0.5f, g_PlayerPosition.y - 0.5f ,g_PlayerPosition.z - 0.5f}
	};
}

AABB Player_ConvertPositionToAABB(const DirectX::XMVECTOR position)
{
	AABB aabb;
	XMStoreFloat3(&aabb.min, position - XMVECTOR{ 0.5f,0.5f,0.5f });
	XMStoreFloat3(&aabb.max, position + XMVECTOR{ 0.5f,0.5f,0.5f });
	return aabb;
}
