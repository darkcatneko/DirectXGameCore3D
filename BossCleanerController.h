#ifndef BOSSCLEANERCONTROLLER_H
#define BOSSCLEANERCONTROLLER_H
#include "DirectXMath.h"
#include "Collision.h"
#include <vector>

struct shootPoint
{
	DirectX::XMFLOAT2 shootPoint;
	int segmentCount;
	DirectX::XMFLOAT2 direction;
};
void CleanerBossInitialize();
inline DirectX::XMFLOAT2 Normalize(const DirectX::XMFLOAT2& v);
void CleanerBulletShooter(std::vector <DirectX::XMFLOAT2 >& target, DirectX::XMFLOAT2 start, DirectX::XMFLOAT2 dir);
void CleanerSweapShooter(std::vector<DirectX::XMFLOAT2>& target, DirectX::XMFLOAT2 start, DirectX::XMFLOAT2 dir);
void GenerateCleanerEyePos();
void debugDraw();
void DrawEYES();
Box GetEyesCollision(int index, int corner);
void EyeUpdate();
bool GetAllEyeCover();
void ResetBossHitCount();
void HitCleaner();
int GetCleanerHealth();
shootPoint GetRandomShootPoint();
inline DirectX::XMFLOAT2 RandomDirectionByMode(int mode);
#endif
