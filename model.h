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
#include "GameObject.h"


struct Bone
{
	std::string name;
	int parentIndex;
	DirectX::XMMATRIX offsetMatrix; // 綁定姿勢
	DirectX::XMMATRIX globalBind; // 綁定姿勢
	DirectX::XMMATRIX finalTransform; // 每幀更新後的矩陣
};

struct AnimationChannel {
	std::vector<std::pair<double, DirectX::XMFLOAT3>> positions;
	std::vector<std::pair<double, DirectX::XMFLOAT4>> rotations;
	std::vector<std::pair<double, DirectX::XMFLOAT3>> scales;
};
struct Animation {
	double duration;
	double ticksPerSecond;
	std::unordered_map<std::string, AnimationChannel> channels;
};
struct MODEL
{
	const aiScene* AiScene = nullptr;
	std::vector<Bone> bones;
	std::unordered_map<std::string, int> boneIndex;
	Animation animation;



	ID3D11Buffer** VertexBuffer;
	ID3D11Buffer** IndexBuffer;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;

	AABB Local;
};

MODEL* ModelLoad(const char* FileName, float scale, bool bBlender);
void ModelRelease(MODEL* model);

void ModelDraw(MODEL* model, GameObject* gameobject);
DirectX::XMMATRIX AiToXMMATRIX(const aiMatrix4x4& m);
void BuildSkeletonHierarchy(MODEL* model, aiNode* node, int parentIndex, DirectX::XMMATRIX parentTransform);

template <typename T>
T Lerp(const T& a, const T& b, float t)
{
	return a + (b - a) * t;
}

void SaveSkeletonAsJSON(const MODEL* model, const std::string& path);
void ExportAnimation(MODEL* model, const std::string& outPath);
Animation ImportAnimation(const std::string& path);

