#include "BattleBossBoomber.h"
#include "BossCleanerController.h"
#include "DirectXMath.h"
#include "BattleSystem.h"
#include "Effect.h"
#include <vector>
#include <cmath>
#include <limits>
#include <random>
#include "Collision.h"
#include "NekoTool.h"
#include "BattlePlayer.h"
#include "sprite.h"
#include "Texture.h"
#include "BattleGround.h"
#include "Fade.h"
#include "Scene.h"
#include "BattlePlayerUpdater.h"

using namespace DirectX;


std::vector<XMFLOAT2> target;
static constexpr int MINE_COUNT = 30;
static constexpr int BOOMCOLLIDERCOUNT = 500;
static constexpr int MISSILE_COUNT = 5000;
static constexpr int ROCKET_COUNT = 5000;
int BOMBLINERCOUNT = 4;
static int g_Boomber_Health = 8;
static int g_Boomber_HitCount = 0;
static int g_Realease_Mine = 0;
//MineAttack
struct Mine
{
	bool active;
	bool bombActive;
	float delayCountDown;
	float CountDown;
	XMFLOAT2 MinePos;

	void Start(float delay, float count, XMFLOAT2 pos)
	{
		active = true;
		bombActive = false;
		delayCountDown = delay;
		CountDown = count;
		MinePos = pos;
		//Effect
	}
	void Update(float time)
	{
		if (active)
		{
			if (delayCountDown >= 0)
			{
				delayCountDown -= time;
			}
			else
			{
				if (bombActive == false)
				{
					bombActive = true;
					Effect_Creater(MinePos, 2);
				}
				if (CountDown >= 0)
				{
					CountDown -= time;
				}
				else
				{
					GenerateBoomCollider(0.35, MinePos);
					active = false;
				}
			}
		}
		else
		{
			return;
		}
	}
};
struct BoomCollider
{
	bool active;
	float lifetime;
	XMFLOAT2 ColliderPos;
	XMFLOAT2 ColliderHalf_length = { 125,125 };
	void Start(float life, XMFLOAT2 Pos)
	{
		active = true;
		lifetime = life;
		ColliderPos = Pos;
		ColliderHalf_length = { 125,125 };
		Effect_Creater(Pos, 3);
		//Effect
	}
	void StartBig(float life, XMFLOAT2 Pos, XMFLOAT2 Half)
	{
		active = true;
		lifetime = life;
		ColliderPos = Pos;
		ColliderHalf_length = Half;
		Effect_Creater(Pos, 4);
		//Effect
	}
	void StartSmall(float life, XMFLOAT2 Pos, XMFLOAT2 Half)
	{
		active = true;
		lifetime = life;
		ColliderPos = Pos;
		ColliderHalf_length = Half;
		Effect_Creater(Pos, 6);
		//Effect
	}
	void StartSmall_2(float life, XMFLOAT2 Pos, XMFLOAT2 Half, bool through)
	{
		active = true;
		lifetime = life;
		ColliderPos = Pos;
		ColliderHalf_length = Half;
		if (through)
		{
			Effect_Creater(Pos, 8);
		}
		else
		{
			Effect_Creater(Pos, 7);
		}
		//Effect
	}
	void Update(float time)
	{
		if (active)
		{
			if (lifetime >= 0)
			{
				lifetime -= time;
			}
			else
			{
				active = false;
				g_Realease_Mine--;
				if (g_Realease_Mine == 0)
				{
					Battle_State_ChangeState(BattleSystemState::PLAYER_MOVE);
				}
			}
		}
		else
		{
			return;
		}
	}
};
static Mine Mines[MINE_COUNT];
static BoomCollider BoomColliders[BOOMCOLLIDERCOUNT];
//bombLiner
static int g_BombLinerGen = 0;
struct BombLiner
{
	bool active;
	float CountDown;
	float TotalTime;
	XMFLOAT2 StartPos;
	XMFLOAT2 EndPos;
	XMFLOAT2 NowPos;
	void Start(XMFLOAT2 startPos, XMFLOAT2 endPos, float countDown)
	{
		StartPos = startPos;
		EndPos = endPos;
		TotalTime = countDown;
		CountDown = countDown;
		NowPos = startPos;
		active = true;
	}
	void Update(float time)
	{
		if (active)
		{
			CountDown -= time;
			float t = 1.0f - (CountDown / TotalTime);
			t = clamp(t, 0.0f, 1.0f);

			XMVECTOR s = XMLoadFloat2(&StartPos);
			XMVECTOR e = XMLoadFloat2(&EndPos);
			XMVECTOR p = XMVectorLerp(s, e, t);
			XMStoreFloat2(&NowPos, p);
			if (CountDown < 0)
			{
				GenerateBoomColliderBig(0.35f, EndPos, { 250,250 });
				GenerateBoomColliderBig(0.35f, StartPos, { 250,250 });
				g_BombLinerGen--;
				active = false;
				if (g_BombLinerGen == 0)
				{
					Battle_State_ChangeState(BattleSystemState::PLAYER_SHOOT);
				}
			}

		}
	}
};
static BombLiner BombLiners[20];
static int g_BombLinerStart_TexId;
static int g_BombLinerEnd_TexId;
static int g_BombLinerMid_TexId;
//missile
#pragma region  Missile
static int g_PressedCount = 0;
static bool g_PressedRight = true;
static int MissileTexId = -1;
static int BombLeft = 100;
std::vector<XMFLOAT2> DebugMissileLine(XMFLOAT2 StartPos, XMFLOAT2 MissileVector);
inline float    Len(XMFLOAT2 v);
struct MissilePackageAttack
{
	bool active = false;
	float nowCountDown = 0;
	int nowReleaseCount = 0;
	float SecPerOnce = 0.1;
	int nowRealeaseMissile = 0;
	void Start()
	{
		active = true;
		nowCountDown = 0;
		nowReleaseCount = 0;
		nowRealeaseMissile = 0;
	}
	void Update(float time)
	{
		if (active)
		{
			nowCountDown += time;
			if (nowCountDown >= SecPerOnce)
			{
				nowCountDown = 0;
				StartMissile();
				nowReleaseCount++;
				if (nowReleaseCount == 50)
				{
					active = false;
				}
			}
		}
	}
	void MissileBack()
	{
		g_missilePack.nowRealeaseMissile--;
		if (nowRealeaseMissile == 0)
		{
			Battle_State_ChangeState(BattleSystemState::PLAYER_MOVE);
		}
	}
}g_missilePack;
struct  Missile
{
	bool active = false;
	XMFLOAT2 BasicAim;
	std::vector<XMFLOAT2> BasicLine;
	XMFLOAT2 StartPos;
	XMFLOAT2 NowPos;
	float NowTime;
	float TotalTime;
	XMFLOAT2 NowDirection;
	float NowSpeed;
	float PlusSpeed;
	float PathS = 0.0f; // ★ 新增：沿路徑已行走的弧長（像素）
	float turnRateDegPerSec = 180.0f;
	float NowAim;
	// ---- 小工具 ----
	float PI = 3.14159265f;
	inline float WrapPI(float a) {            // (-π, π]
		while (a > PI) a -= 2 * PI;
		while (a <= -PI) a += 2 * PI;
		return a;
	}
	// 螢幕座標：Y朝下；以「正上」為0度、逆時針為正
	inline float DirToAim_UpZeroCCW(const XMFLOAT2& d) {
		return WrapPI(atan2f(d.y, d.x) + PI * 0.5f);
	}
	inline XMFLOAT2 AimToDir_UpZeroCCW(float aim) {
		return XMFLOAT2{ sinf(aim), -cosf(aim) }; // 等價於 θx=aim-π/2
	}
	inline float Dot(const XMFLOAT2& a, const XMFLOAT2& b) { return a.x * b.x + a.y * b.y; }
	inline float Len(const XMFLOAT2& v) { return sqrtf(v.x * v.x + v.y * v.y); }
	inline XMFLOAT2 Sub(const XMFLOAT2& a, const XMFLOAT2& b) { return { a.x - b.x, a.y - b.y }; }
	inline XMFLOAT2 Add(const XMFLOAT2& a, const XMFLOAT2& b) { return { a.x + b.x, a.y + b.y }; }
	inline XMFLOAT2 Mul(const XMFLOAT2& a, float s) { return { a.x * s, a.y * s }; }
	inline XMFLOAT2 Normalize(const XMFLOAT2& v) {
		float l = Len(v); return (l > 1e-6f) ? XMFLOAT2{ v.x / l, v.y / l } : XMFLOAT2{ 0,0 };
	}
	template<typename T> inline T Clamp(T x, T lo, T hi) { return (x < lo) ? lo : ((x > hi) ? hi : x); }
	inline float CrossZ(const XMFLOAT2& a, const XMFLOAT2& b) { return a.x * b.y - a.y * b.x; }
	inline XMFLOAT2 Rot(const XMFLOAT2& v, float ang) {
		float c = cosf(ang), s = sinf(ang);
		return { v.x * c - v.y * s, v.x * s + v.y * c };
	}
	// 把折線每段長度做 prefix sum，回傳總長；cum[0]=0, cum[i]=0..i-1段累計
	float BuildCumulative(const std::vector<XMFLOAT2>& pts, std::vector<float>& cum) {
		cum.clear();
		const int n = (int)pts.size();
		if (n == 0) { cum.push_back(0.f); return 0.f; }
		cum.resize(n, 0.f);
		for (int i = 1; i < n; ++i) {
			cum[i] = cum[i - 1] + Len(Sub(pts[i], pts[i - 1]));
		}
		return cum.back(); // 總長
	}

	// 在折線上以弧長 s 取樣位置與方向（切線）
	void SampleAtS(const std::vector<XMFLOAT2>& pts,
		const std::vector<float>& cum,
		float s,
		XMFLOAT2& outPos,
		XMFLOAT2& outDir)
	{
		const int n = (int)pts.size();
		if (n == 1) { outPos = pts[0]; outDir = { 0,0 }; return; }

		// clamp s 到 [0, totalLen]
		float total = cum.back();
		s = Clamp(s, 0.0f, total);

		// 找到 s 落在哪一段 [i-1, i]
		int i = 1;
		while (i < n && cum[i] < s) ++i;
		i = Clamp(i, 1, n - 1);

		float segLen = cum[i] - cum[i - 1];
		float t = (segLen > 1e-6f) ? ((s - cum[i - 1]) / segLen) : 0.0f;

		XMFLOAT2 P0 = pts[i - 1];
		XMFLOAT2 P1 = pts[i];
		outPos = Add(P0, Mul(Sub(P1, P0), t));
		outDir = Normalize(Sub(P1, P0)); // 當前段方向
	}

	// 每幀最大轉角限制：把 currDir 朝 targetDir 旋一小步（最多 maxDeltaRad）
	XMFLOAT2 RotateTowards(const XMFLOAT2& currDir,
		const XMFLOAT2& targetDir,
		float maxDeltaRad)
	{
		XMFLOAT2 a = Normalize(currDir);
		XMFLOAT2 b = Normalize(targetDir);
		if (Len(a) < 1e-6f) return b;
		if (Len(b) < 1e-6f) return a;

		float d = Clamp(Dot(a, b), -1.0f, 1.0f);
		float ang = atan2f(CrossZ(a, b), d); // (-pi, pi]
		float delta = Clamp(ang, -maxDeltaRad, maxDeltaRad);
		return Normalize(Rot(a, delta));
	}

	// （可選）把當前 NowPos 投影回新路徑，估計新的弧長 s（避免路徑重算時脫線）
	float ReprojectS(const std::vector<XMFLOAT2>& pts,
		const std::vector<float>& cum,
		const XMFLOAT2& pos)
	{
		const int n = (int)pts.size();
		if (n <= 1) return 0.0f;

		float bestS = 0.0f;
		float bestD2 = FLT_MAX;

		for (int i = 1; i < n; ++i) {
			XMFLOAT2 A = pts[i - 1], B = pts[i];
			XMFLOAT2 AB = Sub(B, A);
			XMFLOAT2 AP = Sub(pos, A);
			float ab2 = Dot(AB, AB);
			float t = (ab2 > 1e-6f) ? Clamp(Dot(AP, AB) / ab2, 0.0f, 1.0f) : 0.0f;
			XMFLOAT2 Q = Add(A, Mul(AB, t));
			XMFLOAT2 diff = Sub(pos, Q);
			float d2 = Dot(diff, diff);
			if (d2 < bestD2) {
				bestD2 = d2;
				bestS = cum[i - 1] + t * (cum[i] - cum[i - 1]);
			}
		}
		return bestS;
	}
	Circle GetMissileCircleCollider()
	{
		float cx = NowPos.x;
		float cy = NowPos.y;
		return { {cx,cy},25 };
	}
	void Start(XMFLOAT2 basicAim, XMFLOAT2 startPos, float totalTime, float plusSpeed)
	{
		active = true;
		BasicAim = basicAim;
		StartPos = startPos;
		BasicLine.clear();
		BasicLine = DebugMissileLine(StartPos, BasicAim);
		NowPos = BasicLine[0];
		NowTime = 0;
		TotalTime = totalTime;
		NowDirection = Normalize(XMFLOAT2{ (BasicLine[1].x - BasicLine[0].x),(BasicLine[1].y - BasicLine[0].y) });
		NowSpeed = 0;
		PlusSpeed = plusSpeed;
		PathS = 0.0f;
		NowAim = DirToAim_UpZeroCCW(NowDirection);
		XMFLOAT2 playerpos = GetPlayerPos();
		Effect_Creater(playerpos, 5);
	}
	void Update(float elapsedTime)
	{
		if (!active) return;

		// 1) 可能每秒更新的路徑
		//BasicLine.clear();
		//BasicLine = DebugMissileLine(StartPos, BasicAim);

		if (BasicLine.empty()) {
			active = false; return;
		}

		// 2) 弧長表
		static std::vector<float> cumLen;
		float totalLen = BuildCumulative(BasicLine, cumLen);

		// 3) （可選）路徑更新後，把 PathS 重新投影到新路徑，避免漂移
		//    若你不希望這樣，可註解掉。
		PathS = ReprojectS(BasicLine, cumLen, NowPos);

		// 4) 速度/時間（都乘 Δt）
		NowSpeed = Clamp(NowSpeed + PlusSpeed * elapsedTime, 0.0f, 1000.0f);
		NowTime = Clamp(NowTime + elapsedTime, 0.0f, TotalTime);

		// 5) 用「期望點」與「期望朝向」
		//    這裡用「前視距離」能讓導引更穩（可調），太小會抖、太大會切彎
		float lookAhead = Clamp(NowSpeed * 0.25f, 30.0f, 200.0f); // 依速度前視 0.25 秒，介於 30~200px
		float targetS = Clamp(PathS + lookAhead, 0.0f, totalLen);
		XMFLOAT2 targetPos, tangentDir;
		SampleAtS(BasicLine, cumLen, targetS, targetPos, tangentDir);

		// 期望朝向：指向前視點（能自動拉回路徑）
		XMFLOAT2 desiredDir = Normalize(Sub(targetPos, NowPos));

		// 6) 每幀最大轉角限制（角度版：即使速度≈0也會更新朝向）
		float maxDeltaRad = (turnRateDegPerSec * PI / 180.0f) * elapsedTime;
		if (Len(desiredDir) < 1e-6f) desiredDir = NowDirection;           // 防零向量
		float desiredAim = DirToAim_UpZeroCCW(desiredDir);
		float diff = WrapPI(desiredAim - NowAim);
		diff = Clamp(diff, -maxDeltaRad, maxDeltaRad);
		NowAim = WrapPI(NowAim + diff);                                   // 更新瞄準角
		NowDirection = AimToDir_UpZeroCCW(NowAim);                        // 用角度回寫方向

		// 7) 依受限後的朝向移動（位移乘 Δt）
		XMFLOAT2 step = Mul(NowDirection, NowSpeed * elapsedTime);

		// 超步保護：避免一步跨過 targetPos 造成回頭抖
		float distToTarget = Len(Sub(targetPos, NowPos));
		float stepLen = Len(step);
		if (stepLen > distToTarget + 1e-4f) {
			NowPos = targetPos;
		}
		else
		{
			NowPos = Add(NowPos, step);
		}

		// 8) 更新弧長進度（以實際移動推進；也可直接 PathS += NowSpeed*dt）
		//    這裡用「貼近路徑的分量」估計更穩：用當前切線做投影
		XMFLOAT2 onSPos, onSDir;
		SampleAtS(BasicLine, cumLen, PathS, onSPos, onSDir);
		// 估計投影位移量（沿切線分量）
		float deltaAlong = Dot(Sub(NowPos, onSPos), onSDir);
		PathS = Clamp(PathS + deltaAlong, 0.0f, totalLen);
		if (OnCircleCollisionEnter(BattlePlayer_GetCollision_Circle(), GetMissileCircleCollider()))
		{
			GenerateBoomColliderSmall(0.35f, NowPos, { 75.0f ,75.0f });
			active = false;
			g_missilePack.MissileBack();
			return;
		}
		if (BattlePlayerGameObjectCollisionCheck(&NowPos).collide)
		{
			GenerateBoomColliderSmall(0.35f, NowPos, { 75.0f ,75.0f });
			active = false;
			g_missilePack.MissileBack();
			return;
		}
		// 9) 終止判定：時間到 或 路徑走完
		if (NowTime >= TotalTime - 1e-4f || PathS >= totalLen - 1e-3f) {
			NowPos = BasicLine.back();
			GenerateBoomColliderSmall(0.35f, NowPos, { 75.0f ,75.0f });
			active = false;
			g_missilePack.MissileBack();
		}
	}
};
Missile* TryGetEmptyMissile();
Missile Missiles_data[MISSILE_COUNT];
struct HermiteTangents
{
	XMFLOAT2 T0, T1;
};

inline XMFLOAT2 Sub(XMFLOAT2 a, XMFLOAT2 b) { return { a.x - b.x, a.y - b.y }; }
inline float    Len(XMFLOAT2 v) { return std::sqrt(v.x * v.x + v.y * v.y); }
inline XMFLOAT2 Norm(XMFLOAT2 v) { float l = Len(v); return (l > 1e-6f) ? XMFLOAT2{ v.x / l, v.y / l } : XMFLOAT2{ 0,0 }; }
inline XMFLOAT2 PerpCCW(XMFLOAT2 v) { return { -v.y, v.x }; } // 逆時針 90°

HermiteTangents MakeMissileTangents(XMFLOAT2 P0, XMFLOAT2 P1, XMFLOAT2 missileForward, XMFLOAT2 playerVel,
	float k0 = 0.5f, float k1 = 0.5f,
	float lateral = 0.55f,     // 側向彎曲強度
	float side = -1.0f         // +1 左彎, -1 右彎
) {
	if (missileForward.x > 0)
	{
		side = -1;
	}
	else
	{
		side = +1;
	}
	XMFLOAT2 d = Sub(P1, P0);
	float    D = Len(d);
	XMFLOAT2 dir = Norm(d);

	XMFLOAT2 fwd = (Len(missileForward) > 1e-6f) ? Norm(missileForward) : dir;
	XMFLOAT2 approach = (Len(playerVel) > 1e-3f) ? Norm(playerVel) : dir;

	XMFLOAT2 n = PerpCCW(dir);

	XMFLOAT2 T0 = { (fwd.x + side * n.x * lateral) * k0 * D,
					(fwd.y + side * n.y * lateral) * k0 * D };

	XMFLOAT2 T1 = { (approach.x - side * n.x * lateral) * k1 * D,
					(approach.y - side * n.y * lateral) * k1 * D };

	return { T0, T1 };
}

std::vector<XMFLOAT2> DebugMissileLine(XMFLOAT2 StartPos, XMFLOAT2 MissileVector)
{
	XMFLOAT2 missilePos = StartPos;
	HermiteTangents tangent = MakeMissileTangents(missilePos, GetPlayerPos(), MissileVector, { 0,0 });
	std::vector<XMFLOAT2> MissilePoint;
	float t = 0;
	for (int i = 0; i < 20; i++)
	{
		t += 1 / 20.0f;
		float t2 = t * t;
		float t3 = t2 * t;

		float P0 = 2.0f * t3 - 3.0f * t2 + 1.0f;
		float P1 = -2.0f * t3 + 3.0f * t2;
		float T0 = t3 - 2.0f * t2 + t;
		float T1 = t3 - t2;

		missilePos.x = P0 * missilePos.x + T0 * tangent.T0.x + P1 * GetPlayerPos().x + T1 * tangent.T1.x;
		missilePos.y = P0 * missilePos.y + T0 * tangent.T0.y + P1 * GetPlayerPos().y + T1 * tangent.T1.y;
		tangent = MakeMissileTangents(missilePos, GetPlayerPos(), MissileVector, { 0,0 });
		MissilePoint.push_back(missilePos);
	}
	return MissilePoint;
}
void DrawDebugMissileLine()
{
	for (int i = 0; i < MISSILE_COUNT; i++)
	{
		if (Missiles_data[i].active == false)continue;
		/*for (int j = 0; j < 20; j++)
		{
			Sprite_Draw(g_BombLinerMid_TexId, Missiles_data[i].BasicLine[j].x, Missiles_data[i].BasicLine[j].y, 15, 15);
		}*/
		for (int j = 0; j < 20 - 1; j++)
		{
			Collision_DebugDraw_LINE(Missiles_data[i].BasicLine[j], Missiles_data[i].BasicLine[j + 1]);
		}
	}
}
void StartMissile()
{
	if (g_PressedRight && g_PressedCount > 640)
	{
		g_PressedRight = false;
	}
	if (g_PressedRight == false && g_PressedCount == 0)
	{
		g_PressedRight = true;
	}
	if (g_PressedRight)
	{
		g_PressedCount += 40;
	}
	else
	{
		g_PressedCount -= 40;
	}
	TryGetEmptyMissile()->Start(RandomDirectionByMode(2), { 500.0f - 290 + g_PressedCount,40 }, 25.0f, 600.0f);
	TryGetEmptyMissile()->Start(RandomDirectionByMode(2), { 327.0f - 290 + g_PressedCount,140 }, 25.0f, 600.0f);
	TryGetEmptyMissile()->Start(RandomDirectionByMode(2), { 1039.0f - 290 + g_PressedCount,40 }, 25.0f, 600.0f);
	TryGetEmptyMissile()->Start(RandomDirectionByMode(2), { 1209.0f - 290 + g_PressedCount,140 }, 25.0f, 600.0f);
	g_missilePack.nowRealeaseMissile += 4;
}
Missile* TryGetEmptyMissile()
{
	for (int i = 0; i < MISSILE_COUNT; i++)
	{
		if (!Missiles_data[i].active)
		{
			return &Missiles_data[i];
		}
	}
	return nullptr;
}
void UpdateMissile(float time)
{
	for (int i = 0; i < MISSILE_COUNT; i++)
	{
		Missiles_data[i].Update(time);
	}
}
void DrawMissiles()
{
	for (int i = 0; i < MISSILE_COUNT; i++)
	{
		if (!Missiles_data[i].active)continue;
		Sprite_Draw_Rotate(MissileTexId, Missiles_data[i].NowPos.x - 37.5f, Missiles_data[i].NowPos.y - 37.5f, 75, 75, Missiles_data[i].NowAim);
		//Collision_DebugDraw(Missiles_data[i].GetMissileCircleCollider());
	}
}
void StartMissilePack()
{
	g_missilePack.Start();
}
#pragma endregion
//Rocket
static int Rocket_Texid = -1;
static int RocketB_Texid = -1;
struct Rocket
{
	bool active;
	float NowAngle;
	XMFLOAT2 Dir;
	XMFLOAT2 NowPos;
	float LifeTime;
	float NowSpeed;
	float A_Speed;
	float delayTimer;
	bool IsThroughWall = false;

	void Start(XMFLOAT2 dir, XMFLOAT2 nowPos, float a_Speed, float nowAngle, bool isThroughWall)
	{
		active = true;
		Dir = Normalize(dir);
		NowPos = nowPos;
		LifeTime = 0;
		NowSpeed = 0;
		A_Speed = a_Speed;
		NowAngle = nowAngle;
		delayTimer = 0;
		IsThroughWall = isThroughWall;
	}
	void Update(float time)
	{
		if (active)
		{
			if (delayTimer < 0.5f)
			{
				delayTimer += time;

			}
			XMVECTOR nowVec = XMLoadFloat2(&NowPos);
			auto nor = Normalize(Dir);
			XMVECTOR dirV = XMLoadFloat2(&nor);
			nowVec = XMVectorAdd(nowVec, XMVectorScale(dirV, NowSpeed * time));
			float t = LifeTime / 10.0f; // 0 ~ 1
			NowSpeed = 2000 * t * t * A_Speed;  // 二次方，會慢慢起跑然後加速
			XMStoreFloat2(&NowPos, nowVec);
			LifeTime += time;
			if (LifeTime >= 10.0f)
			{
				GenerateBoomColliderSmall_2(0.35f, NowPos, { 75,75 }, IsThroughWall);
				active = false;
				return;
			}
			if (OnCircleCollisionEnter(GetCircleCollider(), BattlePlayer_GetCollision_Circle()))
			{
				GenerateBoomColliderSmall_2(0.35f, NowPos, { 75,75 }, IsThroughWall);
				active = false;
				return;
			}
			if (BattlePlayerGameObjectCollisionCheck(&NowPos).collide)
			{
				if (IsThroughWall)return;
				GenerateBoomColliderSmall_2(0.35f, NowPos, { 75,75 }, IsThroughWall);
				active = false;
				return;
			}
		}
	}

	Circle GetCircleCollider()
	{
		return { {NowPos.x, NowPos.y}, 60 };
	}
};
static Rocket Rockets_Data[ROCKET_COUNT];
void TryStartRocket(XMFLOAT2 dir, XMFLOAT2 pos, float a_speed, float nowAngle, bool through)
{
	for (int i = 0; i < ROCKET_COUNT; i++)
	{
		if (Rockets_Data[i].active == false)
		{
			Rockets_Data[i].Start(dir, { pos.x + dir.x * 150,pos.y + dir.y * 150 }, a_speed, nowAngle, through);
			return;
		}
	}
}

struct RocketFlowerLuncher
{
	bool active;
	float delayTime;
	int rocketLunched;
	int Max;
	float Once;
	bool Through;
	RocketFlowerType Type;
	XMFLOAT2 StartPos;
	void Start(float once, int max, bool through, RocketFlowerType type, XMFLOAT2 startPos)
	{
		active = true;
		Once = once;
		Max = max;
		delayTime = Once;
		rocketLunched = 0;
		Through = through;
		Type = type;
		StartPos = startPos;
	}

	void Update(float time)
	{
		if (active)
		{
			delayTime -= time;
			if (delayTime <= 0)
			{
				delayTime = Once;
				switch (Type)
				{
				case RocketFlowerType::BASIC:
					TryStartRocket(DirFromDeg(360 / Max * rocketLunched), StartPos, 50, 360.0f / Max * rocketLunched, Through);
					rocketLunched++;
					if (rocketLunched == Max)
					{
						active = false;
					}
					break;
				case RocketFlowerType::BR_BETWEEN:
					Through = !Through;
					TryStartRocket(DirFromDeg(360 / Max * rocketLunched), StartPos, 50, 360.0f / Max * rocketLunched, Through);
					rocketLunched++;
					if (rocketLunched == Max)
					{
						active = false;
					}
					break;
				}

			}
		}
	}
}g_RocketFlowerLunchers[5];
struct FullRocketLuncher
{
	bool active;
	float delayTime;
	int plot = 0;
	int AttackKey;
	void Start(int attackKey)
	{
		active = true;
		delayTime = 0.5f;
		plot = 0;
		AttackKey = attackKey;
	}
	void Update(float time)
	{
		if (active)
		{
			switch (AttackKey)
			{
			case 5:
				switch (plot)
				{
				case 0:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(false, { 780,260 });
						plot++;
						delayTime = 1.5f;
					}
					break;
				case 1:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(true, { 780,260 });
						plot++;
						delayTime = 1.5f;
					}
					break;
				case 2:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(false, { 780,260 });
						plot++;
						delayTime = 1.5f;
					}
					break;
				case 3:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(true, { 780,260 });
						plot++;
						delayTime = 1.5f;
					}
					break;
				case 4:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(true, { 780,260 });
						StartRocket16(false, { 780,260 });
						plot++;
						delayTime = 0.5f;
					}
					break;
				case 5:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocketFlower(false, RocketFlowerType::BASIC, { 780,260 }, 25);
						plot++;
						delayTime = 2.5f;
					}
					break;
				case 6:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocketFlower(true, RocketFlowerType::BASIC, { 780,260 }, 25);
						plot++;
						delayTime = 2.5f;
					}
					break;
				case 7:
					delayTime -= time;
					if (delayTime <= 0)
					{
						active = false;
						Battle_State_ChangeState(BattleSystemState::PLAYER_MOVE);
					}
					break;
				}
				break;
			case 6:
				switch (plot)
				{
				case 0:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(false, { 780,260 });
						plot++;
						delayTime = 2.5f;
					}
					break;
				case 1:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocketFlower(true, RocketFlowerType::BASIC, { 780,260 }, 25);
						plot++;
						delayTime = 2.5f;
					}
					break;
				case 2:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocketFlower(false, RocketFlowerType::BASIC, { 780,260 }, 25);
						plot++;
						delayTime = 0.5f;
					}
					break;
				case 3:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(true, { 780,260 });
						plot++;
						delayTime = 0.5f;
					}
					break;
				case 4:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocketFlower(true, RocketFlowerType::BR_BETWEEN, { 780,260 }, 25);
						plot++;
						delayTime = 0.5f;
					}
					break;
				case 5:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocketFlower(false, RocketFlowerType::BR_BETWEEN, { 780,260 }, 25);
						plot++;
						delayTime = 0.5f;
					}
					break;
				case 6:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocketFlower(false, RocketFlowerType::BR_BETWEEN, { 780,260 }, 25);
						plot++;
						delayTime = 0.5f;
					}
					break;
				case 7:
					delayTime -= time;
					if (delayTime <= 0)
					{
						active = false;
						Battle_State_ChangeState(BattleSystemState::PLAYER_MOVE);
					}
					break;
				}
				break;
			case 7:
				switch (plot)
				{
				case 0:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(false, { 1600,400 });
						StartRocket16(true, { 0,400 });
						StartRocket16(false, { 1600,400 });
						StartRocket16(true, { 0,400 });
						plot++;
						delayTime = 2.0f;
					}
					break;
				case 1:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(true, { 1600,400 });
						StartRocket16(false, { 0,400 });
						StartRocket16(true, { 1600,400 });
						StartRocket16(false, { 0,400 });
						plot++;
						delayTime = 2.0f;
					}
					break;
				case 2:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(false, { 1600,400 });
						StartRocket16(true, { 0,400 });
						StartRocket16(false, { 1600,400 });
						StartRocket16(true, { 0,400 });
						plot++;
						delayTime = 2.0f;
					}
					break;
				case 3:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocketFlower(true, RocketFlowerType::BR_BETWEEN, { 800,0 }, 25);
						StartRocketFlower(false, RocketFlowerType::BR_BETWEEN, { 800,900 }, 25);
						plot++;
						delayTime = 1.5f;
					}
					break;
				case 4:
					delayTime -= time;
					if (delayTime <= 0)
					{
						active = false;
						Battle_State_ChangeState(BattleSystemState::PLAYER_MOVE);
					}
					break;
				}
				break;
			case 8:
				switch (plot)
				{
				case 0:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(false, { 1600,400 });
						StartRocket16(true, { 0,400 });
						StartRocket16(false, { 1600,400 });
						StartRocket16(true, { 0,400 });
						plot++;
						delayTime = 1.5f;
					}
					break;
				case 1:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(true, { 1600,400 });
						StartRocket16(false, { 0,400 });
						StartRocket16(true, { 1600,400 });
						StartRocket16(false, { 0,400 });
						plot++;
						delayTime = 1.5f;
					}
					break;
				case 2:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocket16(false, { 1600,400 });
						StartRocket16(true, { 0,400 });
						StartRocket16(false, { 1600,400 });
						StartRocket16(true, { 0,400 });
						plot++;
						delayTime = 1.5f;
					}
					break;
				case 3:
					delayTime -= time;
					if (delayTime <= 0)
					{
						StartRocketFlower(false, RocketFlowerType::BASIC, { 0,0 }, 25);
						StartRocketFlower(true, RocketFlowerType::BASIC, { 1600,0 }, 25);
						StartRocketFlower(false, RocketFlowerType::BASIC, { 0,900 }, 25);
						StartRocketFlower(true, RocketFlowerType::BASIC, { 1600,900 }, 25);
						plot++;
						delayTime = 1.5f;
					}
					break;
				case 4:
					delayTime -= time;
					if (delayTime <= 0)
					{
						active = false;
						Battle_State_ChangeState(BattleSystemState::PLAYER_MOVE);
					}
					break;
				}
				break;
				break;
			default:
				break;
			}

		}
	}
}g_fullRocketLuncher;
static int debug_press = 0;
void StartRocket16(bool through, XMFLOAT2 start)
{
	debug_press += 30;
	for (int i = 0; i < 16; i++)
	{
		TryStartRocket(DirFromDeg(360 / 16.0f * i + debug_press), start, 50, 360 / 16.0f * i + debug_press, through);
	}
}
void StartRocketFlower(bool through, RocketFlowerType type, XMFLOAT2 start, int count)
{
	for (int i = 0; i < 5; i++)
	{
		if (g_RocketFlowerLunchers[i].active == false)
		{
			g_RocketFlowerLunchers[i].Start(0.05f, count, through, type, start);
			return;
		}
	}
}
void StartRocketFullLuncher(int key)
{
	g_fullRocketLuncher.Start(key);
}
void UpdateRocket(float time)
{
	g_fullRocketLuncher.Update(time);
	for (int i = 0; i < 5; i++)
	{
		g_RocketFlowerLunchers[i].Update(time);

	}
	for (int i = 0; i < ROCKET_COUNT; i++)
	{
		if (Rockets_Data[i].active)
		{
			Rockets_Data[i].Update(time);
		}
	}
}
void DrawRocket()
{
	for (int i = 0; i < ROCKET_COUNT; i++)
	{
		if (Rockets_Data[i].active)
		{
			if (Rockets_Data[i].IsThroughWall)
			{
				Sprite_Draw_Rotate(RocketB_Texid, Rockets_Data[i].NowPos.x - 60.0f, Rockets_Data[i].NowPos.y - 60.0f, 120, 120, DegToRad(Rockets_Data[i].NowAngle));
				Collision_DebugDraw_LINE(Rockets_Data[i].NowPos, { Rockets_Data[i].NowPos.x + Rockets_Data[i].Dir.x * 10000.0f,  Rockets_Data[i].NowPos.y + Rockets_Data[i].Dir.y * 10000.0f });
			}
			else
			{
				Sprite_Draw_Rotate(Rocket_Texid, Rockets_Data[i].NowPos.x - 60.0f, Rockets_Data[i].NowPos.y - 60.0f, 120, 120, DegToRad(Rockets_Data[i].NowAngle));
			}
		}
	}
}


void BoomberBossInitialize()
{
	g_Boomber_Health = 8;
	g_Boomber_HitCount = 0;
	for (int i = 0; i < MINE_COUNT; i++)
	{
		Mines[i].active = false;
	}
	for (int i = 0; i < BOOMCOLLIDERCOUNT; i++)
	{
		BoomColliders[i].active = false;
	}
	for (int i = 0; i < MISSILE_COUNT; i++)
	{
		Missiles_data[i].active = false;
	}
	for (int i = 0; i < ROCKET_COUNT; i++)
	{
		Rockets_Data[i].active = false;
	}
	for (int i = 0; i < 5; i++)
	{
		g_RocketFlowerLunchers[i].active = false;
	}
	g_fullRocketLuncher.active = false;
	g_Realease_Mine = 0;
	g_missilePack.active = false;
	g_BombLinerStart_TexId = Texture_Load(L"BombLinerStart.png");
	g_BombLinerEnd_TexId = Texture_Load(L"BombLinerEnd.png");
	g_BombLinerMid_TexId = Texture_Load(L"BombLinerMid.png");
	MissileTexId = Texture_Load(L"Missile.png");
	Rocket_Texid = Texture_Load(L"Rocket.png");
	RocketB_Texid = Texture_Load(L"RocketThrough.png");
}
void BoomberBossFinitialize()
{
	g_Boomber_Health = 8;
	g_Boomber_HitCount = 0;
	for (int i = 0; i < MINE_COUNT; i++)
	{
		Mines[i].active = false;
	}
	for (int i = 0; i < BOOMCOLLIDERCOUNT; i++)
	{
		BoomColliders[i].active = false;
	}
	for (int i = 0; i < MISSILE_COUNT; i++)
	{
		Missiles_data[i].active = false;
	}
	for (int i = 0; i < ROCKET_COUNT; i++)
	{
		Rockets_Data[i].active = false;
	}
	for (int i = 0; i < 5; i++)
	{
		g_RocketFlowerLunchers[i].active = false;
	}
	g_Realease_Mine = 0;
	g_missilePack.active = false;
	g_fullRocketLuncher.active = false;
}
void BoomberBossUpdate(float time)
{
	for (int i = 0; i < MINE_COUNT; i++)
	{
		Mines[i].Update(time);
	}
	for (int i = 0; i < BOOMCOLLIDERCOUNT; i++)
	{
		BoomColliders[i].Update(time);
	}
	UpdateMissile(time);
	g_missilePack.Update(time);
	UpdateRocket(time);
}

void GenerateMine(float delay, float countdown, XMFLOAT2 pos)
{
	for (int i = 0; i < MINE_COUNT; i++)
	{
		if (Mines[i].active == false)
		{
			Mines[i].Start(delay, countdown, pos);
			g_Realease_Mine++;
			break;
		}
	}
}
void GenerateBoomCollider(float life, XMFLOAT2 pos)
{
	for (int i = 0; i < BOOMCOLLIDERCOUNT; i++)
	{
		if (BoomColliders[i].active == false)
		{
			BoomColliders[i].Start(life, pos);
			break;
		}
	}
}
void GenerateBoomColliderBig(float life, XMFLOAT2 pos, XMFLOAT2 half)
{
	for (int i = 0; i < BOOMCOLLIDERCOUNT; i++)
	{
		if (BoomColliders[i].active == false)
		{
			BoomColliders[i].StartBig(life, { pos.x - 250,pos.y - 250 }, half);
			break;
		}
	}
}
void GenerateBoomColliderSmall(float life, XMFLOAT2 pos, XMFLOAT2 half)
{
	for (int i = 0; i < BOOMCOLLIDERCOUNT; i++)
	{
		if (BoomColliders[i].active == false)
		{
			BoomColliders[i].StartSmall(life, { pos.x - 75,pos.y - 75 }, half);
			break;
		}
	}
}
void GenerateBoomColliderSmall_2(float life, XMFLOAT2 pos, XMFLOAT2 half, bool through)
{
	for (int i = 0; i < BOOMCOLLIDERCOUNT; i++)
	{
		if (BoomColliders[i].active == false)
		{
			BoomColliders[i].StartSmall_2(life, { pos.x - 75,pos.y - 75 }, half, through);
			break;
		}
	}
}
void GenerateMineAttack(int round)
{
	std::vector<XMFLOAT2> BoomberPos;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distX(0.0f, 1400.f);
	std::uniform_real_distribution<float> distY(0.0f, 700.0f);

	const int targetCount = round;
	const float minDistance = 100.0f; // 最小間隔，依需求可調

	while (BoomberPos.size() < targetCount) {
		XMFLOAT2 candidate{ distX(gen), distY(gen) };

		bool valid = true;
		for (const auto& p : BoomberPos) {
			if (Distance(candidate, p) < minDistance) {
				valid = false;
				break;
			}
		}

		if (valid) {
			BoomberPos.push_back(candidate);
		}
	}
	for (int i = 0; i < BoomberPos.size(); i++)
	{
		GenerateMine(0.1f * i, 0.75f, BoomberPos[i]);
	}
}
void BombLinerUpdate(float time)
{
	for (int i = 0; i < BOMBLINERCOUNT; i++)
	{
		BombLiners[i].Update(time);
	}
}
void BombLinerDestroyed(int index)
{
	if (BombLiners[index].active)
	{
		BombLiners[index].active = false;
		g_BombLinerGen--;
		if (g_BombLinerGen == 0)
		{
			Battle_State_ChangeState(BattleSystemState::PLAYER_SHOOT);
		}
	}
}
void DrawBombLine()
{
	for (int i = 0; i < BOMBLINERCOUNT; i++)
	{
		if (BombLiners[i].active)
		{
			Sprite_Draw(g_BombLinerMid_TexId, BombLiners[i].NowPos.x - 75 / 2.0f, BombLiners[i].NowPos.y - 75 / 2.0f, 75, 75);
			Sprite_Draw(g_BombLinerStart_TexId, BombLiners[i].StartPos.x - 150 / 2.0f, BombLiners[i].StartPos.y - 150 / 2.0f, 150, 150);
			Sprite_Draw(g_BombLinerEnd_TexId, BombLiners[i].EndPos.x - 75 / 2.0f, BombLiners[i].EndPos.y - 75 / 2.0f, 75, 75);
			Collision_DebugDraw_LINE(BombLiners[i].StartPos, BombLiners[i].EndPos);
#if defined(DEBUG)||defined(_DEBUG)
			Collision_DebugDraw_Box(GetBombLinerMidPointCollision(i));
#endif
		}

	}
}
void Generate_BombLiner()
{
	std::vector<XMFLOAT2> BoomLinerPos;
	std::vector<XMFLOAT2> BoomLinerEnd;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distX(0.0f, 1500.f);
	std::uniform_real_distribution<float> distY(0.0f, 800.0f);

	const int targetCount = BOMBLINERCOUNT;
	const float minDistance = 500.0f;
	const float minP_PDistance = 850.0f;// 最小間隔，依需求可調

	while (BoomLinerPos.size() < targetCount) {
		XMFLOAT2 candidate{ distX(gen), distY(gen) };

		bool valid = true;
		for (const auto& p : BoomLinerPos) {
			if (Distance(candidate, p) < minDistance) {
				valid = false;
				break;
			}
		}

		if (valid) {
			BoomLinerPos.push_back(candidate);
		}
	}
	while (BoomLinerEnd.size() < targetCount) {
		XMFLOAT2 candidate{ distX(gen), distY(gen) };

		bool valid = true;

		if (Distance(candidate, BoomLinerPos[BoomLinerEnd.size()]) < minP_PDistance)
		{
			valid = false;
		}


		if (valid) {
			BoomLinerEnd.push_back(candidate);
		}
	}
	for (int i = 0; i < targetCount; i++)
	{
		BombLiners[i].Start(BoomLinerPos[i], BoomLinerEnd[i], 5.0f);
		g_BombLinerGen++;
	}
}
int GetBoomberHealth()
{
	return g_Boomber_Health;
}
Box GetExplosionCollision(int index)
{
	float cx = BoomColliders[index].ColliderPos.x + BoomColliders[index].ColliderHalf_length.x;
	float cy = BoomColliders[index].ColliderPos.y + BoomColliders[index].ColliderHalf_length.y;



	return { {cx,cy}, BoomColliders[index].ColliderHalf_length.x,BoomColliders[index].ColliderHalf_length.y };

}
Box GetBombLinerMidPointCollision(int index)
{
	float cx = BombLiners[index].NowPos.x;
	float cy = BombLiners[index].NowPos.y;



	return { {cx,cy}, 75 / 2.0f,75 / 2.0f };
}
bool GetExplosionColliderEnable(int index)
{
	return BoomColliders[index].active;
}
void HitBoomber()
{
	if (g_Boomber_HitCount < 9)
	{
		g_Boomber_HitCount++;
	}
	else
	{
		g_Boomber_HitCount = 0;
		g_Boomber_Health--;
		if (g_Boomber_Health >= 1)
		{
			Battle_State_ChangeState(BattleSystemState::BOSS_DESIDE);
		}
		else
		{
			Battle_State_ChangeState(BattleSystemState::PLAYERWIN);
		}
	}
}
void CheatBoomberHealth()
{
	g_Boomber_Health--;
}