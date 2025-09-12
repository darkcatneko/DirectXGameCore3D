#ifndef POLYGONCOLLIDER_H
#define POLYGONCOLLIDER_H
#include "iostream"
#include "vector"
#include "DirectXMath.h"
struct PolygonCollider
{
    std::vector<DirectX::XMFLOAT2> vertices; // ’¸êyCˆËÆ‡jˆ½‹tj‡˜
};

struct Projection {
    float min;
    float max;
};
Projection ProjectPolygon(const PolygonCollider& poly, const DirectX::XMFLOAT2& axis);
bool Overlap(const Projection& a, const Projection& b);
bool PolygonCollision(const PolygonCollider& A, const PolygonCollider& B);
#endif