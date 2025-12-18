#ifndef MAP_H
#define MAP_H
#include "DirectXMath.h"
#include "Collision3D.h"


struct MapObject
{
	int KindId = -1;
	DirectX::XMFLOAT3 Position;
	AABB Collision;
	bool IsTriggered = false;
};

void Map_Initialize();
void Map_Finitialize();
void Map_Update(double elapsed_time);
void Map_Draw();
int Map_GetObjectsCount();
const MapObject* Map_GetObjects(int index);
void SaveMap(const char* filename);
void LoadMap(const char* filename);
#endif
