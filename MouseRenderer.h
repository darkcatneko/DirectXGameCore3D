#ifndef MOUSERENDERER_H
#define MOUSERENDERER_H
#include "DirectXMath.h"
#include "mouse.h"

void MouseRenderer_Initialize();
void MouseRenderer_Finitialize();
Mouse_State Get_Mouse_Info();
void MouseRenderer_Update(double elapsed_time);
void MouseRenderer_Draw();

DirectX::XMFLOAT3 GetMouseToMapLocation();
#endif
