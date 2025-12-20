#ifndef PLAYER3D_H
#define PLAYER3D_H
#include "DirectXMath.h"
#include "Collision3D.h"

void Player3D_Initialize(const DirectX::XMFLOAT3 position, const DirectX::XMFLOAT3 front);
void Player3D_Finitialize();
void Player3D_Update(double elapsed_time);
void Player3D_Draw();

const DirectX::XMFLOAT3& GetPlayerPosition();
const DirectX::XMFLOAT3& GetPlayerFront();
AABB GetPlayer_AABB();
AABB Player_ConvertPositionToAABB(const DirectX::XMVECTOR position);

void StartPlayer_MonsterControl();

class StateMachine
{
	protected :
		class State {
		public:
			virtual ~State() = default;
			virtual void InState() = 0;
			virtual void Update(double elapsed_time) = 0;
			virtual void OutState() = 0;
			virtual void Draw() const = 0;
		};
private: 
	State* m_pState{};
	State* m_pNextState{};
public:
	virtual ~StateMachine() = default;
	virtual void Update(double elapsed_time);
	virtual void Draw() const;
	void UpdateState();
	virtual bool IsDestroy() const = 0;
protected:
	void ChangeState(State* pNext);
};

class PlayerStateMachine : public StateMachine
{
	public:
		PlayerStateMachine() {
			ChangeState(new PlayerIdleState(this));
		}
		bool IsDestroy() const override {
			return false;
		}
private:
	class PlayerRunState : public State 
	{
	private:
		PlayerStateMachine* m_pOwner{};

	public:
		PlayerRunState(PlayerStateMachine* pOwner)
			: m_pOwner(pOwner) {
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
		void InState() override;
		void OutState() override;
	};
	class PlayerIdleState : public State
	{
	private:
		PlayerStateMachine* m_pOwner{};

	public:
		PlayerIdleState(PlayerStateMachine* pOwner)
			: m_pOwner(pOwner) 
{
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
		void InState() override;
		void OutState() override;
	};
	class PlayerJumpState : public State
	{
	private:
		PlayerStateMachine* m_pOwner{};
		float fallTimer = 0.0f;

	public:
		PlayerJumpState(PlayerStateMachine* pOwner)
			: m_pOwner(pOwner) {
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
		void InState() override;
		void OutState() override;
	};
	class PlayerThrowState : public State
	{
	private:
		PlayerStateMachine* m_pOwner{};

	public:
		PlayerThrowState(PlayerStateMachine* pOwner)
			: m_pOwner(pOwner) {
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
		void InState() override;
		void OutState() override;
	};

	class PlayerFallState : public State
	{
	private:
		PlayerStateMachine* m_pOwner{};

	public:
		PlayerFallState(PlayerStateMachine* pOwner)
			: m_pOwner(pOwner) {
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
		void InState() override;
		void OutState() override;
	};

	class PlayerControllingState : public State
	{
	private:
		PlayerStateMachine* m_pOwner{};

	public:
		PlayerControllingState(PlayerStateMachine* pOwner)
			: m_pOwner(pOwner) {
		}
		void Update(double elapsed_time) override;
		void Draw() const override;
		void InState() override;
		void OutState() override;
	};
};
#endif