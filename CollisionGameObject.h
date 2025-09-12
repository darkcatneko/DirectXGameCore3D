#ifndef COLLISIONGAMEOBJECT_H
#define COLLISIONGAMEOBJECT_H
#include <DirectXMath.h>
#include "d3d11.h"
#include "MapCollisionInteraction.h"

static constexpr unsigned int COLLIDER_MAX = 5;

void CollisionGameObject_Initialize();
void CollisionGameObject_Finalize();
void CollisionGameObject_Update(double time);
void CollisionGameObject_Draw();

void CollisionGameObjectInteraction();

GameobjectColliderBox CollisionGameObject_GetGameobjectCollision(int scene, int index);
int GetCollisionObjectSceneCount(int scene);
#endif
