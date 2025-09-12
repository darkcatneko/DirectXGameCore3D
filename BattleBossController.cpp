#include "BattleBossController.h"
#include "sprite_anim.h"
#include "sprite.h"
#include "Texture.h"
#include "Key_Logger.h"
#include "BossCleanerController.h"
#include <vector>
#include <cmath>
#include "Effect.h"
#include "Collision.h"
#include "bullet.h"
#include "BattleSystem.h"
#include "NekoTool.h"
#include "Camera.h"
#include "particle.h"
#include "BattleBossBoomber.h"

constexpr float EyesTimer = 5.0f;

struct BounceBullet
{
	bool g_bulletEnable = false;
	float g_bulletSpeed;
	std::vector<XMFLOAT2> g_BulletPathWay;
	XMFLOAT2 g_NowBulletDestination;
	XMFLOAT2 g_NowBulletPosition;
	int g_NowDestinationCount = 0;
};
static int BulletShooted = 0;
static BounceBullet g_BounceBullet_data[3];
static float g_time = 0.0f;
static int g_BattleBoss_Cleaner_AnimPatternID = -1;
static int g_BattleBoss_CleanerAttack_AnimPatternID = -1;
static int g_BattleBoss_Cleaner_AnimPlayerID = -1;
static int g_BattleBoss_CleanerAttack_AnimPlayerID = -1;
static int g_BattleBoss_Boomber_AnimPatternID = -1;
static int g_BattleBoss_Boomber_AnimPlayerID = -1;
static int g_BattleBoss_BoomberAttack_AnimPatternID = -1;
static int g_BattleBoss_BoomberAttack_AnimPlayerID = -1;
static int g_Cleaner_HealthIcon_Texid = -1;
static int g_CleanerBullet_TexId = -1;
static int g_EyesCountDown_TexId = -1;
static int g_Cleaner_Eyes_AimFail_TexID = -1;
static int g_PathCount = 21;

static float g_MoveEyeMiniGameTimer = EyesTimer;
static bool g_BossAnger = false;
static bool g_BossFlashTrigger = false;
static float g_BossFlashTimer = 0.15f;
static XMFLOAT4 g_BossBaseColor = { 1.0f,1.0f,1.0f,1.0f };
static XMFLOAT4 g_BossCurrentColor = { 1.0f,1.0f,1.0f,1.0f };

//CleanerSpiningAttack
static int g_CleanerSweapAttack_TexId = -1;
static bool g_CleanerSweapEnable = false;
static XMFLOAT2 g_CleanerSweap_NowPosition;
static XMFLOAT2 g_CleanerSweap_NowDestination;
static int g_CleanerSweap_NowDestinationCount;
static float g_CleanerSweapRotateAngle;
std::vector<XMFLOAT2> g_SweapPathWay;

//BossDead_Animation
static XMFLOAT2 g_BossNowPosition;
static XMFLOAT2 g_BossBasicPosition_Data[]
{
	{550,100},
	{588,13},
};
struct BossShake {
	bool   active = false;
	float  timeLeft = 0.0f;
	float  duration = 0.0f;
	float  intensity = 0.0f;   // 像素振幅
	float  frequency = 25.0f;  // 每秒換一次目標方向的次數(可調)
	XMFLOAT2 basicPos = { 0,0 };

	// 兩個目標方向，用插值讓晃動更順
	float  curX = 0.0f, curY = 0.0f;
	float  nextX = 0.0f, nextY = 0.0f;

	// 取樣計時
	float  sampleInterval = 0.0f;
	float  sampleT = 0.0f;

	// RNG 狀態
	uint32_t rng = 0xA53F9E4Du;

	void Start(float seconds, float strength, XMFLOAT2 pos) {
		active = true;
		duration = std::max(0.0001f, seconds);
		timeLeft = seconds;
		intensity = strength;
		sampleInterval = 1.0f / std::max(1.0f, frequency);
		sampleT = sampleInterval; // 讓第一幀就抽一次方向
		curX = curY = 0.0f;
		basicPos = pos;
		PickNext();
	}

	void PickNext() {
		nextX = RandNeg1To1(rng);
		nextY = RandNeg1To1(rng);
		float len = std::sqrt(nextX * nextX + nextY * nextY);
		if (len > 1e-6f) { nextX /= len; nextY /= len; } // 單位向量
	}

	// 每幀呼叫：dt = deltaTime(秒)，baseX/baseY = 你原本想要的相機位置(例如跟隨玩家)
	void Update(float dt) {
		if (!active) { return; }

		timeLeft -= dt;
		if (timeLeft <= 0.0f) {
			active = false;
			g_BossNowPosition = basicPos;
			return;
		}

		// 方向插值（把上一個方向平滑過渡到下一個方向）
		sampleT += dt;
		if (sampleT >= sampleInterval) {
			sampleT -= sampleInterval;
			// 新輪開始：把 next 搬到 cur，再抽一個 next
			curX = nextX; curY = nextY;
			PickNext();
		}
		float a = sampleT / sampleInterval; // 0..1
		float dirX = curX + (nextX - curX) * a;
		float dirY = curY + (nextY - curY) * a;

		// 震幅衰減：二次衰減(更自然)，也可改成 exp 或線性
		float t = timeLeft / duration;      // 1→0
		float amp = intensity * (t * t);    // 二次衰減

		g_BossNowPosition.x = dirX * amp + basicPos.x;
		g_BossNowPosition.y = dirY * amp + basicPos.y;
	}
} g_BossShaker;
struct CleanerDeadAnimator
{
	bool   active = false;
	float  timeLeft = 0.0f;
	int	now_Plot = 0;


	void Start(float seconds) {
		active = true;
		timeLeft = seconds;
		now_Plot = 0;
	}


	// 每幀呼叫：dt = deltaTime(秒)，baseX/baseY = 你原本想要的相機位置(例如跟隨玩家)
	void Update(float dt) {
		if (!active) { return; }
		timeLeft -= dt;
		if (now_Plot == 0)
		{
			g_BossShaker.Start(0.35, 10, { 550,100 });
			now_Plot++;
		}
		if (now_Plot == 3)
		{
			g_BossCurrentColor = { 1.0f,1.0f,1.0f,timeLeft / 5.0f };
		}
		if (timeLeft <= 0.0f) {
			switch (now_Plot)
			{
			case 1:
				timeLeft = 1.55f;
				g_BossShaker.Start(0.65, 10, { 550,100 });
				now_Plot++;
				break;
			case 2:
				timeLeft = 5.0f;
				g_BossShaker.Start(4.95f, 35, { 550,100 });
				now_Plot++;
				break;
			case 3:
				Battle_State_ChangeState(BattleSystemState::DIALOGUE);
				active = false;
				return;
			default:
				break;
			}
		}

	}
}g_BossDeadAnimation;

static float g_BeforeShootTimer = 0;
static bool g_StartBulletCountDown;
static bool g_StartBulletMovement = false;
//static bool g_StartSweapMovement = false;
void BossFlashUpdate(float time);
void Bullet_Back();


void BattleBossController_Initialize()
{
	BulletShooted = 0;
	g_MoveEyeMiniGameTimer = EyesTimer;
	g_time = 0;
	g_BeforeShootTimer = 0;
	g_BossAnger = false;
	g_StartBulletMovement = false;
	//g_StartSweapMovement = false;
	g_EyesCountDown_TexId = Texture_Load(L"Line.png");
	g_Cleaner_HealthIcon_Texid = Texture_Load(L"Scavenger_BattleEye.png");
	g_CleanerBullet_TexId = Texture_Load(L"Scavenger_Bullet.png");
	g_BattleBoss_Cleaner_AnimPatternID = AnimDataInitialize(L"Scavenger_BAnimation.png", 12, 12, 0, 0, { 0,0 }, { 1.0f / 12.0f,1 }, { 455,455 }, true, 0.1f, 0.55f);
	g_BattleBoss_CleanerAttack_AnimPatternID = AnimDataInitialize(L"Scavenger_AtkS_Animation.png", 12, 12, 0, 0, { 0,0 }, { 1.0f / 12.0f,1 }, { 455,455 }, true, 0.1f, 0.5f);
	g_BattleBoss_Boomber_AnimPatternID = AnimDataInitialize(L"Celeste_BAnimation.png", 12, 12, 0, 0, { 0,0 }, { 1.0f / 12,1 }, { 376,531 }, true, 0.1f, 0.5f);
	g_BattleBoss_BoomberAttack_AnimPatternID = AnimDataInitialize(L"Celeste_Atk_Animation.png", 12, 12, 0, 0, { 0,0 }, { 1.0f / 12,1 }, { 407,570 }, true, 0.1f, 0.5f);
	g_BattleBoss_Cleaner_AnimPlayerID = SpriteAnim_CreatePlayer(g_BattleBoss_Cleaner_AnimPatternID, { 0,0 });
	g_BattleBoss_CleanerAttack_AnimPlayerID = SpriteAnim_CreatePlayer(g_BattleBoss_CleanerAttack_AnimPatternID, { 0,0 });
	g_BattleBoss_Boomber_AnimPlayerID = SpriteAnim_CreatePlayer(g_BattleBoss_Boomber_AnimPatternID, { 0,0 });
	g_BattleBoss_BoomberAttack_AnimPlayerID = SpriteAnim_CreatePlayer(g_BattleBoss_BoomberAttack_AnimPatternID, { 0,0 });
	g_BossNowPosition = g_BossBasicPosition_Data[0];
	g_BossCurrentColor = { 1.0f,1.0f,1.0f,1.0f };
	g_Cleaner_Eyes_AimFail_TexID = Texture_Load(L"EvilEyes.png");
	g_CleanerSweapAttack_TexId = Texture_Load(L"Sweap.png");
	for (int i = 0; i < 3; i++)
	{
		g_BounceBullet_data[i].g_bulletEnable = false;
		g_BounceBullet_data[i].g_NowDestinationCount = 0;
		g_BounceBullet_data[i].g_NowBulletPosition = { 2000,2000 };
		g_BounceBullet_data[i].g_NowBulletDestination = { 2000,2000 };
	}
	g_CleanerSweap_NowDestinationCount = 0;
	g_CleanerSweap_NowPosition = { 2000,2000 };
	g_CleanerSweap_NowDestination = { 2000,2000 };
	g_CleanerSweapEnable = false;
}
void BattleBoss_BomberInit()
{
	BoomberBossInitialize();
	g_BossAnger = false;
	g_BossNowPosition = g_BossBasicPosition_Data[1];
	g_BossCurrentColor = { 1.0f,1.0f,1.0f,1.0f };
}
void BattleBossController_Finitialize()
{
	BoomberBossFinitialize();
}

inline float AngleToTargetDeg(const XMFLOAT2& from, const XMFLOAT2& to)
{
	float dx = to.x - from.x;
	float dy = to.y - from.y; // 注意：Y 向下為正

	// 0°在上、順時針為正：使用 atan2(dx, -dy)
	float rad = atan2f(dx, -dy);                 // 回傳 [-pi, pi]
	float deg = rad * (180.0f / XM_PI);          // 轉成角度
	if (deg < 0.0f) deg += 360.0f;               // 正規化到 [0, 360)
	return rad;
}
void BattleBossControllerUpdate(float time)
{
	g_time += time;
	g_BossShaker.Update(time);
	g_BossDeadAnimation.Update(time);
	switch (GetBattleBoss())
	{
	case BattleBossState::CLEANER:
		if (g_StartBulletCountDown == true)
		{
			g_BeforeShootTimer += time;
			if (g_BeforeShootTimer >= 1.3f)
			{
				g_StartBulletMovement = true;
				g_StartBulletCountDown = false;
				g_BeforeShootTimer = 0;
			}
		}
		for (int i = 0; i < 3; i++)
		{
			if (g_StartBulletMovement == true)
			{
				if (g_BounceBullet_data[i].g_bulletEnable == false)continue;
				const float eps = 1e-6f;
				const int pathCount = g_PathCount;             // ← 用你的實際路徑點數
				float remain = g_BounceBullet_data[i].g_bulletSpeed * time;           // 這幀可移動距離
				int guard = 64;                                // 安全閂避免無限迴圈

				while (remain > 0.0f && g_StartBulletMovement && guard > 0)
				{
					guard--;
					// 目標向量
					float dx = g_BounceBullet_data[i].g_NowBulletDestination.x - g_BounceBullet_data[i].g_NowBulletPosition.x;
					float dy = g_BounceBullet_data[i].g_NowBulletDestination.y - g_BounceBullet_data[i].g_NowBulletPosition.y;
					float distSq = dx * dx + dy * dy;

					// 已在目標點上 → 切下一個
					if (distSq <= eps) {
						if (g_BounceBullet_data[i].g_NowDestinationCount + 1 < pathCount) {
							StartCameraShake(0.35f, 3.0f);
							++g_BounceBullet_data[i].g_NowDestinationCount;
							g_BounceBullet_data[i].g_NowBulletDestination = g_BounceBullet_data[i].g_BulletPathWay[g_BounceBullet_data[i].g_NowDestinationCount];
							continue;
						}
						else {
							g_BounceBullet_data[i].g_NowBulletPosition = g_BounceBullet_data[i].g_NowBulletDestination; // 對齊
							g_BounceBullet_data[i].g_NowBulletPosition = { 2000,2000 };
							g_BounceBullet_data[i].g_NowBulletDestination = { 2000,2000 };
							g_BounceBullet_data[i].g_bulletEnable = false;
							Bullet_Back();
							break;
						}
					}

					float dist = sqrtf(distSq);
					float step = (remain < dist) ? remain : dist; // 這次前進量 = min(剩餘, 到目標距離)

					// 單位方向
					float invLen = 1.0f / dist;
					XMFLOAT2 dir{ dx * invLen, dy * invLen };

					// 移動
					XMVECTOR pos = XMLoadFloat2(&g_BounceBullet_data[i].g_NowBulletPosition) +
						XMLoadFloat2(&dir) * step;
					DirectX::XMStoreFloat2(&g_BounceBullet_data[i].g_NowBulletPosition, pos);
					Particle_Create(g_BounceBullet_data[i].g_NowBulletPosition, { 1.0f,1.0f,1.0f,1.0f }, 20, 0.25f);
					remain -= step;

					// 若剛好抵達節點（或非常接近），切下一個
					if (step >= dist - 1e-7f) {
						if (g_BounceBullet_data[i].g_NowDestinationCount + 1 < pathCount) {
							StartCameraShake(0.35f, 3.0f);
							XMFLOAT2 temp = { g_BounceBullet_data[i].g_NowBulletDestination.x - 32,g_BounceBullet_data[i].g_NowBulletDestination.y - 32.0f };
							Effect_Creater(temp, 0);
							++g_BounceBullet_data[i].g_NowDestinationCount;
							g_BounceBullet_data[i].g_NowBulletDestination = g_BounceBullet_data[i].g_BulletPathWay[g_BounceBullet_data[i].g_NowDestinationCount];
						}
						else {
							g_BounceBullet_data[i].g_NowBulletPosition = g_BounceBullet_data[i].g_NowBulletDestination; // 對齊終點		
							g_BounceBullet_data[i].g_NowBulletPosition = { 2000,2000 };
							g_BounceBullet_data[i].g_NowBulletDestination = { 2000,2000 };
							g_BounceBullet_data[i].g_bulletEnable = false;
							Bullet_Back();
							break;
						}
					}
				}
			}

		}
		if (g_StartBulletMovement == true)
		{
			if (g_CleanerSweapEnable == false)return;
			const float eps = 1e-6f;
			const int pathCount = 11;             // ← 用你的實際路徑點數
			float remain = 500 * time;           // 這幀可移動距離
			int guard = 64;                      // 安全閂避免無限迴圈

			while (remain > 0.0f && g_StartBulletMovement && guard-- > 0)
			{
				// 目標向量
				float dx = g_CleanerSweap_NowDestination.x - g_CleanerSweap_NowPosition.x;
				float dy = g_CleanerSweap_NowDestination.y - g_CleanerSweap_NowPosition.y;
				float distSq = dx * dx + dy * dy;

				// 已在目標點上 → 切下一個
				if (distSq <= eps) {
					if (g_CleanerSweap_NowDestinationCount + 1 < pathCount) {
						StartCameraShake(0.55f, 15.0f);
						++g_CleanerSweap_NowDestinationCount;
						g_CleanerSweap_NowDestination = g_SweapPathWay[g_CleanerSweap_NowDestinationCount];
						continue;
					}
					else {
						g_CleanerSweap_NowPosition = g_CleanerSweap_NowDestination; // 對齊
						g_CleanerSweap_NowPosition = { 2000,2000 };
						g_CleanerSweap_NowDestination = { 2000,2000 };
						g_CleanerSweapEnable = false;
						Bullet_Back();
						break;
					}
				}

				float dist = sqrtf(distSq);
				float step = (remain < dist) ? remain : dist; // 這次前進量 = min(剩餘, 到目標距離)

				// 單位方向
				float invLen = 1.0f / dist;
				XMFLOAT2 dir{ dx * invLen, dy * invLen };

				// 移動
				XMVECTOR pos = XMLoadFloat2(&g_CleanerSweap_NowPosition) +
					XMLoadFloat2(&dir) * step;
				DirectX::XMStoreFloat2(&g_CleanerSweap_NowPosition, pos);

				remain -= step;

				// 若剛好抵達節點（或非常接近），切下一個
				if (step >= dist - 1e-7f) {
					if (g_CleanerSweap_NowDestinationCount + 1 < pathCount) {
						StartCameraShake(0.55f, 15.0f);
						XMFLOAT2 temp = { g_CleanerSweap_NowDestination.x - 32,g_CleanerSweap_NowDestination.y - 32.0f };
						Effect_Creater(temp, 0);
						++g_CleanerSweap_NowDestinationCount;
						g_CleanerSweap_NowDestination = g_SweapPathWay[g_CleanerSweap_NowDestinationCount];
					}
					else {
						g_CleanerSweap_NowPosition = g_CleanerSweap_NowDestination; // 對齊終點		
						g_CleanerSweap_NowPosition = { 2000,2000 };
						g_CleanerSweap_NowDestination = { 2000,2000 };
						g_CleanerSweapEnable = false;
						Bullet_Back();
						break;
					}
				}
			}
		}
		g_CleanerSweapRotateAngle += time * 20;
		break;
	case BattleBossState::BOOMBER:
		BoomberBossUpdate(time);
		break;
	}

	if (KeyLogger_IsTrigger(KK_T))
	{
		Battle_Stage_ChangeBoss();
	}
	if (KeyLogger_IsTrigger(KK_U))
	{
		CheatBoomberHealth();
	}
	bullet_boss_Collision_Update();
	BossFlashUpdate(time);
}
void BossFlashUpdate(float time)
{
	if (g_BossFlashTrigger == true)
	{
		g_BossFlashTimer = clamp<float>(g_BossFlashTimer - time, 0, 10);
		float t = g_BossFlashTimer * 20.0f; // 控制閃爍頻率 (20.0f = 頻率)
		float blink = (sinf(t) * 0.5f + 0.5f); // [0,1] 之間
		// 顏色在原始顏色 和 白色之間切換
		XMFLOAT4 white{ 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT4 red{ 1.0f, 0.0f, 0.0f, 1.0f };

		g_BossCurrentColor.x = white.x + (red.x - white.x) * blink;
		g_BossCurrentColor.y = white.y + (red.y - white.y) * blink;
		g_BossCurrentColor.z = white.z + (red.z - white.z) * blink;
		g_BossCurrentColor.w = 1.0f;
		if (g_BossFlashTimer == 0)
		{
			g_BossFlashTrigger = false;
			g_BossFlashTimer = 0.15f;
			g_BossCurrentColor = g_BossBaseColor;
		}
	}
}
void BattleBossEyeMiniGameTimerUpdate(float time)
{
	if (GetBattleBoss() != BattleBossState::CLEANER)return;
	if (g_MoveEyeMiniGameTimer > 0)
	{
		g_MoveEyeMiniGameTimer -= time;
	}
	else
	{
		if (GetAllEyeCover() == false)
		{
			g_BossAnger = true;
		}
		Battle_State_ChangeState(BattleSystemState::PLAYER_SHOOT);
	}
}
void BattleBossEyeMiniGameTimerReset()
{
	if (GetBattleBoss() != BattleBossState::CLEANER)return;
	g_MoveEyeMiniGameTimer = EyesTimer;
	g_BossAnger = false;
}
void BattleBossControllerDraw()
{
	switch (GetBattleBoss())
	{
	case BattleBossState::CLEANER:
		if (GetBattleState() == BattleSystemState::BOSS_DESIDE)
		{
			SpriteAnim_Draw(g_BattleBoss_CleanerAttack_AnimPlayerID, g_BossNowPosition.x, g_BossNowPosition.y, g_BossCurrentColor);
		}
		else
		{
			SpriteAnim_Draw(g_BattleBoss_Cleaner_AnimPlayerID, g_BossNowPosition.x, g_BossNowPosition.y, g_BossCurrentColor);
		}

		if (GetCleanerHealth() >= 6)
		{
			Sprite_Draw(g_Cleaner_HealthIcon_Texid, 480, 243.0f + sinf(g_time * 4.0f + 2) * 3.0f, 75, 75, 0.55f);
		}
		if (GetCleanerHealth() >= 5)
		{
			Sprite_Draw(g_Cleaner_HealthIcon_Texid, 480, 344.0f + sinf(g_time * 4.0f + 4) * 3.0f, 75, 75, 0.55f);
		}
		if (GetCleanerHealth() >= 4)
		{
			Sprite_Draw(g_Cleaner_HealthIcon_Texid, 579, 290.0f + sinf(g_time * 4.0f + 6) * 3.0f, 75, 75, 0.55f);
		}
		if (GetCleanerHealth() >= 3)
		{
			Sprite_Draw(g_Cleaner_HealthIcon_Texid, 1018, 243.0f + sinf(g_time * 4.0f + 8) * 3.0f, 75, 75, 0.55f);
		}
		if (GetCleanerHealth() >= 2)
		{
			Sprite_Draw(g_Cleaner_HealthIcon_Texid, 1018, 344.0f + sinf(g_time * 4.0f + 10) * 3.0f, 75, 75, 0.55f);
		}
		if (GetCleanerHealth() >= 1)
		{
			Sprite_Draw(g_Cleaner_HealthIcon_Texid, 919, 290.0f + sinf(g_time * 4.0f + 12) * 3.0f, 75, 75, 0.55f);
		}
		if (g_StartBulletMovement == true)
		{
			for (int i = 0; i < 3; i++)
			{
				if (g_BounceBullet_data[i].g_bulletEnable == true)
				{
					Sprite_Draw_Rotate(g_CleanerBullet_TexId, g_BounceBullet_data[i].g_NowBulletPosition.x - 75.0f / 2, g_BounceBullet_data[i].g_NowBulletPosition.y - 75.0f / 2, 75, 75, AngleToTargetDeg(g_BounceBullet_data[i].g_NowBulletPosition, g_BounceBullet_data[i].g_NowBulletDestination), 0.55f);
					/*for (int j = 0; j < 20; j++)
					{
						Collision_DebugDraw_LINE(g_BounceBullet_data[i].g_BulletPathWay[j], g_BounceBullet_data[i].g_BulletPathWay[j + 1]);
					}*/
				}
			}
			if (g_CleanerSweapEnable == true)
			{
				Sprite_Draw_Rotate(g_CleanerSweapAttack_TexId, g_CleanerSweap_NowPosition.x - 75.0f / 2 * 1.5f, g_CleanerSweap_NowPosition.y - 200.0f / 2 * 1.5f, 75 * 1.5f, 200 * 1.5f, fmod(g_CleanerSweapRotateAngle, 360.0f), 1.0f);
			}
			//debugDraw();		
		}
		Sprite_Draw_Rotate(g_CleanerSweapAttack_TexId, g_CleanerSweap_NowPosition.x - 75.0f / 2 * 1.5f, g_CleanerSweap_NowPosition.y - 200.0f / 2 * 1.5f, 75 * 1.5f, 200 * 1.5f, fmod(g_CleanerSweapRotateAngle, 360.0f), 1.0f);
		if (GetBossAnger())
		{
			Sprite_Draw(g_Cleaner_Eyes_AimFail_TexID, 725, 40, 100, 100);
		}
		if (GetBossAnger() == false && GetBattleState() == BattleSystemState::BOSS_DESIDE)
		{
			for (int i = 0; i < 3; i++)
			{
				if (g_BounceBullet_data[i].g_bulletEnable)
				{
					Collision_DebugDraw_LINE(g_BounceBullet_data[i].g_NowBulletPosition, g_BounceBullet_data[i].g_NowBulletDestination);
					Collision_DebugDraw_LINE(g_BounceBullet_data[i].g_BulletPathWay[g_BounceBullet_data[i].g_NowDestinationCount], g_BounceBullet_data[i].g_BulletPathWay[clamp(g_BounceBullet_data[i].g_NowDestinationCount + 1, 0, 20)]);

				}
			}
		}
		break;
	case BattleBossState::BOOMBER:
		if (GetBattleState() == BattleSystemState::BOSS_DESIDE)
		{
			SpriteAnim_Draw(g_BattleBoss_BoomberAttack_AnimPlayerID, 570, -24, g_BossCurrentColor);
		}
		else
		{
			SpriteAnim_Draw(g_BattleBoss_Boomber_AnimPlayerID, g_BossNowPosition.x, g_BossNowPosition.y, g_BossCurrentColor);

		}
		break;
	default:
		break;
	}

	//debugDraw();
#if defined(DEBUG)||defined(_DEBUG)
	Collision_DebugDraw_Box(Get_BoomberBoxCollision());
#endif
}
void DrawEyeMiniGame_Timer()
{
	Sprite_Draw_Y_Percentage(g_EyesCountDown_TexId, 530, 50, 490, 18, g_MoveEyeMiniGameTimer / EyesTimer);
}
Box Get_CleanerBoxCollision()
{
	float cx = 455 / 2.0f + 550;
	float cy = 455 / 2.0f + 100;

	return { {cx,cy},100,455 / 2.0f };
}
Box Get_BoomberBoxCollision()
{
	float cx = 405 / 2.0f + 588;
	float cy = 531 / 2.0f + 13;

	return { {cx,cy},100,531 / 2.0f };
}

void bullet_boss_Collision_Update()
{
	switch (GetBattleBoss())
	{
	case BattleBossState::CLEANER:
		for (int bi = 0; bi < BULLET_MAX; bi++)
		{
			if (!Bullet_IsEnable(bi))continue;
			if (OnBoxCollisionEnter(Bullet_GetCollision_Box(bi), Get_CleanerBoxCollision()))
			{
				//HIT!
				g_BossFlashTrigger = true;
				g_BossFlashTimer = 0.15f;
				g_BossShaker.Start(0.35, 10, { 550,100 });
				HitCleaner();
				Bullet_Destroy(bi);
			}
		}
		break;
	case BattleBossState::BOOMBER:
		for (int bi = 0; bi < BULLET_MAX; bi++)
		{
			if (!Bullet_IsEnable(bi))continue;
			if (OnBoxCollisionEnter(Bullet_GetCollision_Box(bi), Get_BoomberBoxCollision()))
			{
				//HIT!
				g_BossFlashTrigger = true;
				g_BossFlashTimer = 0.15f;
				g_BossShaker.Start(0.35, 10, { 588,13 });
				HitBoomber();
				Bullet_Destroy(bi);
			}
		}
		break;
		break;
	}

}
bool GetBossAnger()
{
	return g_BossAnger;
}
void SetBossAnger(bool set)
{
	g_BossAnger = set;
}
void BossAttack_CreateBullet(int slot, float speed)
{
	BulletShooted++;
	g_BounceBullet_data[slot].g_bulletEnable = true;
	g_BounceBullet_data[slot].g_bulletSpeed = speed;
	shootPoint sp;
	sp = GetRandomShootPoint();
	CleanerBulletShooter(g_BounceBullet_data[slot].g_BulletPathWay, sp.shootPoint, sp.direction);
	XMFLOAT2 effect;
	switch (sp.segmentCount)
	{
	case 0:
		Effect_Creater({ sp.shootPoint }, 1);
		break;
	case 1:
		effect = { sp.shootPoint.x - 150.0f,sp.shootPoint.y };
		Effect_Creater(effect, 1);
		break;
	case 2:
		effect = { sp.shootPoint.x,sp.shootPoint.y - 150.0f };
		Effect_Creater(effect, 1);
		break;
	case 3:
		effect = { sp.shootPoint.x,sp.shootPoint.y };
		Effect_Creater(effect, 1);
		break;
	default:
		break;
	}
	g_BounceBullet_data[slot].g_NowBulletPosition = g_BounceBullet_data[slot].g_BulletPathWay[0];
	g_BounceBullet_data[slot].g_NowBulletDestination = g_BounceBullet_data[slot].g_BulletPathWay[1];
	g_BounceBullet_data[slot].g_NowDestinationCount = 1;
	g_StartBulletCountDown = true;
}
void BossAttack_CreateSweap()
{
	BulletShooted++;
	g_CleanerSweapEnable = true;
	CleanerSweapShooter(g_SweapPathWay, { 1500,50 }, { -1,1 });
	/*XMFLOAT2 effect;
	switch (sp.segmentCount)
	{
	case 0:
		Effect_Creater({ sp.shootPoint }, 1);
		break;
	case 1:
		effect = { sp.shootPoint.x - 150.0f,sp.shootPoint.y };
		Effect_Creater(effect, 1);
		break;
	case 2:
		effect = { sp.shootPoint.x,sp.shootPoint.y - 150.0f };
		Effect_Creater(effect, 1);
		break;
	case 3:
		effect = { sp.shootPoint.x,sp.shootPoint.y };
		Effect_Creater(effect, 1);
		break;
	default:
		break;
	}*/
	g_CleanerSweap_NowPosition = g_SweapPathWay[0];
	g_CleanerSweap_NowDestination = g_SweapPathWay[1];
	g_CleanerSweap_NowDestinationCount = 1;
	g_StartBulletCountDown = true;
}
Circle GetCleanerBulletCollider(int slot)
{
	float cx = g_BounceBullet_data[slot].g_NowBulletPosition.x;
	float cy = g_BounceBullet_data[slot].g_NowBulletPosition.y;



	return { {cx,cy}, 75.0f / 2.0f };
}
Circle GetSweapCollider()
{
	float cx = g_CleanerSweap_NowPosition.x;
	float cy = g_CleanerSweap_NowPosition.y;



	return { {cx,cy}, 200.0f / 2.0f * 1.5f };
}
bool Get_Cleaner_Bullet_Enable(int slot)
{
	return g_BounceBullet_data[slot].g_bulletEnable;
}
bool Get_CleanerSweapEnable()
{
	return g_CleanerSweapEnable;
}
void Bullet_Back()
{
	BulletShooted--;
	if (BulletShooted == 0)
	{
		Battle_State_ChangeState(BattleSystemState::PLAYER_MOVE);
		g_StartBulletMovement = false;
	}
}
int GetBulletCount(int index)
{
	return g_BounceBullet_data[index].g_NowDestinationCount;
}
void CleanerDeadAnimationStart()
{
	g_BossDeadAnimation.Start(0.55f);
}
