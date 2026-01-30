#ifndef MODEL_STATIC_H
#define MODEL_STATIC_H

#include <unordered_map>

#include "d3d11.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "DirectXMath.h"
#pragma comment (lib, "assimp-vc143-mt.lib")
#include "Collision3D.h"
#include "GameObject.h"

struct TriMeshCollider
{
	std::vector<DirectX::XMFLOAT3> positions; // local-space vertex positions
	std::vector<uint32_t> indices;            // 3 indices per triangle
	AABB localAabb;                           // broadphase per mesh
};

struct MODEL_STATIC
{
	const aiScene* AiScene = nullptr;

	ID3D11Buffer** VertexBuffer;
	ID3D11Buffer** IndexBuffer;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;

	std::vector<TriMeshCollider> colliders;   // <= 新增：每個 aiMesh 一個                          
	AABB Local;
};


MODEL_STATIC* Model_Static_Load(const char* FileName, float scale, bool bBlender);
void Model_Static_Release(MODEL_STATIC* model);

void Model_Static_Draw(MODEL_STATIC* model, GameObject* gameobject);
void Model_Static_Draw_Q(MODEL_STATIC* model, GameObject* gameobject);
void ModelUnlitDraw(MODEL_STATIC* model, const DirectX::XMMATRIX& mtxWorld);
AABB ModelStatic_GetAABBInWorldSpace(MODEL_STATIC* model, DirectX::XMFLOAT3 pos);
#endif