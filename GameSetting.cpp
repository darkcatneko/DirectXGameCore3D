#include "GameSetting.h"

GameLanguage g_NowLanguage;

void GameLanguage_Initialize(GameLanguage Language)
{
	g_NowLanguage = Language;
}

void GameLanguage_Set(GameLanguage Language)
{
	g_NowLanguage = Language;
}

GameLanguage GameLanguage_Get()
{
	return g_NowLanguage;
}
