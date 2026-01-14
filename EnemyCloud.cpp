#include "EnemyCloud.h"
#include "collision.h"
#include "Player3D.h"
#include "Cube.h"
#include "Grid.h"
#include "Bullet3D.h"
#include  "Key_Logger.h"
#include "Model_Static.h"
#include<DirectXMath.h>
#include "NekoTool.h"
using namespace DirectX;

void EnemyCloud::EnemyCloudStatePatrol::Update(double elapsed_time)
{
	//左右に往復移動
	m_AccumulatedTime += elapsed_time;
	m_pOwner->m_Position.x = m_PointX + sinf((float)m_AccumulatedTime);


	if (IsTooFar(m_pOwner->m_Position, m_pOwner->spawn_Position, 5))
	{
		m_pOwner->ChangeState(new EnemyCloudStateBack(m_pOwner));
	}
	//プレイヤーが索敵範囲に入ったらChaseステートに移行
	if (Collision_IsOverlapSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, GetPlayerPosition()))
	{
		m_pOwner->ChangeState(new EnemyCloudStateChase(m_pOwner));
	}
	if (IsPointInSector(m_pOwner->m_DetectionSector, GetPlayerPosition()))
	{
		m_pOwner->ChangeState(new EnemyCloudStateChase(m_pOwner));
	}
}

void EnemyCloud::EnemyCloudStatePatrol::Draw() const
{
	Model_Static_Draw(m_pOwner->cloud_model, new GameObject(m_pOwner->m_Position, { 0,m_pOwner->m_Rotation.y,0 }));
	Grid_DebugDrawSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, { 1,0,0,1 });
	DebugDrawSector(m_pOwner->m_DetectionSector, { 1,0,0,1 });
}

void EnemyCloud::EnemyCloudStateChase::Update(double elapsed_time)
{
	//如果距離不夠，追上
	//距離內，發射子彈
	
	//自分からプレイヤーまでのベクトル算出
	XMVECTOR toPlayer = XMLoadFloat3(&GetPlayerPosition()) - XMLoadFloat3(&m_pOwner->m_Position);
	XMFLOAT3 toPlayer_f; XMStoreFloat3(&toPlayer_f,toPlayer);
	toPlayer = XMVector3Normalize(toPlayer);
	toPlayer *= {1.0f, 0.0f, 1.0f};//Y軸移動無効化

	//移動
	XMVECTOR position = XMLoadFloat3(&m_pOwner->m_Position) + toPlayer * 1.5f * elapsed_time;
	//向き調整
	XMVECTOR vDir = XMVector3Normalize(toPlayer);
	float angle = atan2f(toPlayer_f.x, toPlayer_f.z);

	// 模型 forward = 180°
	angle += 2*XM_PI;

	m_pOwner->m_Rotation.y = angle;


	XMStoreFloat3(&m_pOwner->m_Position, position);
	//諦める
	if (!Collision_IsOverlapSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, GetPlayerPosition()))
	{
		m_AccumulatedTime += elapsed_time;
		if (m_AccumulatedTime >= 5.0) {
			m_pOwner->ChangeState(new EnemyCloudStatePatrol(m_pOwner));
		}
	}
	else
	{
		m_AccumulatedTime = 0.0;
	}
	
}

void EnemyCloud::EnemyCloudStateChase::Draw() const
{
	Model_Static_Draw(m_pOwner->cloud_model, new GameObject(m_pOwner->m_Position, {0,m_pOwner->m_Rotation.y,0}));
	Grid_DebugDrawSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, { 1,0,0,1 });
}

void EnemyCloud::EnemyCloudStateBeHit::Update(double elapsed_time)
{
	if (KeyLogger_IsPressed(KK_I)) {
		m_pOwner->m_Position.z += 1.5 * (float)elapsed_time;
	}

	if (KeyLogger_IsPressed(KK_K)) {
		m_pOwner->m_Position.z -= 1.5 * (float)elapsed_time;
	}


	if (KeyLogger_IsPressed(KK_L)) {
		m_pOwner->m_Position.x += 1.5 * (float)elapsed_time;
	}

	if (KeyLogger_IsPressed(KK_J)) {
		m_pOwner->m_Position.x -= 1.5 * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_UP))
	{
		m_pOwner->m_Position.y += 1.5 * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_DOWN))
	{
		m_pOwner->m_Position.y -= 1.5 * (float)elapsed_time;
	}
	if (KeyLogger_IsTrigger(KK_C))
	{
		m_pOwner->IsCollider =true;
		m_pOwner->ChangeState(new EnemyCloudStatePlate(m_pOwner));
	}
}

void EnemyCloud::EnemyCloudStateBeHit::Draw() const
{
	Model_Static_Draw(m_pOwner->cloud_model, new GameObject(m_pOwner->m_Position, { 0,m_pOwner->m_Rotation.y,0 }));
	Grid_DebugDrawSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, { 0,1,0,1 });
}

void EnemyCloud::Update(double elapsed_time)
{
	Enemy::Update(elapsed_time);
	m_DetectionSector.origin = m_Position;
	for (int i = 0; i < Bullet3D_GetObjectsCount(); i++)
	{
		if (Collision_IsOverlapSphere({ this->m_Position,this->m_DetectionRadius }, Bullet3D_GetPos(i)))
		{
			this->HurtEnemy(1);
			Bullet3D_Destroy(i);
			if (m_Hp<=0)
			{
				this->ChangeState(new EnemyCloudStateBeHit(this));
				StartPlayer_MonsterControl();
			}
		}
	}
}

void EnemyCloud::HurtEnemy(int amount)
{
	m_Hp -= amount;
}

void EnemyCloud::EnemyCloudStateBack::Update(double elapsed_time)
{
	//自分からプレイヤーまでのベクトル算出
	XMVECTOR toSpawn = XMLoadFloat3(&m_pOwner->spawn_Position) - XMLoadFloat3(&m_pOwner->m_Position);
	XMFLOAT3 toSpawn_f; XMStoreFloat3(&toSpawn_f, toSpawn);
	toSpawn = XMVector3Normalize(toSpawn);
	//toSpawn *= {1.0f, 0.0f, 1.0f};//Y軸移動無効化

	//移動
	XMVECTOR position = XMLoadFloat3(&m_pOwner->m_Position) + toSpawn * 3.5f * elapsed_time;
	//向き調整
	XMVECTOR vDir = XMVector3Normalize(toSpawn);
	float angle = atan2f(toSpawn_f.x, toSpawn_f.z);

	// 模型 forward = 180°
	angle += 2 * XM_PI;

	m_pOwner->m_Rotation.y = angle;
	XMStoreFloat3(&m_pOwner->m_Position, position);
	//到達したらPatrolステートに移行
	if (!IsTooFar(m_pOwner->m_Position, m_pOwner->spawn_Position, 0.05f))
	{
		m_pOwner->m_Rotation.y = XM_PI;
		m_pOwner->ChangeState(new EnemyCloudStatePatrol(m_pOwner));
	}
}

void EnemyCloud::EnemyCloudStateBack::Draw() const
{
	Model_Static_Draw(m_pOwner->cloud_model, new GameObject(m_pOwner->m_Position, { 0,m_pOwner->m_Rotation.y,0 }));
	Grid_DebugDrawSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, { 0,0,1,1 });
}

void EnemyCloud::EnemyCloudStatePlate::Update(double elapsed_time)
{
	m_AccumulatedTime += elapsed_time;
	if (m_AccumulatedTime >= 5.0) {
		m_AccumulatedTime = 0;
		m_pOwner->IsCollider = false;
		m_pOwner->ChangeState(new EnemyCloudStateBack(m_pOwner));
	}
	m_pOwner->Collision = ModelStatic_GetAABBInWorldSpace(m_pOwner->cloud_model, m_pOwner->m_Position);
}

void EnemyCloud::EnemyCloudStatePlate::Draw() const
{
	Model_Static_Draw(m_pOwner->cloud_model, new GameObject(m_pOwner->m_Position, { 0,m_pOwner->m_Rotation.y,0 }));
	AABB_Draw_Debug({ m_pOwner->Collision });
}
