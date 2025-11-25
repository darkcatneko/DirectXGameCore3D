#include "Cube.h"
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "shader.h"
#include "Shader3D_Static.h"
#include "Texture.h"
using namespace DirectX;


#pragma region 宣告
static constexpr int NUM_VERTEX = 24; // 頂点数
static constexpr int NUM_INDEX = 36; // 頂点数

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11Buffer* g_pIndexBuffer = nullptr; // インデックスバッファ
static ID3D11ShaderResourceView* g_pTexture = nullptr;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static float g_x;
static float g_rotate;
static float g_scale;
static double g_time;
#pragma endregion


#pragma region Vertex

struct  Vertex3D
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 texcoord;
};

static Vertex3D g_CubeVertex[24]
{
	//前
	{{-0.5f, 0.5f,-0.5f},{0.0f,0.0f,-1.0f},{1.0f,0.0f,0.0f,0.4f}, {0.0f, 0.0f}},
	{{ 0.5f,-0.5f,-0.5f},{0.0f,0.0f,-1.0f},{1.0f,0.0f,0.0f,0.4f}, {1.0f, 1.0f}},
	{{-0.5f,-0.5f,-0.5f},{0.0f,0.0f,-1.0f},{1.0f,0.0f,0.0f,0.4f}, {0.0f, 1.0f}},
	{{ 0.5f, 0.5f,-0.5f},{0.0f,0.0f,-1.0f},{1.0f,0.0f,0.0f,0.4f}, {1.0f, 0.0f}},
	//右
	{{ 0.5f, 0.5f,-0.5f},{1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f,0.4f}, {0.0f, 0.0f}},
	{{ 0.5f,-0.5f, 0.5f},{1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f,0.4f}, {1.0f, 1.0f}},
	{{ 0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f,0.4f}, {0.0f, 1.0f}},
	{{ 0.5f, 0.5f, 0.5f},{1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f,0.4f}, {1.0f, 0.0f}},
	//上	
	{{-0.5f, 0.5f, 0.5f},{0.0f,1.0f,0.0f},{0.5f,0.3f,0.0f,0.4f}, {0.0f, 0.0f}},
	{{ 0.5f, 0.5f,-0.5f},{0.0f,1.0f,0.0f},{0.5f,0.3f,0.0f,0.4f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f,-0.5f},{0.0f,1.0f,0.0f},{0.5f,0.3f,0.0f,0.4f}, {0.0f, 1.0f}},
	{{ 0.5f, 0.5f, 0.5f},{0.0f,1.0f,0.0f},{0.5f,0.3f,0.0f,0.4f}, {1.0f, 0.0f}},
	//左
	{{-0.5f, 0.5f, 0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,0.2f,0.4f}, {0.0f, 0.0f}},
	{{-0.5f,-0.5f,-0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,0.2f,0.4f}, {1.0f, 1.0f}},
	{{-0.5f,-0.5f, 0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,0.2f,0.4f}, {0.0f, 1.0f}},
	{{-0.5f, 0.5f,-0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,0.2f,0.4f}, {1.0f, 0.0f}},
	//後
	{{ 0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f}, {1.0f,0.0f,1.0f,0.4f}, {0.0f, 0.0f}},
	{{-0.5f,-0.5f, 0.5f},{0.0f,0.0f,1.0f}, {1.0f,0.0f,1.0f,0.4f}, {1.0f, 1.0f}},
	{{ 0.5f,-0.5f, 0.5f},{0.0f,0.0f,1.0f}, {1.0f,0.0f,1.0f,0.4f}, {0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f}, {1.0f,0.0f,1.0f,0.4f}, {1.0f, 0.0f}},
	//下
	{{-0.5f,-0.5f,-0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,0.4f }, {0.0f, 0.0f}},
	{{ 0.5f,-0.5f, 0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,0.4f}, {1.0f, 1.0f}},
	{{-0.5f,-0.5f, 0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,0.4f}, {0.0f, 1.0f}},
	{{ 0.5f,-0.5f,-0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,0.4f}, {1.0f, 0.0f}},

};
//static Vertex3D g_CubeVertex[24] = {
//	// 前面 (Front face)
//	{{-0.5f, -0.5f, -0.5f},{1.0f,0.0f,0.0f,1.0f}, {0.0f, 1.0f}},
//	{{-0.5f,  0.5f, -0.5f},{1.0f,0.0f,0.0f,1.0f}, {0.0f, 0.0f}},
//	{{ 0.5f,  0.5f, -0.5f},{1.0f,0.0f,0.0f,1.0f}, {1.0f, 0.0f}},
//	{{-0.5f, -0.5f, -0.5f},{1.0f,0.0f,0.0f,1.0f}, {0.0f, 1.0f}},
//	{{ 0.5f,  0.5f, -0.5f},{1.0f,0.0f,0.0f,1.0f}, {1.0f, 0.0f}},
//	{{ 0.5f, -0.5f, -0.5f},{1.0f,0.0f,0.0f,1.0f}, {1.0f, 1.0f}},
//	// 背面 (Back face)
//	{{ 0.5f, -0.5f,  0.5f},{0.0f,1.0f,0.0f,1.0f}, {0.0f, 1.0f}},
//	{{ 0.5f,  0.5f,  0.5f},{0.0f,1.0f,0.0f,1.0f}, {0.0f, 0.0f}},
//	{{-0.5f,  0.5f,  0.5f},{0.0f,1.0f,0.0f,1.0f}, {1.0f, 0.0f}},
//	{{ 0.5f, -0.5f,  0.5f},{0.0f,1.0f,0.0f,1.0f}, {0.0f, 1.0f}},
//	{{-0.5f,  0.5f,  0.5f},{0.0f,1.0f,0.0f,1.0f}, {1.0f, 0.0f}},
//	{{-0.5f, -0.5f,  0.5f},{0.0f,1.0f,0.0f,1.0f}, {1.0f, 1.0f}},
//	// 左面 (Left face)
//	{{-0.5f, -0.5f,  0.5f},{0.5f,0.3f,0.0f,1.0f}, {0.0f, 1.0f}},
//	{{-0.5f,  0.5f,  0.5f},{0.5f,0.3f,0.0f,1.0f}, {0.0f, 0.0f}},
//	{{-0.5f,  0.5f, -0.5f},{0.5f,0.3f,0.0f,1.0f}, {1.0f, 0.0f}},
//	{{-0.5f, -0.5f,  0.5f},{0.5f,0.3f,0.0f,1.0f}, {0.0f, 1.0f}},
//	{{-0.5f,  0.5f, -0.5f},{0.5f,0.3f,0.0f,1.0f}, {1.0f, 0.0f}},
//	{{-0.5f, -0.5f, -0.5f},{0.5f,0.3f,0.0f,1.0f}, {1.0f, 1.0f}},
//	// 右面 (Right face)
//	{{ 0.5f, -0.5f, -0.5f},{1.0f,1.0f,0.2f,1.0f}, {0.0f, 1.0f}},
//	{{ 0.5f,  0.5f, -0.5f},{1.0f,1.0f,0.2f,1.0f}, {0.0f, 0.0f}},
//	{{ 0.5f,  0.5f,  0.5f},{1.0f,1.0f,0.2f,1.0f}, {1.0f, 0.0f}},
//	{{ 0.5f, -0.5f, -0.5f},{1.0f,1.0f,0.2f,1.0f}, {0.0f, 1.0f}},
//	{{ 0.5f,  0.5f,  0.5f},{1.0f,1.0f,0.2f,1.0f}, {1.0f, 0.0f}},
//	{{ 0.5f, -0.5f,  0.5f},{1.0f,1.0f,0.2f,1.0f}, {1.0f, 1.0f}},
//	// 上面 (Top face)
//	{{-0.5f,  0.5f, -0.5f},{1.0f,0.0f,1.0f,1.0f}, {0.0f, 1.0f}},
//	{{-0.5f,  0.5f,  0.5f},{1.0f,0.0f,1.0f,1.0f}, {0.0f, 0.0f}},
//	{{ 0.5f,  0.5f,  0.5f},{1.0f,0.0f,1.0f,1.0f}, {1.0f, 0.0f}},
//	{{-0.5f,  0.5f, -0.5f},{1.0f,0.0f,1.0f,1.0f}, {0.0f, 1.0f}},
//	{{ 0.5f,  0.5f,  0.5f},{1.0f,0.0f,1.0f,1.0f}, {1.0f, 0.0f}},
//	{{ 0.5f,  0.5f, -0.5f},{1.0f,0.0f,1.0f,1.0f}, {1.0f, 1.0f}},
//	// 下面 (Bottom face)
//	{{-0.5f, -0.5f,  0.5f},{1.0f,1.0f,1.0f,1.0f}, {0.0f, 1.0f}},
//	{{-0.5f, -0.5f, -0.5f},{1.0f,1.0f,1.0f,1.0f}, {0.0f, 0.0f}},
//	{{ 0.5f, -0.5f, -0.5f},{1.0f,1.0f,1.0f,1.0f}, {1.0f, 0.0f}},
//	{{-0.5f, -0.5f,  0.5f},{1.0f,1.0f,1.0f,1.0f}, {0.0f, 1.0f}},
//	{{ 0.5f, -0.5f, -0.5f},{1.0f,1.0f,1.0f,1.0f}, {1.0f, 0.0f}},
//	{{ 0.5f, -0.5f,  0.5f},{1.0f,1.0f,1.0f,1.0f}, {1.0f, 1.0f}}
//};

#pragma endregion
static int g_CubeTexTempId = -1;

static unsigned short g_cubeIndex[]
{
	0,1,2,0,3,1,
	4,5,6,4,7,5,
	8,9,10,8,11,9,
	12,13,14,12,15,13,
	16,17,18,16,19,17,
	20,21,22,20,23,21
};
void Cube_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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
	sd.pSysMem = g_CubeVertex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

	// インデックスバファ
	bd.ByteWidth = sizeof(unsigned short) * NUM_INDEX;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	sd.pSysMem = g_cubeIndex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pIndexBuffer);

	g_CubeTexTempId = Texture_Load(L"BlackWhite.png");
}

void Cube_Finitialize()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
}

void Cube_Update(double elapsed_time)
{
	g_time += elapsed_time;

	g_x = sin(g_time * 4.5);
	//g_rotate = g_time * 3.0f;
	g_scale = g_time * 0.1f;
}

void Cube_Draw(XMFLOAT3 gameobjectPos)
{
	Shader3D_Static_Begin();
	Shader3d_Static_SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	// 頂点バッファを描画パイプラインに設定
	Texture_SetTexture(g_CubeTexTempId);
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// インデックスバッファを描画パイプラインに設定
	g_pContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	//world matrix
	//XMMATRIX mtxWorld = XMMatrixIdentity();
	XMMATRIX mtxTrans = XMMatrixTranslation(gameobjectPos.x, gameobjectPos.y, gameobjectPos.z);
	XMMATRIX mtxRot = XMMatrixRotationY(g_rotate);
	XMMATRIX mtxScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX mtxWorld = mtxScale * mtxRot * mtxTrans;
	Shader3D_Static_SetWorldMatrix(mtxWorld);


	// プリミティブトポロジ設定
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ポリゴン描画命令発行
	g_pContext->DrawIndexed(NUM_INDEX, 0, 0); //TO DELETE

}

AABB Cube_GetAABB(const DirectX::XMFLOAT3& position)
{
	return {
		{position.x + 0.5f, position.y + 0.5f,position.z + 0.5f},
		{position.x - 0.5f, position.y - 0.5f,position.z - 0.5f}
	};
}
