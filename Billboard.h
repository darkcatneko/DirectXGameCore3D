#ifndef BILLBOARD_H
#define BILLBOARD_H
#include "DirectXMath.h"

void Billboard_Initialize();
void Billboard_Finitialize();
void Billboard_Update(double elapsed_time);
void Billboard_Draw(int texid,DirectX::XMFLOAT3 gameobjectPos,float scale_x,float scale_y);


#endif
