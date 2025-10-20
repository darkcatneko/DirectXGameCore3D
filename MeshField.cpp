#include "MeshField.h"
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "shader.h"
#include "ShaderField.h"
#include "Shader3D.h"
#include "Texture.h"
#include "Camera3D.h"
using namespace DirectX;

static constexpr float FIELD_MESH_SIZE = 1.0f;

static constexpr int FIELD_H_MESH_COUNT = 50; 
static constexpr int FIELD_V_MESH_COUNT = 25; 
static constexpr int FIELD_H_MESH_VERTEX_COUNT = FIELD_H_MESH_COUNT + 1; 
static constexpr int FIELD_V_MESH_VERTEX_COUNT = FIELD_V_MESH_COUNT + 1; 

static constexpr int NUM_VERTEX = FIELD_H_MESH_VERTEX_COUNT* FIELD_V_MESH_VERTEX_COUNT; // 頂点数
static constexpr int NUM_INDEX = 3 * 2 * FIELD_H_MESH_COUNT* FIELD_V_MESH_COUNT;

struct  Vertex3D
{
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT2 texcoord;
};
static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11Buffer* g_pIndexBuffer = nullptr; // インデックスバッファ
static ID3D11ShaderResourceView* g_pTexture = nullptr;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;
static Vertex3D g_MeshFieldVertex[NUM_VERTEX];
static unsigned short g_MeshFieldIndex[NUM_INDEX];
static int g_MeshTexTempId = -1;
static int g_MeshTexTemp2Id = -1;

static float g_x;
static float g_rotate;
static float g_scale;
static double g_time;

void MeshField_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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

	//製作頂點情報
	for (int z = 0; z < FIELD_V_MESH_VERTEX_COUNT; z++)
	{
		for (int x = 0; x < FIELD_H_MESH_VERTEX_COUNT; x++)
		{
			//橫+橫的最大數 * 縱;
			int index = x + FIELD_H_MESH_VERTEX_COUNT * z;
			g_MeshFieldVertex[index].position = { x * FIELD_MESH_SIZE,0.0f,z * FIELD_MESH_SIZE };
			g_MeshFieldVertex[index].color = { 1.0f,1.0f,1.0f,1.0f };
			g_MeshFieldVertex[index].texcoord = { x * 1.0f, z * 1.0f };
		}

	}
	

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = g_MeshFieldVertex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

	// インデックスバファ
	bd.ByteWidth = sizeof(unsigned short) * NUM_INDEX;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	for (int z = 0; z < FIELD_V_MESH_COUNT; z++)
	{
		for (int x = 0; x < FIELD_H_MESH_COUNT; x++)
		{
			g_MeshFieldIndex[6 * (x + FIELD_H_MESH_COUNT * z)] = x + FIELD_H_MESH_VERTEX_COUNT * (z + 1);
			g_MeshFieldIndex[6 * (x + FIELD_H_MESH_COUNT * z) + 1] = x + 1 + FIELD_H_MESH_VERTEX_COUNT * z;
			g_MeshFieldIndex[6 * (x + FIELD_H_MESH_COUNT * z) + 2] = x + FIELD_H_MESH_VERTEX_COUNT * z;
			g_MeshFieldIndex[6 * (x + FIELD_H_MESH_COUNT * z) + 3] = x + FIELD_H_MESH_VERTEX_COUNT * (z + 1);
			g_MeshFieldIndex[6 * (x + FIELD_H_MESH_COUNT * z) + 4] = x + FIELD_H_MESH_VERTEX_COUNT * (z + 1) + 1;
			g_MeshFieldIndex[6 * (x + FIELD_H_MESH_COUNT * z) + 5] = x + 1 + FIELD_H_MESH_VERTEX_COUNT * z;
		}
	}
	sd.pSysMem = g_MeshFieldIndex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pIndexBuffer);

	g_MeshTexTempId = Texture_Load(L"Grass.png");

	ShaderField_Initialize(pDevice, pContext);
}

void MeshField_Finitialize()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
}

void MeshField_Draw(DirectX::XMFLOAT3 gameobjectPos)
{
	ShaderField_Begin();
	// 頂点バッファを描画パイプラインに設定
	Texture_SetTexture(g_MeshTexTempId);
	Texture_SetTexture(g_MeshTexTempId,1);
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// インデックスバッファを描画パイプラインに設定
	g_pContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	//world matrix
	//XMMATRIX mtxWorld = XMMatrixIdentity();
	XMMATRIX mtxTrans = XMMatrixTranslation(gameobjectPos.x, gameobjectPos.y, gameobjectPos.z);
	XMMATRIX mtxRot = XMMatrixRotationX(XMConvertToRadians(-90.0f));
	XMMATRIX mtxScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX mtxWorld = mtxScale *mtxRot * mtxTrans ;
	ShaderField_SetWorldMatrix(mtxWorld);



	// プリミティブトポロジ設定
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float offset_x = FIELD_H_MESH_COUNT * FIELD_MESH_SIZE;
	float offset_y = FIELD_V_MESH_COUNT * FIELD_MESH_SIZE;

	ShaderField_SetViewMatrix(XMLoadFloat4x4(&Camera_GetMatrix()));
	ShaderField_SetProjectionMatrix(XMLoadFloat4x4(&Camera_GetMatrixPerspective()));

	// ポリゴン描画命令発行
	g_pContext->DrawIndexed(NUM_INDEX, 0, 0); //TO DELETE
}
