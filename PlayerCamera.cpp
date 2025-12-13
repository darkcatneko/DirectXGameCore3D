#include "PlayerCamera.h"
#include <DirectXMath.h>
#include "key_logger.h"
#include "debug_text.h"
#include <sstream>
#include "shader3d.h"
#include "shader3d_Static.h"
#include "direct3d.h"
#include "Player3D.h"
#include "Shader_Billboard.h"
#include "shader3d_unlit.h"
#include "ShaderField.h"
using namespace DirectX;

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
	// =====================================================
	// 玩家位置
	// =====================================================
	XMVECTOR playerPos = XMLoadFloat3(&GetPlayerPosition());

	// 想要的相機偏移（第三人稱）
	// 後下 → 更穩定
	static const XMVECTOR CAMERA_OFFSET = { 0.0f, 6.0f, -5.5f };

	// -----------------------------------------------------
	// 1. 計算理想相機位置
	// -----------------------------------------------------
	XMVECTOR desiredPos = playerPos + CAMERA_OFFSET;

	// -----------------------------------------------------
	// 2. 平滑移動（阻尼 SmoothDamp / Lerp）
	// -----------------------------------------------------
	static XMVECTOR currentCameraPos = desiredPos;

	float followSpeed = 5.0f;  // 越大越貼近玩家
	currentCameraPos = XMVectorLerp(currentCameraPos, desiredPos, followSpeed * elapsed_time);

	// -----------------------------------------------------
	// 3. 設定 target（看著玩家，但也要平滑）
	// -----------------------------------------------------
	static XMVECTOR currentTarget = playerPos;
	currentTarget = XMVectorLerp(currentTarget, playerPos, followSpeed * elapsed_time);

	// -----------------------------------------------------
	// 4. 計算相機 front
	// -----------------------------------------------------
	XMVECTOR front = XMVector3Normalize(currentTarget - currentCameraPos);

	// =====================================================
	// 寫回全域
	// =====================================================
	XMStoreFloat3(&g_PlayerCameraPosition, currentCameraPos);
	XMStoreFloat3(&g_PlayerCameraFront, front);

	// =====================================================
	// 生成 View Matrix
	// =====================================================
	XMMATRIX mtxView = XMMatrixLookAtLH(
		currentCameraPos,
		currentTarget,
		{ 0, 1, 0 }
	);


	DirectX::XMStoreFloat4x4(&g_PlayerCameraMatrix, mtxView);
	Shader3D_SetViewMatrix(mtxView);
	Shader3D_Static_SetViewMatrix(mtxView);
	Shader_Billboard_SetViewMatrix(mtxView);
	Shader3DUnilt_SetViewMatrix(mtxView);
	ShaderField_SetViewMatrix(mtxView);

	constexpr float fovAngleY = DirectX::XMConvertToRadians(60.0f);
	float aspectRatio = static_cast<float>(Direct3D_GetBackBufferWidth()) / static_cast<float>(Direct3D_GetBackBufferHeight());
	float nearZ = 0.1f;
	float farZ = 1000.0f;

	DirectX::XMMATRIX mtxPerspective = DirectX::XMMatrixPerspectiveFovLH(
		fovAngleY,
		aspectRatio,
		nearZ,
		farZ
	);

	XMStoreFloat4x4(&g_PlayerCameraMatrix_Perspective, mtxPerspective);
	Shader3D_SetProjectionMatrix(mtxPerspective);
	Shader3D_Static_SetProjectionMatrix(mtxPerspective);
	Shader_Billboard_SetProjectionMatrix(mtxPerspective);
	Shader3DUnilt_SetProjectMatrix(mtxPerspective);
	ShaderField_SetProjectionMatrix(mtxPerspective);
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


