#ifndef CUBE_H
#define CUBE_H
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "shader.h"
#include "Shader3D.h"



void Cube_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Cube_Finitialize();
void Cube_Update(double elapsed_time);
void Cube_Draw();

#endif
