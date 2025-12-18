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
#include "PlayerData.h"
#include <sstream>
#include "debug_text.h"
#include "NekoTool.h"
#include "Grid.h"
using namespace DirectX; 

static constexpr int g_MapObjectCount = 1024;
static int nowMappingIndex = 1;

static MapObject* chosingObj;

static MODEL_STATIC* CoinModelTexId;
void Map_IsTriggerUpdate();
void Map_MakingUpdate(double elapsed_time);
int PickObjectIndex(float mouseX, float mouseY); 
void Map_MoveObjectUpdate();

XMFLOAT3 x_point;
XMFLOAT3 y_point;
XMFLOAT3 z_point;	
AABB x_aabb;
AABB y_aabb;
AABB z_aabb;
XMFLOAT2 prevMousePos;
XMFLOAT3 prevObjPos;

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
bool isMovingObject = false;

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
		if (isMovingObject)
		{
			isMovingObject = false;
			chosingObj = nullptr;
		}
		isMapping = !isMapping;
	}
	if (KeyLogger_IsTrigger(KK_N))
	{
		if (isMapping)
		{
			isMapping = false;
		}
		isMovingObject = !isMovingObject;
	}
	if (KeyLogger_IsTrigger(KK_R))
	{
		SaveMap("TestMap.map");
	}
	if (KeyLogger_IsTrigger(KK_T))
	{
		LoadMap("TestMap.map");
	}
	Map_IsTriggerUpdate();
	Map_MakingUpdate(elapsed_time);
	if (!isMapping && isMovingObject)
	{
		if (MouseLogger_IsTrigger(0))
		{
			int i = PickObjectIndex(Get_Mouse_Info().x, Get_Mouse_Info().y);
			if (i == -1)
			{
				chosingObj = nullptr;
			}
			else
			{
			chosingObj = &g_MapObjects[i];
			}			
		}
		Ray ray = MakeMouseRay(Get_Mouse_Info().x, Get_Mouse_Info().y);
		float t = 0.0f;
		if (RayVsAABB(ray, x_aabb, t))
		{
			if (MouseLogger_IsTrigger(1))
			{
				if (chosingObj != nullptr)
				{
					prevObjPos = chosingObj->Position;
				}
				prevMousePos = { (float)Get_Mouse_Info().x,(float)Get_Mouse_Info().y };
			}
			if (MouseLogger_IsDown(1))
			{
				if (chosingObj!=nullptr)
				{
					chosingObj->Position.x = prevObjPos.x + (Get_Mouse_Info().x - prevMousePos.x)*0.05f;
					chosingObj->Collision =Cube_GetAABB(chosingObj->Position);
				}
			}
			
		}
	}

	Map_MoveObjectUpdate();
}
void Map_MoveObjectUpdate()
{
	if (chosingObj != nullptr)
	{
	x_point = { chosingObj->Collision.max.x+1.0f , chosingObj->Position.y			  ,chosingObj->Position.z			  };
	y_point = { chosingObj->Position.x,			chosingObj->Collision.max.y+ 1.0f ,chosingObj->Position.z			  };
	z_point = { chosingObj->Position.x,			chosingObj->Position.y			  ,chosingObj->Collision.max.z + 1.0f };
	
	x_aabb = {
		{x_point.x + 1.0f , x_point.y + 0.05f , x_point.z + 0.05f},
		{x_point.x - 1.0f , x_point.y - 0.05f , x_point.z - 0.05f},
	};
	y_aabb = {
		{y_point.x + 0.05f , y_point.y + 1.0f , y_point.z + 0.05f},
		{y_point.x - 0.05f , y_point.y - 1.0f , y_point.z - 0.05f},
	};
	z_aabb = {
		{z_point.x + 0.05f , z_point.y + 0.05f , z_point.z + 1.0f},
		{z_point.x - 0.05f , z_point.y - 0.05f , z_point.z - 1.0f},
	};
	


	}
}
void Map_IsTriggerUpdate()
{
	AABB player = GetPlayer_AABB();
	for (int i = 0; i < Map_GetObjectsCount(); i++)
	{
		if (Map_GetObjects(i)->IsTriggered == false)continue;
		AABB Object = Map_GetObjects(i)->Collision;

		//被重力拖的物件有被撞到嗎
		Hit hit = Collision_IsHitAABB(Object, player);
		if (hit.isHit)
		{
			switch (g_MapObjects[i].KindId)
			{
			case 2: // 撿到金幣
				PlayerData_AddCoin(1);
				break;
			default:
				break;
			}
			g_MapObjects[i].KindId = -1;
		}
	}
}
void Map_MakingUpdate(double elapsed_time)
{
	if (isMapping)
	{
		if (MouseLogger_IsScroll().trigger)
		{
			if (MouseLogger_IsScroll().value>0)
			{
				nowMappingIndex = clamp(nowMappingIndex + 1, 1, 2);
			}
			else
			{
				nowMappingIndex = clamp(nowMappingIndex - 1, 1, 2);
			}
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
			AABB_Draw_Debug(o.Collision);
			break;
		default:
			break;
		}
	}
	if (chosingObj!=nullptr)
	{

		AABB_Draw_Debug_Size(chosingObj->Collision,0.1f);
	}
	if (chosingObj != nullptr)
	{
		Grid_DebugDrawRay({ x_aabb.min.x,x_point.y   ,x_point.z }, { 1,0,0 }, 2.0f);
		Grid_DebugDrawRay({ y_point.x   ,x_aabb.min.y,y_point.z }, { 0,1,0 }, 2.0f, { 0,1,0,1 });
		Grid_DebugDrawRay({ z_point.x   ,z_point.y   ,z_aabb.min.z }, { 0,0,1 }, 2.0f, { 0,0,1,1 });
	}
	if (isMapping)
	{
		Sprite_Draw(mapIconTexId, 200, 50, 50, 50, 1.0f, {1,1,1,1});
		
		switch (nowMappingIndex)
		{
		case 1:
			Cube_Draw(GetMouseToMapLocation());
			break;
		case 2:
			Model_Static_Draw(CoinModelTexId, new GameObject(GetMouseToMapLocation()));
			break;
		default:
			break;
		}
		if (MouseLogger_IsTrigger(0))
		{
			for (int i = 0; i < g_MapObjectCount; i++)
			{
				if (g_MapObjects[i].KindId != -1)continue;
				g_MapObjects[i].KindId = nowMappingIndex;
				g_MapObjects[i].Position = GetMouseToMapLocation();
				switch (g_MapObjects[i].KindId)
				{
				case 1:
					g_MapObjects[i].Collision = Cube_GetAABB(g_MapObjects[i].Position);
					g_MapObjects[i].IsTriggered = false;
					break;
				case 2:
					g_MapObjects[i].Collision = Cube_GetAABB(g_MapObjects[i].Position);
					g_MapObjects[i].IsTriggered = true;
					break;
				default:
					break;
				}
				return;
			}
		}
	}
#if defined(DEBUG)||defined(_DEBUG)
	hal::DebugText dt(Direct3D_GetDevice(), Direct3D_GetContext(),
		L"consolab_ascii_512.png",
		Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight(),
		0.0f, Direct3D_GetBackBufferHeight()-50.0f,
		0, 0,
		0.0f, 0.0f);
	std::stringstream ss;
	ss <<"COIN: "<< PlayerData_GetCoin();
	dt.SetText(ss.str().c_str());
	//dt.SetText("YOUHEI", { 0.0f,0.0f,1.0f,1.0f });

	dt.Draw();
	dt.Clear();
#endif 
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
int PickObjectIndex(float mouseX, float mouseY)
{
	Ray ray = MakeMouseRay(mouseX, mouseY);

	int best = -1;
	float bestT = FLT_MAX;

	for (int i = 0; i < g_MapObjectCount; i++)
	{
		if (g_MapObjects[i].KindId == -1)continue;
		const MapObject* obj = Map_GetObjects(i);
		float t;
		if (RayVsAABB(ray, obj->Collision, t))
		{
			if (t >= 0.0f && t < bestT)
			{
				bestT = t;
				best = i;
			}
		}
	}
	return best;
}