#ifndef SHADER3D_H
#define	SHADER3D_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

struct GammaBuffer
{
	float gamma_in;
	float gamma_out;
	float padding1;
	float padding2;
};

bool Shader3D_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Shader3D_Finalize();

void Shader3D_SetWorldMatrix(const DirectX::XMMATRIX& matrix);
void Shader3D_SetViewMatrix(const DirectX::XMMATRIX& matrix);
void Shader3D_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);
void Shader3D_SetLightViewProjectionMatrix(const DirectX::XMMATRIX& matrix);
void Shader3D_SetShadowPass(bool enabled);

void Shader3d_SetColor(const DirectX::XMFLOAT4 color);

void Shader3D_Begin();
void Shader3D_SkinningBegin(std::vector<DirectX::XMMATRIX> &g_SkinMatrices);
#endif 
