#ifndef PLAYERCAMERA_H
#define PLAYERCAMERA_H
#include "DirectXMath.h"

void PlayerCamera_Initialize();
void PlayerCamera_Finitialize();
void PlayerCamera_Update(double elapsed_time);

DirectX::XMFLOAT4X4& PlayerCamera_GetMatrix();
DirectX::XMFLOAT4X4& PlayerCamera_GetMatrixPerspective();
const DirectX::XMFLOAT3 PlayerCamera_GetFrontVector();
const DirectX::XMFLOAT3 PlayerCamera_GetCameraPos();


#endif // !PLAYERCAMERA_H

