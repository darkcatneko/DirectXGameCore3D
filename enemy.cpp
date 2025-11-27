/*==============================================================================

  エネミー制御[enemy.cpp]
														 Author : 
														 Date   : 2025/11/26
--------------------------------------------------------------------------------

==============================================================================*/
#include "enemy.h"
#include "enemy_normal.h"

void Enemy::Update(double elapsed_time){
	m_pState->Update(elapsed_time);
}

void Enemy::Draw() const{
	m_pState->Draw();
}

void Enemy::UpdateState() {
	if (m_pNextState != m_pState) {
		delete m_pState;
		m_pState = m_pNextState;
	}
}

void Enemy::ChangeState(State* pNext){
	m_pNextState = pNext;
}

static constexpr int MAX_ENEMY{ 32 };
static Enemy* g_Enemys[MAX_ENEMY]{};
static int g_EnemyCount{ 0 };

void Enemy_Initialize(){
	g_EnemyCount = 0;
}

void Enemy_Finalize(){
	for (int i = 0;i < g_EnemyCount;i++) {
		delete g_Enemys[i];
	}
}

void Enemy_Update(double elapsed_time){
	//エネミーステート切り替え
	for (int i = 0;i < g_EnemyCount;i++) {
		g_Enemys[i]->UpdateState();
	}

	//エネミー更新
	for (int i = 0;i < g_EnemyCount;i++) {
		g_Enemys[i]->Update(elapsed_time);
	}

	//死亡判定
	for (int i = g_EnemyCount - 1;i >= 0;i--) {
		if (g_Enemys[i]->IsDestroy()) {
			delete g_Enemys[i];
			g_Enemys[i] = g_Enemys[--g_EnemyCount];
		}
	}
}

void Enemy_Draw(){
	for (int i = 0;i < g_EnemyCount;i++) {
		g_Enemys[i]->Draw();
	}
}

void Enemy_Create(const DirectX::XMFLOAT3& position){
	g_Enemys[g_EnemyCount] = new EnemyNormal(position);
	g_EnemyCount++;
}
