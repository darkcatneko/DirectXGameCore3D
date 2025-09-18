#include "Camera3D.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "Shader3D.h"
#include "Key_Logger.h"
#include "debug_text.h"
#include "mouse.h"
using namespace DirectX;


static XMFLOAT3 Camera3D_Pos;
static XMFLOAT3 Camera3D_AimPos;

static XMFLOAT3 g_CameraPosition{ 0.0f,0.0f,-5.0f };
static XMFLOAT3 g_CameraFront{ 0.0f,0.0f,1.0f };
static XMFLOAT3 g_CameraUP{ 0.0f,1.0f,0.0f };
static XMFLOAT3 g_CameraRight{ 1.0f,0.0f,0.0f };

static constexpr float CAMERA_MOVE_SPEED = 8.0f;
static constexpr float CAMERA_ROTATION_SPEED = XMConvertToRadians(60);
static XMFLOAT4X4 g_CameraMatrix;
static XMFLOAT4X4 g_CameraMatrix_Perspective;

static bool g_CameraDragging = false;
static XMFLOAT2 g_MousePrevDragPosition;

float now_angle;
void Camera3D_Initialize()
{
	Camera3D_Pos = { 2.0f, 2.0f, -5.0f };
	Camera3D_AimPos = { 0,0,0 };
	g_CameraPosition = { 0.0f,0.0f,-5.0f };
	g_CameraFront = { 0.0f,0.0f, 1.0f };
	g_CameraUP = { 0.0f,1.0f, 0.0f };
	g_CameraRight = { 1.0f,0.0f, 0.0f };
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
		DirectX::XMStoreFloat3(&Camera3D_AimPos, aimPos);
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
		DirectX::XMStoreFloat3(&Camera3D_AimPos, aimPos);
	}
	if (KeyLogger_IsPressed(KK_R))
	{
		// 1) 載入
		XMVECTOR camPos = XMLoadFloat3(&Camera3D_Pos);
		XMVECTOR aimPos = XMLoadFloat3(&Camera3D_AimPos);

		// 2) forward
		XMVECTOR fwd = XMVector3Normalize(aimPos - camPos);

		// 3) 算 Right = normalize( cross(WorldUp, forward) )
		//    這樣 Pitch 會以相機自身的右側為軸來抬頭/低頭7
		const XMVECTOR WORLD_UP = XMVectorSet(0, 1, 0, 0);
		XMVECTOR right = XMVector3Normalize(XMVector3Cross(WORLD_UP, fwd));

		// 4) 繞 Right 軸旋轉（上下看）
		float deg = 1.0f; // 每次按鍵旋轉角度
		float rad = XMConvertToRadians(deg);
		XMVECTOR qPitch = XMQuaternionRotationAxis(right, rad);

		// 5) 旋轉 forward
		XMVECTOR newFwd = XMVector3Normalize(XMVector3Rotate(fwd, qPitch));

		// （可選）避免翻轉：限制抬頭/低頭過度接近 90°
		// 若 dot(newFwd, WORLD_UP) 絕對值太大，代表快翻過頭了，就略過本次旋轉
		float cosLimit = 0.99f; // 越接近 1 限制越嚴
		float upDot = XMVectorGetX(XMVector3Dot(newFwd, WORLD_UP));
		if (fabsf(upDot) < cosLimit) {
			fwd = newFwd;
		}

		// 6) 維持原本目標距離
		float dist = XMVectorGetX(XMVector3Length(aimPos - camPos));
		aimPos = camPos + fwd * dist;

		// 7) 存回
		DirectX::XMStoreFloat3(&Camera3D_AimPos, aimPos);
	}
	if (KeyLogger_IsPressed(KK_F))
	{
		// 1) 載入
		XMVECTOR camPos = XMLoadFloat3(&Camera3D_Pos);
		XMVECTOR aimPos = XMLoadFloat3(&Camera3D_AimPos);

		// 2) forward
		XMVECTOR fwd = XMVector3Normalize(aimPos - camPos);

		// 3) 算 Right = normalize( cross(WorldUp, forward) )
		//    這樣 Pitch 會以相機自身的右側為軸來抬頭/低頭
		const XMVECTOR WORLD_UP = XMVectorSet(0, 1, 0, 0);
		XMVECTOR right = XMVector3Normalize(XMVector3Cross(WORLD_UP, fwd));

		// 4) 繞 Right 軸旋轉（上下看）
		float deg = -1.0f; // 每次按鍵旋轉角度
		float rad = XMConvertToRadians(deg);
		XMVECTOR qPitch = XMQuaternionRotationAxis(right, rad);

		// 5) 旋轉 forward
		XMVECTOR newFwd = XMVector3Normalize(XMVector3Rotate(fwd, qPitch));

		// （可選）避免翻轉：限制抬頭/低頭過度接近 90°
		// 若 dot(newFwd, WORLD_UP) 絕對值太大，代表快翻過頭了，就略過本次旋轉
		float cosLimit = 0.99f; // 越接近 1 限制越嚴
		float upDot = XMVectorGetX(XMVector3Dot(newFwd, WORLD_UP));
		if (fabsf(upDot) < cosLimit) {
			fwd = newFwd;
		}

		// 6) 維持原本目標距離
		float dist = XMVectorGetX(XMVector3Length(aimPos - camPos));
		aimPos = camPos + fwd * dist;

		// 7) 存回
		DirectX::XMStoreFloat3(&Camera3D_AimPos, aimPos);
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

void HAL_Camera_Movement_Update(float time)
{
	XMVECTOR front = XMLoadFloat3(&g_CameraFront);
	XMVECTOR right = XMLoadFloat3(&g_CameraRight);
	XMVECTOR up = XMLoadFloat3(&g_CameraUP);
	XMVECTOR pos = XMLoadFloat3(&g_CameraPosition);

	if (KeyLogger_IsPressed(KK_DOWN))
	{
		XMMATRIX rotation = XMMatrixRotationAxis(right, CAMERA_ROTATION_SPEED * time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		up = XMVector3Cross(front, right);
	}
	if (KeyLogger_IsPressed(KK_UP))
	{
		XMMATRIX rotation = XMMatrixRotationAxis(right, -CAMERA_ROTATION_SPEED * time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		up = XMVector3Cross(front, right);
	}
	if (KeyLogger_IsPressed(KK_Q))
	{
		XMMATRIX rotation = XMMatrixRotationAxis(up, CAMERA_ROTATION_SPEED * time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		right = XMVector3Cross(up, front);
	}
	if (KeyLogger_IsPressed(KK_E))
	{
		XMMATRIX rotation = XMMatrixRotationAxis(up, -CAMERA_ROTATION_SPEED * time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		right = XMVector3Cross(up, front);
	}
	if (KeyLogger_IsPressed(KK_RIGHT))
	{
		XMFLOAT3 worldUP = { 0,1,0 };
		XMVECTOR worldUP_Vec = XMLoadFloat3(&worldUP);
		XMMATRIX rotation = XMMatrixRotationAxis(worldUP_Vec, CAMERA_ROTATION_SPEED * time);
		front = XMVector3TransformNormal(front, rotation);
		right = XMVector3TransformNormal(right, rotation);
		up = XMVector3Cross(front, right);
	}
	if (KeyLogger_IsPressed(KK_LEFT))
	{
		XMFLOAT3 worldUP = { 0,1,0 };
		XMVECTOR worldUP_Vec = XMLoadFloat3(&worldUP);
		XMMATRIX rotation = XMMatrixRotationAxis(worldUP_Vec, -CAMERA_ROTATION_SPEED * time);
		front = XMVector3TransformNormal(front, rotation);
		right = XMVector3TransformNormal(right, rotation);
		up = XMVector3Cross(front, right);
	}
	if (KeyLogger_IsPressed(KK_W))
	{
		pos += up * CAMERA_MOVE_SPEED * time;
	}
	if (KeyLogger_IsPressed(KK_A))
	{
		pos += -right * CAMERA_MOVE_SPEED * time;
	}
	if (KeyLogger_IsPressed(KK_S))
	{
		pos += -up * CAMERA_MOVE_SPEED * time;
	}
	if (KeyLogger_IsPressed(KK_D))
	{
		pos += right * CAMERA_MOVE_SPEED * time;
	}
	if (MouseLogger_IsScroll().trigger)
	{
		pos += MouseLogger_IsScroll().value / 40.0f * front * CAMERA_MOVE_SPEED * time;
	}

	DirectX::XMStoreFloat3(&g_CameraPosition, pos);
	DirectX::XMStoreFloat3(&g_CameraRight, right);
	DirectX::XMStoreFloat3(&g_CameraFront, front);
	DirectX::XMStoreFloat3(&g_CameraUP, up);
	XMMATRIX mtxView = XMMatrixLookAtLH
	(
		pos,
		pos + front,
		up
	);
	DirectX::XMStoreFloat4x4(&g_CameraMatrix, mtxView);
	Shader3D_SetViewMatrix(mtxView);

	constexpr float fovAngleY = XMConvertToRadians(60.0f);
	float aspectRatio = (float)Direct3D_GetBackBufferWidth() / Direct3D_GetBackBufferHeight();
	float nearZ = 0.1f;
	float farZ = 200.0f;
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);

	DirectX::XMStoreFloat4x4(&g_CameraMatrix_Perspective, mtxPerspective);
	Shader3D_SetProjectionMatrix(mtxPerspective);
}

DirectX::XMFLOAT4X4& Camera_GetMatrix()
{
	return g_CameraMatrix;
}

DirectX::XMFLOAT4X4& Camera_GetMatrixPerspective()
{
	return g_CameraMatrix_Perspective;
}

DirectX::XMFLOAT3 Camera_GetFrontVector()
{
	return g_CameraFront;
}
DirectX::XMFLOAT3 Camera_GetCameraPos()
{
	return g_CameraPosition;
}

void CameraDragUpdate(float time)
{
	Mouse_State mouse_state{};
	Mouse_GetState(&mouse_state);

	XMVECTOR front = XMLoadFloat3(&g_CameraFront);
	XMVECTOR right = XMLoadFloat3(&g_CameraRight);
	XMVECTOR up = XMLoadFloat3(&g_CameraUP);
	XMVECTOR pos = XMLoadFloat3(&g_CameraPosition);

	if (MouseLogger_IsTrigger(1) && KeyLogger_IsPressed(KK_LEFTALT))
	{
		g_CameraDragging = true;
		g_MousePrevDragPosition = { (float)mouse_state.x,(float)mouse_state.y };
	}
	if (MouseLogger_IsRelease(1))
	{
		g_CameraDragging = false;
	}
	if (g_CameraDragging)
	{
		XMFLOAT2 value = XMFLOAT2{ (float)mouse_state.x - g_MousePrevDragPosition.x,(float)mouse_state.y - g_MousePrevDragPosition.y };
		XMMATRIX rotation = XMMatrixRotationAxis(right, value.y * time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		up = XMVector3Cross(front, right);

		XMFLOAT3 worldUP = { 0,1,0 };
		XMVECTOR worldUP_Vec = XMLoadFloat3(&worldUP);
		rotation = XMMatrixRotationAxis(worldUP_Vec, value.x * time);
		front = XMVector3TransformNormal(front, rotation);
		right = XMVector3TransformNormal(right, rotation);
		up = XMVector3Cross(front, right);

		g_MousePrevDragPosition = { (float)mouse_state.x,(float)mouse_state.y };
	}

	DirectX::XMStoreFloat3(&g_CameraPosition, pos);
	DirectX::XMStoreFloat3(&g_CameraRight, right);
	DirectX::XMStoreFloat3(&g_CameraFront, front);
	DirectX::XMStoreFloat3(&g_CameraUP, up);
	XMMATRIX mtxView = XMMatrixLookAtLH
	(
		pos,
		pos + front,
		up
	);
	DirectX::XMStoreFloat4x4(&g_CameraMatrix, mtxView);
	Shader3D_SetViewMatrix(mtxView);

	constexpr float fovAngleY = XMConvertToRadians(60.0f);
	float aspectRatio = (float)Direct3D_GetBackBufferWidth() / Direct3D_GetBackBufferHeight();
	float nearZ = 0.1f;
	float farZ = 200.0f;
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);

	DirectX::XMStoreFloat4x4(&g_CameraMatrix_Perspective, mtxPerspective);
	Shader3D_SetProjectionMatrix(mtxPerspective);
}