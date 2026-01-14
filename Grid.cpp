#include "Grid.h"
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
#include "shader.h"
#include "shader3d_unlit.h"
#include "Texture.h"
#include "Collision3D.h"
#include "Model_Static.h"
#include "NekoTool.h"
#include "Camera3D.h"
using namespace DirectX;

#pragma region 宣告
static constexpr int GRID_H_COUNT = 100;
static constexpr int GRID_V_COUNT = 100;
static constexpr int GRID_H_LINE_COUNT = GRID_H_COUNT + 1;
static constexpr int GRID_V_LINE_COUNT = GRID_V_COUNT + 1;
static constexpr int NUM_VERTEX = GRID_H_LINE_COUNT * 2 + GRID_V_LINE_COUNT * 2; // 頂点数

static constexpr int SPHERE_SEGMENTS = 32;
static constexpr int SPHERE_VERTEX_COUNT = SPHERE_SEGMENTS * 2 * 3;


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11Buffer* g_pDebugRayVB = nullptr;
static ID3D11Buffer* g_pDebugSphereVB = nullptr;
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

	bd.ByteWidth = sizeof(Vertex3D) * SPHERE_VERTEX_COUNT;
	g_pDevice->CreateBuffer(&bd, nullptr, &g_pDebugSphereVB);

	whiteTex = Texture_Load(L"white.png");
}

void Grid_Finitialize()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pDebugRayVB);
	SAFE_RELEASE(g_pDebugSphereVB);
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
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

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
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// World = Identity（因為 v 已經是 world space）
	Shader3DUnilt_SetWorldMatrix(XMMatrixIdentity());

	g_pContext->Draw(2, 0);
}

void Grid_DebugDrawSphere(const Sphere& sphere, const XMFLOAT4& color)
{
	Vertex3D vertices[SPHERE_VERTEX_COUNT];
	int index = 0;

	constexpr float PI = XM_PI;

	auto addCircle = [&](int axis)
		{
			for (int i = 0; i < SPHERE_SEGMENTS; i++)
			{
				float t0 = (float)i / SPHERE_SEGMENTS * PI * 2.0f;
				float t1 = (float)(i + 1) / SPHERE_SEGMENTS * PI * 2.0f;

				XMFLOAT3 p0{}, p1{};

				switch (axis)
				{
				case 0: // XZ
					p0 = { cosf(t0), 0, sinf(t0) };
					p1 = { cosf(t1), 0, sinf(t1) };
					break;
				case 1: // XY
					p0 = { cosf(t0), sinf(t0), 0 };
					p1 = { cosf(t1), sinf(t1), 0 };
					break;
				case 2: // YZ
					p0 = { 0, cosf(t0), sinf(t0) };
					p1 = { 0, cosf(t1), sinf(t1) };
					break;
				}

				vertices[index++] = {
					{
						sphere.center.x + p0.x * sphere.radius,
						sphere.center.y + p0.y * sphere.radius,
						sphere.center.z + p0.z * sphere.radius
					},
					color
				};

				vertices[index++] = {
					{
						sphere.center.x + p1.x * sphere.radius,
						sphere.center.y + p1.y * sphere.radius,
						sphere.center.z + p1.z * sphere.radius
					},
					color
				};
			}
		};

	addCircle(0);
	addCircle(1);
	addCircle(2);

	// Update VB
	D3D11_MAPPED_SUBRESOURCE mapped{};
	g_pContext->Map(g_pDebugSphereVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, vertices, sizeof(vertices));
	g_pContext->Unmap(g_pDebugSphereVB, 0);

	// Draw
	Shader3dUnlit_Begin();
	Shader3DUnilt_SetColor(color);
	Texture_SetTexture(whiteTex, 0);

	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pDebugSphereVB, &stride, &offset);
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	Shader3DUnilt_SetWorldMatrix(XMMatrixIdentity());

	g_pContext->Draw(SPHERE_VERTEX_COUNT, 0);
}

void DrawTriMesh_Gizmo(
	 TriMeshCollider& col,
	const DirectX::XMFLOAT3& world,
	const DirectX::XMFLOAT4& color)
{
	using namespace DirectX;

	for (size_t i = 0; i < col.indices.size(); i += 3)
	{
		const XMFLOAT3& p0 = col.positions[col.indices[i + 0]];
		const XMFLOAT3& p1 = col.positions[col.indices[i + 1]];
		const XMFLOAT3& p2 = col.positions[col.indices[i + 2]];
		//world matrix
		XMMATRIX mtxTrans = XMMatrixTranslation(world.x , world.y, world.z );
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(0),   // X
			XMConvertToRadians(0),   // Y
			XMConvertToRadians(0)    // Z
		);
		XMMATRIX mtxScale = XMMatrixScaling(
			1,
			1,
			1
		);

		XMMATRIX mtxWorld = mtxScale * mtxRot * mtxTrans;
		XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&p0), mtxWorld);
		XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&p1), mtxWorld);
		XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&p2), mtxWorld);

		XMFLOAT3 vv0, vv1, vv2;
		XMStoreFloat3(&vv0, v0);
		XMStoreFloat3(&vv1, v1);
		XMStoreFloat3(&vv2, v2);

		Debug_DrawLine(vv0, vv1, color);
		Debug_DrawLine(vv1, vv2, color);
		Debug_DrawLine(vv2, vv0, color);
	}
}
void Debug_DrawLine(
	const XMFLOAT3& origin,
	const XMFLOAT3& end,
	XMFLOAT4 color
)
{
	Vertex3D v[2];

	v[0].position = origin;
	v[0].color = color; // 紅色起點

	v[1].position = end;
	v[1].color = color; // 紅色起點

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
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// World = Identity（因為 v 已經是 world space）
	Shader3DUnilt_SetWorldMatrix(XMMatrixIdentity());

	g_pContext->Draw(2, 0);
}

void DebugDrawSector(const Sector& sector, const XMFLOAT4& color, int arcSegments)
{
	XMFLOAT3 f = sector.forward;
	if (sector.ignoreY) f = NormalizeXZ(f);

	float halfRad = XMConvertToRadians(sector.fovDeg * 0.5f);

	XMFLOAT3 leftDir = RotateY(f, -halfRad);
	XMFLOAT3 rightDir = RotateY(f, halfRad);

	// 邊界線
	XMFLOAT3 o = sector.origin;

	XMFLOAT3 leftEnd = XMFLOAT3(o.x + leftDir.x * sector.radius, o.y + leftDir.y * sector.radius, o.z + leftDir.z * sector.radius);
	XMFLOAT3 rightEnd = XMFLOAT3(o.x + rightDir.x * sector.radius, o.y + rightDir.y * sector.radius, o.z + rightDir.z * sector.radius);

	Debug_DrawLine(o, leftEnd, color);
	Debug_DrawLine(o, rightEnd, color);

	// 圓弧（用多段線近似）
	XMFLOAT3 prev = leftEnd;
	for (int i = 1; i <= arcSegments; i++)
	{
		float t = (float)i / (float)arcSegments;
		float a = (-halfRad) + (2.0f * halfRad) * t;

		XMFLOAT3 dir = RotateY(f, a);
		XMFLOAT3 cur = XMFLOAT3(o.x + dir.x * sector.radius, o.y + dir.y * sector.radius, o.z + dir.z * sector.radius);

		Debug_DrawLine(prev, cur, color);
		prev = cur;
	}

	// (可選) 畫 forward 中線
	XMFLOAT3 midEnd = XMFLOAT3(o.x + f.x * sector.radius, o.y + f.y * sector.radius, o.z + f.z * sector.radius);
	Debug_DrawLine(o, midEnd, XMFLOAT4(1, 1, 1, 1));
}

#pragma region RotateGismos

static inline XMVECTOR Load3(const XMFLOAT3& v) { return XMLoadFloat3(&v); }
static inline XMFLOAT3 Store3(XMVECTOR v) { XMFLOAT3 o; XMStoreFloat3(&o, v); return o; }

static inline float Length3(XMVECTOR v) { return XMVectorGetX(XMVector3Length(v)); }

bool IsPointInSector(const Sector& s, const XMFLOAT3& point)
{
	XMVECTOR o = XMLoadFloat3(&s.origin);
	XMVECTOR p = XMLoadFloat3(&point);
	XMVECTOR f = XMLoadFloat3(&s.forward);

	XMVECTOR to = p - o;

	if (s.ignoreY)
	{
		// 只做水平：把 Y 分量清掉
		to = XMVectorSetY(to, 0.0f);
		f = XMVectorSetY(f, 0.0f);
	}

	// 距離（用平方避免 sqrt）
	float distSq = XMVectorGetX(XMVector3LengthSq(to));
	float rSq = s.radius * s.radius;
	if (distSq > rSq) return false;

	// forward / dir normalize
	XMVECTOR fN = XMVector3Normalize(f);
	XMVECTOR dN = XMVector3Normalize(to);

	// dot >= cos(halfFov)
	float half = s.fovDeg * 0.5f;
	float cosHalf = cosf(XMConvertToRadians(half));
	float dot = XMVectorGetX(XMVector3Dot(fN, dN));

	return dot >= cosHalf;
}

static float ComputeGizmoRadiusWorld(float dist, float fovYRad, int viewportH, float radiusPx)
{
	// worldHeight = 2 * dist * tan(fovY/2)
	float worldHeight = 2.0f * dist * tanf(fovYRad * 0.5f);
	float worldPerPixel = worldHeight / (float)viewportH;
	return radiusPx * worldPerPixel;
}

// 在 planeNormal 所定義的平面上畫圓（用 basisU/basisV 定義平面內兩軸）
static void DrawCircle(
	const XMFLOAT3& center,
	const XMFLOAT3& basisU,   // 平面內單位向量
	const XMFLOAT3& basisV,   // 平面內單位向量
	float radius,
	int segments,
	const XMFLOAT4& color)
{
	XMVECTOR c = Load3(center);
	XMVECTOR u = XMVector3Normalize(Load3(basisU));
	XMVECTOR v = XMVector3Normalize(Load3(basisV));

	// 預防數值問題：確保 u、v 垂直（可選）
	v = XMVector3Normalize(v - u * XMVectorGetX(XMVector3Dot(u, v)));

	XMFLOAT3 prev{};
	for (int i = 0; i <= segments; i++)
	{
		float t = (float)i / (float)segments;
		float a = t * XM_2PI;

		XMVECTOR p = c + (u * cosf(a) + v * sinf(a)) * radius;
		XMFLOAT3 cur = Store3(p);

		if (i > 0) Debug_DrawLine(prev, cur, color);
		prev = cur;
	}
}

static void BuildOrthonormalBasisFromAxis(const XMFLOAT3& axis, XMFLOAT3& outU, XMFLOAT3& outV)
{
	// 給一個 axis（要垂直於圓環平面），產生平面內兩個基底 U/V
	XMVECTOR n = XMVector3Normalize(Load3(axis));

	// 選一個不平行的參考向量
	XMVECTOR ref = (fabsf(XMVectorGetX(n)) < 0.9f) ? XMVectorSet(1, 0, 0, 0) : XMVectorSet(0, 1, 0, 0);

	XMVECTOR u = XMVector3Normalize(XMVector3Cross(ref, n));
	XMVECTOR v = XMVector3Cross(n, u);

	outU = Store3(u);
	outV = Store3(v);
}

void DrawRotatingGizmo_TranslateRotateStyle(
	const XMFLOAT3& objectPos,
	const XMFLOAT3& objectForward, // 物件朝向（若你要以物件 local 軸當環的方向）
	float timeSeconds,
	float screenRadiusPx)
{
	XMVECTOR camPos = Load3(Camera_GetCameraPos());
	XMVECTOR objPos = Load3(objectPos);

	float dist = Length3(objPos - camPos);
	dist = (dist < 0.01f) ? 0.01f : dist;

	float radiusWorld = ComputeGizmoRadiusWorld(dist, Camera_GetFov(), Direct3D_GetBackBufferHeight(), screenRadiusPx);

	// 你可以選擇「環對齊世界軸」或「環對齊物件local軸」
	// 這裡示範：對齊世界軸（X/Y/Z）
	XMFLOAT3 axisX{ 1,0,0 };
	XMFLOAT3 axisY{ 0,1,0 };
	XMFLOAT3 axisZ{ 0,0,1 };

	// 旋轉效果：讓整個 gizmo 隨時間繞 Y 軸旋轉（你也可以改成繞相機 forward）
	XMMATRIX Rspin = XMMatrixRotationY(timeSeconds * 1.5f); // 旋轉速度自行調
	auto rotVec = [&](const XMFLOAT3& v)->XMFLOAT3
		{
			XMVECTOR x = XMVector3TransformNormal(Load3(v), Rspin);
			return Store3(x);
		};

	XMFLOAT3 xSpin = rotVec(axisX);
	XMFLOAT3 ySpin = rotVec(axisY);
	XMFLOAT3 zSpin = rotVec(axisZ);

	// 對每個軸畫一個圓環（圓環平面的法線=該軸方向）
	// X 環（法線 X） => 平面是 YZ
	{
		XMFLOAT3 u, v;
		BuildOrthonormalBasisFromAxis(xSpin, u, v);
		DrawCircle(objectPos, u, v, radiusWorld, 64, XMFLOAT4(1, 0, 0, 1));
	}
	// Y 環（法線 Y） => 平面是 XZ
	{
		XMFLOAT3 u, v;
		BuildOrthonormalBasisFromAxis(ySpin, u, v);
		DrawCircle(objectPos, u, v, radiusWorld, 64, XMFLOAT4(0, 1, 0, 1));
	}
	// Z 環（法線 Z） => 平面是 XY
	{
		XMFLOAT3 u, v;
		BuildOrthonormalBasisFromAxis(zSpin, u, v);
		DrawCircle(objectPos, u, v, radiusWorld, 64, XMFLOAT4(0, 0, 1, 1));
	}

	
}

#pragma endregion