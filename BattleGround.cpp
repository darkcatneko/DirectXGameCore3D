#include "BattleGround.h"
#include "BattlePlayer.h"
#include "Background.h"
#include "DirectXMath.h"
#include "Texture.h"
#include "sprite.h"
#include "algorithm"
#include "Collision.h"
#include "mouse.h"
#include "MouseRenderer.h"
#include "UIInteraction.h"
#include "Key_Logger.h"
#include "BattleSystem.h"
#include "BossCleanerController.h"
#include "BattleBossController.h"
#include <cmath>
#include "BattleBossBoomber.h"

constexpr float screen_shrink = 1600.0f / 1920.0f;
static bool g_mouseCanGrab = true;
static int g_HoveredBattleButton_TexId = -1;
static int g_Player_Health_TexId = -1;

static PolygonCollider g_BattleGroundColliderBase[]
{
	{-1,162,682,330 * screen_shrink,205 * screen_shrink,{162,682},{{0,0},{276.1f * screen_shrink,45.5f * screen_shrink},{330 * screen_shrink,205 * screen_shrink},{57.6f * screen_shrink,155.5f * screen_shrink}}},
	{-1,489,597,316 * screen_shrink,178 * screen_shrink,{494,602},{{55.6f * screen_shrink,24.3f * screen_shrink},{247.7f * screen_shrink,0 * screen_shrink},{301 * screen_shrink,157 * screen_shrink},{0 * screen_shrink,134.0f * screen_shrink}}},
	{-1,903,591,327 * screen_shrink,154 * screen_shrink,{903,591},{{0 * screen_shrink,0 * screen_shrink},{281 * screen_shrink,19.4f * screen_shrink},{330.0f * screen_shrink,150.0f * screen_shrink},{47.4f * screen_shrink,121.3f * screen_shrink}}},
	{-1,1195,723,315 * screen_shrink,182 * screen_shrink,{1195,723},{{28.7f * screen_shrink,58.4f * screen_shrink},{305 * screen_shrink,0 * screen_shrink},{286.7f * screen_shrink,162 * screen_shrink},{0 * screen_shrink,162 * screen_shrink}}},
};
static PolygonCollider g_BattleGroundCollider[]
{
	{-1,162,682,330 * screen_shrink,205 * screen_shrink,{162,682},{{0,0},{276.1f * screen_shrink,45.5f * screen_shrink},{330 * screen_shrink,205 * screen_shrink},{57.6f * screen_shrink,155.5f * screen_shrink}}},
	{-1,489,597,316 * screen_shrink,178 * screen_shrink,{494,602},{{55.6f * screen_shrink,24.3f * screen_shrink},{247.7f * screen_shrink,0 * screen_shrink},{301 * screen_shrink,157 * screen_shrink},{0 * screen_shrink,134.0f * screen_shrink}}},
	{-1,903,591,327 * screen_shrink,154 * screen_shrink,{903,591},{{0 * screen_shrink,0 * screen_shrink},{281 * screen_shrink,19.4f * screen_shrink},{330.0f * screen_shrink,150.0f * screen_shrink},{47.4f * screen_shrink,121.3f * screen_shrink}}},
	{-1,1195,723,315 * screen_shrink,182 * screen_shrink,{1195,723},{{28.7f * screen_shrink,58.4f * screen_shrink},{305 * screen_shrink,0 * screen_shrink},{286.7f * screen_shrink,162 * screen_shrink},{0 * screen_shrink,162 * screen_shrink}}},
};


void BattleGround_Initialize()
{
	g_BattleGroundColliderBase[0].TexId = Texture_Load(L"FightButton.png");
	g_BattleGroundColliderBase[1].TexId = Texture_Load(L"ITEM1.png");
	g_BattleGroundColliderBase[2].TexId = Texture_Load(L"TALK1.png");
	g_BattleGroundColliderBase[3].TexId = Texture_Load(L"RUN1.png");
	g_HoveredBattleButton_TexId = Texture_Load(L"FIGHTHover.png");
	g_Player_Health_TexId = Texture_Load(L"PlayerHealth.png");
	for (int i = 0; i < 4; i++)
	{
		g_BattleGroundCollider[i] = g_BattleGroundColliderBase[i];
	}
}
void BattleGround_Finitialize()
{
}
void BattleGroundUpdate(float time)
{
	if (g_BattleGroundCollider[0].is_Hover)
	{
		if (MouseLogger_IsTrigger(0))
		{
			switch (GetBattleState())
			{
			case BattleSystemState::PLAYER_MOVE:
				Battle_State_ChangeState(BattleSystemState::PLAYER_FIGHT);
				break;
			case BattleSystemState::PLAYER_FIGHT:
				Battle_State_ChangeState(BattleSystemState::PLAYER_SHOOT);
				switch (GetBattleBoss())
				{
				case BattleBossState::CLEANER:
					if (GetAllEyeCover() == false)
					{
						SetBossAnger(true);
					}
					break;
				case BattleBossState::BOOMBER:
					break;
				}

				break;
			default:
				break;
			}

		}
	}

}
void ButtonDragUpdate()
{
	for (int i = 0; i < 4; i++)
	{
		if (OnUIBoxCollisionEnter(MouseRenderer_GetBoxCollision(), GetBattleUICollider(i)))
		{
			g_BattleGroundCollider[i].is_Hover = true;
		}
		else
		{
			g_BattleGroundCollider[i].is_Hover = false;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		if (g_BattleGroundCollider[i].is_Hover == true)
		{
			if (MouseLogger_IsTrigger(1))
			{
				if (g_mouseCanGrab)
				{
					g_BattleGroundCollider[i].is_Dragging = true;
					g_mouseCanGrab = false;
				}
			}
			if (MouseLogger_IsRelease(1))
			{
				g_BattleGroundCollider[i].is_Dragging = false;
				g_mouseCanGrab = true;
				if (GetBattleState() == BattleSystemState::PLAYER_FIGHT && GetBattleBoss() == BattleBossState::BOOMBER)
				{
					for (int j = 0; j < BOMBLINERCOUNT; j++)
					{
						if (OnBoxCollisionEnter(GetBattleUICollider_Box(i), GetBombLinerMidPointCollision(j)))
						{
							BombLinerDestroyed(j);
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < 4; i++)
	{
		if (g_BattleGroundCollider[i].is_Dragging == true)
		{
			g_BattleGroundCollider[i].tx = Get_Mouse_Info().x - g_BattleGroundCollider[i].tw / 2;
			g_BattleGroundCollider[i].ty = Get_Mouse_Info().y - g_BattleGroundCollider[i].th / 2;
			g_BattleGroundCollider[i].objectPOS.x = Get_Mouse_Info().x - g_BattleGroundCollider[i].tw / 2;
			g_BattleGroundCollider[i].objectPOS.y = Get_Mouse_Info().y - g_BattleGroundCollider[i].th / 2;
		}
	}
}
void BattleGroundDraw()
{
	Sprite_Draw_Y_Percentage(g_Player_Health_TexId, 655, 804, 257, 55, GetPlayerHealth() / 3.0f);
	for (int i = 0; i < MAX_BATTLEGROUND_OBJECT; i++)
	{
		Sprite_Draw(g_BattleGroundCollider[i].TexId, g_BattleGroundCollider[i].tx, g_BattleGroundCollider[i].ty, g_BattleGroundCollider[i].tw, g_BattleGroundCollider[i].th);
#if defined(DEBUG)||defined(_DEBUG)
		UICollision_DebugDraw(GetBattleUICollider(i));
#endif
	}
}
PolygonColliderBase Get_PolygonCollider_World(PolygonCollider poly)
{
	return
	{
		{poly.ColliderPoints.A.x + poly.objectPOS.x,poly.ColliderPoints.A.y + poly.objectPOS.y},
		{poly.ColliderPoints.B.x + poly.objectPOS.x,poly.ColliderPoints.B.y + poly.objectPOS.y},
		{poly.ColliderPoints.C.x + poly.objectPOS.x,poly.ColliderPoints.C.y + poly.objectPOS.y},
		{poly.ColliderPoints.D.x + poly.objectPOS.x,poly.ColliderPoints.D.y + poly.objectPOS.y}
	};
}

float Cross2(const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v)
{
	return u.x * v.y - u.y * v.x;
}

float SignedArea(const PolygonColliderBase& q)
{
	// 多邊形有向面積：>0 表 CCW，<0 表 CW
	return 0.5f * (
		q.A.x * q.B.y - q.B.x * q.A.y +
		q.B.x * q.C.y - q.C.x * q.B.y +
		q.C.x * q.D.y - q.D.x * q.C.y +
		q.D.x * q.A.y - q.A.x * q.D.y
		);
}
PolygonColliderBase GetBattleGroundColliderData(int index)
{
	return Get_PolygonCollider_World(g_BattleGroundCollider[index]);
}
bool PointInQuad(const PolygonColliderBase& Q, const DirectX::XMFLOAT2& P, float eps)
{
	// 依四邊形的繞行方向決定「內側」的符號
	const float sgn = (SignedArea(Q) >= 0.0f) ? 1.0f : -1.0f;

	auto halfspace = [&](const DirectX::XMFLOAT2& E0, const DirectX::XMFLOAT2& E1)
		{
			DirectX::XMFLOAT2 e{ E1.x - E0.x, E1.y - E0.y };     // 邊向量
			DirectX::XMFLOAT2 w{ P.x - E0.x, P.y - E0.y };     // 點相對邊起點
			// 對 CCW 多邊形：叉積 >= 0 表示在邊的左側（內側）
			// 對 CW 多邊形：乘上 sgn 之後同樣以「>= 0」為內側
			return sgn * Cross2(e, w) >= -eps;
		};

	return  halfspace(Q.A, Q.B) &&   // 對應「在 AB 之上」的內側
		halfspace(Q.B, Q.C) &&   // 對應「在 BC 之右」
		halfspace(Q.C, Q.D) &&   // 對應「在 CD 之下」
		halfspace(Q.D, Q.A);     // 對應「在 DA 之左」
}
float DistancePointSegment(const XMFLOAT2& P, const XMFLOAT2& A, const XMFLOAT2& B)
{
	XMFLOAT2 AB{ B.x - A.x, B.y - A.y };
	XMFLOAT2 AP{ P.x - A.x, P.y - A.y };
	float ab2 = AB.x * AB.x + AB.y * AB.y;
	if (ab2 <= 1e-12f) {
		float dx = P.x - A.x, dy = P.y - A.y;
		return std::sqrt(dx * dx + dy * dy);
	}
	float t = (AP.x * AB.x + AP.y * AB.y) / ab2;
	t = std::max(0.0f, std::min(1.0f, t));
	XMFLOAT2 Q{ A.x + AB.x * t, A.y + AB.y * t }; // 線段上最近點
	float dx = P.x - Q.x, dy = P.y - Q.y;
	return std::sqrt(dx * dx + dy * dy);
}

OnCollide CircleCompletelyOutsideQuad(const PolygonColliderBase& Q, const Circle& C, bool allow_tangent, float eps)
{
	OnCollide result = { false,{0,0},{0,0} };
	// 1) 圓心必須在四邊形外（含邊界都不行，避免圓跨進去）
	if (PointInQuad(Q, C.center, eps)) return result;

	// 2) 圓心到四邊形邊界的最小距離
	float dmin = std::numeric_limits<float>::infinity();
	XMFLOAT2 pointA, pointB;
	if (DistancePointSegment(C.center, Q.A, Q.B) < dmin)
	{
		pointA = Q.A, pointB = Q.B;
	}
	dmin = std::min(dmin, DistancePointSegment(C.center, Q.A, Q.B));
	if (DistancePointSegment(C.center, Q.B, Q.C) < dmin)
	{
		pointA = Q.B, pointB = Q.C;
	}
	dmin = std::min(dmin, DistancePointSegment(C.center, Q.B, Q.C));
	if (DistancePointSegment(C.center, Q.C, Q.D) < dmin)
	{
		pointA = Q.C, pointB = Q.D;
	}
	dmin = std::min(dmin, DistancePointSegment(C.center, Q.C, Q.D));
	if (DistancePointSegment(C.center, Q.D, Q.A) < dmin)
	{
		pointA = Q.D, pointB = Q.A;
	}
	dmin = std::min(dmin, DistancePointSegment(C.center, Q.D, Q.A));

	// 3) 距離與半徑比較
	if (allow_tangent) {
		// 允許相切：d >= r 視為完全外部（含貼邊）
		result.collide = dmin >= C.radius - eps;
		return result;
	}
	else {
		// 嚴格完全外部：d > r（不接觸）
		result.collide = dmin > C.radius + eps;
		if (dmin <= C.radius + eps)
		{
			result.pointA = pointA;
			result.pointB = pointB;
			return result;
		}
		return result;
	}
}

UIBox GetBattleUICollider(int index)
{
	float cx = g_BattleGroundCollider[index].tw / 2 + g_BattleGroundCollider[index].tx;
	float cy = g_BattleGroundCollider[index].th / 2 + g_BattleGroundCollider[index].ty;



	return { {cx,cy},g_BattleGroundCollider[index].tw / 2.0f,  g_BattleGroundCollider[index].th / 2.0f };
}
Box GetBattleUICollider_Box(int index)
{
	float cx = g_BattleGroundCollider[index].tw / 2 + g_BattleGroundCollider[index].tx;
	float cy = g_BattleGroundCollider[index].th / 2 + g_BattleGroundCollider[index].ty;



	return { {cx,cy},g_BattleGroundCollider[index].tw / 2.0f,  g_BattleGroundCollider[index].th / 2.0f };
}


