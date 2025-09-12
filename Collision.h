#ifndef COLLISION_H
#define COLLISION_H

#include <DirectXMath.h>
#include "d3d11.h"

struct Circle
{
	DirectX::XMFLOAT2 center;
	float radius;
};

struct  Box
{
	DirectX::XMFLOAT2 center;
	float half_width;
	float half_height;
};


bool OnCircleCollisionEnter(const Circle& a, const Circle& b);
bool OnBoxCollisionEnter(const Box& a, const Box& b);
void Collision_Debug_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Collision_Debug_Finitialize();
void Collision_DebugDraw(const Circle& circle, DirectX::XMFLOAT4 color = { 1.0,1.0,1.0,1.0 });
void Collision_DebugDraw_Box(const Box& box);
void Collision_DebugDraw_LINE(const DirectX::XMFLOAT2 pointA, const DirectX::XMFLOAT2 pointB);
#endif