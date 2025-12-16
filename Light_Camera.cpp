#include "Light_Camera.h"
#include "DirectXMath.h"

using namespace DirectX;

static XMFLOAT3 g_LightPosition = {};
static XMFLOAT3 g_LightFront = {0.0f,1.0f,0.0f};

void LightCamera_Initialize(const DirectX::XMFLOAT3 world_directional, const DirectX::XMFLOAT3 position)
{
	g_LightFront = world_directional;
	g_LightPosition = position;
}

void LightCamera_Finalize()
{
}

void LightCamera_SetPosition(DirectX::XMFLOAT3 position)
{
	g_LightPosition = position;
}

void LightCamera_SetFront(DirectX::XMFLOAT3 front)
{
	g_LightFront = front;
}

const DirectX::XMFLOAT4X4& LightCamera_GetViewMatrix()
{
	XMFLOAT4X4 mtxView{};
	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&g_LightPosition), XMVECTOR{ 0.0f,-1.0f,0.0f }, XMLoadFloat3(&g_LightFront));
	XMStoreFloat4x4(&mtxView, view);
	return mtxView;
}

const DirectX::XMFLOAT4X4& LightCamera_GetProjectionMatrix()
{
	XMFLOAT4X4 mtxProj{};

	float value = 8;
	XMMATRIX proj = XMMatrixOrthographicOffCenterLH(-value, value, -value, value, 0.1f, 1000.0f);
	XMStoreFloat4x4(&mtxProj, proj);
	return mtxProj;
}
