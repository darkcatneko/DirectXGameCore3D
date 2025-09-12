#ifndef NEKOTOOL_H
#define NEKOTOOL_H
#include <cmath>
#include <cstdint>
#include <iostream>
#include "DirectXMath.h"
template <typename T>
T clamp(T value, T min, T max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

float easeOutQuad(float t);
float RandNeg1To1(uint32_t& s);
float Distance(const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b);
float Length(const  DirectX::XMFLOAT2& a);
DirectX::XMFLOAT2 Normalize(const DirectX::XMFLOAT2& v);
float DirectionToDegree(const DirectX::XMFLOAT2& dir);
DirectX::XMFLOAT2 DirFromDeg(float deg);
float DegToRad(float deg);
#endif
