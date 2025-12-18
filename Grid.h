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
void Grid_Draw(DirectX::XMFLOAT3 gameobjectPos, DirectX::XMFLOAT3 gameobjectRot, DirectX::XMFLOAT3 gameobjectScale);

void Grid_DebugDrawRay(
	const DirectX::XMFLOAT3& origin,
	const DirectX::XMFLOAT3& dir,
	float length,
	DirectX::XMFLOAT4 = { 1.0f,0.0f,0.0f,1.0f }
);
#endif // !GRID_H

