#include "Collision3D.h"
#include <algorithm>
#include "DirectXMath.h"
#include "Camera3D.h"
#include "direct3d.h"
#include "Grid.h"
#include "Cube.h"
#include "PlayerCamera.h"
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

bool Collision_IsOverlapSphere(const Sphere& a, const DirectX::XMFLOAT3& point)
{
	XMVECTOR centerA = XMLoadFloat3(&a.center);
	XMVECTOR centerB = XMLoadFloat3(&point);
	XMVECTOR lsq = XMVector3LengthSq(centerB - centerA);

	return a.radius * a.radius > XMVectorGetX(lsq);
}
Ray MakeMouseRay(float mouseX, float mouseY)
{
	DirectX::XMFLOAT4X4 view = Camera_GetMatrix();
	DirectX::XMFLOAT4X4 proj = Camera_GetMatrixPerspective();

	DirectX::XMFLOAT3 nearP = Camera_GetCameraPos();
	DirectX::XMFLOAT3 farP = Direct3D_ScreenToWorld(mouseX, mouseY, 1.0f, view, proj);

	using namespace DirectX;
	XMVECTOR o = XMLoadFloat3(&nearP);
	XMVECTOR f = XMLoadFloat3(&farP);
	XMVECTOR d = XMVector3Normalize(f - o);

	Ray ray{};
	XMVector3Normalize(d);
	XMStoreFloat3(&ray.origin, o);
	XMStoreFloat3(&ray.dir, d);
	return ray;
}

bool RayVsAABB(const Ray& ray, const AABB& box, float& tHit)
{
	// slab method
	float tmin = 0.0f;
	float tmax = FLT_MAX;

	auto checkAxis = [&](float origin, float dir, float bmin, float bmax) -> bool
		{
			if (fabsf(dir) < 1e-6f)
			{
				// 平行：origin 必須在區間內
				return (origin >= bmin && origin <= bmax);
			}
			float invD = 1.0f / dir;
			float t0 = (bmin - origin) * invD;
			float t1 = (bmax - origin) * invD;
			if (t0 > t1) std::swap(t0, t1);
			tmin = (t0 > tmin) ? t0 : tmin;
			tmax = (t1 < tmax) ? t1 : tmax;
			return (tmax >= tmin);
		};

	if (!checkAxis(ray.origin.x, ray.dir.x, box.min.x, box.max.x)) return false;
	if (!checkAxis(ray.origin.y, ray.dir.y, box.min.y, box.max.y)) return false;
	if (!checkAxis(ray.origin.z, ray.dir.z, box.min.z, box.max.z)) return false;

	tHit = tmin;
	return true;
}



void Debug_DrawRay(const Ray& ray)
{
	XMFLOAT3 start = ray.origin;
	XMFLOAT3 end =
	{
		ray.origin.x + ray.dir.x * 100.0f,
		ray.origin.y + ray.dir.y * 100.0f,
		ray.origin.z + ray.dir.z * 100.0f,
	};
	Cube_Draw(end);
	Grid_DebugDrawRay(start, ray.dir, 10000.0f); // 紅色
}

