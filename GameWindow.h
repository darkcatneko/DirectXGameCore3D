#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H
#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;


struct button_input_info
{
	bool UP = false;
	bool DOWN = false;
	bool LEFT = false;
	bool RIGHT = false;
};
enum gameStatus
{

};
HWND GameWindow_Create(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void change_vector(XMFLOAT2& target, int x, int y);
XMFLOAT2 return_vector();
void change_vector_to(XMFLOAT2& target, int x, int y);
void check_face(bool input_right);
bool return_face();
#endif // !GAMEWINDOW_H

