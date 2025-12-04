/*==============================================================================

  ノーマルエネミークラス[enemy_normal.h]
														 Author :
														 Date   : 2025/11/26
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef ENEMY_NORMAL_H
#define ENEMY_NORMAL_H

#include "enemy.h"
#include <DirectXMath.h>

class EnemyNormal : public Enemy 
{
private:
	DirectX::XMFLOAT3 m_Position{};
	float m_DetectionRadius{ 3.0f };
	int m_Hp{ 100 };

public:
	EnemyNormal(const DirectX::XMFLOAT3& position) 
		: m_Position(position)
	{
		ChangeState(new EnemyNormalStatePatrol(this));
	}
	bool IsDestroy() const override {
		return m_Hp <= 0;
	}

private:
	class EnemyNormalStatePatrol : public State {
	private:
		EnemyNormal* m_pOwner{};
		float m_PointX{};
		double m_AccumulatedTime{};

	public:
		EnemyNormalStatePatrol(EnemyNormal* pOwner) 
			: m_pOwner(pOwner) ,m_PointX(pOwner->m_Position.x){
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
	};

	class EnemyNormalStateChase : public State {
	private:
		EnemyNormal* m_pOwner{};
		double m_AccumulatedTime{};

	public:
		EnemyNormalStateChase(EnemyNormal* pOwner)
			: m_pOwner(pOwner) {
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
	};
};




#endif //ENEMY_NORMAL_H