#ifndef GAMESETTING_H
#define GAMESETTING_H

enum class GameLanguage
{
	CHINESE,
	JAPANESE,
};
extern GameLanguage g_NowLanguage;
void GameLanguage_Initialize(GameLanguage Language);
void GameLanguage_Set(GameLanguage Language);
GameLanguage GameLanguage_Get();
#endif
