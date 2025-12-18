#include "Grid.h"
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "shader.h"
#include "shader3d_unlit.h"
#include "Texture.h"
using namespace DirectX;

#pragma region 宣告
static constexpr int GRID_H_COUNT = 100;
static constexpr int GRID_V_COUNT = 100;
static constexpr int GRID_H_LINE_COUNT = GRID_H_COUNT + 1;
static constexpr int GRID_V_LINE_COUNT = GRID_V_COUNT + 1;
static constexpr int NUM_VERTEX = GRID_H_LINE_COUNT * 2 + GRID_V_LINE_COUNT * 2; // 頂点数


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11Buffer* g_pDebugRayVB = nullptr;
static ID3D11ShaderResourceView* g_pTexture = nullptr;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;


static int whiteTex = -1;
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
	float x = -5.0f * 10;
	for (int i = 0; i < GRID_H_LINE_COUNT * 2; i += 2)
	{
		g_GridVertex[i] = { {x,0.0f, 5.0f * 10} ,{0.0f,1.0f,0.0f,1.0f} };
		g_GridVertex[i + 1] = { {x,0.0f,-5.0f * 10} ,{0.0f,1.0f,0.0f,1.0f} };
		x += 1;
	}
	float z = -5.0f * 10;
	for (int i = GRID_V_LINE_COUNT * 2; i < NUM_VERTEX; i += 2)
	{
		g_GridVertex[i] = { { 5.0f * 10,0.0f,z} ,{0.0f,1.0f,0.0f,1.0f} };
		g_GridVertex[i + 1] = { {-5.0f * 10,0.0f,z} ,{0.0f,1.0f,0.0f,1.0f} };
		z += 1;
	}
	g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex3D) * 2;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&bd, nullptr, &g_pDebugRayVB);
	whiteTex = Texture_Load(L"white.png");
}

void Grid_Finitialize()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pDebugRayVB);
}

void Grid_Update(double elapsed_time)
{
}

void Grid_Draw(DirectX::XMFLOAT3 gameobjectPos, DirectX::XMFLOAT3 gameobjectRot, DirectX::XMFLOAT3 gameobjectScale)
{
	Shader3dUnlit_Begin();
	Shader3DUnilt_SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	//world matrix
	XMMATRIX mtxTrans = XMMatrixTranslation(gameobjectPos.x-0.5f, gameobjectPos.y-0.5f, gameobjectPos.z-0.5f);
	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(gameobjectRot.x),   // X
		XMConvertToRadians(gameobjectRot.y),   // Y
		XMConvertToRadians(gameobjectRot.z)    // Z
	);
	XMMATRIX mtxScale = XMMatrixScaling(
		gameobjectScale.x,
		gameobjectScale.y,
		gameobjectScale.z
	);
	XMMATRIX mtxWorld = mtxScale * mtxRot * mtxTrans;
	//XMMATRIX mtxWorld = XMMatrixIdentity();
	Shader3DUnilt_SetWorldMatrix(mtxWorld);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0); //TO DELETE
}
void Grid_DebugDrawRay(
	const XMFLOAT3& origin,
	const XMFLOAT3& dir,
	float length,
	XMFLOAT4 color
)
{
	Vertex3D v[2];

	v[0].position = origin;
	v[0].color = { 1,1,1,1 }; // 紅色起點

	v[1].position =
	{
		origin.x + dir.x * length,
		origin.y + dir.y * length,
		origin.z + dir.z * length
	};
	v[1].color = { 1,1,1,1 };

	// 更新 VertexBuffer
	D3D11_MAPPED_SUBRESOURCE mapped{};
	g_pContext->Map(g_pDebugRayVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, v, sizeof(v));
	g_pContext->Unmap(g_pDebugRayVB, 0);

	// Draw
	Shader3dUnlit_Begin();
	Shader3DUnilt_SetColor(color);
	Texture_SetTexture(whiteTex, 0);

	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pDebugRayVB, &stride, &offset);
	g_pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	// World = Identity（因為 v 已經是 world space）
	Shader3DUnilt_SetWorldMatrix(XMMatrixIdentity());

	g_pContext->Draw(2, 0);
}