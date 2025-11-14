#include "Camera3D.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "Shader3D.h"
#include "Key_Logger.h"
#include "debug_text.h"
#include "mouse.h"
#include "PlayerCamera.h"
#include "Shader_Billboard.h"
using namespace DirectX;

enum CameraControlStatus
{
	PlayerFollow,
	Free,
};
static CameraControlStatus g_cameraControlStatus;

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
static float g_Fov = XMConvertToRadians(60);

static bool g_CameraDragging = false;
static XMFLOAT2 g_MousePrevDragPosition;

float now_angle;
void Camera3D_Initialize(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& right)
{
	g_cameraControlStatus = PlayerFollow;
	Camera3D_Pos = { 2.0f, 2.0f, -5.0f };
	Camera3D_AimPos = { 0,0,0 };

	g_CameraPosition = position;
	XMVECTOR f = XMVector3Normalize(XMLoadFloat3(&front));
	XMVECTOR r = XMVector3Normalize(XMLoadFloat3(&right) * XMVECTOR {1.0f,0.0f,1.0f});
	XMVECTOR u = XMVector3Normalize(XMVector3Cross(f, r));
	XMStoreFloat3(&g_CameraFront, f);
	XMStoreFloat3(&g_CameraRight, r);
	XMStoreFloat3(&g_CameraUP, u);

}

void Camera3D_Finitialize()
{
}

void Camera3D_Update(double elapsed_time)
{
	switch (g_cameraControlStatus)
	{
	case PlayerFollow:
		PlayerCamera_Update(elapsed_time);
		break;
	case Free:
		HAL_Camera_Movement_Update(elapsed_time);
		CameraDragUpdate(elapsed_time);
		break;
	default:
		break;
	}
	if (KeyLogger_IsTrigger(KK_P))
	{
		switch (g_cameraControlStatus)
		{
		case PlayerFollow:
			g_cameraControlStatus = Free;
			break;
		case Free:
			g_cameraControlStatus = PlayerFollow;
			break;
		default:
			break;
		}
	}
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
	if (KeyLogger_IsPressed(KK_Z))
	{
		g_Fov += XMConvertToRadians(-10)*time;
	}
	if (KeyLogger_IsPressed(KK_C))
	{
		g_Fov += XMConvertToRadians(10) * time;
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
	Shader_Billboard_SetViewMatrix(mtxView);

	//constexpr float fovAngleY = XMConvertToRadians(60.0f);
	float aspectRatio = (float)Direct3D_GetBackBufferWidth() / Direct3D_GetBackBufferHeight();
	float nearZ = 0.1f;
	float farZ = 200.0f;
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(g_Fov, aspectRatio, nearZ, farZ);

	DirectX::XMStoreFloat4x4(&g_CameraMatrix_Perspective, mtxPerspective);
	Shader3D_SetProjectionMatrix(mtxPerspective);
	Shader_Billboard_SetProjectionMatrix(mtxPerspective);
}

DirectX::XMFLOAT4X4& Camera_GetMatrix()
{
	switch (g_cameraControlStatus)
	{
	case PlayerFollow:
		return PlayerCamera_GetMatrix();
	case Free:
		return g_CameraMatrix;
	default:
		break;
	}
}

DirectX::XMFLOAT4X4& Camera_GetMatrixPerspective()
{
	switch (g_cameraControlStatus)
	{
	case PlayerFollow:
		return PlayerCamera_GetMatrixPerspective();
	case Free:
	return g_CameraMatrix_Perspective;
		
	default:
		break;
	}
}

DirectX::XMFLOAT3 Camera_GetFrontVector()
{
	switch (g_cameraControlStatus)
	{
	case PlayerFollow:
		return PlayerCamera_GetFrontVector();
	case Free:
		return g_CameraFront;
	default:
		break;
	}
}
DirectX::XMFLOAT3 Camera_GetCameraPos()
{
	switch (g_cameraControlStatus)
	{
	case PlayerFollow:
		PlayerCamera_GetCameraPos();
		break;
	case Free:
	return g_CameraPosition;
	default:
		break;
	}
}

float Camera_GetFov()
{
	return g_Fov;
}

void CameraDragUpdate(float time)
{
	Mouse_State mouse_state{};
	Mouse_GetState(&mouse_state);

	XMVECTOR front = XMLoadFloat3(&g_CameraFront);
	XMVECTOR right = XMLoadFloat3(&g_CameraRight);
	XMVECTOR up = XMLoadFloat3(&g_CameraUP);
	XMVECTOR pos = XMLoadFloat3(&g_CameraPosition);

	if (MouseLogger_IsTrigger(1) && KeyLogger_IsPressed(KK_CAPSLOCK))
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
	Shader_Billboard_SetViewMatrix(mtxView);
	
	float aspectRatio = (float)Direct3D_GetBackBufferWidth() / Direct3D_GetBackBufferHeight();
	float nearZ = 0.1f;
	float farZ = 200.0f;
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(g_Fov, aspectRatio, nearZ, farZ);

	DirectX::XMStoreFloat4x4(&g_CameraMatrix_Perspective, mtxPerspective);
	Shader3D_SetProjectionMatrix(mtxPerspective);
	Shader_Billboard_SetProjectionMatrix(mtxPerspective);
}
