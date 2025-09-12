#include "Background.h"
#include "Texture.h"
#include "sprite.h"
#include "sprite_anim.h"
#include "Scene.h"
#include "DirectXMath.h"
#include "TriggerCollider.h"

using namespace DirectX;

void anim_init();

struct  Background_Sprite_Draw_Info
{
	int id = -1;
	int backgroundItem_TexId = -1;
	int tx = 0, ty = 0, tw = 0, th = 0;
	XMFLOAT4 sprite_color = { 1.0f,1.0f,1.0f,1.0f };

	bool isEnable = false;
};

static int g_BackGround_TexID = -1;
static int g_Background_BoatA_animPattern = -1;
static int g_Background_BoatB_animPattern = -1;
static int g_Background_Grass_animPattern = -1;
static int g_Background_BirdA_animPattern = -1;
static int g_Background_BirdB_animPattern = -1;
static int g_Background_NPC_A_animPattern = -1;

static int g_Background_BoatA_animPlayer = -1;
static int g_Background_BoatB_animPlayer = -1;
static int g_Background_Grass_animPlayer = -1;
static int g_Background_BirdA_animPlayer = -1;
static int g_Background_BirdB_animPlayer = -1;
static int g_Background_NPC_A_animPlayer = -1;

static int g_BackGround_2_TexID = -1;

static int g_BattleBackground_TexId = -1;
static int g_BattleBackgroundDebug_TexId = -1;
static Background_Sprite_Draw_Info BackDrawInfo[MAX_BACKGROUND_OBJECT]
{
	{0,-1,1634,0,596,220,     {1.0f,1.0f,1.0f,1.0f},true},
	{1,-1,1700,51,534,506,    {1.0f,1.0f,1.0f,1.0f},true},
	{2,-1,1099,0,672,560,     {1.0f,1.0f,1.0f,1.0f},true},
	{3,-1,19,0,406,660,       {1.0f,1.0f,1.0f,1.0f},true},
	{4,-1,411,20,552,636,     {1.0f,1.0f,1.0f,1.0f},true},
	{5,-1,0,684,946,316,      {1.0f,1.0f,1.0f,1.0f},true},
	{6,-1,917,791,257,206,    {1.0f,1.0f,1.0f,1.0f},true},
	{7,-1,1112,601,277,507,   {1.0f,1.0f,1.0f,1.0f},true},
	{8,-1,1820,411,286,665,   {1.0f,1.0f,1.0f,1.0f},true},
	{9,-1,1783,361,440,628,   {1.0f,1.0f,1.0f,1.0f},true},
	{10,-1,1369,468,471,665,  {1.0f,1.0f,1.0f,1.0f},true},
	{11,-1,242,822,901,680,   {1.0f,1.0f,1.0f,1.0f},true},
	{12,-1,1288,1115,942,385, {1.0f,1.0f,1.0f,1.0f},true},
};
static Background_Sprite_Draw_Info FrontDrawInfo[MAX_BACKGROUND_OBJECT]
{
	{0,-1,1634,0,596,220,     {1.0f,1.0f,1.0f,0.75f},false},
	{1,-1,1700,51,534,506,    {1.0f,1.0f,1.0f,0.75f},false},
	{2,-1,1099,0,672,560,     {1.0f,1.0f,1.0f,1.0f},false},
	{3,-1,19,0,406,660,       {1.0f,1.0f,1.0f,0.75f},false},
	{4,-1,411,20,552,636,     {1.0f,1.0f,1.0f,1.0f},false},
	{5,-1,0,684,946,316,      {1.0f,1.0f,1.0f,0.75f},false},
	{6,-1,917,791,257,206,    {1.0f,1.0f,1.0f,0.75f},false},
	{7,-1,1112,601,277,507,   {1.0f,1.0f,1.0f,0.75f},false},
	{8,-1,1820,411,286,665,   {1.0f,1.0f,1.0f,0.75f},false},
	{9,-1,1783,361,440,628,   {1.0f,1.0f,1.0f,0.75f},false},
	{10,-1,1369,468,471,665,  {1.0f,1.0f,1.0f,0.75f},false},
	{11,-1,242,822,901,680,   {1.0f,1.0f,1.0f,0.75f},false},
	{12,-1,1288,1115,942,385, {1.0f,1.0f,1.0f,0.75f},false},
};
static Background_Sprite_Draw_Info  MidDrawInfo
{
	0, -1, 1099, 0, 672, 560, { 1.0f,1.0f,1.0f,1.0f }, false
};

void BackGround_Initialize()
{
#if defined(DEBUG)||defined(_DEBUG)
	g_BattleBackgroundDebug_TexId = Texture_Load(L"1600_900.png");
#endif
	g_BattleBackground_TexId = Texture_Load(L"DOT.png");
	g_BackGround_TexID = Texture_Load(L"Background.png");
	g_BackGround_2_TexID = Texture_Load(L"Babet_map2_items_background.png");
	BackDrawInfo[0].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_tr_backbuilding.png");
	BackDrawInfo[1].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_tr_warehouse.png");
	BackDrawInfo[2].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_tr_tea_house.png");
	BackDrawInfo[3].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_tl_repair_shop.png");
	BackDrawInfo[4].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_tr_apothecary.png");
	BackDrawInfo[5].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_bl_clothesline_ribbons.png");
	BackDrawInfo[6].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_center_barrier.png");
	BackDrawInfo[7].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_br_small_house.png");
	BackDrawInfo[8].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_br_clinic.png");
	BackDrawInfo[9].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_r_utility_pole.png");
	BackDrawInfo[10].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_br_large_house.png");
	BackDrawInfo[11].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_bl_buddha_lotuspond.png");
	BackDrawInfo[12].backgroundItem_TexId = Texture_Load(L"Babet_map2_items_br_residential_casino.png");
	MidDrawInfo.backgroundItem_TexId = BackDrawInfo[2].backgroundItem_TexId;
	for (size_t i = 0; i < 13; i++)
	{
		FrontDrawInfo[i].backgroundItem_TexId = BackDrawInfo[i].backgroundItem_TexId;
	}
	anim_init();
}

void BackGround_Finitialize()
{
}

void BackGroundUpdate(float time)
{
	switch (GetScene())
	{
	case SCENE_GAME:

		break;
	case SCENE_SECOND:
		BackDrawInfo[7].isEnable = !(check_Scene_Trigger_Stay(1, 1) || check_Scene_Trigger_Stay(1, 0));
		BackDrawInfo[8].isEnable = !(check_Scene_Trigger_Stay(1, 1) || check_Scene_Trigger_Stay(1, 0));
		BackDrawInfo[9].isEnable = !(check_Scene_Trigger_Stay(1, 1) || check_Scene_Trigger_Stay(1, 0));
		BackDrawInfo[10].isEnable = !(check_Scene_Trigger_Stay(1, 1) || check_Scene_Trigger_Stay(1, 0));
		FrontDrawInfo[7].isEnable = (check_Scene_Trigger_Stay(1, 1) || check_Scene_Trigger_Stay(1, 0));
		FrontDrawInfo[8].isEnable = (check_Scene_Trigger_Stay(1, 1) || check_Scene_Trigger_Stay(1, 0));
		FrontDrawInfo[9].isEnable = (check_Scene_Trigger_Stay(1, 1) || check_Scene_Trigger_Stay(1, 0));
		FrontDrawInfo[10].isEnable = (check_Scene_Trigger_Stay(1, 1) || check_Scene_Trigger_Stay(1, 0));
		if (check_Scene_Trigger_Stay(1, 1))
		{
			FrontDrawInfo[7].sprite_color = { 1.0f,1.0f,1.0f,0.75f };
			FrontDrawInfo[8].sprite_color = { 1.0f,1.0f,1.0f,0.75f };
			FrontDrawInfo[9].sprite_color = { 1.0f,1.0f,1.0f,0.75f };
			FrontDrawInfo[10].sprite_color = { 1.0f,1.0f,1.0f,0.75f };
		}
		else
		{
			FrontDrawInfo[7].sprite_color = { 1.0f,1.0f,1.0f ,1.0f };
			FrontDrawInfo[8].sprite_color = { 1.0f,1.0f,1.0f ,1.0f };
			FrontDrawInfo[9].sprite_color = { 1.0f,1.0f,1.0f ,1.0f };
			FrontDrawInfo[10].sprite_color = { 1.0f,1.0f,1.0f,1.0f };
		}


		BackDrawInfo[2].isEnable = !check_Scene_Trigger_Stay(1, 0);
		BackDrawInfo[4].isEnable = !check_Scene_Trigger_Stay(1, 0);
		FrontDrawInfo[2].isEnable = check_Scene_Trigger_Stay(1, 0);
		FrontDrawInfo[4].isEnable = check_Scene_Trigger_Stay(1, 0);


		BackDrawInfo[12].isEnable = !check_Scene_Trigger_Stay(1, 2);
		FrontDrawInfo[12].isEnable = check_Scene_Trigger_Stay(1, 2);

		if (check_Scene_Trigger_Stay(1, 0) && check_Scene_Trigger_Stay(1, 1))
		{
			BackDrawInfo[2].isEnable = false;
			FrontDrawInfo[2].isEnable = false;
			MidDrawInfo.isEnable = true;
		}
		else
		{
			MidDrawInfo.isEnable = false;
		}
		break;
	default:
		break;
	}
}

void BackGroundDraw()
{
	switch (GetScene())
	{
	case SCENE_GAME:
		Sprite_Draw(g_BackGround_TexID, 0, 0, 1920.0f, 1080.0f, 0.5f);
		SpriteAnim_Draw(g_Background_BoatA_animPlayer, 647, 105);
		SpriteAnim_Draw(g_Background_BoatB_animPlayer, 1147, 135);
		SpriteAnim_Draw(g_Background_Grass_animPlayer, 1430, 688);
		SpriteAnim_Draw(g_Background_BirdA_animPlayer, 906, 830);
		SpriteAnim_Draw(g_Background_BirdB_animPlayer, 1006, 760);
		SpriteAnim_Draw(g_Background_NPC_A_animPlayer, 777, 400);
		break;
	case SCENE_SECOND:
		Sprite_Draw(g_BackGround_2_TexID, 0, 0, 2230.0f, 1500.0f);
		break;
	default:
		break;
	}
}

void BattleBackGroundDraw()
{
	Sprite_Draw(g_BattleBackground_TexId, 0, 0, 1600.0f, 900.0f, 1.0f, { 0.0f,0.0f,0.0f,1.0f });
#if defined(DEBUG)||defined(_DEBUG)
	//Sprite_Draw(g_BattleBackgroundDebug_TexId, 0, 0, 1600.0f, 900.0f, 1.0f, { 1.0f,1.0f,1.0f,0.5f });
#endif
}

void BackGroundDraw_Back()
{
	if (GetScene() != SCENE_SECOND)return;
	for (int i = 0; i < MAX_BACKGROUND_OBJECT; i++)
	{
		if (BackDrawInfo[i].isEnable == false)continue;
		Sprite_Draw(BackDrawInfo[i].backgroundItem_TexId, BackDrawInfo[i].tx, BackDrawInfo[i].ty, BackDrawInfo[i].tw, BackDrawInfo[i].th, 0.5f, BackDrawInfo[i].sprite_color);
	}
}

void BackGroundDraw_Mid()
{
	if (GetScene() != SCENE_SECOND)return;
	if (MidDrawInfo.isEnable == false) return;
	Sprite_Draw(MidDrawInfo.backgroundItem_TexId, MidDrawInfo.tx, MidDrawInfo.ty, MidDrawInfo.tw, MidDrawInfo.th, 0.5f, MidDrawInfo.sprite_color);

}

void BackGroundDraw_Front()
{
	if (GetScene() != SCENE_SECOND)return;
	for (int i = 0; i < MAX_BACKGROUND_OBJECT; i++)
	{
		if (FrontDrawInfo[i].isEnable == false)continue;
		Sprite_Draw(FrontDrawInfo[i].backgroundItem_TexId, FrontDrawInfo[i].tx, FrontDrawInfo[i].ty, FrontDrawInfo[i].tw, FrontDrawInfo[i].th, 0.5f, FrontDrawInfo[i].sprite_color);
	}
}

void anim_init()
{
	g_Background_BoatA_animPattern = AnimDataInitialize(L"BoatA.png", 8, 8, 0, 0, { 0,0 }, { 0.125f,1 }, { 343,509 }, true, 0.1f, 0.55f);
	g_Background_BoatB_animPattern = AnimDataInitialize(L"BoatB.png", 8, 8, 0, 0, { 0,0 }, { 0.125f,1 }, { 217,350 }, true, 0.1f, 0.55f);
	g_Background_Grass_animPattern = AnimDataInitialize(L"Grass.png", 8, 8, 0, 0, { 0,0 }, { 0.125f,1 }, { 80,56 }, true,   0.1f, 0.55f);
	g_Background_BirdA_animPattern = AnimDataInitialize(L"BirdA.png", 8, 8, 0, 0, { 0,0 }, { 0.125f,1 }, { 108,91 }, true,  0.1f, 0.55f);
	g_Background_BirdB_animPattern = AnimDataInitialize(L"BirdB.png", 8, 8, 0, 0, { 0,0 }, { 0.125f,1 }, { 108,91 }, true,  0.1f, 0.55f);
	g_Background_NPC_A_animPattern = AnimDataInitialize(L"PenC.png", 8, 8, 0, 0, { 0,0 }, { 0.125f,1 }, { 99,124 }, true,   0.1f, 0.55f);

	g_Background_BoatA_animPlayer = SpriteAnim_CreatePlayer(g_Background_BoatA_animPattern, { 0,0 });
	g_Background_BoatB_animPlayer = SpriteAnim_CreatePlayer(g_Background_BoatB_animPattern, { 0,0 });
	g_Background_Grass_animPlayer = SpriteAnim_CreatePlayer(g_Background_Grass_animPattern, { 0,0 });
	g_Background_BirdA_animPlayer = SpriteAnim_CreatePlayer(g_Background_BirdA_animPattern, { 0,0 });
	g_Background_BirdB_animPlayer = SpriteAnim_CreatePlayer(g_Background_BirdB_animPattern, { 0,0 });
	g_Background_NPC_A_animPlayer = SpriteAnim_CreatePlayer(g_Background_NPC_A_animPattern, { 0,0 });
}