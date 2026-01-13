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
	if (IsTooFar(m_pOwner->m_Position, m_pOwner->spawn_Position, 5))
	{
		m_pOwner->ChangeState(new EnemyCloudStateBack(m_pOwner));
	}
	//プレイヤーが索敵範囲に入ったらChaseステートに移行
	if (Collision_IsOverlapSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, GetPlayerPosition()))
	{
		m_pOwner->ChangeState(new EnemyCloudStateChase(m_pOwner));
	}
}

void EnemyCloud::EnemyCloudStatePatrol::Draw() const
{
	Model_Static_Draw(m_pOwner->cloud_model, new GameObject(m_pOwner->m_Position, { 0,XMConvertToRadians(180),0 }));
	Grid_DebugDrawSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, { 1,0,0,1 });
}

void EnemyCloud::EnemyCloudStateChase::Update(double elapsed_time)
{
	//自分からプレイヤーまでのベクトル算出
	XMVECTOR toPlayer = XMLoadFloat3(&GetPlayerPosition()) - XMLoadFloat3(&m_pOwner->m_Position);
	toPlayer = XMVector3Normalize(toPlayer);
	toPlayer *= {1.0f, 0.0f, 1.0f};//Y軸移動無効化

	//移動
	XMVECTOR position = XMLoadFloat3(&m_pOwner->m_Position) + toPlayer * 1.5f * elapsed_time;
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
	Model_Static_Draw(m_pOwner->cloud_model, new GameObject(m_pOwner->m_Position, {0,XMConvertToRadians(180),0}));
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
		m_pOwner->ChangeState(new EnemyCloudStatePlate(m_pOwner));
	}
}

void EnemyCloud::EnemyCloudStateBeHit::Draw() const
{
	Model_Static_Draw(m_pOwner->cloud_model, new GameObject(m_pOwner->m_Position, { 0,XMConvertToRadians(180),0 }));
	Grid_DebugDrawSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, { 0,1,0,1 });
}

void EnemyCloud::Update(double elapsed_time)
{
	Enemy::Update(elapsed_time);
	for (int i = 0; i < Bullet3D_GetObjectsCount(); i++)
	{
		if (Collision_IsOverlapSphere({ this->m_Position,this->m_DetectionRadius }, Bullet3D_GetPos(i)))
		{
			this->ChangeState(new EnemyCloudStateBeHit(this));
			StartPlayer_MonsterControl();
		}
	}
}

void EnemyCloud::EnemyCloudStateBack::Update(double elapsed_time)
{
	//自分からプレイヤーまでのベクトル算出
	XMVECTOR toSpawn = XMLoadFloat3(&m_pOwner->spawn_Position) - XMLoadFloat3(&m_pOwner->m_Position);
	toSpawn = XMVector3Normalize(toSpawn);
	toSpawn *= {1.0f, 0.0f, 1.0f};//Y軸移動無効化

	//移動
	XMVECTOR position = XMLoadFloat3(&m_pOwner->m_Position) + toSpawn * 3.5f * elapsed_time;
	XMStoreFloat3(&m_pOwner->m_Position, position);
	//到達したらPatrolステートに移行
	if (!IsTooFar(m_pOwner->m_Position, m_pOwner->spawn_Position, 0.05f))
	{
		m_pOwner->ChangeState(new EnemyCloudStatePatrol(m_pOwner));
	}
}

void EnemyCloud::EnemyCloudStateBack::Draw() const
{
	Model_Static_Draw(m_pOwner->cloud_model, new GameObject(m_pOwner->m_Position, { 0,XMConvertToRadians(180),0 }));
	Grid_DebugDrawSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, { 0,0,1,1 });
}

void EnemyCloud::EnemyCloudStatePlate::Update(double elapsed_time)
{
	m_pOwner->Collision = ModelStatic_GetAABBInWorldSpace(m_pOwner->cloud_model, m_pOwner->m_Position);
}

void EnemyCloud::EnemyCloudStatePlate::Draw() const
{
	Model_Static_Draw(m_pOwner->cloud_model, new GameObject(m_pOwner->m_Position, { 0,XMConvertToRadians(180),0 }));
	AABB_Draw_Debug({ m_pOwner->Collision });
}
