#include "PlayerCamera.h"
#include <DirectXMath.h>
#include "key_logger.h"
#include "debug_text.h"
#include <sstream>
#include "shader3d.h"
#include "direct3d.h"
#include "Player3D.h"

namespace 
{
	DirectX::XMFLOAT3 g_PlayerCameraFront = { 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT3 g_PlayerCameraPosition = { 0.0f, 0.0f, 0.0f };
	static DirectX::XMFLOAT4X4 g_PlayerCameraMatrix;
	static DirectX::XMFLOAT4X4 g_PlayerCameraMatrix_Perspective;
}


void PlayerCamera_Initialize()
{

}

void PlayerCamera_Finalize()
{
}

void PlayerCamera_Update(double elapsed_time)
{
	DirectX::XMVECTOR position = DirectX::XMVectorSubtract(
		DirectX::XMLoadFloat3(&GetPlayerPosition()),
		DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetPlayerFront()), 5.0f)
	);

	DirectX::XMVECTOR target = DirectX::XMLoadFloat3(&GetPlayerPosition());

	DirectX::XMVECTOR front = DirectX::XMVector3Normalize(
		DirectX::XMVectorSubtract(target, position)
	);
	DirectX::XMStoreFloat3(&g_PlayerCameraPosition, position);
	DirectX::XMStoreFloat3(&g_PlayerCameraFront, front);


	DirectX::XMMATRIX mtxView = DirectX::XMMatrixLookAtLH(
		position,
		target,
		{ 0.0f,1.0f,0.0f });

	DirectX::XMStoreFloat4x4(&g_PlayerCameraMatrix, mtxView);
	Shader3D_SetViewMatrix(mtxView);

	constexpr float fovAngleY = DirectX::XMConvertToRadians(60.0f);
	float aspectRatio = static_cast<float>(Direct3D_GetBackBufferWidth()) / static_cast<float>(Direct3D_GetBackBufferHeight());
	float nearZ = 0.1f;
	float farZ = 100.0f;

	DirectX::XMMATRIX mtxPerspective = DirectX::XMMatrixPerspectiveFovLH(
		fovAngleY,
		aspectRatio,
		nearZ,
		farZ
	);

	XMStoreFloat4x4(&g_PlayerCameraMatrix_Perspective, mtxPerspective);
	Shader3D_SetProjectionMatrix(mtxPerspective);
}

DirectX::XMFLOAT4X4& PlayerCamera_GetMatrix()
{
	return g_PlayerCameraMatrix;
}

DirectX::XMFLOAT4X4& PlayerCamera_GetMatrixPerspective()
{
	return g_PlayerCameraMatrix_Perspective;
}

const DirectX::XMFLOAT3 PlayerCamera_GetFrontVector()
{
	return g_PlayerCameraFront;
}

const DirectX::XMFLOAT3 PlayerCamera_GetCameraPos()
{
	return g_PlayerCameraPosition;
}


