#ifndef BACKGROUND_H
#define BACKGROUND_H

constexpr int MAX_BACKGROUND_OBJECT = 20;
constexpr int MAX_BATTLEGROUND_OBJECT = 4;

void  BackGround_Initialize();
void  BackGround_Finitialize();
void  BackGroundUpdate(float time);
void  BackGroundDraw();
void  BattleBackGroundDraw();

void BackGroundDraw_Back();
void BackGroundDraw_Mid();
void BackGroundDraw_Front();

#endif