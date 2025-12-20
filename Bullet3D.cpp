#include "model_Static.h"
#include "Bullet3D.h"
#include "DirectXMath.h"
#include "enemy.h"
using namespace DirectX;
static Bullet3D* g_pBullets[MAX_BULLET]{};
static int g_BulletsCount{ 0 };
static MODEL_STATIC* bullet_model;
void Bullet3D_Initialize()
{
	bullet_model = Model_Static_Load("test.fbx", 0.01f, false);
}

void Bullet3D_Finitialize()
{
	for (size_t i = 0; i < g_BulletsCount; i++)
	{
		delete g_pBullets[i];
	}
	g_BulletsCount = 0;
}

void Bullet3D_Update(double elapsed_time)
{
	for (size_t i = 0; i < g_BulletsCount; i++)
	{
		g_pBullets[i]->Update(elapsed_time);
	}
	for (size_t i = 0; i < g_BulletsCount; i++)
	{
		if (g_pBullets[i]->IsDestroy())
		{
			Bullet3D_Destroy(i);
		}
	}
}


void Bullet3D_Draw()
{
	for (size_t i = 0; i < g_BulletsCount; i++)
	{
		Model_Static_Draw(bullet_model,new GameObject(g_pBullets[i]->GetPosition()));
	}
}

int Bullet3D_GetObjectsCount()
{
	return g_BulletsCount;
}

void Bullet3D_CreateBullet(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity)
{
	if (g_BulletsCount < MAX_BULLET)
	{
		g_pBullets[g_BulletsCount++] = new Bullet3D(position, velocity);
	}
}

void Bullet3D_Destroy(int index)
{
	delete g_pBullets[index];
	g_pBullets[index] = g_pBullets[g_BulletsCount - 1];
	g_BulletsCount--;
}

AABB Bullet_GetAABB(int index)
{
	return 
	{
		{ g_pBullets[index]->GetPosition().x + 0.05f, g_pBullets[index]->GetPosition().y + 0.05f,g_pBullets[index]->GetPosition().z + 0.05f},
		{ g_pBullets[index]->GetPosition().x - 0.05f, g_pBullets[index]->GetPosition().y - 0.05f,g_pBullets[index]->GetPosition().z - 0.05f}
	};
}

DirectX::XMFLOAT3 Bullet3D_GetPos(int index)
{
	return g_pBullets[index]->GetPosition();
}


void Bullet3D::Update(double elasped_time)
{
	m_accumulatedTime += elasped_time;
	DirectX::XMStoreFloat3(&m_position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&m_position), DirectX::XMLoadFloat3(&m_velocity)*0.5f));
}
