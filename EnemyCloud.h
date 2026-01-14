/*==============================================================================

  ノーマルエネミークラス[enemy_normal.h]
														 Author :
														 Date   : 2025/11/26
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef ENEMYCLOUD_H
#define ENEMYCLOUD_H

#include "enemy.h"
#include <DirectXMath.h>
#include "Model_Static.h"
#include "Collision3D.h"



class EnemyCloud : public Enemy
{

private:
	bool this_isDestroy = false;
	DirectX::XMFLOAT3 m_Position{};
	DirectX::XMFLOAT3 spawn_Position{};
	DirectX::XMFLOAT3 m_Rotation{};
	float m_DetectionRadius{ 2.5f };
	int m_Hp{ 3 };
	Sector m_DetectionSector{};

public:
	MODEL_STATIC* cloud_model;
	EnemyCloud(const DirectX::XMFLOAT3& position)
		: m_Position(position), spawn_Position(position)
	{
		m_DetectionSector.origin = m_Position;
		m_DetectionSector.forward = { 0,0,-1 };
		m_DetectionSector.radius = 5.0f;
		m_DetectionSector.fovDeg = 90.0f;
		m_Rotation.y += DirectX::XM_PI;
		cloud_model = Model_Static_Load("Cloud_01a.fbx", 1.0f, true);
		Collision = ModelStatic_GetAABBInWorldSpace(cloud_model, m_Position);
		ChangeState(new EnemyCloudStatePatrol(this));
	}
	bool IsDestroy() const override {
		return this_isDestroy;
	}
	void Update(double elapsed_time) override;
	void HurtEnemy(int amount) override;

private:
	class EnemyCloudStatePatrol : public State {
	private:
		EnemyCloud* m_pOwner{};
		float m_PointX{};
		double m_AccumulatedTime{};

	public:
		EnemyCloudStatePatrol(EnemyCloud* pOwner)
			: m_pOwner(pOwner), m_PointX(pOwner->m_Position.x) {
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
	};

	class EnemyCloudStateChase : public State {
	private:
		EnemyCloud* m_pOwner{};
		double m_AccumulatedTime{};

	public:
		EnemyCloudStateChase(EnemyCloud* pOwner)
			: m_pOwner(pOwner) {
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
	};

	class EnemyCloudStateBeHit : public State {
	private:
		EnemyCloud* m_pOwner{};
		double m_AccumulatedTime{};

	public:
		EnemyCloudStateBeHit(EnemyCloud* pOwner)
			: m_pOwner(pOwner) {
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
	};
	class EnemyCloudStateBack : public State {
	private:
		EnemyCloud* m_pOwner{};
		double m_AccumulatedTime{};

	public:
		EnemyCloudStateBack(EnemyCloud* pOwner)
			: m_pOwner(pOwner) {
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
	};
	class EnemyCloudStatePlate : public State {
	private:
		EnemyCloud* m_pOwner{};
		double m_AccumulatedTime{};

	public:
		EnemyCloudStatePlate(EnemyCloud* pOwner)
			: m_pOwner(pOwner) {
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
	};
};




#endif //ENEMY_NORMAL_H