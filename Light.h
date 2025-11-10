#ifndef LIGHT_H
#define LIGHT_H
#include "DirectXMath.h"
#include "d3d11.h"

bool Light_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Light_Finalize();
void Light_SetAmbient(const DirectX::XMFLOAT3& color);
void Light_SetDirectionalWorld(const DirectX::XMFLOAT4& world_directional, const DirectX::XMFLOAT4& color);
void Light_SetSpecularWorld(const DirectX::XMFLOAT4& color,float power, const DirectX::XMFLOAT3& camearPos);

void Light_SetPointLight(int n, const DirectX::XMFLOAT3& position, float range,
	const DirectX::XMFLOAT3& color);
void Light_SetPointLightCount(int count);
#endif // !LIGHT_H

