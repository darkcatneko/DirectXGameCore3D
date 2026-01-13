#include "GameUI.h"
#include "sprite.h"
#include "Texture.h"

int clearTexID = -1;

void GameUI_Initialize()
{
	clearTexID = Texture_Load(L"GameClear.png");
}

void GameUI_Finitialize()
{
}

void GameUI_Update(double elapsed_time)
{
}

void GameUI_Draw()
{

}
