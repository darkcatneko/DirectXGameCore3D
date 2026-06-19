#ifndef SHADERFIELD_H
#define SHADERFIELD_H
#include <d3d11.h>
#include <DirectXMath.h>

bool ShaderField_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void ShaderField_Finalize();
void ShaderField_SetWorldMatrix(const DirectX::XMMATRIX& matrix);
void ShaderField_SetViewMatrix(const DirectX::XMMATRIX& matrix);
void ShaderField_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);
void ShaderField_SetLightViewProjectionMatrix(const DirectX::XMMATRIX& matrix);
void ShaderField_SetShadowPass(bool enabled);
void ShaderField_Begin();
void ShaderField_SetColor(const DirectX::XMFLOAT4 color);

#endif
