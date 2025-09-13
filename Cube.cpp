#include "Cube.h"
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "shader.h"
#include "Shader3D.h"
using namespace DirectX;


#pragma region 宣告
static constexpr int NUM_VERTEX = 36; // 頂点数


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
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
	XMFLOAT4 color;
};

static Vertex3D g_CubeVertex[36]
{
	{{-0.5f, 0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{ 0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{-0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{-0.5f, 0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{ 0.5f, 0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{ 0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f}},

	{{ 0.5f, 0.5f,-0.5f},{0.0f,1.0f,0.0f,1.0f}},
	{{ 0.5f,-0.5f, 0.5f},{0.0f,1.0f,0.0f,1.0f}},
	{{ 0.5f,-0.5f,-0.5f},{0.0f,1.0f,0.0f,1.0f}},
	{{ 0.5f, 0.5f,-0.5f},{0.0f,1.0f,0.0f,1.0f}},
	{{ 0.5f, 0.5f, 0.5f},{0.0f,1.0f,0.0f,1.0f}},
	{{ 0.5f,-0.5f, 0.5f},{0.0f,1.0f,0.0f,1.0f}},

	{{-0.5f, 0.5f, 0.5f},{0.5f,0.3f,0.0f,1.0f}},
	{{ 0.5f, 0.5f,-0.5f},{0.5f,0.3f,0.0f,1.0f}},
	{{-0.5f, 0.5f,-0.5f},{0.5f,0.3f,0.0f,1.0f}},
	{{-0.5f, 0.5f, 0.5f},{0.5f,0.3f,0.0f,1.0f}},
	{{ 0.5f, 0.5f, 0.5f},{0.5f,0.3f,0.0f,1.0f}},
	{{ 0.5f, 0.5f,-0.5f},{0.5f,0.3f,0.0f,1.0f}},

	{{-0.5f, 0.5f, 0.5f},{1.0f,1.0f,0.2f,1.0f}},
	{{-0.5f,-0.5f,-0.5f},{1.0f,1.0f,0.2f,1.0f}},
	{{-0.5f,-0.5f, 0.5f},{1.0f,1.0f,0.2f,1.0f}},
	{{-0.5f, 0.5f, 0.5f},{1.0f,1.0f,0.2f,1.0f}},
	{{-0.5f, 0.5f,-0.5f},{1.0f,1.0f,0.2f,1.0f}},
	{{-0.5f,-0.5f,-0.5f},{1.0f,1.0f,0.2f,1.0f}},

	{{ 0.5f, 0.5f, 0.5f},{1.0f,0.0f,1.0f,1.0f}},
	{{-0.5f,-0.5f, 0.5f},{1.0f,0.0f,1.0f,1.0f}},
	{{ 0.5f,-0.5f, 0.5f},{1.0f,0.0f,1.0f,1.0f}},
	{{ 0.5f, 0.5f, 0.5f},{1.0f,0.0f,1.0f,1.0f}},
	{{-0.5f, 0.5f, 0.5f},{1.0f,0.0f,1.0f,1.0f}},
	{{-0.5f,-0.5f, 0.5f},{1.0f,0.0f,1.0f,1.0f}},

	{{-0.5f,-0.5f,-0.5f},{1.0f,1.0f,1.0f,1.0f}},
	{{ 0.5f,-0.5f, 0.5f},{1.0f,1.0f,1.0f,1.0f}},
	{{-0.5f,-0.5f, 0.5f},{1.0f,1.0f,1.0f,1.0f}},
	{{-0.5f,-0.5f,-0.5f},{1.0f,1.0f,1.0f,1.0f}},
	{{ 0.5f,-0.5f,-0.5f},{1.0f,1.0f,1.0f,1.0f}},
	{{ 0.5f,-0.5f, 0.5f},{1.0f,1.0f,1.0f,1.0f}},
};
#pragma endregion

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
}

void Cube_Finitialize()
{
	SAFE_RELEASE(g_pVertexBuffer);
}

void Cube_Update(double elapsed_time)
{
	g_time += elapsed_time;

	g_x = sin(g_time * 4.5);
	g_rotate = g_time * 3.0f;
	g_scale = g_time * 0.1f;
}

void Cube_Draw(XMFLOAT3 gameobjectPos)
{
	Shader3D_Begin();
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);



	//world matrix
	//XMMATRIX mtxWorld = XMMatrixIdentity();
	XMMATRIX mtxTrans = XMMatrixTranslation(gameobjectPos.x, gameobjectPos.y, gameobjectPos.z);
	XMMATRIX mtxRot = XMMatrixRotationY(g_rotate);
	XMMATRIX mtxScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX mtxWorld = mtxTrans * mtxRot * mtxScale;
	Shader3D_SetWorldMatrix(mtxWorld);



	// プリミティブトポロジ設定
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ポリゴン描画命令発行
	g_pContext->Draw(36, 0); //TO DELETE

}
