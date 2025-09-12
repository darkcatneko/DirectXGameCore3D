#ifndef TITLEUIDRAWER_H
#define TITLEUIDRAWER_H
#include "UIInteraction.h"
#include <Windows.h>

static constexpr unsigned int BUTTON_MAX = 4;

void TitleUIDrawer_Initialize(HWND& hWnd);
void TitleUIDrawer_Finalize();
void TitleUIDrawer_Update(double time);
void TitleUIDrawer_Draw();

void TitleUIInteraction();

UIBox TitleUI_GetUICollision(int index);


#endif
