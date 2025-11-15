#include "model.h"
#include "Bullet3D.h"
static Bullet3D* g_pBullets[MAX_BULLET]{};
static int g_BulletsCount{ 0 };
static MODEL* bullet_model;
void Bullet3D_Initialize()
{
	bullet_model = ModelLoad("test.fbx", 0.01f, false);
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
		ModelDraw(bullet_model,new GameObject(g_pBullets[i]->GetPosition()));
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
