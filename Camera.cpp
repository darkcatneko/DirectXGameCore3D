#include "Camera.h"
#include "DirectXMath.h"
#include "NekoTool.h"
#include "direct3d.h"
#include <cmath>
#include <cstdint>
#include <iostream>
using namespace DirectX;

float CameraPos_X = 0.0f;
float CameraPos_Y = 0.0f;
XMFLOAT2 Camera_Vec = { 0.0f,0.0f };


void changeCameraPos(float camX, float camY)
{
	CameraPos_X = camX;
	CameraPos_Y = camY;
}
void resetCam(float camX, float camY)
{
	CameraPos_X = camX;
	CameraPos_Y = camY;
	XMStoreFloat2(&Camera_Vec, { camX ,camY });
}
DirectX::XMFLOAT2 getClampedCameraPos()
{
	XMFLOAT2 result = { Camera_Vec.x,Camera_Vec.y };
	float half_x = (float)Direct3D_GetBackBufferWidth() / 2;
	float half_y = (float)Direct3D_GetBackBufferHeight() / 2;
	result.x = clamp<float>(result.x, -3200, 0);
	result.y = clamp<float>(result.y, -3200, 0);
	return result;
}



struct CameraShake {
	bool   active = false;
	float  timeLeft = 0.0f;
	float  duration = 0.0f;
	float  intensity = 0.0f;   // 像素振幅
	float  frequency = 25.0f;  // 每秒換一次目標方向的次數(可調)

	// 兩個目標方向，用插值讓晃動更順
	float  curX = 0.0f, curY = 0.0f;
	float  nextX = 0.0f, nextY = 0.0f;

	// 取樣計時
	float  sampleInterval = 0.0f;
	float  sampleT = 0.0f;

	// RNG 狀態
	uint32_t rng = 0xA53F9E4Du;

	void Start(float seconds, float strength) {
		active = true;
		duration = std::max(0.0001f, seconds);
		timeLeft = seconds;
		intensity = strength;
		sampleInterval = 1.0f / std::max(1.0f, frequency);
		sampleT = sampleInterval; // 讓第一幀就抽一次方向
		curX = curY = 0.0f;
		PickNext();
	}

	void PickNext() {
		nextX = RandNeg1To1(rng);
		nextY = RandNeg1To1(rng);
		float len = std::sqrt(nextX * nextX + nextY * nextY);
		if (len > 1e-6f) { nextX /= len; nextY /= len; } // 單位向量
	}

	// 每幀呼叫：dt = deltaTime(秒)，baseX/baseY = 你原本想要的相機位置(例如跟隨玩家)
	void Update(float dt) {
		if (!active) { return; }

		timeLeft -= dt;
		if (timeLeft <= 0.0f) {
			active = false;
			Camera_Vec = { 0,0 };
			return;
		}

		// 方向插值（把上一個方向平滑過渡到下一個方向）
		sampleT += dt;
		if (sampleT >= sampleInterval) {
			sampleT -= sampleInterval;
			// 新輪開始：把 next 搬到 cur，再抽一個 next
			curX = nextX; curY = nextY;
			PickNext();
		}
		float a = sampleT / sampleInterval; // 0..1
		float dirX = curX + (nextX - curX) * a;
		float dirY = curY + (nextY - curY) * a;

		// 震幅衰減：二次衰減(更自然)，也可改成 exp 或線性
		float t = timeLeft / duration;      // 1→0
		float amp = intensity * (t * t);    // 二次衰減

		Camera_Vec.x = dirX * amp;
		Camera_Vec.y = dirY * amp;
	}
} gCameraShake;

// 對外介面 -------------------------------------------------------
void StartCameraShake(float seconds, float strength) {
	gCameraShake.Start(seconds, strength);
}

// 每幀更新：dt 秒、baseX/baseY 是你原本的相機目標位置（例如玩家位置+偏移）
void CameraUpdate(float dt) {
	gCameraShake.Update(dt);
}
