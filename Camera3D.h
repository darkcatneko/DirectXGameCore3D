#ifndef CAMERA3D_H
#define CAMERA3D_H
#include "DirectXMath.h"


void Camera3D_Initialize(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& right);
void Camera3D_Finitialize();
void Camera3D_Update(double elapsed_time);
void CameraDragUpdate(float time);

void HAL_Camera_Movement_Update(float time);
DirectX::XMFLOAT4X4& Camera_GetMatrix();
DirectX::XMFLOAT4X4& Camera_GetMatrixPerspective();
DirectX::XMFLOAT3 Camera_GetFrontVector();
DirectX::XMFLOAT3 Camera_GetCameraPos();
float Camera_GetFov();
#endif