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
#include "GameUI.h"
#include "Camera3D.h"
using namespace DirectX; 

static const int MAP_OBJECT_KIND_COUNT = 5;
static constexpr int g_MapObjectCount = 1024;
static int nowMappingIndex = 1;

static MapObject* chosingObj;

static MODEL_STATIC* CoinModelTexId;
static MODEL_STATIC* MushroomTexId;
static MODEL_STATIC* Grass3X3TexId;
static MODEL_STATIC* GateTexId;
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
XMFLOAT3 prevMousePos;
XMFLOAT3 prevObjPos;

bool isPlacingOnX = false;
bool isPlacingOnY = false;
bool isPlacingOnZ = false;

bool isPlacingOnGridX = false;

static MapObject g_MapObjects[g_MapObjectCount]
{
	{1,{ 0.0f,10.0f, 0.0f},{0,0,0}},
	{1,{ 1.0f,10.0f, 0.0f},{0,0,0}},
	{1,{-1.0f,10.0f, 0.0f},{0,0,0}},
	{1,{ 0.0f,10.0f, 1.0f},{0,0,0}},
	{1,{ 1.0f,10.0f, 1.0f},{0,0,0}},
	{1,{-1.0f,10.0f, 1.0f},{0,0,0}},
	{1,{ 0.0f,10.0f, 2.0f},{0,0,0}},
	{1,{ 1.0f,10.0f, 2.0f},{0,0,0}},
	{1,{-1.0f,10.0f, 2.0f},{0,0,0}},
	{1,{ 0.0f+5.0f,5.0f, 0.0f},{0,0,0}},
	{1,{ 1.0f+5.0f,5.0f, 0.0f},{0,0,0}},
	{1,{-1.0f+5.0f,5.0f, 0.0f},{0,0,0}},
	{1,{ 0.0f+5.0f,5.0f, 1.0f},{0,0,0}},
	{1,{ 1.0f+5.0f,5.0f, 1.0f},{0,0,0}},
	{1,{-1.0f+5.0f,5.0f, 1.0f},{0,0,0}},
	{1,{ 0.0f+5.0f,5.0f, 2.0f},{0,0,0}},
	{1,{ 1.0f+5.0f,5.0f, 2.0f},{0,0,0}},
	{1,{-1.0f+5.0f,5.0f, 2.0f},{0,0,0}},
	{2,{-1.0f,11.5f, 2.0f},{0,0,0}},
	{2,{-2.0f,12.5f,-1.0f},{0,0,0}},
};

static int mapIconTexId = 0;
bool isMapping = false;
bool isMovingObject = false;

static inline XMVECTOR Load3(const XMFLOAT3& v) { return XMLoadFloat3(&v); }
static inline XMFLOAT3 Store3(FXMVECTOR v) { XMFLOAT3 o; XMStoreFloat3(&o, v); return o; }
static inline float Length3(XMVECTOR v) { return XMVectorGetX(XMVector3Length(v)); }

void Map_Initialize()
{
	//LoadMap("TestMap.map");
	mapIconTexId = Texture_Load(L"mapIcon.png");
	CoinModelTexId = Model_Static_Load("coin.fbx", 0.005f, true);
	MushroomTexId = Model_Static_Load("Amanita_big.fbx", 0.01f, false);
	Grass3X3TexId = Model_Static_Load("GrassBlock_3x3_04a.fbx", 1.0f, true);
	GateTexId = Model_Static_Load("Gate_01a.fbx", 1.0f, true);
	for (MapObject& o : g_MapObjects) ///PLUS
	{
		if (o.KindId == -1)continue;
		switch (o.KindId)
		{
		case 1:
			o.Collision = Cube_GetAABB(o.Position);
			o.IsTriggered = false;
			break;
		case 2:
			o.Collision = ModelStatic_GetAABBInWorldSpace(CoinModelTexId,o.Position);
			o.IsTriggered = true;
			break;
		case 3:
			o.Collision = Cube_GetAABB(o.Position);
			o.IsTriggered = false;
			break;
		case 4:
			o.Collision = ModelStatic_GetAABBInWorldSpace(Grass3X3TexId, o.Position);
			o.IsTriggered = false;
			break;
		case 5:
			o.Collision = ModelStatic_GetAABBInWorldSpace(GateTexId, o.Position);
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
				float planeZ = chosingObj->Position.z;   // 或 0.0f

				// 避免 ray 跟平面平行
				if (fabs(ray.dir.z) < 1e-6f)
					return; // 沒有交點

				float t = (planeZ - ray.origin.z) / ray.dir.z;

				prevMousePos =
				{
					ray.origin.x + ray.dir.x * t,
					ray.origin.y + ray.dir.y * t,
					planeZ
				};
				isPlacingOnX = true;
			}
		}
		else if (RayVsAABB(ray, y_aabb, t))
		{
			if (MouseLogger_IsTrigger(1))
			{
				if (chosingObj != nullptr)
				{
					prevObjPos = chosingObj->Position;
				}
				float planeZ = chosingObj->Position.z;   // 或 0.0f

				// 避免 ray 跟平面平行
				if (fabs(ray.dir.z) < 1e-6f)
					return; // 沒有交點

				float t = (planeZ - ray.origin.z) / ray.dir.z;

				prevMousePos =
				{
					ray.origin.x + ray.dir.x * t,
					ray.origin.y + ray.dir.y * t,
					planeZ
				};
				isPlacingOnY = true;
			}
		}
		else if (RayVsAABB(ray, z_aabb, t))
		{
			if (MouseLogger_IsTrigger(1))
			{
				if (chosingObj != nullptr)
				{
					prevObjPos = chosingObj->Position;
				}
				float planeY = chosingObj->Position.y;   // 或 0.0f

				// 避免 ray 跟平面平行
				if (fabs(ray.dir.y) < 1e-6f)
					return; // 沒有交點

				float t = (planeY - ray.origin.y) / ray.dir.y;

				prevMousePos =
				{
					ray.origin.x + ray.dir.x * t,
					planeY,
					ray.origin.z + ray.dir.z * t
				};
				isPlacingOnZ = true;
			}
		}

		if (chosingObj!=nullptr)
		{
			XMVECTOR camPos = Load3(Camera_GetCameraPos());
			XMVECTOR objPos = Load3(chosingObj->Position);

			float dist = Length3(objPos - camPos);
			dist = (dist < 0.01f) ? 0.01f : dist;
			float t; XMFLOAT3 hp;
			float radiusWorld = ComputeGizmoRadiusWorld(dist, Camera_GetFov(), Direct3D_GetBackBufferHeight(), 0);
			if (RayVsRing_Plane(ray, chosingObj->Position, {1,0,0}, radiusWorld, 0.05f, t, &hp))
			{
				isPlacingOnGridX = true;
			}
		}



		if (MouseLogger_IsDown(1))
		{
			if (isPlacingOnX)
			{
				if (chosingObj != nullptr)
				{
					float planeZ = chosingObj->Position.z;   // 或 0.0f

					// 避免 ray 跟平面平行
					if (fabs(ray.dir.z) < 1e-6f)
						return; // 沒有交點

					float t = (planeZ - ray.origin.z) / ray.dir.z;

					XMFLOAT3 thisPos =
					{
						ray.origin.x + ray.dir.x * t,
						ray.origin.y + ray.dir.y * t,
						planeZ
					};
					chosingObj->Position.x = prevObjPos.x + thisPos.x - prevMousePos.x;
					if (chosingObj->KindId == 4)
					{
						chosingObj->Collision = ModelStatic_GetAABBInWorldSpace(Grass3X3TexId, chosingObj->Position);
					}
					else
					{
						chosingObj->Collision = Cube_GetAABB(chosingObj->Position);
					}
				}
			}
			else if(isPlacingOnY)
			{
				if (chosingObj != nullptr)
				{
					float planeZ = chosingObj->Position.z;   // 或 0.0f

					// 避免 ray 跟平面平行
					if (fabs(ray.dir.z) < 1e-6f)
						return; // 沒有交點

					float t = (planeZ - ray.origin.z) / ray.dir.z;

					XMFLOAT3 thisPos =
					{
						ray.origin.x + ray.dir.x * t,
						ray.origin.y + ray.dir.y * t,
						planeZ
					};
					chosingObj->Position.y = prevObjPos.y + thisPos.y - prevMousePos.y;
					if (chosingObj->KindId == 4)
					{
						chosingObj->Collision = ModelStatic_GetAABBInWorldSpace(Grass3X3TexId, chosingObj->Position);
					}
					else
					{
						chosingObj->Collision = Cube_GetAABB(chosingObj->Position);
					}
				}
			}
			else if (isPlacingOnZ)
			{
				if (chosingObj != nullptr)
				{
					float planeY = chosingObj->Position.y;   // 或 0.0f

					// 避免 ray 跟平面平行
					if (fabs(ray.dir.y) < 1e-6f)
						return; // 沒有交點

					float t = (planeY - ray.origin.y) / ray.dir.y;

					XMFLOAT3 thisPos =
					{
						ray.origin.x + ray.dir.x * t,
						planeY,
						ray.origin.z + ray.dir.z * t
					};
					chosingObj->Position.z = prevObjPos.z + thisPos.z - prevMousePos.z;
					if (chosingObj->KindId == 4)
					{
						chosingObj->Collision = ModelStatic_GetAABBInWorldSpace(Grass3X3TexId, chosingObj->Position);
					}
					else
					{
						chosingObj->Collision = Cube_GetAABB(chosingObj->Position);
					}
				}
			}
			
		}
			if (MouseLogger_IsRelease(1))
			{
				isPlacingOnX = false;
				isPlacingOnY = false;
				isPlacingOnZ = false;
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
				g_MapObjects[i].KindId = -1;
				break;
			case 5: // 到達終點
				Game_Clear();
					break;
			default:
				break;
			}
			
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
				nowMappingIndex = clamp(nowMappingIndex + 1, 1, MAP_OBJECT_KIND_COUNT);
			}
			else
			{
				nowMappingIndex = clamp(nowMappingIndex - 1, 1, MAP_OBJECT_KIND_COUNT);
			}
		}
	}
	if (isMapping)
	{
		if (MouseLogger_IsTrigger(0))///PLUS
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
					g_MapObjects[i].Collision = ModelStatic_GetAABBInWorldSpace(CoinModelTexId, g_MapObjects[i].Position);
					g_MapObjects[i].IsTriggered = true;
					break;
				case 3:
					g_MapObjects[i].Collision = Cube_GetAABB(g_MapObjects[i].Position);
					g_MapObjects[i].IsTriggered = false;
					break;
				case 4:
					g_MapObjects[i].Collision = ModelStatic_GetAABBInWorldSpace(Grass3X3TexId, g_MapObjects[i].Position);
					g_MapObjects[i].IsTriggered = false;
					break;
				case 5:
					g_MapObjects[i].Collision = ModelStatic_GetAABBInWorldSpace(GateTexId, g_MapObjects[i].Position);
					g_MapObjects[i].IsTriggered = true;
					break;
				default:
					break;
				}
				return;
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
			Cube_Draw(o.Position, o.Rotation, {1,1,1});
			break;
		case 2:
			Model_Static_Draw(CoinModelTexId,new GameObject(o.Position,o.Rotation));
			AABB_Draw_Debug(o.Collision);
			break;
		case 3:
			Model_Static_Draw(MushroomTexId, new GameObject(o.Position, o.Rotation));
			for (size_t i = 0; i < MushroomTexId->colliders.size(); i++)
			{
				DrawTriMesh_Gizmo(
					MushroomTexId->colliders[i],
					o.Position,
					{ 1,1,0,1 });
			}
		case 4:
			Model_Static_Draw(Grass3X3TexId, new GameObject(o.Position, o.Rotation));
			AABB_Draw_Debug(o.Collision);			
			break;
		case 5:
			Model_Static_Draw(GateTexId, new GameObject(o.Position, {0,XMConvertToRadians(-90),0}));
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
//		AABB_Draw_Debug(x_aabb);
//		AABB_Draw_Debug(y_aabb);
//		AABB_Draw_Debug(z_aabb);
	}
	if (isMapping)
	{
		Sprite_Draw(mapIconTexId, 200, 50, 50, 50, 1.0f, { 1,1,1,1 });

		switch (nowMappingIndex)
		{
		case 1:
			Cube_Draw(GetMouseToMapLocation());
			break;
		case 2:
			Model_Static_Draw(CoinModelTexId, new GameObject(GetMouseToMapLocation()));
			break;
		case 3:
			Model_Static_Draw(MushroomTexId, new GameObject(GetMouseToMapLocation()));
			break;
		case 4:
			Model_Static_Draw(Grass3X3TexId, new GameObject(GetMouseToMapLocation()));
			break;
		case 5:
			Model_Static_Draw(GateTexId, new GameObject(GetMouseToMapLocation()));
			break;
		default:
			break;
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
	if (isPlacingOnGridX)
	{
		dt.SetText("YOUHEI", { 0.0f,0.0f,1.0f,1.0f });

	}

	dt.Draw();
	dt.Clear();
#endif 
}

void Map_DrawRotatingGizmos()
{
	for (const MapObject& o : g_MapObjects)
	{
		switch (o.KindId)
		{		
		case 4:
			DrawRotatingGizmo_TranslateRotateStyle(o.Position, { 0,0,1 }, 0);
			break;
		default:
			break;
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