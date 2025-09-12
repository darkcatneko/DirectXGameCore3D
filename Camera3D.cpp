#include "Camera3D.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "Shader3D.h"
using namespace DirectX;

void Camera3D_Initialize()
{
}

void Camera3D_Finitialize()
{
}

void Camera3D_Update(double elapsed_time)
{
	//view matrix
	XMMATRIX mtxView = XMMatrixLookAtLH(
		{ 2.0f, 2.0f, -5.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f });
	Shader3D_SetViewMatrix(mtxView);

	//Perspective array
	//NearZ一定要大于0 是距离
	constexpr float fovAngleY = XMConvertToRadians(60.0f);
	float aspectRatio = (float)Direct3D_GetBackBufferWidth() / Direct3D_GetBackBufferHeight();
	float nearZ = 0.1f;
	float farZ = 100.0f;
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);

	Shader3D_SetProjectionMatrix(mtxPerspective);
}
