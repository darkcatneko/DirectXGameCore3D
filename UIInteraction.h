#ifndef UIINTERACTION_H
#define UIINTERACTION_H
#include <DirectXMath.h>
#include "d3d11.h"

struct UICircle
{
	DirectX::XMFLOAT2 center;
	float radius;
};

struct  UIBox
{
	DirectX::XMFLOAT2 center;
	float half_width;
	float half_height;
};

bool OnUICircleCollisionEnter(const UICircle& a, const UICircle& b);
bool OnUIBoxCollisionEnter(const UIBox& a, const UIBox& b);

void UICollision_Debug_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void UICollision_Debug_Finitialize();
void UICollision_DebugDraw(const UICircle& circle, DirectX::XMFLOAT4 color);
void UICollision_DebugDraw(const UIBox& box);

#endif
