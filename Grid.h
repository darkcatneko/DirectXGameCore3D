#ifndef GRID_H
#define GRID_H
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "shader.h"
#include "Shader3D.h"


void Grid_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Grid_Finitialize();
void Grid_Update(double elapsed_time);
void Grid_Draw();


#endif // !GRID_H

