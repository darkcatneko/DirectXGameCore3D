#include "CollisionGameObject.h"
#include "DirectXMath.h"
#include "Texture.h"
#include "sprite.h"
#include "Game.h"
using namespace DirectX;

static int g_DebugTexId = -1;

struct CollisionGameObject_Draw_Info
{
	int Gameobject_Texid;
	int tx, ty, tw, th;
	XMFLOAT4 ColliderColor;

	GameobjectColliderBox collision;
};
struct CollisionGameObject_SceneInfo
{
	int sceneId;
	CollisionGameObject_Draw_Info* mapObjects;
	int mapObjectCount;
};
static CollisionGameObject_Draw_Info g_MapGameObjects[]
{
	//{-1,0,0,680,1080,   {0.0f,0.0f,0.0f,0.5f},{{340.0f,540.0f},340,540}},
	{-1,0,0,1920,490,   {0.0f,0.0f,0.0f,0.0f},{{860.0f,245.0f},860,245}},
	{-1,1420,0,680,1080,{0.0f,0.0f,0.0f,0.0f},{{340.0f,540.0f},340,540}},
	{-1,680,0,276,600,  {0.0f,0.0f,0.0f,0.0f},{{138.0f,300.0f},138,300}},
	{-1,0,870,906,600,  {0.0f,0.0f,0.0f,0.0f},{{453.0f,300.0f},453,300}},
	{-1,930,805,200,600,{0.0f,0.0f,0.0f,0.0f},{{100.0f,300.0f},100,300}},
};
static CollisionGameObject_Draw_Info g_MapGameObjects_2[]
{
	{-1,0   ,0   ,922 ,1500,   {0.0f,0.0f,0.0f,0.0f},{{461.0f,750.0f},461,750}},
	{-1,1158,0   ,1072,490 ,   {0.0f,0.0f,0.0f,0.0f},{{536.0f,245.0f},536,245}},
	{-1,1158,677 ,894 ,336 ,   {0.0f,0.0f,0.0f,0.0f},{{447.0f,168.0f},447,168}},
	{-1,922 ,677 ,214 ,815 ,   {0.0f,0.0f,0.0f,0.0f},{{107.0f,408.0f},107,408}},
	{-1,1312,1223,920 ,218 ,   {0.0f,0.0f,0.0f,0.0f},{{460.0f,109.0f},460,109}},
	{-1,1428,980,60 , 60 ,   {0.0f,0.0f,0.0f,0.0f},{{30.0f,30.0f},30,30}},
};
static CollisionGameObject_Draw_Info g_MapGameObjects_2_Roof[]
{
	{-1,0   ,0   ,1314 ,1500,   {0.0f,0.0f,0.0f,0.0f},{{657.0f,750.0f},657.0f,750.0f}},
	{-1,1314,0   ,916  ,1196,   {0.0f,0.0f,0.0f,0.0f},{{458.0f,598.0f},458.0f,598.0f}},
	{-1,1314,1324   ,916  ,146,   {0.0f,0.0f,0.0f,0.0f},{{458.0f,73},458.0f,73}},
};
static CollisionGameObject_SceneInfo CollisionGameObject_Database[]
{
	{0,g_MapGameObjects,  5},
    {1,g_MapGameObjects_2,6},
	{2,g_MapGameObjects_2_Roof,3},
};
void CollisionGameObject_Initialize()
{
	g_MapGameObjects[0].Gameobject_Texid = Texture_Load(L"DOT.png");
}

void CollisionGameObject_Finalize()
{

}

void CollisionGameObject_Update(double time)
{
}

void CollisionGameObject_Draw()
{
	for (int i = 0; i < CollisionGameObject_Database[Get_GameScene()].mapObjectCount; i++)
	{
		Sprite_Draw(g_MapGameObjects[0].Gameobject_Texid, CollisionGameObject_Database[Get_GameScene()].mapObjects[i].tx, CollisionGameObject_Database[Get_GameScene()].mapObjects[i].ty, CollisionGameObject_Database[Get_GameScene()].mapObjects[i].tw, CollisionGameObject_Database[Get_GameScene()].mapObjects[i].th, 1.0f, CollisionGameObject_Database[Get_GameScene()].mapObjects[i].ColliderColor);
	}

}

void CollisionGameObjectInteraction()
{

}

GameobjectColliderBox CollisionGameObject_GetGameobjectCollision(int scene,int index)
{
	float cx = CollisionGameObject_Database[scene].mapObjects[index].collision.center.x + CollisionGameObject_Database[scene].mapObjects[index].tx;
	float cy = CollisionGameObject_Database[scene].mapObjects[index].collision.center.y + CollisionGameObject_Database[scene].mapObjects[index].ty;



	return { {cx,cy}, CollisionGameObject_Database[scene].mapObjects[index].collision.half_width,  CollisionGameObject_Database[scene].mapObjects[index].collision.half_height };
}

int GetCollisionObjectSceneCount(int scene)
{
	return  CollisionGameObject_Database[scene].mapObjectCount;
}


