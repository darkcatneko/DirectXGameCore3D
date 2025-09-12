#include "NekoTool.h"
#include "DirectXMath.h"
#include <cmath>
#include <cstdint>
#include <iostream>
using namespace DirectX;
float easeOutQuad(float t) {
	return -1.0f * t * (t - 2);
}
float RandNeg1To1(uint32_t& s) {
	s ^= s << 13; s ^= s >> 17; s ^= s << 5;
	// 轉成 [0,1) 再映射到 [-1,1]
	return float(double(s) * 2.3283064365386963e-10) * 2.0f - 1.0f;
}
float Distance(const XMFLOAT2& a, const XMFLOAT2& b)
{
	float dx = b.x - a.x;
	float dy = b.y - a.y;
	return sqrtf(dx * dx + dy * dy);
}
float Length(const XMFLOAT2& a) { return std::sqrt(a.x * a.x + a.y * a.y); }
XMFLOAT2 Normalize(const XMFLOAT2& v) {
	float len = Length(v);
	return (len > 1e-6f) ? XMFLOAT2{ v.x / len, v.y / len } : XMFLOAT2{ 0,0 };
}
float DirectionToDegree(const XMFLOAT2& dir)
{
	float rad = atan2f(dir.y, dir.x);
	float deg = 90.0f - rad * (180.0f / XM_PI);

	if (deg < 0) deg += 360.0f;
	if (deg >= 360.0f) deg -= 360.0f;

	return deg;
}
// 角度(度) -> 方向(單位向量)，0° = (0,-1)朝上，順時針為正
XMFLOAT2 DirFromDeg(float deg)
{
	float rad = deg * XM_PI / 180.0f;
	return XMFLOAT2{ sinf(rad), -cosf(rad) };
}
float DegToRad(float deg)
{
	return deg * (XM_PI / 180.0f);
}

