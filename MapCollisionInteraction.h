#ifndef MAPCOLLISIONINTERACTION_H
#define MAPCOLLISIONINTERACTION_H
#include <DirectXMath.h>
#include "Collision.h"
#include "d3d11.h"


struct GameobjectColliderCircle
{
	DirectX::XMFLOAT2 center;
	float radius;
};

struct  GameobjectColliderBox
{
	DirectX::XMFLOAT2 center;
	float half_width;
	float half_height;
};
bool OnGameObjectCircleCollisionEnter(const Circle& a, const GameobjectColliderCircle& b);
bool OnGameObjectCollisionEnter(const Box& a, const GameobjectColliderBox& b);
#endif
