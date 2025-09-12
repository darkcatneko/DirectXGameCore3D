#ifndef SHADER3D_H
#define	SHADER3D_H

#include <d3d11.h>
#include <DirectXMath.h>

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


void Shader3D_Begin();

#endif 
