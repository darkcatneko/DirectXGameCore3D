/*==============================================================================

   ライトなし3dシェーダー [shader3d_unlit.h]
														 Author : Harada Ren
														 Date   : 2025/11/21
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SHADER3D_UNLIT_H
#define	SHADER3D_UNLIT_H

#include <d3d11.h>
#include <DirectXMath.h>

bool Shader3DUnilt_Initialize();
void Shader3DUnilt_Finalize();


void Shader3DUnilt_SetWorldMatrix(const DirectX::XMMATRIX& matrix);
void Shader3DUnilt_SetViewMatrix(const DirectX::XMMATRIX& matrix);
void Shader3DUnilt_SetProjectMatrix(const DirectX::XMMATRIX& matrix);
void Shader3DUnilt_SetColor(const DirectX::XMFLOAT4& color);
void Shader3dUnlit_Begin();

#endif // SHADER3D_H
