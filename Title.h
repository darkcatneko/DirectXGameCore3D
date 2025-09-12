#ifndef TITLE_H
#define TITLE_H
#include <Windows.h>

void Title_Initialize(HWND& hWnd);
void Title_Finalize();
void Title_Update(double time);
void Title_Draw();

#endif