#ifndef BATTLEBOSSBOOMBER_H
#define BATTLEBOSSBOOMBER_H
#include "DirectXMath.h"
#include "Collision.h"

extern int BOMBLINERCOUNT;

void BoomberBossInitialize();
void BoomberBossFinitialize();
void BoomberBossUpdate(float time);
void GenerateMine(float delay, float countdown, DirectX::XMFLOAT2 pos);
void GenerateMineAttack(int round);
void Generate_BombLiner();
void DrawBombLine();
void BombLinerUpdate(float time);
void BombLinerDestroyed(int index);
void GenerateBoomCollider(float life, DirectX::XMFLOAT2 pos);
void GenerateBoomColliderBig(float life, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 half);
void GenerateBoomColliderSmall(float life, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 half);
void GenerateBoomColliderSmall_2(float life, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 half, bool through);
int GetBoomberHealth();
Box GetExplosionCollision(int index);
Box GetBombLinerMidPointCollision(int index);
bool GetExplosionColliderEnable(int index);
void HitBoomber();
void CheatBoomberHealth();
void DrawDebugMissileLine();
void StartMissile();
void UpdateMissile(float time);
void DrawMissiles();
void StartMissilePack();
void TryStartRocket(DirectX::XMFLOAT2 dir, DirectX::XMFLOAT2 pos, float a_speed, float nowAngle, bool through);
void StartRocket16(bool through , DirectX::XMFLOAT2 start);
enum class RocketFlowerType
{
	BASIC,
	BR_BETWEEN,
};
void StartRocketFlower(bool through, RocketFlowerType type,DirectX::XMFLOAT2 start, int count);
void StartRocketFullLuncher(int key);
void UpdateRocket(float time);
void DrawRocket();
#endif
