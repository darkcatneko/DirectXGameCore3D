/*==============================================================================

  ノーマルエネミークラス[enemy_normal.cpp]
														 Author : 
														 Date   : 2025/11/26
--------------------------------------------------------------------------------

==============================================================================*/
#include "enemy_normal.h"
#include "collision.h"
#include "Player3D.h"
#include "Cube.h"
#include "Grid.h"
#include "Bullet3D.h"
#include  "Key_Logger.h"
#include<DirectXMath.h>
using namespace DirectX;

void EnemyNormal::EnemyNormalStatePatrol::Update(double elapsed_time){
	m_AccumulatedTime += elapsed_time;
	m_pOwner->m_Position.x = sinf(m_AccumulatedTime);

	//プレイヤーが索敵範囲に入ったらChaseステートに移行
	if (Collision_IsOverlapSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, GetPlayerPosition())) 
	{
		m_pOwner->ChangeState(new EnemyNormalStateChase(m_pOwner));
	}
}

void EnemyNormal::EnemyNormalStatePatrol::Draw() const
{
	Cube_Draw(m_pOwner->m_Position);
	Grid_DebugDrawSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, { 1,0,0,1 });
}

void EnemyNormal::EnemyNormalStateChase::Update(double elapsed_time){
	//自分からプレイヤーまでのベクトル算出
	XMVECTOR toPlayer = XMLoadFloat3(&GetPlayerPosition()) - XMLoadFloat3(&m_pOwner->m_Position);
	toPlayer = XMVector3Normalize(toPlayer);

	//移動
	XMVECTOR position = XMLoadFloat3(&m_pOwner->m_Position) + toPlayer * 1.5f * elapsed_time;
	XMStoreFloat3(&m_pOwner->m_Position, position);

	//諦める
	if (!Collision_IsOverlapSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, GetPlayerPosition()))
	{
		m_AccumulatedTime += elapsed_time;
		if (m_AccumulatedTime >= 3.0) {
			m_pOwner->ChangeState(new EnemyNormalStatePatrol(m_pOwner));
		}
	}
	else
	{
		m_AccumulatedTime = 0.0;
	}
}

void EnemyNormal::EnemyNormalStateChase::Draw() const
{ 
	Cube_Draw(m_pOwner->m_Position);
	Grid_DebugDrawSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, { 1,0,0,1 });
}

void EnemyNormal::Update(double elapsed_time)
{
	Enemy::Update(elapsed_time);
	for (int i = 0; i <Bullet3D_GetObjectsCount() ; i++)
	{
		if (Collision_IsOverlapSphere({ this->m_Position,this->m_DetectionRadius }, Bullet3D_GetPos(i)))
		{
			this->ChangeState(new EnemyNormalStateBeHit(this));
			StartPlayer_MonsterControl();
		}
	}
}

void EnemyNormal::EnemyNormalStateBeHit::Update(double elapsed_time)
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
	if (KeyLogger_IsTrigger(KK_C))
	{
		m_pOwner->ChangeState(new EnemyNormalStatePatrol(m_pOwner));
	}
}

void EnemyNormal::EnemyNormalStateBeHit::Draw() const
{
	Cube_Draw(m_pOwner->m_Position);
	Grid_DebugDrawSphere({ m_pOwner->m_Position,m_pOwner->m_DetectionRadius }, { 0,1,0,1 });
}
