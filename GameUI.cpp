#include "GameUI.h"
#include "sprite.h"
#include "Texture.h"
#include "Key_Logger.h"
#include "Player3D.h"

int clearTexID = -1;
bool isGameClear = false;

void GameUI_Initialize()
{
	clearTexID = Texture_Load(L"GameClear.png");
}

void GameUI_Finitialize()
{
}

void GameUI_Update(double elapsed_time)
{
	if (isGameClear&&KeyLogger_IsTrigger(KK_ENTER))
	{
		isGameClear = false;
		ResetPlayerPosition();

	}
}

void GameUI_Draw()
{
	if (isGameClear)
	{
		Sprite_Draw(clearTexID, 193, 338, 607, 112);
	}
}

void Game_Clear()
{
	isGameClear = true;
}
