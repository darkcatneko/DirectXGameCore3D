#ifndef BATTLEBOSSCONTROLLER_H
#define BATTLEBOSSCONTROLLER_H
#include "DirectXMath.h"
#include "Collision.h"

float Distance(const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b);
inline float AngleToTargetDeg(const DirectX::XMFLOAT2& from, const DirectX::XMFLOAT2& to);
void BattleBossController_Initialize();
void BattleBoss_BomberInit();
void BattleBossController_Finitialize();
void BattleBossControllerUpdate(float time);
void BattleBossControllerDraw();
Box Get_CleanerBoxCollision();
Box Get_BoomberBoxCollision();
void bullet_boss_Collision_Update();
void BattleBossEyeMiniGameTimerUpdate(float time);
void BattleBossEyeMiniGameTimerReset();
void DrawEyeMiniGame_Timer();
bool GetBossAnger();
void SetBossAnger(bool set);
void BossAttack_CreateBullet(int slot, float speed);
void BossAttack_CreateSweap();
Circle GetCleanerBulletCollider(int slot);
Circle GetSweapCollider();
bool Get_Cleaner_Bullet_Enable(int slot);
bool Get_CleanerSweapEnable();
int GetBulletCount(int index);
void CleanerDeadAnimationStart();
#endif