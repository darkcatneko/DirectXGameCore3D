/*==============================================================================

   ポリゴン描画 [polygon.h]
														 Author : Youhei Sato
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef POLYGON_H
#define POLYGON_H

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
// 頂点構造体
//struct Vertex
//{
//	XMFLOAT3 position; // 頂点座標
//	XMFLOAT4 color;
//	XMFLOAT2 uv;
//};
void Polygon_Draw_time(int time, XMFLOAT2 vector2);
void Polygon_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Polygon_Finalize(void);
void Polygon_Draw(void);

#endif // POLYGON_H
