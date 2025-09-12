#include "PolygonCollider.h"
#include "iostream"
#include "vector"
#include "DirectXMath.h"
using namespace DirectX;

Projection ProjectPolygon(const PolygonCollider& poly, const DirectX::XMFLOAT2& axis)
{
    float min = XMVectorGetX(XMVector2Dot(XMLoadFloat2(&poly.vertices[0]), XMLoadFloat2(&axis)));
    float max = min;
    for (int i = 1; i < poly.vertices.size(); i++) {
        float p = XMVectorGetX(XMVector2Dot(XMLoadFloat2(&poly.vertices[i]), XMLoadFloat2(&axis)));
        min = std::min(min, p);
        max = std::max(max, p);
    }
    return { min, max };
}

bool Overlap(const Projection& a, const Projection& b)
{
    return !(a.max < b.min || b.max < a.min);
}

bool PolygonCollision(const PolygonCollider& A, const PolygonCollider& B)
{
    // A的邊
    for (int i = 0; i < A.vertices.size(); i++) {
        XMFLOAT2 p1 = A.vertices[i];
        XMFLOAT2 p2 = A.vertices[(i + 1) % A.vertices.size()];
        XMFLOAT2 edge = { p2.x - p1.x, p2.y - p1.y };
        XMFLOAT2 axis = { -edge.y, edge.x }; // 法線

        auto projA = ProjectPolygon(A, axis);
        auto projB = ProjectPolygon(B, axis);

        if (!Overlap(projA, projB)) return false; // 存在分離軸 → 無碰撞
    }

    // B的邊
    for (int i = 0; i < B.vertices.size(); i++) {
        XMFLOAT2 p1 = B.vertices[i];
        XMFLOAT2 p2 = B.vertices[(i + 1) % B.vertices.size()];
        XMFLOAT2 edge = { p2.x - p1.x, p2.y - p1.y };
        XMFLOAT2 axis = { -edge.y, edge.x };

        auto projA = ProjectPolygon(A, axis);
        auto projB = ProjectPolygon(B, axis);

        if (!Overlap(projA, projB)) return false;
    }

    return true; // 沒有分離軸 → 相交
}
