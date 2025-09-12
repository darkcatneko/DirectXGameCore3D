#ifndef BATTLEGROUND_H
#define BATTLEGROUND_H
#include "DirectXMath.h"
#include "Collision.h"
#include "UIInteraction.h"
#include "vector"

void  BattleGround_Initialize();
void  BattleGround_Finitialize();
void  BattleGroundUpdate(float time);
void  ButtonDragUpdate();
void  BattleGroundDraw();
struct PolygonColliderBase
{
	DirectX::XMFLOAT2 A, B, C, D; // í∏ÍyÅCàÀè∆èáéûêjàΩãtéûêjèáèò
};

struct PolygonCollider
{
	int TexId;
	int tx, ty, tw, th;
	DirectX::XMFLOAT2 objectPOS;
	PolygonColliderBase ColliderPoints;
	bool is_Hover = false;
	bool is_Dragging = false;
};
struct OnCollide
{
	bool collide;
	DirectX::XMFLOAT2 pointA;
	DirectX::XMFLOAT2 pointB;
};
PolygonColliderBase Get_PolygonCollider_World(PolygonCollider poly);
float Cross2(const  DirectX::XMFLOAT2& u, const  DirectX::XMFLOAT2& v);
float SignedArea(const PolygonColliderBase& q);
PolygonColliderBase GetBattleGroundColliderData(int index);
bool PointInQuad(const PolygonColliderBase& Q, const  DirectX::XMFLOAT2& P, float eps = 1e-6f);
float DistancePointSegment(const DirectX::XMFLOAT2& P, const DirectX::XMFLOAT2& A, const DirectX::XMFLOAT2& B);
OnCollide CircleCompletelyOutsideQuad(const PolygonColliderBase& Q,
	const Circle& C,
	bool allow_tangent = false,
	float eps = 1e-6f);
UIBox GetBattleUICollider(int index);
Box GetBattleUICollider_Box(int index);
#endif // !BATTLEGROUND_H

