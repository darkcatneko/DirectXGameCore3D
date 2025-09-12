#include "DialogueUIManager.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "Texture.h"
#include "sprite.h"
#include "Player.h"
#include "iostream"
#include "NekoTool.h"
#include "TriggerCollider.h"
#include "Game.h"
#include "EnemyChaser.h"
#include <sstream>
#include "GameSetting.h"
using namespace DirectX;

void do_conversation_Animation();
void trigger_second_scene_first_dialogue();

static const double UI_SCALE = 0.84375;
static const int DIALOGUE_CHARACTER_H = 1103;

static int g_DialogueDatabaseId = -1;
static int g_DialogueBlack_Texid = -1;
static int g_DialogueHint_TexId = -1;
static int g_DialigueHint_JP_TexId = -1;
static int g_nowDialogue = -1;//整體
static int g_nowPlotLine = 0;//單plot刷新

static int main_c_TexId = -1;
static int pen_c_TexId = -1;
static int cleaner_TexId = -1;

static Dialogue_Character Right_Character;
static Dialogue_Character Left_Character;
static int RightId;
static int LeftId;
static Dialogue_Character g_Dialogue_Character_Data[3];
static DialogueCanvasObject_DrawInfo g_CanvasDrawInfos[79];
static DialogueCanvasObject_DrawInfo g_CanvasDrawInfos_B[34];
static DialogueCanvasObject_DrawInfo g_CanvasDrawInfos_JP[79];
static DialogueCanvasObject_DrawInfo g_CanvasDrawInfos_B_JP[34];
static std::wstring g_DialigueFileName[2];
static Plot_Dialogue_DrawInfo_Data g_CanvasDrawInfo_Database[]
{
	{0,g_CanvasDrawInfos,79,3},
	{1,g_CanvasDrawInfos_B,34,1},
};
static Plot_Dialogue_DrawInfo_Data g_CanvasDrawInfo_Database_JP[]
{
	{0,g_CanvasDrawInfos_JP,79,3},
	{1,g_CanvasDrawInfos_B_JP,34,1},
};
static Dialogue_Character_ConversationStatus Dialogue_A_Conversation_Status[]
{
	Dialogue_Character_ConversationStatus::None,
	Dialogue_Character_ConversationStatus::None,
	Dialogue_Character_ConversationStatus::None,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Clear,
};
static Dialogue_Character_ConversationStatus Dialogue_B_Conversation_Status[]
{
	Dialogue_Character_ConversationStatus::None,
	Dialogue_Character_ConversationStatus::None,
	Dialogue_Character_ConversationStatus::None,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Right,
};
static Dialogue_Character_ConversationStatus Dialogue_C_Conversation_Status[]
{
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Right,
};
static Dialogue_Character_ConversationStatus Dialogue_D_Conversation_Status[]
{
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Right,
	Dialogue_Character_ConversationStatus::Clear,
	Dialogue_Character_ConversationStatus::Left,
	Dialogue_Character_ConversationStatus::Left,
};
static Dialogue_Info Dialogue_Database[]
{
	{0,Story_System_Tag::PLOT_A,Dialogue_A_Conversation_Status,19},
	{1,Story_System_Tag::PLOT_B,Dialogue_B_Conversation_Status,5},
	{2,Story_System_Tag::PLOT_C,Dialogue_C_Conversation_Status,55},
	{3,Story_System_Tag::PLOT_D,Dialogue_D_Conversation_Status,13},
	{4,Story_System_Tag::PLOT_D,Dialogue_D_Conversation_Status,4},
	{5,Story_System_Tag::PLOT_D,Dialogue_D_Conversation_Status,4},
	{5,Story_System_Tag::PLOT_D,Dialogue_D_Conversation_Status,12},
};
void DialogueUIManager_Initialize()
{
	main_c_TexId = Texture_Load(L"Character_1.png");
	pen_c_TexId = Texture_Load(L"Character_2.png");
	cleaner_TexId = Texture_Load(L"Scavenger_Plot.png");
	g_DialogueHint_TexId = Texture_Load(L"DialogueHint.png");
	g_DialigueHint_JP_TexId = Texture_Load(L"DialogueHintJP.png");
	g_DialigueFileName[0] = L"A-harbor/A-";
	g_DialigueFileName[1] = L"B-Town/B-";
	g_DialogueDatabaseId = 0;

	g_Dialogue_Character_Data[0] = { main_c_TexId,1000,250,506,DIALOGUE_CHARACTER_H,{0.25f,0.25f,0.25f,1.0f},Dialogue_Character_AnimationStatus::None };
	g_Dialogue_Character_Data[1] = { pen_c_TexId,0,250,506,DIALOGUE_CHARACTER_H,{1.0f,1.0f,1.0f,1.0f},Dialogue_Character_AnimationStatus::None };
	g_Dialogue_Character_Data[2] = { cleaner_TexId,0,150,650,DIALOGUE_CHARACTER_H,{1.0f,1.0f,1.0f,1.0f},Dialogue_Character_AnimationStatus::None };
	Right_Character = g_Dialogue_Character_Data[0];
	Left_Character = g_Dialogue_Character_Data[1];
	RightId = 0;
	LeftId = 1;
	g_DialogueBlack_Texid = Texture_Load(L"DOT.png");
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < g_CanvasDrawInfo_Database[j].max_dialogue; ++i) {
			std::wstring filename = g_DialigueFileName[j] + std::to_wstring(i + 1) + L".png";
			g_CanvasDrawInfo_Database[j].draw_info[i].CanvasObject_Texid = Texture_Load(filename.c_str());
			g_CanvasDrawInfo_Database[j].draw_info[i].tx = 116;
			g_CanvasDrawInfo_Database[j].draw_info[i].ty = 500;
			g_CanvasDrawInfo_Database[j].draw_info[i].tw = 1620;
			g_CanvasDrawInfo_Database[j].draw_info[i].th = 349;
			g_CanvasDrawInfo_Database[j].draw_info[i].CanvasObject_color = { 1.0f,1.0f,1.0f,1.0f };
		}
		for (int z = 0; z < g_CanvasDrawInfo_Database_JP[j].max_dialogue; ++z) {
			std::wstring filename_JP = L"JP-"+g_DialigueFileName[j] + std::to_wstring(z + 1) + L".png";
			g_CanvasDrawInfo_Database_JP[j].draw_info[z].CanvasObject_Texid = Texture_Load(filename_JP.c_str());
			g_CanvasDrawInfo_Database_JP[j].draw_info[z].tx = 116;
			g_CanvasDrawInfo_Database_JP[j].draw_info[z].ty = 500;
			g_CanvasDrawInfo_Database_JP[j].draw_info[z].tw = 1620;
			g_CanvasDrawInfo_Database_JP[j].draw_info[z].th = 349;
			g_CanvasDrawInfo_Database_JP[j].draw_info[z].CanvasObject_color = { 1.0f,1.0f,1.0f,1.0f };
		}
	}
	g_nowDialogue = 0;
}

void DialogueUIManager_Finalize()
{
}

void DialogueUIManager_Update(double time)
{
	float speed = 0.15f;
	float talk_scale = 1.15f;
	trigger_second_scene_first_dialogue();
	if (Left_Character.now_status == Dialogue_Character_AnimationStatus::Start_Front)
	{
		Left_Character.passtime += time;
		float t = Left_Character.passtime / speed;
		if (t >= 1.0f)
		{
			t = 1.0f;
			Left_Character.now_status = Dialogue_Character_AnimationStatus::Finish_Front;
		}
		float easedT = easeOutQuad(t);
		float scale = 1.0f + (talk_scale - 1.0f) * easedT;
		Left_Character.tx = clamp<float>(0 - (talk_scale - 1) * g_Dialogue_Character_Data[LeftId].tw * easedT, 0 - g_Dialogue_Character_Data[LeftId].tw * talk_scale + g_Dialogue_Character_Data[LeftId].tw, 0);
		Left_Character.ty = clamp<float>(g_Dialogue_Character_Data[LeftId].ty - (talk_scale - 1) * DIALOGUE_CHARACTER_H * easedT, g_Dialogue_Character_Data[LeftId].ty - DIALOGUE_CHARACTER_H * talk_scale + DIALOGUE_CHARACTER_H, g_Dialogue_Character_Data[LeftId].ty);
		Left_Character.tw = clamp<float>(g_Dialogue_Character_Data[LeftId].tw * scale, g_Dialogue_Character_Data[LeftId].tw, g_Dialogue_Character_Data[LeftId].tw * talk_scale);
		Left_Character.th = clamp<float>(DIALOGUE_CHARACTER_H * scale, DIALOGUE_CHARACTER_H, DIALOGUE_CHARACTER_H * talk_scale);
		Left_Character.CanvasObject_color = { 0.25f + 0.75f * easedT,0.25f + 0.75f * easedT,0.25f + 0.75f * easedT,1.0f };
	}
	/*if (Left_Character.now_status == Dialogue_Character_AnimationStatus::Start_Back)
	{
		Left_Character.passtime += time;
		float t = Left_Character.passtime / speed;
		if (t >= 1.0f)
		{
			t = 1.0f;
			Left_Character.now_status = Dialogue_Character_AnimationStatus::Finish_Back;
		}
		float easedT = easeOutQuad(t);
		float scale = talk_scale - (talk_scale - 1.0f) * easedT;
		Left_Character.tx = clamp<float>(-(talk_scale - 1)*506 + (talk_scale - 1) * 506 * easedT, 0 - 506 * talk_scale + 506, 0);
		Left_Character.ty = clamp<float>(-(talk_scale - 1) * DIALOGUE_CHARACTER_H + (talk_scale - 1) * DIALOGUE_CHARACTER_H * easedT + 250, 250 - DIALOGUE_CHARACTER_H * talk_scale + DIALOGUE_CHARACTER_H, 250);
		Left_Character.tw = clamp<float>(506 * scale, 506, 506 * talk_scale);
		Left_Character.th = clamp<float>(DIALOGUE_CHARACTER_H * scale, DIALOGUE_CHARACTER_H, DIALOGUE_CHARACTER_H * talk_scale);
		Left_Character.CanvasObject_color = {1.0f - 0.75f * easedT,1.0f - 0.75f * easedT,1.0f - 0.75f * easedT,1.0f };
	}*/
	if (Left_Character.now_status == Dialogue_Character_AnimationStatus::Start_Black)
	{
		Left_Character.passtime += time;
		float t = Left_Character.passtime / speed;
		if (t >= 1.0f)
		{
			t = 1.0f;
			Left_Character.now_status = Dialogue_Character_AnimationStatus::Finish_Black;
		}
		float easedT = easeOutQuad(t);
		Left_Character.CanvasObject_color = { 1.0f - 0.75f * easedT,1.0f - 0.75f * easedT,1.0f - 0.75f * easedT,1.0f };
	}
	if (Left_Character.now_status == Dialogue_Character_AnimationStatus::Start_White)
	{
		Left_Character.passtime += time;
		float t = Left_Character.passtime / speed;
		if (t >= 1.0f)
		{
			t = 1.0f;
			Left_Character.now_status = Dialogue_Character_AnimationStatus::Finish_White;
		}
		float easedT = easeOutQuad(t);
		Left_Character.CanvasObject_color = { 0.25f + 0.75f * easedT,0.25f + 0.75f * easedT,0.25f + 0.75f * easedT,1.0f };
	}
	if (Left_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Front || Left_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Back || Left_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Black || Left_Character.now_status == Dialogue_Character_AnimationStatus::Finish_White)
	{
		Left_Character.passtime = 0;
	}

	/////////////////////////////////////

	if (Right_Character.now_status == Dialogue_Character_AnimationStatus::Start_Front)
	{
		Right_Character.passtime += time;
		float t = Right_Character.passtime / speed;
		if (t >= 1.0f)
		{
			t = 1.0f;
			Right_Character.now_status = Dialogue_Character_AnimationStatus::Finish_Front;
		}
		float easedT = easeOutQuad(t);
		float scale = 1.0f + (talk_scale - 1.0f) * easedT;
		//Right_Character.tx = clamp<float>(1000 + 0.25f * 506 * easedT, 1000,506*0.25f+ 1000);
		Right_Character.ty = clamp<float>(250 - (talk_scale - 1.0f) * DIALOGUE_CHARACTER_H * easedT, 250 - DIALOGUE_CHARACTER_H * talk_scale + DIALOGUE_CHARACTER_H, 250);
		Right_Character.tw = clamp<float>(g_Dialogue_Character_Data[RightId].tw * scale, g_Dialogue_Character_Data[RightId].tw, g_Dialogue_Character_Data[RightId].tw * talk_scale);
		Right_Character.th = clamp<float>(DIALOGUE_CHARACTER_H * scale, DIALOGUE_CHARACTER_H, DIALOGUE_CHARACTER_H * talk_scale);
		Right_Character.CanvasObject_color = { 0.25f + 0.75f * easedT,0.25f + 0.75f * easedT,0.25f + 0.75f * easedT,1.0f };
	}
	//if (Right_Character.now_status == Dialogue_Character_AnimationStatus::Start_Back)
	//{
	//	Right_Character.passtime += time;
	//	float t = Right_Character.passtime / speed;
	//	if (t >= 1.0f)
	//	{
	//		t = 1.0f;
	//		Right_Character.now_status = Dialogue_Character_AnimationStatus::Finish_Back;
	//	}
	//	float easedT = easeOutQuad(t);
	//	float scale = talk_scale - (talk_scale - 1.0f) * easedT;
	//	//Right_Character.tx = clamp<float>(-0.25f * 506 + 0.25f * 506 * easedT+1000, 1000, 506 * 0.25f + 1000);
	//	Right_Character.ty = clamp<float>(-(talk_scale - 1.0f) * DIALOGUE_CHARACTER_H + (talk_scale - 1.0f) * DIALOGUE_CHARACTER_H * easedT + 250, 250 - DIALOGUE_CHARACTER_H * talk_scale + DIALOGUE_CHARACTER_H, 250);
	//	Right_Character.tw = clamp<float>(506 * scale, 506, 506 * talk_scale);
	//	Right_Character.th = clamp<float>(DIALOGUE_CHARACTER_H * scale, DIALOGUE_CHARACTER_H, DIALOGUE_CHARACTER_H * talk_scale);
	//	Right_Character.CanvasObject_color = { 1.0f - 0.75f * easedT,1.0f - 0.75f * easedT,1.0f - 0.75f * easedT,1.0f };
	//}
	if (Right_Character.now_status == Dialogue_Character_AnimationStatus::Start_Black)
	{
		Right_Character.passtime += time;
		float t = Right_Character.passtime / speed;
		if (t >= 1.0f)
		{
			t = 1.0f;
			Right_Character.now_status = Dialogue_Character_AnimationStatus::Finish_Black;
		}
		float easedT = easeOutQuad(t);
		Right_Character.CanvasObject_color = { 1.0f - 0.75f * easedT,1.0f - 0.75f * easedT,1.0f - 0.75f * easedT,1.0f };
	}
	if (Right_Character.now_status == Dialogue_Character_AnimationStatus::Start_White)
	{
		Right_Character.passtime += time;
		float t = Right_Character.passtime / speed;
		if (t >= 1.0f)
		{
			t = 1.0f;
			Right_Character.now_status = Dialogue_Character_AnimationStatus::Finish_White;
		}
		float easedT = easeOutQuad(t);
		Right_Character.CanvasObject_color = { 0.25f + 0.75f * easedT,0.25f + 0.75f * easedT,0.25f + 0.75f * easedT,1.0f };
	}
	if (Right_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Front || Right_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Back || Right_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Black || Right_Character.now_status == Dialogue_Character_AnimationStatus::Finish_White)
	{
		Right_Character.passtime = 0;
	}
}

void DialogueUIManager_Draw()
{
	if (GetPlayerGameStatus() != Player_Status::Dialogue)return;
	Sprite_Draw_UV_UI(g_DialogueBlack_Texid, 0, 0, (float)Direct3D_GetBackBufferWidth(), (float)Direct3D_GetBackBufferHeight(), 1, 1, 0, 0, 1.0f, { 0,0,0,0.25f });
	if (Right_Character.now_status != Dialogue_Character_AnimationStatus::None)
	{
		Sprite_Draw_UV_UI(Right_Character.CanvasObject_Texid, Right_Character.tx, Right_Character.ty, Right_Character.tw, Right_Character.th, 1, 1, 0, 0, 0.57f, Right_Character.CanvasObject_color);
	}
	if (Left_Character.now_status != Dialogue_Character_AnimationStatus::None)
	{
		Sprite_Draw_UV_UI(Left_Character.CanvasObject_Texid, Left_Character.tx, Left_Character.ty, Left_Character.tw, Left_Character.th, 1, 1, 0, 0, 0.57f, Left_Character.CanvasObject_color);
	}
	switch (GameLanguage_Get())
	{
	case GameLanguage::CHINESE:
		Sprite_Draw_UV_UI(g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].CanvasObject_Texid, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tx, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].ty, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tw * UI_SCALE, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].th * UI_SCALE, 1, 1, 0, 0);
		break;
	case GameLanguage::JAPANESE:
		Sprite_Draw_UV_UI(g_CanvasDrawInfo_Database_JP[Get_GameScene()].draw_info[g_nowDialogue].CanvasObject_Texid, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tx, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].ty, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tw * UI_SCALE, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].th * UI_SCALE, 1, 1, 0, 0);
		break;
	}
	switch (GameLanguage_Get())
	{
	case GameLanguage::CHINESE:
		Sprite_Draw_UV_UI(g_DialogueHint_TexId, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tx, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].ty, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tw * UI_SCALE, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].th * UI_SCALE, 1, 1, 0, 0);
		break;
	case GameLanguage::JAPANESE:
		Sprite_Draw_UV_UI(g_DialigueHint_JP_TexId, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tx, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].ty, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tw * UI_SCALE, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].th * UI_SCALE, 1, 1, 0, 0);
		break;
	}
}
void DialogueUIManagerBattleDraw()
{	
	switch (GameLanguage_Get())
	{
	case GameLanguage::CHINESE:
		Sprite_Draw_UV_UI(g_CanvasDrawInfo_Database[1].draw_info[g_nowDialogue].CanvasObject_Texid, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tx, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].ty, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tw * UI_SCALE, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].th * UI_SCALE, 1, 1, 0, 0);
		break;
	case GameLanguage::JAPANESE:
		Sprite_Draw_UV_UI(g_CanvasDrawInfo_Database_JP[1].draw_info[g_nowDialogue].CanvasObject_Texid, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tx, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].ty, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tw * UI_SCALE, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].th * UI_SCALE, 1, 1, 0, 0);
		break;
	}
	switch (GameLanguage_Get())
	{
	case GameLanguage::CHINESE:
		Sprite_Draw_UV_UI(g_DialogueHint_TexId, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tx, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].ty, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tw * UI_SCALE, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].th * UI_SCALE, 1, 1, 0, 0);
		break;
	case GameLanguage::JAPANESE:
		Sprite_Draw_UV_UI(g_DialigueHint_JP_TexId, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tx, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].ty, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].tw * UI_SCALE, g_CanvasDrawInfo_Database[Get_GameScene()].draw_info[g_nowDialogue].th * UI_SCALE, 1, 1, 0, 0);
		break;
	}
}

bool DialogueManager_To_NextLine()
{
	g_nowDialogue += 1;
	g_nowPlotLine += 1;
	do_conversation_Animation();
	if (g_nowPlotLine >= Dialogue_Database[g_DialogueDatabaseId].dialogueLength)
	{
		switch (g_DialogueDatabaseId)
		{
		case 2:
			SetTriggerColliderActive(0, 0, false);
			SetTriggerColliderActive(0, 1, true);
			break;
		case 3:
			EnableChaser();
			break;
		default:
			break;
		}
		g_DialogueDatabaseId++;
		g_nowPlotLine = 0;
		return true;
	}
	return false;
}
void DialogueManager_Skip()
{
	g_nowDialogue = 0;
	for (int i = 0; i < g_DialogueDatabaseId + 1; i++)
	{
		g_nowDialogue += Dialogue_Database[i].dialogueLength;
	}
	switch (g_DialogueDatabaseId)
	{
	case 2:
		SetTriggerColliderActive(0, 0, false);
		SetTriggerColliderActive(0, 1, true);
		break;
	case 3:
		EnableChaser();
		break;
	default:
		break;
	}
	g_DialogueDatabaseId++;
	g_nowPlotLine = 0;

}
void DialogueManager_Reset(int scene)
{
	switch (scene)
	{
	case 0:
		g_nowDialogue = 0;
		g_DialogueDatabaseId = 0;
		break;
	case 1:
		g_nowDialogue = 0;
		g_DialogueDatabaseId = 3;
		break;
	case 2:
		g_nowDialogue = 14;
		g_DialogueDatabaseId = 4;
		break;
	}

}

void do_conversation_Animation()
{
	if ((Left_Character.now_status == Dialogue_Character_AnimationStatus::None || Left_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Back) && Dialogue_Database[g_DialogueDatabaseId].Dialogue_conversationStatus[g_nowPlotLine] == Dialogue_Character_ConversationStatus::Left)
	{
		Left_Character.now_status = Dialogue_Character_AnimationStatus::Start_Front;
	}
	if (Left_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Black && Dialogue_Database[g_DialogueDatabaseId].Dialogue_conversationStatus[g_nowPlotLine] == Dialogue_Character_ConversationStatus::Left)
	{
		Left_Character.now_status = Dialogue_Character_AnimationStatus::Start_White;
	}
	if ((Left_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Front || Left_Character.now_status == Dialogue_Character_AnimationStatus::Finish_White) && Dialogue_Database[g_DialogueDatabaseId].Dialogue_conversationStatus[g_nowPlotLine] == Dialogue_Character_ConversationStatus::Right)
	{
		Left_Character.now_status = Dialogue_Character_AnimationStatus::Start_Black;
	}

	if ((Left_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Front || Left_Character.now_status == Dialogue_Character_AnimationStatus::Finish_White) && Dialogue_Database[g_DialogueDatabaseId].Dialogue_conversationStatus[g_nowPlotLine] == Dialogue_Character_ConversationStatus::Clear)
	{
		Left_Character.now_status = Dialogue_Character_AnimationStatus::Start_Black;
	}

	if ((Right_Character.now_status == Dialogue_Character_AnimationStatus::None || Right_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Back) && Dialogue_Database[g_DialogueDatabaseId].Dialogue_conversationStatus[g_nowPlotLine] == Dialogue_Character_ConversationStatus::Right)
	{
		Right_Character.now_status = Dialogue_Character_AnimationStatus::Start_Front;
	}
	if (Right_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Black && Dialogue_Database[g_DialogueDatabaseId].Dialogue_conversationStatus[g_nowPlotLine] == Dialogue_Character_ConversationStatus::Right)
	{
		Right_Character.now_status = Dialogue_Character_AnimationStatus::Start_White;
	}
	if ((Right_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Front || Right_Character.now_status == Dialogue_Character_AnimationStatus::Finish_White) && Dialogue_Database[g_DialogueDatabaseId].Dialogue_conversationStatus[g_nowPlotLine] == Dialogue_Character_ConversationStatus::Left)
	{
		Right_Character.now_status = Dialogue_Character_AnimationStatus::Start_Black;
	}
	if ((Right_Character.now_status == Dialogue_Character_AnimationStatus::Finish_Front || Right_Character.now_status == Dialogue_Character_AnimationStatus::Finish_White) && Dialogue_Database[g_DialogueDatabaseId].Dialogue_conversationStatus[g_nowPlotLine] == Dialogue_Character_ConversationStatus::Clear)
	{
		Right_Character.now_status = Dialogue_Character_AnimationStatus::Start_Black;
	}
}

void trigger_second_scene_first_dialogue()
{
	if (check_Scene_Trigger_Stay(1, 3))
	{
		SetTriggerColliderActive(1, 3, false);
		PlayerStateChange(Player_Status::Dialogue);
	}
}
void Dialogue_ChangeCharacter(int index)
{
	Left_Character = g_Dialogue_Character_Data[index];
	LeftId = index;
}


