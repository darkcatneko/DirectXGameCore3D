#ifndef CAMERA_H
#define CAMERA_H
#include "DirectXMath.h"

extern float CameraPos_X;
extern float CameraPos_Y;
extern DirectX::XMFLOAT2 Camera_Vec;

void changeCameraPos(float camX, float camY);
void resetCam(float camX, float camY);
DirectX::XMFLOAT2 getClampedCameraPos();
void StartCameraShake(float seconds, float strength);
void CameraUpdate(float dt);
#endif // !CAMERA_H
