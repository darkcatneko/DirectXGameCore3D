#ifndef COLLISION3D_H
#define COLLISION3D_H
#include "DirectXMath.h"

struct AABB
{
	DirectX::XMFLOAT3 max;
	DirectX::XMFLOAT3 min;

	DirectX::XMFLOAT3 GetCenter() const
	{
		DirectX::XMFLOAT3 center;

		center.x = min.x + (max.x - min.x) * 0.5f;
		center.y = min.y + (max.y - min.y) * 0.5f;
		center.z = min.z + (max.z - min.z) * 0.5f;
		return center;
	}
};
struct Hit
{
	bool isHit;
	DirectX::XMFLOAT3 normal;
};
bool Collision_IsOverlapAABB(const AABB& a, const AABB& b);
Hit Collision_IsHitAABB(const AABB& a, const AABB& b);
#endif
