#include "Grid.h"
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "shader.h"
#include "Shader3D.h"
using namespace DirectX;

#pragma region 宣告
static constexpr int GRID_H_COUNT = 10;
static constexpr int GRID_V_COUNT = 10;
static constexpr int GRID_H_LINE_COUNT = GRID_H_COUNT+1;
static constexpr int GRID_V_LINE_COUNT = GRID_V_COUNT + 1;
static constexpr int NUM_VERTEX = GRID_H_LINE_COUNT*2+ GRID_V_LINE_COUNT * 2; // 頂点数


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11ShaderResourceView* g_pTexture = nullptr;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;
#pragma endregion

#pragma region Vertex

struct  Vertex3D
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};

static Vertex3D g_GridVertex[NUM_VERTEX]
{
	{{-0.5f, 0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{ 0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{-0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{-0.5f, 0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{ 0.5f, 0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{ 0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f}},
};
#pragma endregion

void Grid_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex3D) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = g_GridVertex;
	float x = -5.0f;
	for (int i = 0; i < GRID_H_LINE_COUNT *2 ; i+=2)
	{
		g_GridVertex[i  ] = { {x,0.0f, 5.0f} ,{0.0f,1.0f,0.0f,1.0f} };
		g_GridVertex[i+1] = { {x,0.0f,-5.0f} ,{0.0f,1.0f,0.0f,1.0f} };
		x += 1;
	}
	float z = -5.0f;
	for (int i = GRID_V_LINE_COUNT *2; i < NUM_VERTEX; i += 2)
	{
		g_GridVertex[i    ] = { { 5.0f,0.0f,z} ,{0.0f,1.0f,0.0f,1.0f} };
		g_GridVertex[i + 1] = { {-5.0f,0.0f,z} ,{0.0f,1.0f,0.0f,1.0f} };
		z += 1;
	}
	g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);
}

void Grid_Finitialize()
{
	SAFE_RELEASE(g_pVertexBuffer);
}

void Grid_Update(double elapsed_time)
{
}

void Grid_Draw()
{
	Shader3D_Begin();
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	//world matrix
	XMMATRIX mtxWorld = XMMatrixIdentity();
	Shader3D_SetWorldMatrix(mtxWorld);

	//view matrix
	XMMATRIX mtxView = XMMatrixLookAtLH(
		{ 2.0f, 2.0f, -5.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f });
	Shader3D_SetViewMatrix(mtxView);

	//Perspective array
	//NearZ一定要大于0 是距离
	constexpr float fovAngleY = XMConvertToRadians(60.0f);
	float aspectRatio = (float)Direct3D_GetBackBufferWidth() / Direct3D_GetBackBufferHeight();
	float nearZ = 0.1f;
	float farZ = 100.0f;
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);

	Shader3D_SetProjectionMatrix(mtxPerspective);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0); //TO DELETE
}
