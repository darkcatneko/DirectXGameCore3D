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



struct MODEL_STATIC
{
	const aiScene* AiScene = nullptr;

	ID3D11Buffer** VertexBuffer;
	ID3D11Buffer** IndexBuffer;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;

	AABB Local;
};


MODEL_STATIC* Model_Static_Load(const char* FileName, float scale, bool bBlender);
void Model_Static_Release(MODEL_STATIC* model);

void Model_Static_Draw(MODEL_STATIC* model, GameObject* gameobject);
#endif