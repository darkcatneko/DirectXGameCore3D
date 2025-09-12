#include "bullet.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "direct3d.h"
#include "Texture.h"
#include "sprite.h"
#include "Collision.h"
#include <cstdlib>
#include "Audio.h"
#include "BattleBossController.h"
#include "BossCleanerController.h"

struct Bullet
{
	XMFLOAT2 position;
	XMFLOAT2 velocity;
	float rotation;
	double lifetime;
	bool isEnable;
	Circle collision;
	Box box_collision;
};

static Bullet g_Bullets[BULLET_MAX];//畫面上最多子彈數
static int g_BulletTexId = -1;
int shoot_sf = 0;

void Bullet_Initialize()
{
	for (Bullet& b : g_Bullets)
	{
		b.isEnable = false;
	}
	g_BulletTexId = Texture_Load(L"cat.png");
	shoot_sf = LoadAudio("shoot.wav");
}

void Bullet_Finitialize()
{
}

void Bullet_Update(double elapsed_time)
{
	for (Bullet& b : g_Bullets)
	{
		if (!b.isEnable)continue;

		XMVECTOR position = XMLoadFloat2(&b.position);
		XMVECTOR velocity = XMLoadFloat2(&b.velocity);

		//velocity += direction * 6000000.0f / 2500.0f * elapsed_time;
		position += velocity * elapsed_time;

		XMStoreFloat2(&b.position, position);
		XMStoreFloat2(&b.velocity, velocity);

		b.lifetime += elapsed_time;

		if (b.lifetime >= 5.0f)//destroy time
		{
			b.isEnable = false;
		}
		if ((b.position.x) > Direct3D_GetBackBufferWidth())
		{
			b.isEnable = false;
		}
	}
}

void Bullet_Draw()
{
	int i = 0;
	for (Bullet& b : g_Bullets)
	{
		i++;
		if (b.isEnable == false)continue;
		//這裡畫圖
		Sprite_Draw_Rotate(g_BulletTexId, b.position.x, b.position.y, 32.0f, 32.0f, b.rotation);
		Collision_DebugDraw(Bullet_GetCollision(i - 1));
	}
}

void Bullet_Shooter(DirectX::XMFLOAT2& position, DirectX::XMFLOAT2& aimPoint)
{

	for (Bullet& b : g_Bullets)
	{
		if (b.isEnable)continue;
		b.isEnable = true;
		b.lifetime = 0.0f;
		b.position = position;
		b.rotation = AngleToTargetDeg(position, aimPoint);
		XMFLOAT2 dir = Normalize({ aimPoint.x - position.x,aimPoint.y - position.y });
		b.velocity = { dir.x * 1500,dir.y * 1500 };//改造重點
		b.collision = { {16.0f,16.0f},16.0f };
		b.box_collision = { {16.0f,16.0f},16.0f,16.0f };
		PlayAudio(shoot_sf);
		SetAudioVolume(shoot_sf, 0.05f);
		break;
	}
}

bool Bullet_IsEnable(int index)
{
	return g_Bullets[index].isEnable;
}

Circle Bullet_GetCollision(int index)
{
	float cx = g_Bullets[index].collision.center.x + g_Bullets[index].position.x;
	float cy = g_Bullets[index].collision.center.y + g_Bullets[index].position.y;



	return { {cx,cy},g_Bullets[index].collision.radius };
}
Box Bullet_GetCollision_Box(int index)
{
	float cx = g_Bullets[index].box_collision.center.x + g_Bullets[index].position.x;
	float cy = g_Bullets[index].box_collision.center.y + g_Bullets[index].position.y;



	return { {cx,cy},g_Bullets[index].box_collision.half_width,g_Bullets[index].box_collision.half_height };
}

void Bullet_Destroy(int index)
{
	g_Bullets[index].isEnable = false;
}
