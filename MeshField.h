#ifndef MESHFIELD_H
#define MESHFIELD_H
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "shader.h"
#include "Shader3D.h"



void MeshField_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void MeshField_Finitialize();
void MeshField_Draw(DirectX::XMFLOAT3 gameobjectPos);

#endif
