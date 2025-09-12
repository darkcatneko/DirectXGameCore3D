#ifndef GAME_H
#define GAME_H

int Get_GameScene();
enum class Player_Layer
{
	GROUND,
	ROOF,
};
void ChangeGameScene(int scene);
void Game_Initialize();
void Game_Finitialize();

void GameUpdate(float time);
void GameDraw();
void Change_Player_Draw_Layer(Player_Layer layer);
Player_Layer GetPlayerLayer();
#endif
