#pragma once

#include <unordered_map>

#include "d3d11.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "DirectXMath.h"
#pragma comment (lib, "assimp-vc143-mt.lib")
#include "Collision3D.h"



struct MODEL
{
	const aiScene* AiScene = nullptr;

	ID3D11Buffer** VertexBuffer;
	ID3D11Buffer** IndexBuffer;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;

	AABB Local;
};


MODEL* ModelLoad(const char* FileName, float scale, bool bBlender);
void ModelRelease(MODEL* model);

void ModelDraw(MODEL* model, DirectX::XMFLOAT3 gameobjectPos);

