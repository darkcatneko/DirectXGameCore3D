#include "UIInteraction.h"
#include "direct3d.h"
#include "d3d11.h"
#include "DirectXTex.h"
#include "Texture.h"
#include "shader.h"
#include "Camera.h"
using namespace DirectX;

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11ShaderResourceView* g_pTexture = nullptr;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

//static Vertex test[NUM_VERTEX];

static int g_numVertex = 5000;

static int debugUsed_Texid = -1;
// 頂点構造体
struct Vertex
{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT4 color;
	XMFLOAT2 uv;
};
bool OnUICircleCollisionEnter(const UICircle& a, const UICircle& b)
{
	float x1 = b.center.x - a.center.x;
	float y1 = b.center.y - a.center.y;



	return (a.radius * a.radius + b.radius * b.radius) > (x1 * x1 + y1 * y1);
}

bool OnUIBoxCollisionEnter(const UIBox& a, const UIBox& b)
{
	float at = a.center.y - a.half_height;//a top
	float ab = a.center.y + a.half_height;//a bottom
	float al = a.center.x - a.half_width; //a left
	float ar = a.center.x + a.half_width; //a right
	float bt = b.center.y - b.half_height;//b top
	float bb = b.center.y + b.half_height;//b bottom
	float bl = b.center.x - b.half_width; //b left
	float br = b.center.x + b.half_width; //b right
	return al<br && ar>bl && at<bb && ab>bt;
}
void UICollision_Debug_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	debugUsed_Texid = Texture_Load(L"white.png");
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();


	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

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

void UICollision_Debug_Finitialize()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pTexture);
}

void UICollision_DebugDraw(const UICircle& circle, DirectX::XMFLOAT4 color)
{
	int numVertex = (int)(circle.radius * 2.0f * XM_PI + 1);
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

	for (int i = 0; i < g_numVertex; i++)
	{
		v[i].position.x = cosf(rad * i) * circle.radius + circle.center.x;
		v[i].position.y = sinf(rad * i) * circle.radius + +circle.center.y;
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

	Shader_SetWorldMatrix(XMMatrixIdentity());
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));


	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	Texture_SetTexture(debugUsed_Texid);
	// ポリゴン描画命令発行
	g_pContext->Draw(g_numVertex, 0);
}

void UICollision_DebugDraw(const UIBox& box)
{
	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	v[0].position = { box.center.x - box.half_width,box.center.y - box.half_height,0.0f };
	v[1].position = { box.center.x + box.half_width,box.center.y - box.half_height,0.0f };
	v[2].position = { box.center.x + box.half_width,box.center.y + box.half_height,0.0f };
	v[3].position = { box.center.x - box.half_width,box.center.y + box.half_height,0.0f };
	v[4].position = { box.center.x - box.half_width,box.center.y - box.half_height,0.0f };

	for (int i = 0; i < 5; ++i) {

		v[i].color = { 1.0f,1.0f,1.0f,1.0f };
		v[i].uv = { 0.0f,0.0f };
	}

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	Shader_SetWorldMatrix(XMMatrixIdentity());


	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	Texture_SetTexture(debugUsed_Texid); // 白色のテクスチャを設定;

	g_pContext->Draw(5, 0);
}