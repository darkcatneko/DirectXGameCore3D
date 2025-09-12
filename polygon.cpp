/*==============================================================================

   ポリゴン描画 [polygon.cpp]
														 Author : Youhei Sato
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "direct3d.h"
#include "shader.h"
#include "debug_ostream.h"
#include "DirectXTex.h"
#include "polygon.h"
#include "sprite.h"
#include "Camera.h"


#pragma coment(lib,"d3d11.lib")

#if defined(DEBUG)||defined(_DEBUG)
#pragma comment(lib,"DirectXTex_Debug.lib")
#else
#pragma comment(lib,"DirectXTex_Release.lib")
#endif


//static constexpr int NUM_VERTEX = 2000; // 頂点数


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11ShaderResourceView* g_pTexture = nullptr;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

//static Vertex test[NUM_VERTEX];

static int g_numVertex = 0;
static float g_Radius = 500.0f;
static float g_Cx = 500.0f;
static float g_Cy = 500.0f;



void Polygon_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

	// デバイスとデバイスコンテキストのチェック
	if (!pDevice || !pContext) {
		hal::dout << "Polygon_Initialize() : 与えられたデバイスかコンテキストが不正です" << std::endl;
		return;
	}

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;
	g_numVertex = g_Radius * 2.0f * XM_PI + 1;
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * g_numVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);

	TexMetadata metadata;
	ScratchImage image;

	LoadFromWICFile(L"resource/DOT.png", WIC_FLAGS_NONE, &metadata, image);
	HRESULT hr = CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, &g_pTexture);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "NO FILE", "ERROR", MB_OK);

	}
}

void Polygon_Finalize(void)
{
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(g_pVertexBuffer);
}

void Polygon_Draw(void)
{


	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	// 頂点情報を書き込み
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

	// 画面の左上から右下に向かう線分を描画する
	const float rad = XM_2PI / (float)g_numVertex;

	//v[0].position = { g_Cx, g_Cy, 0 };
	//v[0].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	//v[0].uv = { 1.0f,1.0f };

	for (int i = 0; i < g_numVertex; i++)
	{
		v[i].position.x = cosf(rad * i) * g_Radius + g_Cx;
		v[i].position.y = sinf(rad * i) * g_Radius + g_Cy;
		v[i].position.z = 0.0f;
		v[i].color = { 1.0f,1.0f, 1.0f, 1.0f };
		v[i].uv = { 0.0f,0.0f };
	}
	/*v[0].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[1].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[2].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[3].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[0].uv = { 0.0f,0.0f };
	v[1].uv = { 1.0f,0.0f };
	v[2].uv = { 0.0f,1.0f };
	v[3].uv = { 1.0f,1.0f };*/

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);
	XMMATRIX camera = DirectX::XMMatrixTranslation(getClampedCameraPos().x, getClampedCameraPos().y, 0.0f);
	Shader_SetWorldMatrix(camera);
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));


	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);


	// ポリゴン描画命令発行
	g_pContext->Draw(g_numVertex, 0);
}
void Polygon_Draw_time(int time, XMFLOAT2 vector2)
{
	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	// 頂点情報を書き込み
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

	// 画面の左上から右下に向かう線分を描画する
	/*v[0].position = { SCREEN_WIDTH * .3f + time * vector2.x , SCREEN_HEIGHT * .3f + time * vector2.y, 0.0f };
	v[1].position = { SCREEN_WIDTH * .7f + time * vector2.x , SCREEN_HEIGHT * .3f + time * vector2.y, 0.0f };
	v[2].position = { SCREEN_WIDTH * .3f + time * vector2.x , SCREEN_HEIGHT * .7f + time * vector2.y, 0.0f };
	v[3].position = { SCREEN_WIDTH * .7f + time * vector2.x , SCREEN_HEIGHT * .7f + time * vector2.y, 0.0f };*/



	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	g_pContext->PSSetShaderResources(0, 1, &g_pTexture);

	//三角形
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//連線
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//POINT render
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// ポリゴン描画命令発行
	//g_pContext->Draw(NUM_VERTEX, 0);
}
