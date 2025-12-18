#ifndef COLLISION3D_H
#define COLLISION3D_H
#include "DirectXMath.h"

struct Sphere
{
	DirectX::XMFLOAT3 center; // 圆心坐标
	float radius; // 半径
};
struct Ray
{
	DirectX::XMFLOAT3 origin;
	DirectX::XMFLOAT3 dir; // normalized
};

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
	DirectX::XMFLOAT3 GetSize() const
	{
		return {
			max.x - min.x,
			max.y - min.y,
			max.z - min.z
		};
	}
};
struct Hit
{
	bool isHit;
	DirectX::XMFLOAT3 normal;
};
bool Collision_IsOverlapAABB(const AABB& a, const AABB& b);
Hit Collision_IsHitAABB(const AABB& a, const AABB& b);
bool Collision_IsOverlapSphere(const Sphere& a, const DirectX::XMFLOAT3& point);
Ray MakeMouseRay(float mouseX, float mouseY);
bool RayVsAABB(const Ray& ray, const AABB& box, float& tHit);
void Debug_DrawRay(const Ray& ray);
#endif
