#include "TriggerCollider.h"
#include "DirectXMath.h"
#include "CollisionGameObject.h"
#include "Texture.h"
#include "sprite.h"
#include "Player.h"
#include "Game.h"
#include "EnemyChaser.h"

using namespace DirectX;



static int g_DialogueCollider_Texid = -1;

struct TriggerCollision_Draw_Info
{
	int Gameobject_Texid;
	int tx, ty, tw, th;
	XMFLOAT4 ColliderColor;
	bool isTrigger = false;

	GameobjectColliderBox collision;
	bool isEnable;
	TriggerColliderType type;
};
struct TriggerColliderGameObject_SceneInfo
{
	int sceneId;
	TriggerCollision_Draw_Info* mapObject;
	int mapTriggerColliderCount;
};
static TriggerCollision_Draw_Info g_DialogueColliderObjects[]
{
	{-1,950,515,200,120, {1.0f,1.0f,1.0f,1.0f}, false,{{100,60},100,60},true,TriggerColliderType::DIALOGUE},
	{-1,1150,715,200,120,{1.0f,1.0f,1.0f,1.0f}, false,{{100,60},100,60},false,TriggerColliderType::SCENE_CHANGER},
};
static TriggerCollision_Draw_Info g_Second_DialogueColliderObjects[]
{
	{-1,929,170, 238, 203,  {1.0f,1.0f,1.0f,1.0f}, false,{{119,102},119,102},true,TriggerColliderType::DRAW},
	{-1,1158,543,1066,200,  {1.0f,1.0f,1.0f,1.0f}, false,{{533,100},533,100},true,TriggerColliderType::DRAW},
	{-1,1287,1116,754,384,  {1.0f,1.0f,1.0f,1.0f}, false,{{377,192},377,192},true,TriggerColliderType::DRAW},
	{-1,929,370, 238, 203,  {1.0f,1.0f,1.0f,1.0f}, false,{{119,102},119,102},true,TriggerColliderType::DIALOGUE},
	{-1,1287,1116, 100, 100,{1.0f,1.0f,1.0f,1.0f}, false,{{50,50},50,50},true,TriggerColliderType::INDICATOR},
	{-1,2180,1266, 50, 50,  {1.0f,1.0f,1.0f,1.0f}, false,{{25,25},25,25},false,TriggerColliderType::SCENE_CHANGER},
};
static TriggerColliderGameObject_SceneInfo TriggerCollider_Database[]
{
	{0,g_DialogueColliderObjects,2},
	{1,g_Second_DialogueColliderObjects,6},
};
void TriggerCollider_Initialize()
{
	g_DialogueColliderObjects[0].Gameobject_Texid = Texture_Load(L"Hint.png");
	g_DialogueColliderObjects[0].isEnable = true;
	
}

void TriggerCollider_Finalize()
{
}

void TriggerCollider_Update(double time)
{
	for (int i = 0; i < TriggerCollider_Database[Get_GameScene()].mapTriggerColliderCount; i++)
	{
		if (TriggerCollider_Database[Get_GameScene()].mapObject[i].isEnable == false) continue;
		if (OnGameObjectCollisionEnter(Player_GetBoxCollision(), TriggerCollider_GetCollision(i)) || (OnGameObjectCollisionEnter(EnemyChaser_GetCollision(), TriggerCollider_GetCollision(i)) && TriggerCollider_Database[Get_GameScene()].mapObject[i].type == TriggerColliderType::DRAW))
		{
			TriggerCollider_Database[Get_GameScene()].mapObject[i].isTrigger = true;
		}
		else
		{
			TriggerCollider_Database[Get_GameScene()].mapObject[i].isTrigger = false;
		}
	}
}

void TriggerCollider_Draw()
{
	for (int i = 0; i < TriggerCollider_Database[Get_GameScene()].mapTriggerColliderCount; i++)
	{
		if (TriggerCollider_Database[Get_GameScene()].mapObject[i].isEnable == false || TriggerCollider_Database[Get_GameScene()].mapObject[i].type == TriggerColliderType::DRAW) continue;
		Sprite_Draw(g_DialogueColliderObjects[0].Gameobject_Texid, TriggerCollider_GetCollision(i).center.x - 50, TriggerCollider_GetCollision(i).center.y - 50, 100, 100, 1.0f, TriggerCollider_Database[Get_GameScene()].mapObject[i].ColliderColor);
	}
}

void TriggerColliderInteraction()
{

}

void SetTriggerColliderActive(int scene, int index, bool active) //需更改
{
	TriggerCollider_Database[scene].mapObject[index].isEnable = active;
}

GameobjectColliderBox TriggerCollider_GetCollision(int index)
{
	float cx = TriggerCollider_Database[Get_GameScene()].mapObject[index].collision.center.x + TriggerCollider_Database[Get_GameScene()].mapObject[index].tx;
	float cy = TriggerCollider_Database[Get_GameScene()].mapObject[index].collision.center.y + TriggerCollider_Database[Get_GameScene()].mapObject[index].ty;



	return { {cx,cy},TriggerCollider_Database[Get_GameScene()].mapObject[index].collision.half_width, TriggerCollider_Database[Get_GameScene()].mapObject[index].collision.half_height };
}

int checkTriggerHintBlock()
{
	int istriggered = -1;
	for (int i = 0; i < TriggerCollider_Database[Get_GameScene()].mapTriggerColliderCount; i++)
	{
		if (TriggerCollider_Database[Get_GameScene()].mapObject[i].isEnable == false) continue;
		if (TriggerCollider_Database[Get_GameScene()].mapObject[i].isTrigger && TriggerCollider_Database[Get_GameScene()].mapObject[i].type != TriggerColliderType::DRAW)
		{
			istriggered = i;
		}
	}
	return istriggered;
}

bool check_Scene_Trigger_Stay(int scene, int index)
{
	if (TriggerCollider_Database[scene].mapObject[index].isEnable == false) return false;
	return TriggerCollider_Database[scene].mapObject[index].isTrigger;
}

void RefreshTrigger()
{
	for (int i = 0; i < 6; i++)
	{
		g_Second_DialogueColliderObjects[i].isEnable = true;
		if (i == 5)
		{
			g_Second_DialogueColliderObjects[i].isEnable = false;
		}
		g_Second_DialogueColliderObjects[i].isTrigger = false;
	}
}
