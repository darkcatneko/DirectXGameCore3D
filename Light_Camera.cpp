#include "Light_Camera.h"
#include "DirectXMath.h"

using namespace DirectX;

static XMFLOAT3 g_LightPosition = {};
static XMFLOAT3 g_LightFront = {0.0f,1.0f,0.0f};
static XMFLOAT4X4 g_LightViewMatrix = {};
static XMFLOAT4X4 g_LightProjectionMatrix = {};

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
	XMVECTOR up = XMLoadFloat3(&g_LightFront);
	if (XMVectorGetX(XMVector3LengthSq(up)) < 0.0001f)
	{
		up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	}
	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&g_LightPosition), XMVECTOR{ 0.0f,-1.0f,0.0f }, up);
	XMStoreFloat4x4(&g_LightViewMatrix, view);
	return g_LightViewMatrix;
}

const DirectX::XMFLOAT4X4& LightCamera_GetProjectionMatrix()
{
	float value = 24;
	XMMATRIX proj = XMMatrixOrthographicOffCenterLH(-value, value, -value, value, 0.1f, 1000.0f);
	XMStoreFloat4x4(&g_LightProjectionMatrix, proj);
	return g_LightProjectionMatrix;
}
