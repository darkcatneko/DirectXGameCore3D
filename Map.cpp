#include "Map.h"
#include "DirectXMath.h"
#include "Cube.h"
using namespace DirectX; 

static MapObject g_MapObjects[]
{
	{1,{ 0.0f,0.0f,0.0f}},
	{1,{ 1.0f,0.0f,0.0f}},
	{1,{-1.0f,0.0f,0.0f}},
	{1,{ 0.0f,0.0f,1.0f}},
	{1,{ 1.0f,0.0f,1.0f}},
	{1,{-1.0f,0.0f,1.0f}},
	{1,{ 0.0f,0.0f,2.0f}},
	{1,{ 1.0f,0.0f,2.0f}},
	{1,{-1.0f,0.0f,2.0f}},
	{1,{-1.0f,1.5f,2.0f}},
	{1,{0.0f,2.5f,-1.0f}},
};
void Map_Initialize()
{
	for (MapObject& o : g_MapObjects)
	{
		o.Collision = Cube_GetAABB(o.Position);
	}
}

void Map_Finitialize()
{
}

void Map_Draw()
{
	for (const MapObject& o:g_MapObjects)
	{
		switch (o.KindId)
		{
		case 1:
			Cube_Draw(o.Position);
		default:
			break;
		}
	}
}

int Map_GetObjectsCount()
{
	return sizeof(g_MapObjects)/sizeof(g_MapObjects[0]);
}

const MapObject* Map_GetObjects(int index)
{
	return &g_MapObjects[index];
}
