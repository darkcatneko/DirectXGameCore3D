#include "Map.h"
#include "DirectXMath.h"
#include "Cube.h"
#include <vector>
#include "Texture.h"
#include "Key_Logger.h"
#include "sprite.h"
#include <fstream>
#include "MouseRenderer.h"
#include "Model_Static.h"
#include "Player3D.h"
using namespace DirectX; 

static constexpr int g_MapObjectCount = 1024;

static MODEL_STATIC* CoinModelTexId;

static MapObject g_MapObjects[g_MapObjectCount]
{
	{1,{ 0.0f,10.0f, 0.0f}},
	{1,{ 1.0f,10.0f, 0.0f}},
	{1,{-1.0f,10.0f, 0.0f}},
	{1,{ 0.0f,10.0f, 1.0f}},
	{1,{ 1.0f,10.0f, 1.0f}},
	{1,{-1.0f,10.0f, 1.0f}},
	{1,{ 0.0f,10.0f, 2.0f}},
	{1,{ 1.0f,10.0f, 2.0f}},
	{1,{-1.0f,10.0f, 2.0f}},
	{1,{ 0.0f+5.0f,5.0f, 0.0f}},
	{1,{ 1.0f+5.0f,5.0f, 0.0f}},
	{1,{-1.0f+5.0f,5.0f, 0.0f}},
	{1,{ 0.0f+5.0f,5.0f, 1.0f}},
	{1,{ 1.0f+5.0f,5.0f, 1.0f}},
	{1,{-1.0f+5.0f,5.0f, 1.0f}},
	{1,{ 0.0f+5.0f,5.0f, 2.0f}},
	{1,{ 1.0f+5.0f,5.0f, 2.0f}},
	{1,{-1.0f+5.0f,5.0f, 2.0f}},
	{2,{-1.0f,11.5f, 2.0f}},
	{2,{-2.0f,12.5f,-1.0f}},
};

static int mapIconTexId = 0;
bool isMapping = false;
void Map_Initialize()
{
	mapIconTexId = Texture_Load(L"mapIcon.png");
	CoinModelTexId = Model_Static_Load("coin.fbx", 0.005f, false);
	for (MapObject& o : g_MapObjects)
	{
		if (o.KindId == -1)continue;
		switch (o.KindId)
		{
		case 1:
			o.Collision = Cube_GetAABB(o.Position);
			o.IsTriggered = false;
			break;
		case 2:
			o.Collision = Cube_GetAABB(o.Position);
			o.IsTriggered = true;
			break;
		default:
			break;
		}
		
	}
}

void Map_Finitialize()
{
}

void Map_Update(double elapsed_time)
{
	if (KeyLogger_IsTrigger(KK_M))
	{
		isMapping = !isMapping;
	}
	if (KeyLogger_IsTrigger(KK_R))
	{
		SaveMap("TestMap.map");
	}
	if (KeyLogger_IsTrigger(KK_T))
	{
		LoadMap("TestMap.map");
	}
	AABB player = GetPlayer_AABB();
	for (int i = 0; i < Map_GetObjectsCount(); i++)
	{
		if (Map_GetObjects(i)->IsTriggered == false)continue;
		AABB Object = Map_GetObjects(i)->Collision;

		//被重力拖的物件有被撞到嗎
		Hit hit = Collision_IsHitAABB(Object, player);
		if (hit.isHit)
		{
			g_MapObjects[i].KindId = -1;
		}
	}
}

void Map_Draw()
{
	for (const MapObject& o:g_MapObjects)
	{
		switch (o.KindId)
		{
		case 1:
			Cube_Draw(o.Position);
			break;
		case 2:
			Model_Static_Draw(CoinModelTexId,new GameObject(o.Position));
			break;
		default:
			break;
		}
	}
	if (isMapping)
	{
		Sprite_Draw(mapIconTexId, 200, 50, 50, 50, 1.0f, {1,1,1,1});
		Cube_Draw(GetMouseToMapLocation());

		if (MouseLogger_IsTrigger(0))
		{
			for (int i = 0; i < g_MapObjectCount; i++)
			{
				if (g_MapObjects[i].KindId != -1)continue;
				g_MapObjects[i].KindId = 1;
				g_MapObjects[i].Position = GetMouseToMapLocation();
				g_MapObjects[i].Collision = Cube_GetAABB(g_MapObjects[i].Position);
				return;
			}
		}
	}
	
}

int Map_GetObjectsCount()
{
	return sizeof(g_MapObjects)/sizeof(g_MapObjects[0]);
}

const MapObject* Map_GetObjects(int index)
{
	return &g_MapObjects[index];
}

void SaveMap(const char* filename)
{
	std::ofstream ofs(filename, std::ios::binary);
	if (!ofs) return;


	// 1. 寫入實際使用的物件
	ofs.write(
		(char*)g_MapObjects,
		sizeof(MapObject) * g_MapObjectCount
	);

	ofs.close();
}

void LoadMap(const char* filename)
{
	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs) return;



	// 1. 讀物件資料
	ifs.read(
		(char*)g_MapObjects,
		sizeof(MapObject) * g_MapObjectCount
	);

	ifs.close();
}
