#include "Light.h"
#include "DirectXMath.h"
#include "d3d11.h"
using namespace DirectX;


static ID3D11Buffer* g_pPSConstantBuffer1 = nullptr;
static ID3D11Buffer* g_pPSConstantBuffer2 = nullptr;
static ID3D11Buffer* g_pPSConstantBuffer3 = nullptr;
static ID3D11Buffer* g_pPSConstantBuffer4 = nullptr;
// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

struct  DirectionalLight
{
	XMFLOAT4 Directional;
	XMFLOAT4 Color;
	

};
struct  SpecularLight
{
	XMFLOAT3 CameraPos;
	float Power = 30.0f;
	XMFLOAT4 Color ;

};

struct  PointLight
{
	XMFLOAT3 LightPosition;
	float Range;
	XMFLOAT4 Color;
};

struct PointLightList
{
	PointLight light[4];
	int count;
	XMFLOAT3 dummy;
};

static PointLightList g_PointLights{};

bool Light_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	g_pDevice = pDevice;
	g_pContext = pContext;
	// 頂点シェーダー用定数バッファの作成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

	buffer_desc.ByteWidth = sizeof(XMFLOAT4); // バッファのサイズ
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer1);

	buffer_desc.ByteWidth = sizeof(DirectionalLight);
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer2);

	buffer_desc.ByteWidth = sizeof(SpecularLight);
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer3);

	buffer_desc.ByteWidth = sizeof(PointLightList);
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer4);
	
	/*PointLightList list
	{
		{
			{{0.0f,2.0f,0.0f},5.0f,{1.0f,0.0f,0.0f,1.0f} },
			{{2.0f,0.0f,0.0f},5.0f,{0.0f,1.0f,0.0f,1.0f} },
			{{0.0f,2.0f,0.0f},5.0f,{1.0f,1.0f,1.0f,1.0f} },
			{{0.0f,2.0f,0.0f},5.0f,{1.0f,1.0f,1.0f,1.0f} },

		},
		2,
	};*/

	
	return true;
}

void Light_Finalize()
{
}

void Light_SetAmbient(const DirectX::XMFLOAT3& color)
{
	// 定数バッファに行列をセット
	g_pContext->UpdateSubresource(g_pPSConstantBuffer1, 0, nullptr, &color, 0, 0);
	g_pContext->PSSetConstantBuffers(1, 1, &g_pPSConstantBuffer1);
}

void Light_SetDirectionalWorld(const DirectX::XMFLOAT4& world_directional, const DirectX::XMFLOAT4& color)
{
	DirectionalLight light{ world_directional,color };
	g_pContext->UpdateSubresource(g_pPSConstantBuffer2, 0, nullptr, &light, 0, 0);
	g_pContext->PSSetConstantBuffers(2, 1, &g_pPSConstantBuffer2);
}

void Light_SetSpecularWorld(const DirectX::XMFLOAT4& color, float power, const DirectX::XMFLOAT3& camearPos)
{
	SpecularLight light{ camearPos,power,color };
	g_pContext->UpdateSubresource(g_pPSConstantBuffer3, 0, nullptr, &light, 0, 0);
	g_pContext->PSSetConstantBuffers(3, 1, &g_pPSConstantBuffer3);
}

void Light_SetPointLight(int n, const DirectX::XMFLOAT3& position, float range, const DirectX::XMFLOAT3& color)
{
	g_PointLights.light[n].LightPosition = position;
	g_PointLights.light[n].Range = range;
	g_PointLights.light[n].Color = { color.x,color.y,color.z ,1.0f};
	g_pContext->UpdateSubresource(g_pPSConstantBuffer4, 0, nullptr, &g_PointLights, 0, 0);
	g_pContext->PSSetConstantBuffers(4, 1, &g_pPSConstantBuffer4);
}

void Light_SetPointLightCount(int count)
{
	g_PointLights.count = count;
	g_pContext->UpdateSubresource(g_pPSConstantBuffer4, 0, nullptr, &g_PointLights, 0, 0);
	g_pContext->PSSetConstantBuffers(4, 1, &g_pPSConstantBuffer4);

}


