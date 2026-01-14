#ifndef GRID_H
#define GRID_H
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "shader.h"
#include "Shader3D.h"
#include "Collision3D.h"
#include "Model_Static.h"


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
void Grid_DebugDrawSphere(
	const Sphere& sphere,
	const DirectX::XMFLOAT4& color
);
void DrawTriMesh_Gizmo(
	      TriMeshCollider& col,
	const DirectX::XMFLOAT3& world,
	const DirectX::XMFLOAT4& color);

void Debug_DrawLine(
	const DirectX::XMFLOAT3& origin,
	const DirectX::XMFLOAT3& end,
	DirectX::XMFLOAT4 color
);
void DebugDrawSector(const Sector& sector, const DirectX::XMFLOAT4& color, int arcSegments = 24);

void DrawRotatingGizmo_TranslateRotateStyle(
	const DirectX::XMFLOAT3& objectPos,
	const  DirectX::XMFLOAT3& objectForward, // 物件朝向（若你要以物件 local 軸當環的方向）
	float timeSeconds,
	float screenRadiusPx = 80.0f);
#endif // !GRID_H

