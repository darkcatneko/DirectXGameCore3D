#include "Enemy.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "direct3d.h"
#include "Texture.h"
#include "sprite.h"
#include <cstdlib>
#include "Effect.h"

struct Emeny_Draw_Info
{
	int Enemy_Texid;
	int tx, ty, tw, th;

	Circle collision;
};
struct Enemy
{
	EnemyTypeId enemyid;
	XMFLOAT2 position;
	XMFLOAT2 velocity;
	XMFLOAT2 offset;
	double lifetime;
	int health;
	bool isEnable;
};

static Enemy g_Enemys[ENEMY_MAX];//畫面上最多子彈數
static int g_EnemyTexId = -1;
static Emeny_Draw_Info g_EmenyType[]
{
	{-1,64,64,1,1,{{32.0f,32.0f},128.0f}},
	{-1,32,32,1,1,{{16.0f,16.0f},32.0f}},
};

void Enemy_Initialize()
{
	for (Enemy& e : g_Enemys)
	{
		e.isEnable = false;
	}
	g_EnemyTexId = Texture_Load(L"cat.png");
	g_EmenyType[0].Enemy_Texid = Texture_Load(L"bruh.png");
	g_EmenyType[1].Enemy_Texid = Texture_Load(L"cat.png");
}
void Enemy_Finitialize()
{

}

void Enemy_Update(double elapsed_time)
{
	for (Enemy& e : g_Enemys)
	{
		if (!e.isEnable)continue;
		e.lifetime += elapsed_time;
		XMVECTOR position = XMLoadFloat2(&e.position);
		XMVECTOR velocity = XMLoadFloat2(&e.velocity);
		switch (e.enemyid)
		{
		case bruh:

			//velocity += direction * 6000000.0f / 2500.0f * elapsed_time;
			position += velocity * elapsed_time;
			XMStoreFloat2(&e.position, position);
			XMStoreFloat2(&e.velocity, velocity);
			break;
		case cat:

			//velocity += direction * 6000000.0f / 2500.0f * elapsed_time;
			position += velocity * elapsed_time;
			XMStoreFloat2(&e.position, position);
			XMStoreFloat2(&e.velocity, velocity);
			e.position.y = e.offset.y + sin(e.lifetime * 15.0f) * 3.0f;

			break;
		}


		if ((e.position.x) > Direct3D_GetBackBufferWidth())
		{
			e.isEnable = false;
		}
	}
}
void Enemy_Draw()
{
	int i = 0;
	for (Enemy& e : g_Enemys)
	{
		i++;
		if (e.isEnable == false)continue;
		//這裡畫圖
		Sprite_Draw_UV(g_EmenyType[e.enemyid].Enemy_Texid, e.position.x, e.position.y, g_EmenyType[e.enemyid].tx, g_EmenyType[e.enemyid].ty, g_EmenyType[e.enemyid].tw, g_EmenyType[e.enemyid].th, 0, 0);
		Collision_DebugDraw(Enemy_GetCollision(i - 1));
	}
}

void Enemy_Creater(EnemyTypeId id, DirectX::XMFLOAT2& position, DirectX::XMFLOAT2 offset)
{
	for (Enemy& e : g_Enemys)
	{
		if (e.isEnable)continue;
		e.isEnable = true;
		e.position = position;
		e.offset.x = offset.x;
		e.offset.y = offset.y;
		e.velocity = { -200.0f,0.0f };//改造重點
		e.health = 1.0f;
		e.enemyid = id;
		break;
	}
}

bool Enemy_IsEnable(int index)
{
	return g_Enemys[index].isEnable;;
}

Circle Enemy_GetCollision(int index)
{
	int id = g_Enemys[index].enemyid;
	float cx = g_EmenyType[id].collision.center.x + g_Enemys[index].position.x;
	float cy = g_EmenyType[id].collision.center.y + g_Enemys[index].position.y;



	return { {cx,cy},g_EmenyType[id].collision.radius };
}

void Enemy_Destroy(int index)
{
	g_Enemys[index].isEnable = false;
}

void Enemy_Hit(int damage, int index)
{
	if (!g_Enemys[index].isEnable)return;
	g_Enemys[index].health -= damage;
	if (g_Enemys[index].health <= 0)
	{
		Effect_Creater(g_Enemys[index].position,0);
		Enemy_Destroy(index);
	}
}
