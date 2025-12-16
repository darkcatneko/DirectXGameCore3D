#include "MouseRenderer.h"
#include "Texture.h"
#include "mouse.h"
#include "sprite.h"
#include "direct3d.h"
#include "Camera3D.h"
#include "Cube.h"
#include "Key_Logger.h"

static int g_cursorRenderTexId = -1;
static Mouse_State g_mouseState;

static int mouse_y = 0;
XMFLOAT3 vtestf;

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
	if (KeyLogger_IsTrigger(KK_Y))
	{
		mouse_y++;
	}
	if (KeyLogger_IsTrigger(KK_U))
	{
		mouse_y--;
	}
	XMFLOAT4X4 mtxview = Camera_GetMatrix();
	XMFLOAT3 test_near = Direct3D_ScreenToWorld(g_mouseState.x, g_mouseState.y, 0.0, mtxview, Camera_GetMatrixPerspective());
	XMFLOAT3 test_far = Direct3D_ScreenToWorld(g_mouseState.x, g_mouseState.y, 1.0, mtxview, Camera_GetMatrixPerspective());

	XMVECTOR vtest = XMLoadFloat3(&test_far) - XMLoadFloat3(&test_near);
	vtest = XMVector3Normalize(vtest);

	float planeY = mouse_y;
	float ratio = (planeY - XMVectorGetY(XMLoadFloat3(&test_near)))
		/ XMVectorGetY(vtest);
	vtest = XMLoadFloat3(&test_near) + vtest * ratio;
	XMMATRIX vmax = XMMatrixTranslationFromVector(vtest);
	
	XMStoreFloat3(&vtestf, vmax.r[3]);
}
Mouse_State Get_Mouse_Info()
{
	return g_mouseState;
}
void MouseRenderer_Draw()
{
	
	//vtestf.y = mouse_y;
	//Cube_Draw(vtestf);
	

	Sprite_Draw_UV_UI(g_cursorRenderTexId, g_mouseState.x, g_mouseState.y, 32.0f, 32.0f, 1.0f, 1.0f, 0, 0);
#if defined(DEBUG)||defined(_DEBUG)
	//UICollision_DebugDraw(MouseRenderer_GetCollision(), { 1.0,1.0,1.0,1.0 });
	//UICollision_DebugDraw(MouseRenderer_GetBoxCollision());
#endif
}

DirectX::XMFLOAT3 GetMouseToMapLocation()
{
	return vtestf;
}



