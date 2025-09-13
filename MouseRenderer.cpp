#include "MouseRenderer.h"
#include "Texture.h"
#include "mouse.h"
#include "sprite.h"
#include "UIInteraction.h"

static int g_cursorRenderTexId = -1;
static Mouse_State g_mouseState;
static UICircle g_MouseUICollision{ {16.0f,16.0f},32.0f };
static UIBox g_MouseUIBoxCollision{ {16.0f,16.0f},16.0f,16.0f };
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

UICircle MouseRenderer_GetCollision()
{
	float cx = g_MouseUICollision.center.x + g_mouseState.x;
	float cy = g_MouseUICollision.center.y + g_mouseState.y;



	return { {cx,cy},g_MouseUICollision.radius };
}

UIBox  MouseRenderer_GetBoxCollision()
{
	float cx = g_MouseUIBoxCollision.center.x + g_mouseState.x;
	float cy = g_MouseUIBoxCollision.center.y + g_mouseState.y;



	return { {cx,cy},g_MouseUIBoxCollision.half_width,g_MouseUIBoxCollision.half_height };
}
