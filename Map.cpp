#include "Map.h"
#include "Texture.h"
#include "sprite.h"
#include <DirectXMath.h>
using namespace DirectX;

static const constexpr unsigned int MAPCHIP_WIDTH = 64;
static const constexpr unsigned int MAPCHIP_HEIGHT = 64;

static int g_MapChipTexId = -1;

static void mapChipDraw(int mapChipId, int mx, int my, XMFLOAT2& offset);

void Map_Initialize()
{
	//g_MapChipTexId = Texture_Load(L"kokosozai.png");
}

void Map_Finitialize()
{
}

void Map_Draw()
{
}

void mapChipDraw(int mapChipId, int mx, int my, XMFLOAT2& offset)
{
	float px = (float)(mx * MAPCHIP_WIDTH);
	float py = (float)(my * MAPCHIP_HEIGHT);

	Sprite_Draw_UV(g_MapChipTexId, px, py, 32, 32, 32.0f / 512.0f, 32.0f / 512.0f, 0, 5);
}
