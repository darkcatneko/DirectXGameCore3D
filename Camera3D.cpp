#include "Camera3D.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "Shader3D.h"
#include "Key_Logger.h"
using namespace DirectX;


static XMFLOAT3 Camera3D_Pos;
static XMFLOAT3 Camera3D_AimPos;
float now_angle;
void Camera3D_Initialize()
{
	Camera3D_Pos = { 2.0f, 2.0f, -5.0f };
	Camera3D_AimPos = { 0,0,0 };
}

void Camera3D_Finitialize()
{
}

void Camera3D_Update(double elapsed_time)
{
	if (KeyLogger_IsPressed(KK_J))
	{
		Camera3D_Pos.x -= elapsed_time * 3.0f;
		Camera3D_AimPos.x -= elapsed_time * 3.0f;
	}
	if (KeyLogger_IsPressed(KK_L))
	{
		Camera3D_Pos.x += elapsed_time * 3.0f;
		Camera3D_AimPos.x += elapsed_time * 3.0f;
	}
	if (KeyLogger_IsPressed(KK_I))
	{
		Camera3D_Pos.y += elapsed_time * 3.0f;
		Camera3D_AimPos.y += elapsed_time * 3.0f;
	}
	if (KeyLogger_IsPressed(KK_K))
	{
		Camera3D_Pos.y -= elapsed_time * 3.0f;
		Camera3D_AimPos.y -= elapsed_time * 3.0f;
	}
	if (KeyLogger_IsPressed(KK_Y))
	{
		Camera3D_Pos.z += elapsed_time * 3.0f;
		Camera3D_AimPos.z += elapsed_time * 3.0f;
	}
	if (KeyLogger_IsPressed(KK_H))
	{
		Camera3D_Pos.z -= elapsed_time * 3.0f;
		Camera3D_AimPos.z -= elapsed_time * 3.0f;
	}
	if (KeyLogger_IsPressed(KK_O))
	{
		// 1. 轉成向量
		XMVECTOR camPos = XMLoadFloat3(&Camera3D_Pos);
		XMVECTOR aimPos = XMLoadFloat3(&Camera3D_AimPos);

		// 2. 計算方向向量 (Aim - Pos)
		XMVECTOR dir = aimPos - camPos;

		// 3. 建立繞 Y 軸的旋轉 quaternion
		float angle = XMConvertToRadians(1.0f); // 每次按下旋轉 1 度，可調
		XMVECTOR qRot = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), angle);

		// 4. 旋轉方向向量
		dir = XMVector3Rotate(dir, qRot);

		// 5. 新的 Aim = 相機位置 + 旋轉後的方向
		aimPos = camPos + dir;

		// 6. 存回
		XMStoreFloat3(&Camera3D_AimPos, aimPos);
	}
	if (KeyLogger_IsPressed(KK_U))
	{
		// 1. 轉成向量
		XMVECTOR camPos = XMLoadFloat3(&Camera3D_Pos);
		XMVECTOR aimPos = XMLoadFloat3(&Camera3D_AimPos);

		// 2. 計算方向向量 (Aim - Pos)
		XMVECTOR dir = aimPos - camPos;

		// 3. 建立繞 Y 軸的旋轉 quaternion
		float angle = XMConvertToRadians(-1.0f); // 每次按下旋轉 1 度，可調
		XMVECTOR qRot = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), angle);

		// 4. 旋轉方向向量
		dir = XMVector3Rotate(dir, qRot);

		// 5. 新的 Aim = 相機位置 + 旋轉後的方向
		aimPos = camPos + dir;

		// 6. 存回
		XMStoreFloat3(&Camera3D_AimPos, aimPos);
	}
	if (KeyLogger_IsPressed(KK_R))
	{
		// 1. 轉成向量
		XMVECTOR camPos = XMLoadFloat3(&Camera3D_Pos);
		XMVECTOR aimPos = XMLoadFloat3(&Camera3D_AimPos);

		// 2. 計算方向向量 (Aim - Pos)
		XMVECTOR dir = aimPos - camPos;

		// 3. 建立繞 Y 軸的旋轉 quaternion
		float angle = XMConvertToRadians(1.0f); // 每次按下旋轉 1 度，可調
		XMVECTOR qRot = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), angle);

		// 4. 旋轉方向向量
		dir = XMVector3Rotate(dir, qRot);

		// 5. 新的 Aim = 相機位置 + 旋轉後的方向
		aimPos = camPos + dir;

		// 6. 存回
		XMStoreFloat3(&Camera3D_AimPos, aimPos);
	}
	if (KeyLogger_IsPressed(KK_F))
	{
		// 1. 轉成向量
		XMVECTOR camPos = XMLoadFloat3(&Camera3D_Pos);
		XMVECTOR aimPos = XMLoadFloat3(&Camera3D_AimPos);

		// 2. 計算方向向量 (Aim - Pos)
		XMVECTOR dir = aimPos - camPos;

		// 3. 建立繞 Y 軸的旋轉 quaternion
		float angle = XMConvertToRadians(-1.0f); // 每次按下旋轉 1 度，可調
		XMVECTOR qRot = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), angle);

		// 4. 旋轉方向向量
		dir = XMVector3Rotate(dir, qRot);

		// 5. 新的 Aim = 相機位置 + 旋轉後的方向
		aimPos = camPos + dir;

		// 6. 存回
		XMStoreFloat3(&Camera3D_AimPos, aimPos);
	}
	//view matrix
	XMMATRIX mtxView = XMMatrixLookAtLH(
		{ Camera3D_Pos.x, Camera3D_Pos.y, Camera3D_Pos.z },
		{ Camera3D_AimPos.x, Camera3D_AimPos.y, Camera3D_AimPos.z },
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
