/*==============================================================================

   ポリゴン描画 [Sprite.cpp]
														 Author : Youhei Sato
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "direct3d.h"
#include "shader.h"
#include "Shader3D.h"
#include "debug_ostream.h"
#include "sprite_anim.h"
//#include "DirectXTex.h"
#include "sprite.h"
#include "Texture.h"
#include "Camera.h"

#pragma coment(lib,"d3d11.lib")

#if defined(DEBUG)||defined(_DEBUG)
#pragma comment(lib,"DirectXTex_Debug.lib")
#else
#pragma comment(lib,"DirectXTex_Release.lib")
#endif


static constexpr int NUM_VERTEX = 4; // 頂点数


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11ShaderResourceView* g_pTexture = nullptr;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static Vertex test;



static ID3D11Buffer* g_pGammaBuffer = nullptr;

void gamma_function(float gamma)
{
	GammaBuffer gb = { gamma, 1.0f }; // 你要套用的 gamma，例如解碼 gamma 為 1.65
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	g_pContext->Map(g_pGammaBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &gb, sizeof(GammaBuffer));
	g_pContext->Unmap(g_pGammaBuffer, 0);
}
void Sprite_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();
	test.position = { 0.0f , 0.0f, 0.0f };
	// デバイスとデバイスコンテキストのチェック
	if (!pDevice || !pContext) {
		hal::dout << "Sprite_Initialize() : 与えられたデバイスかコンテキストが不正です" << std::endl;
		return;
	}

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(GammaBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&bufferDesc, nullptr, &g_pGammaBuffer);
	/*TexMetadata metadata;
	ScratchImage image;

	LoadFromWICFile(L"cat.png", WIC_FLAGS_NONE, &metadata, image);
	HRESULT hr = CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, &g_pTexture);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "NO FILE", "ERROR", MB_OK);

	}*/
}

void Sprite_Finalize(void)
{
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pGammaBuffer);
}

void Sprite_Begin(void)
{
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));
}

void Sprite_Draw(int textureID, float dx, float dy, float sp_w, float sp_h, float gamma, XMFLOAT4 color)
{

	// シェーダーを描画パイプラインに設定
	Shader_Begin();
	gamma_function(gamma);
	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	// 頂点情報を書き込み
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

	float w = sp_w;
	float h = sp_h;

	// 画面の左上から右下に向かう線分を描画する
	v[0].position = { dx, dy, 0.0f };
	v[1].position = { dx + w, dy, 0.0f };
	v[2].position = { dx, dy + h, 0.0f };
	v[3].position = { dx + w, dy + h, 0.0f };
	v[0].color = color;
	v[1].color = color;
	v[2].color = color;
	v[3].color = color;
	v[0].uv = { 0.0f,0.0f };
	v[1].uv = { 1.0f,0.0f };
	v[2].uv = { 0.0f,1.0f };
	v[3].uv = { 1.0f,1.0f };

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);
	//迴轉陣列設定
	XMMATRIX base = XMMatrixIdentity();
	XMMATRIX camera = DirectX::XMMatrixTranslation(getClampedCameraPos().x, getClampedCameraPos().y, 0.0f);
	Shader_SetWorldMatrix(camera);
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pContext->PSSetConstantBuffers(2, 1, &g_pGammaBuffer);

	Texture_SetTexture(textureID);
	//三角形
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//連線
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//POINT render
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}
void Sprite_Draw_Y_Percentage(int textureID, float dx, float dy, float sp_w, float sp_h, float percent, float gamma, XMFLOAT4 color)
{

	// シェーダーを描画パイプラインに設定
	Shader_Begin();
	gamma_function(gamma);
	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	// 頂点情報を書き込み
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

	float w = sp_w;
	float h = sp_h;

	// 画面の左上から右下に向かう線分を描画する
	v[0].position = { dx, dy, 0.0f };
	v[1].position = { dx + w * percent, dy, 0.0f };
	v[2].position = { dx, dy + h, 0.0f };
	v[3].position = { dx + w * percent, dy + h, 0.0f };
	v[0].color = color;
	v[1].color = color;
	v[2].color = color;
	v[3].color = color;
	v[0].uv = { 0.0f,0.0f };
	v[1].uv = { 1.0f * percent,0.0f };
	v[2].uv = { 0.0f,1.0f };
	v[3].uv = { 1.0f * percent,1.0f };

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);
	//迴轉陣列設定
	XMMATRIX base = XMMatrixIdentity();
	XMMATRIX camera = DirectX::XMMatrixTranslation(getClampedCameraPos().x, getClampedCameraPos().y, 0.0f);
	Shader_SetWorldMatrix(camera);
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pContext->PSSetConstantBuffers(2, 1, &g_pGammaBuffer);

	Texture_SetTexture(textureID);
	//三角形
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//連線
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//POINT render
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}
void Sprite_Draw_Rotate(int textureID, float dx, float dy, float sp_w, float sp_h, float angle, float gamma, XMFLOAT4 color)
{


	// シェーダーを描画パイプラインに設定
	Shader_Begin();
	gamma_function(gamma);
	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	// 頂点情報を書き込み
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

	/*float w = sp_w;
	float h = sp_h;*/

	// 画面の左上から右下に向かう線分を描画する
	v[0].position = { -0.5f, -0.5f, 0.0f };
	v[1].position = { +0.5f, -0.5f, 0.0f };
	v[2].position = { -0.5f, +0.5f, 0.0f };
	v[3].position = { +0.5f, +0.5f, 0.0f };
	v[0].color = color;
	v[1].color = color;
	v[2].color = color;
	v[3].color = color;
	v[0].uv = { 0.0f,0.0f };
	v[1].uv = { 1.0f,0.0f };
	v[2].uv = { 0.0f,1.0f };
	v[3].uv = { 1.0f,1.0f };

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);
	//迴轉陣列設定
	XMMATRIX scale = XMMatrixScaling(sp_w, sp_h, 1.0f);
	XMMATRIX rotation = XMMatrixRotationZ(angle);
	XMMATRIX translation = XMMatrixTranslation(dx + sp_w * 0.5f, dy + sp_h * 0.5f, 0.0f);
	Shader_SetWorldMatrix(scale * rotation * translation);
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 頂点シェーダーに変換行列を設定
	//Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pContext->PSSetConstantBuffers(2, 1, &g_pGammaBuffer);

	//三角形
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//連線
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//POINT render
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	Texture_SetTexture(textureID);
	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}
void Sprite_Draw_UV(int textureID, float dx, float dy, float sp_w, float sp_h, float uv_h, float uv_w, float row, float column)
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

	float w = sp_w;
	float h = sp_h;

	// 画面の左上から右下に向かう線分を描画する
	v[0].position = { dx, dy, 0.0f };
	v[1].position = { dx + w, dy, 0.0f };
	v[2].position = { dx, dy + h, 0.0f };
	v[3].position = { dx + w, dy + h, 0.0f };
	v[0].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[1].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[2].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[3].color = { 1.0f,1.0f, 1.0f, 1.0f };


	v[0].uv = { uv_w * (row) , uv_h * (column) };
	v[1].uv = { uv_w * (row + 1), uv_h * (column) };
	v[2].uv = { uv_w * (row), uv_h * (column + 1) };
	v[3].uv = { uv_w * (row + 1) , uv_h * (column + 1) };

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);
	//迴轉陣列設定
	XMMATRIX base = XMMatrixIdentity();
	XMMATRIX camera = DirectX::XMMatrixTranslation(getClampedCameraPos().x, getClampedCameraPos().y, 0.0f);
	Shader_SetWorldMatrix(camera);
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	//三角形
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//連線
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//POINT render
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	Texture_SetTexture(textureID);
	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}
void Sprite_Draw_UV_UI(int textureID, float dx, float dy, float sp_w, float sp_h, float uv_h, float uv_w, float row, float column, float gamma, XMFLOAT4 color)
{
	// シェーダーを描画パイプラインに設定
	Shader_Begin();
	gamma_function(gamma);
	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	// 頂点情報を書き込み
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

	float w = sp_w;
	float h = sp_h;

	// 画面の左上から右下に向かう線分を描画する
	v[0].position = { dx, dy, 0.0f };
	v[1].position = { dx + w, dy, 0.0f };
	v[2].position = { dx, dy + h, 0.0f };
	v[3].position = { dx + w, dy + h, 0.0f };
	v[0].color = color;
	v[1].color = color;
	v[2].color = color;
	v[3].color = color;


	v[0].uv = { uv_w * (row) , uv_h * (column) };
	v[1].uv = { uv_w * (row + 1), uv_h * (column) };
	v[2].uv = { uv_w * (row), uv_h * (column + 1) };
	v[3].uv = { uv_w * (row + 1) , uv_h * (column + 1) };

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);
	//迴轉陣列設定
	XMMATRIX base = XMMatrixIdentity();
	XMMATRIX camera = DirectX::XMMatrixTranslation(getClampedCameraPos().x, getClampedCameraPos().y, 0.0f);
	Shader_SetWorldMatrix(base);
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pContext->PSSetConstantBuffers(2, 1, &g_pGammaBuffer);

	//三角形
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//連線
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//POINT render
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	Texture_SetTexture(textureID);
	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}
void Sprite_Draw_time(float dx, float dy, float sp_w, float sp_h)
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

	float w = sp_w;
	float h = sp_h;


	// 画面の左上から右下に向かう線分を描画する
	/*v[0].position = { SCREEN_WIDTH * .3f + time * vector2.x , SCREEN_HEIGHT * .3f + time * vector2.y, 0.0f };
	v[1].position = { SCREEN_WIDTH * .7f + time * vector2.x , SCREEN_HEIGHT * .3f + time * vector2.y, 0.0f };
	v[2].position = { SCREEN_WIDTH * .3f + time * vector2.x , SCREEN_HEIGHT * .7f + time * vector2.y, 0.0f };
	v[3].position = { SCREEN_WIDTH * .7f + time * vector2.x , SCREEN_HEIGHT * .7f + time * vector2.y, 0.0f };*/
	// 画面の左上から右下に向かう線分を描画する
	Vertex temp;
	temp.position = { test.position.x + dx , test.position.y + dy,  0.0f };

	v[0].position = { temp.position.x + dx , temp.position.y + dy,  0.0f };
	v[1].position = { temp.position.x + dx + w , temp.position.y + dy , 0.0f };
	v[2].position = { temp.position.x + dx , temp.position.y + dy + h, 0.0f };
	v[3].position = { temp.position.x + dx + w , temp.position.y + dy + h,  0.0f };
	v[0].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[1].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[2].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[3].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[0].uv = { 0.0f,0.0f };
	v[1].uv = { 1.0f,0.0f };
	v[2].uv = { 0.0f,1.0f };
	v[3].uv = { 1.0f,1.0f };

	test.position = temp.position;



	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);
	//迴轉陣列設定
	Shader_SetWorldMatrix(XMMatrixIdentity());
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	//三角形
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//連線
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//POINT render
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}

void Sprite_Draw_time_UV(float dx, float dy, float sp_w, float sp_h, float uv_w, float uv_h, float row, float column)
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

	float w = sp_w;
	float h = sp_h;


	// 画面の左上から右下に向かう線分を描画する
	/*v[0].position = { SCREEN_WIDTH * .3f + time * vector2.x , SCREEN_HEIGHT * .3f + time * vector2.y, 0.0f };
	v[1].position = { SCREEN_WIDTH * .7f + time * vector2.x , SCREEN_HEIGHT * .3f + time * vector2.y, 0.0f };
	v[2].position = { SCREEN_WIDTH * .3f + time * vector2.x , SCREEN_HEIGHT * .7f + time * vector2.y, 0.0f };
	v[3].position = { SCREEN_WIDTH * .7f + time * vector2.x , SCREEN_HEIGHT * .7f + time * vector2.y, 0.0f };*/
	// 画面の左上から右下に向かう線分を描画する
	Vertex temp;
	temp.position = { (test.position.x + dx) , (test.position.y + dy) , 0.0f };

	v[0].position = { (temp.position.x + dx) , (temp.position.y + dy) ,  0.0f };
	v[1].position = { (temp.position.x + dx + w) , (temp.position.y + dy) , 0.0f };
	v[2].position = { (temp.position.x + dx) , (temp.position.y + dy + h) , 0.0f };
	v[3].position = { (temp.position.x + dx + w) , (temp.position.y + dy + h),  0.0f };
	v[0].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[1].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[2].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[3].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[0].uv = { uv_w * (row)     , uv_h * (column) };
	v[1].uv = { uv_w * (row + 1) , uv_h * (column) };
	v[2].uv = { uv_w * (row)     , uv_h * (column + 1) };
	v[3].uv = { uv_w * (row + 1) , uv_h * (column + 1) };
	//for (int i = 0; i < NUM_VERTEX; i++)
	//{
	test.position = temp.position;
	//}

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);
	//迴轉陣列設定
	XMMATRIX base = XMMatrixIdentity();
	XMMATRIX camera = DirectX::XMMatrixTranslation(getClampedCameraPos().x, getClampedCameraPos().y, 0.0f);
	Shader_SetWorldMatrix(camera);
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	//三角形
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//連線
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//POINT render
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}
void Sprite_Draw_time_UV(float dx, float dy, float sp_w, float sp_h, float uv_h, float uv_w, float row, float column, bool face_right)//0618轉圖片方向
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

	float w = sp_w;
	float h = sp_h;


	// 画面の左上から右下に向かう線分を描画する
	/*v[0].position = { SCREEN_WIDTH * .3f + time * vector2.x , SCREEN_HEIGHT * .3f + time * vector2.y, 0.0f };
	v[1].position = { SCREEN_WIDTH * .7f + time * vector2.x , SCREEN_HEIGHT * .3f + time * vector2.y, 0.0f };
	v[2].position = { SCREEN_WIDTH * .3f + time * vector2.x , SCREEN_HEIGHT * .7f + time * vector2.y, 0.0f };
	v[3].position = { SCREEN_WIDTH * .7f + time * vector2.x , SCREEN_HEIGHT * .7f + time * vector2.y, 0.0f };*/
	// 画面の左上から右下に向かう線分を描画する
	Vertex temp;
	temp.position = { (test.position.x + dx) , (test.position.y + dy) , 0.0f };
	temp.position = { (test.position.x + dx) , (test.position.y + dy) , 0.0f };
	temp.position = { (test.position.x + dx) , (test.position.y + dy) , 0.0f };
	temp.position = { (test.position.x + dx)  ,(test.position.y + dy) , 0.0f };

	v[0].position = { (temp.position.x + dx) , (temp.position.y + dy) ,  0.0f };
	v[1].position = { (temp.position.x + dx + w) , (temp.position.y + dy) , 0.0f };
	v[2].position = { (temp.position.x + dx) , (temp.position.y + dy + h) , 0.0f };
	v[3].position = { (temp.position.x + dx + w) , (temp.position.y + dy + h),  0.0f };
	v[0].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[1].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[2].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[3].color = { 1.0f,1.0f, 1.0f, 1.0f };
	if (face_right)
	{
		v[0].uv = { uv_w * (row)     , uv_h * (column) };
		v[1].uv = { uv_w * (row + 1)  , uv_h * (column) };
		v[2].uv = { uv_w * (row)     , uv_h * (column + 1) };
		v[3].uv = { uv_w * (row + 1) , uv_h * (column + 1) };
	}
	else
	{
		v[0].uv = { uv_w * (row)     , uv_h * (column) };
		v[1].uv = { uv_w * (row + 1)  , uv_h * (column) };
		v[2].uv = { uv_w * (row)     , uv_h * (column + 1) };
		v[3].uv = { uv_w * (row + 1) , uv_h * (column + 1) };
	}


	test.position = temp.position;



	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);
	//迴轉陣列設定
	XMMATRIX base = XMMatrixIdentity();
	XMMATRIX camera = DirectX::XMMatrixTranslation(getClampedCameraPos().x, getClampedCameraPos().y, 0.0f);
	Shader_SetWorldMatrix(camera);
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	//三角形
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//連線
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//POINT render
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}
void Sprite_Draw_Move_STRUCT(float dx, float dy, float sp_w, float sp_h, float uv_w, float uv_h, float row, float column, float startpos_x, float gamma, XMFLOAT4 color)
{
	// シェーダーを描画パイプラインに設定
	Shader_Begin();
	gamma_function(gamma);

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	// 頂点情報を書き込み
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

	float w = sp_w;
	float h = sp_h;


	// 画面の左上から右下に向かう線分を描画する
	/*v[0].position = { SCREEN_WIDTH * .3f + time * vector2.x , SCREEN_HEIGHT * .3f + time * vector2.y, 0.0f };
	v[1].position = { SCREEN_WIDTH * .7f + time * vector2.x , SCREEN_HEIGHT * .3f + time * vector2.y, 0.0f };
	v[2].position = { SCREEN_WIDTH * .3f + time * vector2.x , SCREEN_HEIGHT * .7f + time * vector2.y, 0.0f };
	v[3].position = { SCREEN_WIDTH * .7f + time * vector2.x , SCREEN_HEIGHT * .7f + time * vector2.y, 0.0f };*/
	// 画面の左上から右下に向かう線分を描画する
	//Vertex temp;
	//temp.position = { (playdata.now_vertex.x + dx) , (playdata.now_vertex.y + dy) , 0.0f };

	v[0].position = { (dx) , (dy) ,  0.0f };
	v[1].position = { (dx + w) , (dy) , 0.0f };
	v[2].position = { (dx) , (dy + h) , 0.0f };
	v[3].position = { (dx + w) , (dy + h),  0.0f };
	v[0].color = color;
	v[1].color = color;
	v[2].color = color;
	v[3].color = color;
	v[0].uv = { uv_w * (startpos_x + row)     , uv_h * (column) };
	v[1].uv = { uv_w * (startpos_x + row + 1)  , uv_h * (column) };
	v[2].uv = { uv_w * (startpos_x + row)     , uv_h * (column + 1) };
	v[3].uv = { uv_w * (startpos_x + row + 1) , uv_h * (column + 1) };
	//for (int i = 0; i < NUM_VERTEX; i++)
	//{
	//playdata.now_vertex.x = temp.position.x;
	//playdata.now_vertex.y = temp.position.y;
	//}

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);
	//迴轉陣列設定
	XMMATRIX base = XMMatrixIdentity();
	XMMATRIX camera = DirectX::XMMatrixTranslation(getClampedCameraPos().x, getClampedCameraPos().y, 0.0f);
	Shader_SetWorldMatrix(camera);
	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pContext->PSSetConstantBuffers(2, 1, &g_pGammaBuffer);
	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	//三角形
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//連線
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//POINT render
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}

void Sprite_Draw_Move_Rotate_STRUCT(AnimPlayData& playdata, float dx, float dy, float sp_w, float sp_h, float angle, float uv_w, float uv_h, float row, float column, float startpos_x)
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

	float w = sp_w;
	float h = sp_h;


	// 画面の左上から右下に向かう線分を描画する
	/*v[0].position = { SCREEN_WIDTH * .3f + time * vector2.x , SCREEN_HEIGHT * .3f + time * vector2.y, 0.0f };
	v[1].position = { SCREEN_WIDTH * .7f + time * vector2.x , SCREEN_HEIGHT * .3f + time * vector2.y, 0.0f };
	v[2].position = { SCREEN_WIDTH * .3f + time * vector2.x , SCREEN_HEIGHT * .7f + time * vector2.y, 0.0f };
	v[3].position = { SCREEN_WIDTH * .7f + time * vector2.x , SCREEN_HEIGHT * .7f + time * vector2.y, 0.0f };*/
	// 画面の左上から右下に向かう線分を描画する
	Vertex temp;
	temp.position = { (playdata.now_vertex.x + dx) , (playdata.now_vertex.y + dy) , 0.0f };

	v[0].position = { (temp.position.x + dx) , (temp.position.y + dy) ,  0.0f };
	v[1].position = { (temp.position.x + dx + w) , (temp.position.y + dy) , 0.0f };
	v[2].position = { (temp.position.x + dx) , (temp.position.y + dy + h) , 0.0f };
	v[3].position = { (temp.position.x + dx + w) , (temp.position.y + dy + h),  0.0f };
	v[0].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[1].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[2].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[3].color = { 1.0f,1.0f, 1.0f, 1.0f };
	v[0].uv = { uv_w * (startpos_x + row)     , uv_h * (column) };
	v[1].uv = { uv_w * (startpos_x + row + 1)  , uv_h * (column) };
	v[2].uv = { uv_w * (startpos_x + row)     , uv_h * (column + 1) };
	v[3].uv = { uv_w * (startpos_x + row + 1) , uv_h * (column + 1) };
	//for (int i = 0; i < NUM_VERTEX; i++)
	//{
	playdata.now_vertex.x = temp.position.x;
	playdata.now_vertex.y = temp.position.y;
	//}

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);


	//迴轉陣列設定
	XMMATRIX base = XMMatrixIdentity();
	XMMATRIX rotation = XMMatrixRotationZ(angle);
	XMMATRIX camera = DirectX::XMMatrixTranslation(getClampedCameraPos().x, getClampedCameraPos().y, 0.0f);
	Shader_SetWorldMatrix(camera);
	Shader_SetWorldMatrix(rotation * camera);



	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	//三角形
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//連線
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//POINT render
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}
//回転対応

