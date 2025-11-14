#ifndef BULLET3D_H
#define BULLET3D_H
#include "DirectXMath.h"

class  Bullet3D
{
private:
	DirectX::XMFLOAT3 m_position{};
	DirectX::XMFLOAT3 m_velocity{}; 
	double m_accumulatedTime{ 0.0 };
	static constexpr double LIMIT_TIME = 3.0f;

public:
	Bullet3D(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity) :m_position(position), m_velocity(velocity) {}
	void Update(double elasped_time) 
	{
		m_accumulatedTime += elasped_time;
		DirectX::XMStoreFloat3(&m_position, DirectX::XMVectorAdd( DirectX::XMLoadFloat3(&m_position) ,DirectX::XMLoadFloat3(&m_velocity)));
	}
	const DirectX::XMFLOAT3& GetPosition() const
	{
		return m_position;
	}
	DirectX::XMFLOAT3 GetFront() const
	{
		DirectX::XMFLOAT3 front;
		DirectX::XMStoreFloat3(&front, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&m_velocity)));
		return front;
	}
	bool IsDestroy() const
	{
		return m_accumulatedTime >= LIMIT_TIME;
	}
};

static constexpr int MAX_BULLET = 2048;


void Bullet3D_Initialize();
void Bullet3D_Finitialize();
void Bullet3D_Update(double elapsed_time);
void Bullet3D_Draw();
int  Bullet3D_GetObjectsCount();
void Bullet3D_CreateBullet(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity);
void Bullet3D_Destroy(int index);

AABB Bullet_GetAABB(int index);

#endif
