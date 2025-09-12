#ifndef TRIGGERCOLLIDER_H
#define TRIGGERCOLLIDER_H
#include "DirectXMath.h"
#include "MapCollisionInteraction.h"

//constexpr int MAX_TRIGGER_COLLIDER = 2;
enum class TriggerColliderType
{
	DIALOGUE,
	SCENE_CHANGER,
	DRAW, 
	INDICATOR,
};

void TriggerCollider_Initialize();
void TriggerCollider_Finalize();
void TriggerCollider_Update(double time);
void TriggerCollider_Draw();

void  TriggerColliderInteraction();
void SetTriggerColliderActive(int scene, int index, bool active);

GameobjectColliderBox TriggerCollider_GetCollision(int index);

int checkTriggerHintBlock();
bool check_Scene_Trigger_Stay(int scene, int index);
void RefreshTrigger();
#endif
