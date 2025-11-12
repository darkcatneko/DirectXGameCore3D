#ifndef MAP_H
#define MAP_H
#include "DirectXMath.h"
#include "Collision3D.h"

struct MapObject
{
	int KindId;
	DirectX::XMFLOAT3 Position;
	AABB Collision;
};

void Map_Initialize();
void Map_Finitialize();
//void Map_Update(double elapsed_time);
void Map_Draw();
int Map_GetObjectsCount();
const MapObject* Map_GetObjects(int index);

#endif
