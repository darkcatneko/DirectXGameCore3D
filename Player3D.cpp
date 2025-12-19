#include "Player3D.h"
#include "model.h"
#include "key_logger.h"
#include "Light.h"
#include "camera.h"
#include "PlayerCamera.h"
#include "Camera3D.h"
#include "DirectXMath.h"
#include "Map.h"
#include "Cube.h"
#include "Bullet3D.h"
#include "AnimationSystem.h"
#include "circle_shadow.h"
using namespace DirectX;
constexpr float EPSILON = 0.0001f;
namespace {
	DirectX::XMFLOAT3 g_PlayerRespawn = { 0.0f,1.0f,0.0f };

	DirectX::XMFLOAT3 g_PlayerPosition = {};
	DirectX::XMFLOAT3 g_PlayerFront = { 0.0f,0.0f,1.0f };
	DirectX::XMFLOAT3 g_PlayerDirection = {};
	float g_PlayerYaw = XMConvertToRadians(0);
	DirectX::XMFLOAT3 g_PlayerVelocity = {};
	MODEL* g_pPlayerModel = nullptr;
	bool g_IsRun = false;
	bool g_IsJump = false;
	Animator g_Animator;
	std::vector<DirectX::XMMATRIX> g_SkinMatrices;
	Animation g_pPlayerAnimation_Run;
	Animation g_pPlayerAnimation_Idle;
	Animation g_pPlayerAnimation_Jump;
	Animation g_pPlayerAnimation_Throw;
	Animation g_pPlayerAnimation_Fall;

	PlayerStateMachine* g_PlayerStateMachine = nullptr;

	bool isThrowing = false;
	float throwTimer = 0.0f;
}
void Player3D_Movement(float elapsed_time);

void Player3D_Initialize(const DirectX::XMFLOAT3 position, const DirectX::XMFLOAT3 front)
{
	g_PlayerPosition = position;
	g_PlayerRespawn = position;
	g_PlayerVelocity = { 0.0f,0.0f,0.0f };
	DirectX::XMStoreFloat3(&g_PlayerFront, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)));
	g_pPlayerModel = ModelLoad( "FALL.fbx", 1.0f, false);
	g_pPlayerAnimation_Run = ImportAnimation("Run.anim");
	g_pPlayerAnimation_Idle = ImportAnimation("Idle.anim");
	g_pPlayerAnimation_Jump = ImportAnimation("Jump.anim");
	g_pPlayerAnimation_Throw = ImportAnimation("Throw.anim");
	g_pPlayerAnimation_Fall = ImportAnimation("Fall.anim");
	g_Animator.Register("Idle", &g_pPlayerAnimation_Idle);
	g_Animator.Register("Run", &g_pPlayerAnimation_Run,true,0.5f);
	g_Animator.Register("Jump", &g_pPlayerAnimation_Jump, false, 0.42f);
	g_Animator.Register("Throw", &g_pPlayerAnimation_Throw, false,0.5f);
	g_Animator.Register("Fall", &g_pPlayerAnimation_Fall,true, 0.5f);
	g_Animator.Initialize("Idle");

	g_PlayerStateMachine = new PlayerStateMachine();
}

void Player3D_Finalize()
{
	ModelRelease(g_pPlayerModel);
}

void  Player3D_Update(double elapsed_time)
{
	g_PlayerStateMachine->UpdateState();
	g_PlayerStateMachine->Update(elapsed_time);
	if (!isThrowing)
	{
		Player3D_Movement(elapsed_time);
	}
	
	//if (KeyLogger_IsTrigger(KK_N))
	//{
	//	g_Animator.CrossFade(*g_pPlayerModel, g_Animator, "Idle", 0.5f);
	//}if (KeyLogger_IsTrigger(KK_M))
	//{
	//	g_pPlayerModel->animation = g_pPlayerAnimation_Idle;
	//}
	
	// 更新動畫
	g_Animator.Update(g_Animator,*g_pPlayerModel, elapsed_time);  // 會更新每個 bone.finalTransform
	// 組 Skin Matrix 陣列
	BuildSkinMatrices(*g_pPlayerModel, g_SkinMatrices);

	Shader3D_SkinningBegin(g_SkinMatrices);

}
void Player3D_Movement(float elapsed_time)
{
	XMVECTOR player_pos = DirectX::XMLoadFloat3(&g_PlayerPosition);
	XMVECTOR player_velocity = DirectX::XMLoadFloat3(&g_PlayerVelocity);
	XMVECTOR gravity_velocity = {};

	if (KeyLogger_IsTrigger(KK_SPACE) && !g_IsJump) {
		player_velocity += {0.0f, 8.0f, 0.0f};
		g_IsJump = true;
		//g_pPlayerModel->animation = g_pPlayerAnimation_Jump;		
	}

	//重力落下
	DirectX::XMVECTOR gdir{ 0.0f,1.0f,0.0f };
	float gravity = -9.8f;
	if (XMVectorGetY(player_velocity) > 0)
	{
		gravity = -1.8f;
	}
	else
	{
		gravity = -20.8f;
	}
	player_velocity += gdir * gravity * 1.0f * (float)(elapsed_time);
	gravity_velocity = player_velocity * (float)elapsed_time;
	player_pos += gravity_velocity;

	DirectX::XMStoreFloat3(&g_PlayerPosition, player_pos);
	AABB player = GetPlayer_AABB();
	for (int i = 0; i < Map_GetObjectsCount(); i++)
	{
		if (Map_GetObjects(i)->IsTriggered)continue;
		AABB Object = Map_GetObjects(i)->Collision;

		//被重力拖的物件有被撞到嗎
		Hit hit = Collision_IsHitAABB(Object, player);
		if (hit.isHit)
		{
			if (hit.normal.y > 0.0f)
			{
				//player_pos -= gravity_velocity;
				player_pos = XMVectorSetY(player_pos, Object.max.y);
				player_velocity *= { 1.0f, 0.0f, 1.0f};				
				g_IsJump = false;
			}
		}
	}

	//有撞到地面嗎
	if (DirectX::XMVectorGetY(player_pos) <= -0.5f)
	{
		//player_pos = XMVectorSetY(player_pos, -0.5f);
		player_pos =  XMLoadFloat3(&g_PlayerRespawn);
		player_velocity *= { 1.0f, 0.0f, 1.0f};
		g_IsJump = false;
	}




	DirectX::XMVECTOR direction{};
	if (g_IsJump)
	{
		direction = XMLoadFloat3(&g_PlayerDirection);
	}
	auto camFront = Camera_GetFrontVector();
	DirectX::XMVECTOR front = DirectX::XMLoadFloat3(&camFront) * XMVECTOR { 1, 0, 1 };

	if (KeyLogger_IsPressed(KK_I)) {
		direction += front;
		if (!g_IsRun)
		{
			g_IsRun = true;
			//g_Animator.CrossFade(*g_pPlayerModel, g_Animator, "Run", 0.25f);
		}
	}

	if (KeyLogger_IsPressed(KK_K)) {
		direction -= front;
		if (!g_IsRun)
		{
			g_IsRun = true;
			//g_Animator.CrossFade(*g_pPlayerModel, g_Animator, "Run", 0.25f);
		}
	}


	if (KeyLogger_IsPressed(KK_L)) {
		direction += DirectX::XMVector3Cross({ 0.0f,1.0f,0.0f }, front);
		if (!g_IsRun)
		{
			g_IsRun = true;
			//g_Animator.CrossFade(*g_pPlayerModel, g_Animator, "Run", 0.25f);
		}
		//g_PlayerYaw += 2.0f * elapsed_time;
	}

	if (KeyLogger_IsPressed(KK_J)) {
		direction -= DirectX::XMVector3Cross({ 0.0f,1.0f,0.0f }, front);
		if (!g_IsRun)
		{
			//g_IsRun = true; g_Animator.CrossFade(*g_pPlayerModel, g_Animator, "Run", 0.25f);
		}
		//g_PlayerYaw -= 2.0f * elapsed_time;
	}

	if (g_IsJump)
	{
		XMStoreFloat3(&g_PlayerDirection, direction);
	}
	else
	{
		g_PlayerDirection = { 0.0f,0.0f,0.0f };
	}


	direction = DirectX::XMVector3Normalize(direction);

	if (!XMVector3Equal(direction, XMVectorZero()))
	{
		XMFLOAT3 dir;
		XMStoreFloat3(&dir, direction);


		// direction 參考 front = (0,0,1)
		float targetYaw = atan2f(dir.x, dir.z);


		//float delta = targetYaw - g_PlayerYaw;
		float delta = fmodf(targetYaw - g_PlayerYaw + XM_PI, XM_2PI) - XM_PI;

		if (delta > XM_PI)  delta -= XM_2PI;
		if (delta < -XM_PI) delta += XM_2PI;

		float turnSpeed = 10.0f;
		g_PlayerYaw += delta * turnSpeed * (float)elapsed_time;
	}

	XMVECTOR newFront =
		XMVector3Normalize(XMVectorSet(sinf(g_PlayerYaw), 0, cosf(g_PlayerYaw), 0));

	XMStoreFloat3(&g_PlayerFront, newFront);

	player_velocity += direction * (float)(2000.0 / 100.0 * elapsed_time);

	player_velocity += -player_velocity * (float)(7.5 * elapsed_time);

	player_pos += player_velocity * (float)elapsed_time;

	for (int i = 0; i < Map_GetObjectsCount(); i++)
	{
		if (Map_GetObjects(i)->IsTriggered)continue;
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
				player_pos = XMVectorSetY(player_pos, Object.min.y - 1.5f);
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
	Light_SetSpecularWorld({ 0.1f,0.1f,0.1f,0.1f }, 10.0f, Camera_GetCameraPos());//镜面反射光
	DirectX::XMMATRIX world = DirectX::XMMatrixTranslation(
		g_PlayerPosition.x,
		g_PlayerPosition.y,
		g_PlayerPosition.z
	);
	ModelDraw(g_pPlayerModel, new GameObject(g_PlayerPosition, {0,XMConvertToDegrees(g_PlayerYaw)+180,0}, { 0.01f,0.01f,0.01f }));	
	
	//Cube_Draw_Debug({ g_PlayerPosition.x,g_PlayerPosition.y + 0.5f,g_PlayerPosition.z }, { 0,0,0 }, {1,1,1});
	AABB_Draw_Debug(GetPlayer_AABB());
}

const DirectX::XMFLOAT3& GetPlayerPosition()
{
	return g_PlayerPosition;
}

const DirectX::XMFLOAT3& GetPlayerFront()
{
	return g_PlayerFront;
}

AABB GetPlayer_AABB()
{
	return {
		{g_PlayerPosition.x + 0.5f, g_PlayerPosition.y + 1.5f,g_PlayerPosition.z + 0.5f},
		{g_PlayerPosition.x - 0.5f, g_PlayerPosition.y  ,g_PlayerPosition.z - 0.5f}
	};
}

AABB Player_ConvertPositionToAABB(const DirectX::XMVECTOR position)
{
	AABB aabb;
	XMStoreFloat3(&aabb.min, position - XMVECTOR{ 0.5f,0.5f,0.5f });
	XMStoreFloat3(&aabb.max, position + XMVECTOR{ 0.5f,0.5f,0.5f });
	return aabb;
}
//////////////////////////////////

void StateMachine::Update(double elapsed_time)
{
	
	m_pState->Update(elapsed_time);
}

void StateMachine::Draw() const
{
	m_pState->Draw();
}

void StateMachine::UpdateState()
{
	if (m_pNextState != m_pState) {
		if (m_pState!= NULL)
		{
		m_pState->OutState();
		}
		delete m_pState;
		m_pState = m_pNextState;
		m_pState->InState();
	}
}

void StateMachine::ChangeState(State* pNext)
{
	//if (m_pState == NULL)
	//{
	//	m_pState= pNext;
	//}
	//else if(m_pState != pNext)
	//{
		m_pNextState = pNext;
	//}
}

void PlayerStateMachine::PlayerRunState::Update(double elapsed_time)
{
	auto speedtest = XMLoadFloat3(&g_PlayerVelocity) * XMVECTOR { 1, 0, 1 };
	float size = XMVectorGetX(XMVector3Length(speedtest));
	if (size <= 0.5f)
	{
		g_Animator.CrossFade(*g_pPlayerModel, g_Animator, "Idle", 0.1f);
		m_pOwner->ChangeState(new PlayerIdleState(m_pOwner));
	}
	if (KeyLogger_IsTrigger(KK_SPACE) && !g_IsJump) {
		m_pOwner->ChangeState(new PlayerJumpState(m_pOwner));
	}
	if (KeyLogger_IsTrigger(KK_F) && !g_IsJump)
	{
		m_pOwner->ChangeState(new PlayerThrowState(m_pOwner));
	}
	if (g_PlayerVelocity.y < 0.0f)
	{
			m_pOwner->ChangeState(new PlayerFallState(m_pOwner));
	}
}

void PlayerStateMachine::PlayerRunState::Draw() const
{
}

void PlayerStateMachine::PlayerRunState::InState()
{
}

void PlayerStateMachine::PlayerRunState::OutState()
{
}

void PlayerStateMachine::PlayerIdleState::Update(double elapsed_time)
{
	auto speedtest = XMLoadFloat3(&g_PlayerVelocity) * XMVECTOR { 1, 0, 1 };
	float size = XMVectorGetX(XMVector3Length(speedtest));
	if (size >= 0.5f && (KeyLogger_IsPressed(KK_I) || KeyLogger_IsPressed(KK_J) || KeyLogger_IsPressed(KK_K) || KeyLogger_IsPressed(KK_L)))
	{
		g_Animator.CrossFade(*g_pPlayerModel, g_Animator, "Run", 0.1f);
		m_pOwner->ChangeState(new PlayerRunState(m_pOwner));
	}
	if (KeyLogger_IsTrigger(KK_SPACE) && !g_IsJump) {
		m_pOwner->ChangeState(new PlayerJumpState(m_pOwner));
	}
	if (KeyLogger_IsTrigger(KK_F) && !g_IsJump)
	{
		m_pOwner->ChangeState(new PlayerThrowState(m_pOwner));
	}
	if (g_PlayerVelocity.y < 0.0f)
	{
		m_pOwner->ChangeState(new PlayerFallState(m_pOwner));
	}
}

void PlayerStateMachine::PlayerIdleState::Draw() const
{
}

void PlayerStateMachine::PlayerIdleState::InState()
{
}

void PlayerStateMachine::PlayerIdleState::OutState()
{
}

void PlayerStateMachine::PlayerJumpState::Update(double elapsed_time)
{
	if (g_IsJump == false)
	{
		auto speedtest = XMLoadFloat3(&g_PlayerVelocity) * XMVECTOR { 1, 0, 1 };
		float size = XMVectorGetX(XMVector3Length(speedtest));
		if (size <= 0.01f || !(KeyLogger_IsPressed(KK_I) || KeyLogger_IsPressed(KK_J) || KeyLogger_IsPressed(KK_K) || KeyLogger_IsPressed(KK_L)))
		{
			g_Animator.CrossFade(*g_pPlayerModel, g_Animator, "Idle", 0.15f);
			m_pOwner->ChangeState(new PlayerIdleState(m_pOwner));
		}
		else
		{
			g_Animator.CrossFade(*g_pPlayerModel, g_Animator, "Run", 0.05f);
			m_pOwner->ChangeState(new PlayerRunState(m_pOwner));
		}
	}
	if (g_PlayerVelocity.y<0.0f)
	{
		fallTimer += elapsed_time;
		if (fallTimer >= 0.3f)
		{
			fallTimer = 0;
			m_pOwner->ChangeState(new PlayerFallState(m_pOwner));
		}
	}
}

void PlayerStateMachine::PlayerJumpState::Draw() const
{
}

void PlayerStateMachine::PlayerJumpState::InState()
{
	g_Animator.CrossFadeToZero(*g_pPlayerModel, g_Animator, "Jump", 0.15f);
}

void PlayerStateMachine::PlayerJumpState::OutState()
{
}
//////////////////////

void PlayerStateMachine::PlayerThrowState::Update(double elapsed_time)
{
	throwTimer += (float)elapsed_time;
	if (throwTimer>=1.0f)
	{
		throwTimer = 0.0f;
		isThrowing = false;
	}
	if (!isThrowing)
	{
		auto speedtest = XMLoadFloat3(&g_PlayerVelocity) * XMVECTOR { 1, 0, 1 };
		float size = XMVectorGetX(XMVector3Length(speedtest));
		if (size <= 0.01f || !(KeyLogger_IsPressed(KK_I) || KeyLogger_IsPressed(KK_J) || KeyLogger_IsPressed(KK_K) || KeyLogger_IsPressed(KK_L)))
		{
			g_Animator.CrossFadeToZero(*g_pPlayerModel, g_Animator, "Idle", 0.7f);
			m_pOwner->ChangeState(new PlayerIdleState(m_pOwner));
		}
		else
		{
			g_Animator.CrossFade(*g_pPlayerModel, g_Animator, "Run", 0.05f);
			m_pOwner->ChangeState(new PlayerRunState(m_pOwner));
		}
	}
}

void PlayerStateMachine::PlayerThrowState::Draw() const
{
}

void PlayerStateMachine::PlayerThrowState::InState()
{
	isThrowing = true;
	XMFLOAT3 throwPoint = {
		g_PlayerPosition.x + g_PlayerFront.x * 0.25f,
		g_PlayerPosition.y + 1.0f,
		g_PlayerPosition.z + g_PlayerFront.z * 0.25f
	};
	Bullet3D_CreateBullet(throwPoint, g_PlayerFront);
	g_Animator.CrossFadeToZero(*g_pPlayerModel, g_Animator, "Throw", 0.25f);
	g_PlayerVelocity = { 0,0,0 };
}

void PlayerStateMachine::PlayerThrowState::OutState()
{
}

void PlayerStateMachine::PlayerFallState::Update(double elapsed_time)
{
	if (g_IsJump == false)
	{

		auto speedtest = XMLoadFloat3(&g_PlayerVelocity) * XMVECTOR { 1, 0, 1 };
		float size = XMVectorGetX(XMVector3Length(speedtest));
		if (size <= 0.01f || !(KeyLogger_IsPressed(KK_I) || KeyLogger_IsPressed(KK_J) || KeyLogger_IsPressed(KK_K) || KeyLogger_IsPressed(KK_L)))
		{
			g_Animator.CrossFade(*g_pPlayerModel, g_Animator, "Idle", 0.15f);
			m_pOwner->ChangeState(new PlayerIdleState(m_pOwner));
		}
		else
		{
			g_Animator.CrossFade(*g_pPlayerModel, g_Animator, "Run", 0.05f);
			m_pOwner->ChangeState(new PlayerRunState(m_pOwner));
		}
	}
}

void PlayerStateMachine::PlayerFallState::Draw() const
{
}

void PlayerStateMachine::PlayerFallState::InState()
{
	g_IsJump = true;
	g_Animator.CrossFadeToZero(*g_pPlayerModel, g_Animator, "Fall", 0.35f);
}

void PlayerStateMachine::PlayerFallState::OutState()
{
}
////////