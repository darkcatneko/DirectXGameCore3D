#include "BossCleanerController.h"
#include "BattleBossController.h"
#include <vector>
#include <cmath>
#include <limits>
#include <random>
#include "BattleGround.h"
#include "Background.h"
#include "Collision.h"
#include <DirectXMath.h>
#include "Texture.h"
#include "sprite.h"
#include "BattleSystem.h"
#include "NekoTool.h"

using DirectX::XMFLOAT2;

static std::vector<XMFLOAT2> debug;
static bool debuger = false;
static std::vector<XMFLOAT2> CleanerEyePos;
static bool CleanerEye_Covered[]
{ false,false,false };
static int g_Cleaner_Eyes_Aim_TexID = -1;
static int g_Cleaner_Health = 6;
static int g_Cleaner_HitCount = 0;

#pragma region  MATH



// ===== 小工具 =====
inline XMFLOAT2 operator+(const XMFLOAT2& a, const XMFLOAT2& b) { return { a.x + b.x, a.y + b.y }; }
inline XMFLOAT2 operator-(const XMFLOAT2& a, const XMFLOAT2& b) { return { a.x - b.x, a.y - b.y }; }
inline XMFLOAT2 operator*(const XMFLOAT2& a, float s) { return { a.x * s, a.y * s }; }

inline float Dot(const XMFLOAT2& a, const XMFLOAT2& b) { return a.x * b.x + a.y * b.y; }
inline float CrossZ(const XMFLOAT2& a, const XMFLOAT2& b) { return a.x * b.y - a.y * b.x; } // 2D cross 的 z

// 反射：r = d - 2(d·n)n，n 需為單位向量
inline XMFLOAT2 Reflect(const XMFLOAT2& d, const XMFLOAT2& n) {
	float k = 2.0f * Dot(d, n);
	return XMFLOAT2{ d.x - k * n.x, d.y - k * n.y };
}
inline XMFLOAT2 ReflectRandom(const XMFLOAT2& d, const XMFLOAT2& n, float maxAngleDeg) {
	// 基本反射
	float k = 2.0f * Dot(d, n);
	XMFLOAT2 r{ d.x - k * n.x, d.y - k * n.y };

	// 隨機角度偏移 (單位: 弧度)
	static std::mt19937 rng{ std::random_device{}() };
	std::uniform_real_distribution<float> dist(-maxAngleDeg, maxAngleDeg);
	float theta = dist(rng) * 3.14159265f / 180.0f;

	// 旋轉 r 向量
	float cosT = cosf(theta), sinT = sinf(theta);
	XMFLOAT2 res{
		r.x * cosT - r.y * sinT,
		r.x * sinT + r.y * cosT
	};

	// 正規化保持單位長
	float len = sqrtf(res.x * res.x + res.y * res.y);
	if (len > 1e-6f) { res.x /= len; res.y /= len; }
	return res;
}
inline std::mt19937& RNG()
{
	static thread_local std::mt19937 rng{ std::random_device{}() };
	return rng;
}

// 1) 產生整數 [lo, hi]（含上下限）
inline int RandomInt(int lo, int hi)
{
	std::uniform_int_distribution<int> dist(lo, hi); // 含 hi
	return dist(RNG());
}
// 2) 產生浮點數 [lo, hi]（含上限的安全寫法）
inline float RandomFloat(float lo, float hi)
{
	// uniform_real_distribution 預設是 [lo, hi)
	// 用 nextafter 把 hi 推一點點，達成「幾乎等於含上限」
	std::uniform_real_distribution<float> dist(lo, std::nextafter(hi, std::numeric_limits<float>::infinity()));
	return dist(RNG());
}


// mode: 0/1/2/3 對應題述四個扇區
inline XMFLOAT2 RandomDirectionByMode(int mode)
{
	float degMin = 0.0f, degMax = 0.0f;
	switch (mode)
	{
	case 0: degMin = 135.0f; degMax = 225.0f; break; // 下方 ±45°
	case 1: degMin = 225.0f; degMax = 315.0f; break; // 左側 ±45°
	case 2: degMin = -45.0f; degMax = 45.0f; break; // 上方 ±45°
	case 3: degMin = 45.0f; degMax = 135.0f; break; // 右側 ±45°
	default: degMin = -45.0f; degMax = 45.0f; break; // 預設回上方扇區
	}

	float deg = RandomFloat(degMin, degMax);
	return DirFromDeg(deg); // 已是單位向量
}
// ===== 射線 vs 線段 相交 =====
// 參數：ray 起點 p、方向 d（需單位化）、線段 e0->e1
// 回傳是否命中；若命中給出 t（沿射線距離）、u（段內參數）、命中點 hit、該邊的單位法線 n（使用右法線 (dy, -dx)）
bool RaySegmentIntersect(const XMFLOAT2& p, const XMFLOAT2& d,
	const XMFLOAT2& e0, const XMFLOAT2& e1,
	float& outT, float& outU, XMFLOAT2& hit, XMFLOAT2& n)
{
	const float EPS = 1e-6f;
	XMFLOAT2 s = e1 - e0;                  // 段向量
	float denom = CrossZ(d, s);            // 平行時趨近 0
	if (std::fabs(denom) < EPS) return false;

	XMFLOAT2 r = e0 - p;
	float t = CrossZ(r, s) / denom;        // 射線參數
	float u = CrossZ(r, d) / denom;        // 線段參數 [0,1]
	if (t >= EPS && u >= -EPS && u <= 1.0f + EPS) {
		outT = t; outU = u;
		hit = p + d * t;

		// 邊的單位法線（取右法線 (dy, -dx)；對順時針四邊形是「內側」法線，但反射結果不受正負影響）
		XMFLOAT2 e = s;
		XMFLOAT2 nn = Normalize(XMFLOAT2{ e.y, -e.x });
		n = nn;
		return true;
	}
	return false;
}

// ===== 蒐集所有牆（螢幕邊）作為線段 =====
struct Segment { XMFLOAT2 a, b; };
inline void BuildScreenSegments(float w, float h, std::vector<Segment>& segs) {
	segs.clear();
	segs.push_back({ {0,0}, {w,0} });       // 上
	segs.push_back({ {w,0}, {w,h} });       // 右
	segs.push_back({ {w,h}, {0,h} });       // 下
	segs.push_back({ {0,h}, {0,0} });       // 左
}

// 把多邊形（四邊形、順時針）拆成 4 條邊
inline void QuadToSegments(const PolygonColliderBase& q, std::vector<Segment>& segs) {
	segs.push_back({ q.A, q.B });
	segs.push_back({ q.B, q.C });
	segs.push_back({ q.C, q.D });
	segs.push_back({ q.D, q.A });
}

// ===== 射線追蹤（最多 bounces 次）=====
std::vector<XMFLOAT2> TraceRay(
	const XMFLOAT2& start,
	const XMFLOAT2& dirNormalized,                  // 必須單位向量
	const std::vector<PolygonColliderBase>& colliders,
	int maxBounces,
	float screenW = 1600.0f,
	float screenH = 900.0f)
{
	const float EPS_PUSH = 0.01f; // 命中後微推避免重複命中同一邊
	XMFLOAT2 p = start;
	XMFLOAT2 d = dirNormalized;
	std::vector<XMFLOAT2> path;
	path.reserve(maxBounces + 2);
	path.push_back(p);

	// 準備牆段
	std::vector<Segment> worldSegs;
	BuildScreenSegments(screenW, screenH, worldSegs);

	// 主循環
	for (int bounce = 0; bounce < maxBounces; ++bounce)
	{
		float bestT = std::numeric_limits<float>::infinity();
		XMFLOAT2 bestHit{}, bestN{};
		bool hitSomething = false;

		// 1) 先測牆
		for (const auto& s : worldSegs) {
			float t, u; XMFLOAT2 hit, n;
			if (RaySegmentIntersect(p, d, s.a, s.b, t, u, hit, n)) {
				if (t < bestT) {
					bestT = t; bestHit = hit; bestN = n; hitSomething = true;
				}
			}
		}

		// 2) 再測所有四邊形
		std::vector<Segment> segs;
		for (const auto& poly : colliders) {
			QuadToSegments(poly, segs);
		}
		for (const auto& s : segs) {
			float t, u; XMFLOAT2 hit, n;
			if (RaySegmentIntersect(p, d, s.a, s.b, t, u, hit, n)) {
				if (t < bestT) {
					bestT = t; bestHit = hit; bestN = n; hitSomething = true;
				}
			}
		}

		if (!hitSomething) {
			// 沒命中任何東西：把射線延伸到螢幕外（或直接結束）
			// 這裡選擇結束
			break;
		}

		// 記錄命中點
		path.push_back(bestHit);

		// 計算反射方向
		XMFLOAT2 n = bestN;                       // 單位法線
		d = Normalize(Reflect(d, n));

		// 從命中點沿反射方向微推，當作下一段的起點
		p = bestHit + d * EPS_PUSH;
	}

	return path; // path[0] = 起點；後續每個元素是一次命中的折點
}

#pragma endregion
void CleanerBossInitialize()
{
	g_Cleaner_Eyes_Aim_TexID = Texture_Load(L"Ghost.png");
	g_Cleaner_Health = 6;
	g_Cleaner_HitCount = 0;
}
void GenerateCleanerEyePos() {
	if (GetBattleBoss() != BattleBossState::CLEANER)return;
	CleanerEyePos.clear();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distX(0.0f, 1200.0f);
	std::uniform_real_distribution<float> distY(0.0f, 500.0f);

	const int targetCount = 3;
	const float minDistance = 200.0f; // 最小間隔，依需求可調

	while (CleanerEyePos.size() < targetCount) {
		XMFLOAT2 candidate{ distX(gen), distY(gen) };

		bool valid = true;
		for (const auto& p : CleanerEyePos) {
			if (Distance(candidate, p) < minDistance) {
				valid = false;
				break;
			}
		}

		if (valid) {
			CleanerEyePos.push_back(candidate);
		}
	}
}
void CleanerBulletShooter(std::vector<XMFLOAT2>& target, XMFLOAT2 start, XMFLOAT2 dir)
{
	std::vector<PolygonColliderBase> polys;
	//XMFLOAT2 start{ 800.0f, 899.0f };
	//XMFLOAT2 dir = Normalize(XMFLOAT2{ 1.0f, -1.0f }); // 左下 45°
	int maxBounces = 20;
	for (int i = 0; i < MAX_BATTLEGROUND_OBJECT; i++)
	{
		polys.push_back(PolygonColliderBase
			{
			GetBattleGroundColliderData(i).A, GetBattleGroundColliderData(i).B, GetBattleGroundColliderData(i).C, GetBattleGroundColliderData(i).D // 順時針
			});
	}
	target = TraceRay(start, dir, polys, maxBounces, 1600.0f, 900.0f);
	debug = target;
	debuger = true;
}

void CleanerSweapShooter(std::vector<XMFLOAT2>& target, XMFLOAT2 start, XMFLOAT2 dir)
{
	std::vector<PolygonColliderBase> polys;
	//XMFLOAT2 start{ 800.0f, 899.0f };
	//XMFLOAT2 dir = Normalize(XMFLOAT2{ 1.0f, -1.0f }); // 左下 45°
	int maxBounces = 10;
	target = TraceRay(start, dir, polys, maxBounces, 1600.0f, 900.0f);
	debug = target;
	debuger = true;
}

void debugDraw()
{
	if (debuger == false)return;
	for (int i = 0; i < 20; i++)
	{
		Collision_DebugDraw_LINE(debug[i], debug[i + 1]);
	}
}

void DrawEYES()
{
	for (int i = 0; i < 3; i++)
	{
		if (CleanerEye_Covered[i] == false)
		{
			Sprite_Draw(g_Cleaner_Eyes_Aim_TexID, CleanerEyePos[i].x + 200, CleanerEyePos[i].y + 200, 150, 150);
		}
#if defined(DEBUG)||defined(_DEBUG)
		for (int j = 0; j < 4; j++)
		{
			Collision_DebugDraw_Box(GetEyesCollision(i, j));
		}
#endif
	}
}

Box GetEyesCollision(int index, int corner)
{
	float cx = 0;
	float cy = 0;
	switch (corner)
	{
	case 0:
		cx = CleanerEyePos[index].x + 200 + 30;
		cy = CleanerEyePos[index].y + 200 + 30;
		break;
	case 1:
		cx = CleanerEyePos[index].x + 200 + 120;
		cy = CleanerEyePos[index].y + 200 + 30;
		break;
	case 2:
		cx = CleanerEyePos[index].x + 200 + 120;
		cy = CleanerEyePos[index].y + 200 + 120;
		break;
	case 3:
		cx = CleanerEyePos[index].x + 200 + 30;
		cy = CleanerEyePos[index].y + 200 + 120;
		break;
	default:
		break;
	}

	return { {cx,cy},10,10 };
}
void EyeUpdate()
{
	if (GetBattleBoss() != BattleBossState::CLEANER)return;
	for (int i = 0; i < 3; i++)
	{
		bool result = false; bool allClear = true;
		for (int j = 0; j < 4; j++)
		{
			if (OnBoxCollisionEnter(GetEyesCollision(i, 0), GetBattleUICollider_Box(j)) && OnBoxCollisionEnter(GetEyesCollision(i, 1), GetBattleUICollider_Box(j)) && OnBoxCollisionEnter(GetEyesCollision(i, 2), GetBattleUICollider_Box(j)) && OnBoxCollisionEnter(GetEyesCollision(i, 3), GetBattleUICollider_Box(j)))
			{
				result = true;
			}
		}
		CleanerEye_Covered[i] = result;
	}
}

bool GetAllEyeCover()
{
	return CleanerEye_Covered[0] && CleanerEye_Covered[1] && CleanerEye_Covered[2];
}

void ResetBossHitCount()
{
	g_Cleaner_HitCount = 0;
}
void HitCleaner()
{
	if (g_Cleaner_HitCount < 9)
	{
		g_Cleaner_HitCount++;
	}
	else
	{
		g_Cleaner_HitCount = 0;
		g_Cleaner_Health--;
		if (g_Cleaner_Health >= 0)
		{
			Battle_State_ChangeState(BattleSystemState::BOSS_DESIDE);
		}
		else
		{
			Battle_State_ChangeState(BattleSystemState::DIALOGUE);
		}
	}
}
int GetCleanerHealth()
{
	return g_Cleaner_Health;
}

shootPoint GetRandomShootPoint()
{
	int segment = RandomInt(0, 3);
	float result_x = 0, result_y = 0;
	switch (segment)
	{
	case 0:
		result_x = RandomFloat(1.0f, 1599.0f);
		result_y = 1;
		break;
	case 1:
		result_x = 1599;
		result_y = RandomFloat(1.0f, 899.0f);
		break;
	case 2:
		result_x = RandomFloat(1.0f, 1599.0f);
		result_y = 899;
		break;
	case 3:
		result_x = 1;
		result_y = RandomFloat(1.0f, 899.0f);
		break;
	default:
		break;
	}
	return { {result_x ,result_y}, segment ,RandomDirectionByMode(segment) };
}