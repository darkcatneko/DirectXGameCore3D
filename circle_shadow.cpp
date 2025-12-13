#include "circle_shadow.h"
#include "direct3d.h"
#include <DirectXMath.h>
#include "Shader_Billboard.h"
using namespace DirectX;
#include "shader3d_unlit.h"
#include "Texture.h"
#include "Collision3D.h"


static constexpr int NUM_VERTEX = 6; // 頂点数
static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static int texid = -1;

struct  Vertex3D
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 texcoord;
};

void CircleShadow_Initialize()
{
	static Vertex3D g_billboardVertex[]
	{
		// 三角形 1
	{{-0.5f, 0.5f,0},{0,1.0f,0},{1,1,1,1},{0,0}},
	{{ 0.5f, 0.5f,0},{0,1.0f,0},{1,1,1,1},{1,0}},
	{{-0.5f,-0.5f,0},{0,1.0f,0},{1,1,1,1},{0,1}},

	// 三角形 2
	{{-0.5f,-0.5f,0},{0,1.0f,0},{1,1,1,1},{0,1}},
	{{ 0.5f, 0.5f,0},{0,1.0f,0},{1,1,1,1},{1,0}},
	{{ 0.5f,-0.5f,0},{0,1.0f,0},{1,1,1,1},{1,1}},

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
	texid = Texture_Load(L"circle_shadow.png");
	//texid = 2;
}

void CircleShadow_Finalize()
{
}

void CircleShadow_Draw(DirectX::XMFLOAT3 position)
{
	Shader3dUnlit_Begin();
	Shader3DUnilt_SetColor({ 1.0f, 1.0f, 1.0f, 0.5f });
	Texture_SetTexture(texid);

	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	Direct3D_GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);


	XMMATRIX mtxTrans = XMMatrixTranslation(position.x, position.y+0.01f, position.z);
	XMMATRIX mtxRot = XMMatrixRotationX(XMConvertToRadians(90.0f));
	XMMATRIX mtxScale = XMMatrixScaling(3.0f, 3.0f, 3.0f);
	XMMATRIX mtxWorld = mtxScale * mtxRot * mtxTrans;

	Shader3DUnilt_SetWorldMatrix(mtxWorld);

	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ポリゴン描画命令発行
	Direct3D_GetContext()->Draw(NUM_VERTEX, 0);
}
