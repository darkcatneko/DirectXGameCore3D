#ifndef DIALOGUEUIMANAGER_H
#define DIALOGUEUIMANAGER_H
#include "DirectXMath.h"

constexpr int MAX_DIALOGUE = 75;
constexpr int PART_A_CONVERSATION_COUNT = 15;
enum class Dialogue_Character_AnimationStatus
{
	None,
	Finish_Back,
	Start_Front,
	Finish_Front,
	Start_Back,
	Finish_Black,
	Start_Black,
	Finish_White,
	Start_White,
};
enum class Dialogue_Character_ConversationStatus
{
	None,
	Left,
	Right,
	Clear,
};
enum class Story_System_Tag
{
	PLOT_A,
	PLOT_B,
	PLOT_C,
	PLOT_D,
};
struct Dialogue_Info
{
	int storyId;
	Story_System_Tag storyTag;
	Dialogue_Character_ConversationStatus* Dialogue_conversationStatus;
	int dialogueLength;
};
struct  DialogueCanvasObject_DrawInfo
{
	int CanvasObject_Texid;
	int tx, ty, tw, th;
	DirectX::XMFLOAT4 CanvasObject_color;
};
struct Plot_Dialogue_DrawInfo_Data
{
	int id;
	DialogueCanvasObject_DrawInfo* draw_info;
	int max_dialogue;
	int max_plot;
};
struct Dialogue_Character
{
	int CanvasObject_Texid;
	int tx, ty, tw, th;
	DirectX::XMFLOAT4 CanvasObject_color;
	Dialogue_Character_AnimationStatus now_status;
	float passtime = 0;
};
struct Conversation_status
{
	Dialogue_Character_ConversationStatus status;
};
void DialogueUIManager_Initialize();
void DialogueUIManager_Finalize();
void DialogueUIManager_Update(double time);
void DialogueUIManager_Draw();
void DialogueUIManagerBattleDraw();
bool DialogueManager_To_NextLine();
void DialogueManager_Skip();
void DialogueManager_Reset(int scene);
void Dialogue_ChangeCharacter(int index);

#endif // !DIALOGUEUIMANAGER_H

