/*==============================================================================

  エネミー制御[enemy.h]
														 Author : 
														 Date   : 2025/11/26
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef ENEMY_H
#define ENEMY_H
#include "Collision3D.h"
#include <DirectXMath.h>

class Enemy {
protected:
	class State {
	public:
		virtual ~State() = default;		
		virtual void Update(double elapsed_time) = 0;
		virtual void Draw() const = 0;
	};

private:
	State* m_pState{};
	State* m_pNextState{};

public:
	virtual ~Enemy() = default;
	virtual void Update(double elapsed_time);
	virtual void Draw() const;
	void UpdateState();
	virtual bool IsDestroy() const = 0;

protected:
	void ChangeState(State* pNext);
};

void Enemy_Initialize();
void Enemy_Finalize();
void Enemy_Update(double elapsed_time);
void Enemy_Draw();
void Enemy_Create(const DirectX::XMFLOAT3& position);

#endif //ENEMY_H