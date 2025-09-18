#ifndef CAMERA3D_H
#define CAMERA3D_H
#include "DirectXMath.h"


void Camera3D_Initialize();
void Camera3D_Finitialize();
void Camera3D_Update(double elapsed_time);
void CameraDragUpdate(float time);

void HAL_Camera_Movement_Update(float time);
DirectX::XMFLOAT4X4& Camera_GetMatrix();
DirectX::XMFLOAT3 Camera_GetFrontVector();
DirectX::XMFLOAT3 Camera_GetCameraPos();
#endif