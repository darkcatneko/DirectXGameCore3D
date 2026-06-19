#ifndef SHADER3D_STATIC_H
#define	SHADER3D_STATIC_H

#include <d3d11.h>
#include <DirectXMath.h>

bool Shader3D_Static_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Shader3D_Static_Finalize();
void Shader3D_Static_SetWorldMatrix(const DirectX::XMMATRIX& matrix);
void Shader3D_Static_SetViewMatrix(const DirectX::XMMATRIX& matrix);
void Shader3D_Static_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);
void Shader3D_Static_SetLightViewProjectionMatrix(const DirectX::XMMATRIX& matrix);
void Shader3D_Static_SetShadowPass(bool enabled);
void Shader3d_Static_SetColor(const DirectX::XMFLOAT4 color);
void Shader3D_Static_Begin();

#endif 
