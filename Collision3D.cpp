#include "Collision3D.h"
#include <algorithm>
#include "DirectXMath.h"
using namespace DirectX;

bool Collision_IsOverlapAABB(const AABB& a,const AABB& b)
{
	return a.min.x < b.max.x
		&& a.max.x > b.min.x
		&& a.min.y < b.max.y
		&& a.max.y > b.min.y
		&& a.min.z < b.max.z
		&& a.max.z > b.min.z;
}

Hit Collision_IsHitAABB(const AABB& a, const AABB& b)
{
	Hit hit{};
	hit.isHit = Collision_IsOverlapAABB(a, b);
	if (!hit.isHit)
	{
		return hit;
	}
	float xdepth = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
	float ydepth = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
	float zdepth = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);
	bool isShallowX = false;
	bool isShallowY = false;
	bool isShallowZ = false;
	if (xdepth>ydepth)
	{
		if (ydepth > zdepth)
		{
			//z的面
			isShallowZ = true;
		}
		else
		{
			//y的面
			isShallowY = true;
		}
	}
	else
	{
		if (zdepth > xdepth)
		{
			//x的面
			isShallowX = true;
		}
		else
		{
			//z的面
			isShallowZ = true;
		}
	}
	XMFLOAT3 a_center = a.GetCenter();
	XMFLOAT3 b_center = b.GetCenter();
	XMVECTOR normal = XMLoadFloat3(&b_center)-XMLoadFloat3(&a_center);
	if (isShallowX)
	{
		normal = XMVector3Normalize(normal * XMVECTOR{ 1.0f,0.0f,0.0f });
	}
	else if (isShallowY)
	{
		normal = XMVector3Normalize(normal * XMVECTOR{ 0.0f,1.0f,0.0f });
	}
	else if (isShallowZ)
	{
		normal = XMVector3Normalize(normal * XMVECTOR{ 0.0f,0.0f,1.0f });
	}
	XMStoreFloat3(&hit.normal, normal);
	return hit;
}
