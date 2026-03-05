#include "Billboard.h"
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "shader.h"
#include "Shader_Billboard.h"
#include "Texture.h"
#include "Camera3D.h"
using namespace DirectX;

static constexpr int NUM_VERTEX = 6; // 頂点数
static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ

struct  Vertex3D
{
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT2 texcoord;
};

void Billboard_Initialize()
{
	static Vertex3D g_billboardVertex[]
	{
		// 三角形 1
	{{-0.5f, 0.5f,0},{1,1,1,1},{0,0}},
	{{ 0.5f, 0.5f,0},{1,1,1,1},{1,0}},
	{{-0.5f,-0.5f,0},{1,1,1,1},{0,1}},

	// 三角形 2
	{{-0.5f,-0.5f,0},{1,1,1,1},{0,1}},
	{{ 0.5f, 0.5f,0},{1,1,1,1},{1,0}},
	{{ 0.5f,-0.5f,0},{1,1,1,1},{1,1}},

	};
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex3D) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = g_billboardVertex;
	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);
}

void Billboard_Finitialize()
{
}

void Billboard_Update(double elapsed_time)
{
}

void Billboard_Draw(int texid, DirectX::XMFLOAT3 gameobjectPos, float scale_x, float scale_y)
{
	Shader_Billboard_SetUVParameter({ { 1.0f,1.0f}, { 0.0f,0.0f } });
	Shader_Billboard_Begin();
	Shader_Billboard_SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	Texture_SetTexture(texid);

	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	Direct3D_GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);


	XMFLOAT4X4 mtxCamera = Camera_GetMatrix();
	mtxCamera._41 = mtxCamera._42 = mtxCamera._43 = 0.0f;
	XMMATRIX iv = XMMatrixInverse(nullptr,XMLoadFloat4x4(&mtxCamera));

	XMMATRIX mtxs = XMMatrixScaling(scale_x, scale_y, 1.0f);
	XMMATRIX mtxt = XMMatrixTranslation(gameobjectPos.x, gameobjectPos.y, gameobjectPos.z);
	Shader_Billboard_SetWorldMatrix(mtxs *iv * mtxt);

	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ポリゴン描画命令発行
	Direct3D_GetContext()->Draw(NUM_VERTEX, 0);
}
