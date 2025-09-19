#include "MouseRenderer.h"
#include "Texture.h"
#include "mouse.h"
#include "sprite.h"

static int g_cursorRenderTexId = -1;
static Mouse_State g_mouseState;
void MouseRenderer_Initialize()
{
	g_cursorRenderTexId = Texture_Load(L"CatCursor.png");
}

void MouseRenderer_Finitialize()
{
}

void MouseRenderer_Update(double elapsed_time)
{
	Mouse_GetState(&g_mouseState);
}
Mouse_State Get_Mouse_Info()
{
	return g_mouseState;
}
void MouseRenderer_Draw()
{
	Sprite_Draw_UV_UI(g_cursorRenderTexId, g_mouseState.x, g_mouseState.y, 32.0f, 32.0f, 1.0f, 1.0f, 0, 0);
#if defined(DEBUG)||defined(_DEBUG)
	//UICollision_DebugDraw(MouseRenderer_GetCollision(), { 1.0,1.0,1.0,1.0 });
	//UICollision_DebugDraw(MouseRenderer_GetBoxCollision());
#endif
}

