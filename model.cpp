#include "assert.h"
#include "direct3d.h"
#include "texture.h"
#include "model.h"
#include "DirectXMath.h"
#include "WICTextureLoader11.h"
#include "Shader3D.h"
#include "GameObject.h"
#include <fstream>

using namespace DirectX;

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 texcoord;

	UINT boneIndex[4];
	float boneWeight[4];
};
void AddBoneWeightToVertex(Vertex& v, int idx, float w);
static int g_textureWhite = -1;
static float g_rotate;

MODEL* ModelLoad( const char *FileName,float scale,bool bBlender )
{
	MODEL* model = new MODEL;

	

	model->AiScene = aiImportFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(model->AiScene);

	model->VertexBuffer = new ID3D11Buffer*[model->AiScene->mNumMeshes];
	model->IndexBuffer = new ID3D11Buffer*[model->AiScene->mNumMeshes];

	
	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = model->AiScene->mMeshes[m];


		// 頂点バッファ生成
		{
			Vertex* vertex = new Vertex[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				if (bBlender) {
					vertex[v].position = XMFLOAT3(mesh->mVertices[v].x * scale, -mesh->mVertices[v].z * scale, mesh->mVertices[v].y * scale);
					vertex[v].normal = XMFLOAT3(mesh->mNormals[v].x, -mesh->mNormals[v].z, mesh->mNormals[v].y);
				}
				else {
					vertex[v].position = XMFLOAT3(mesh->mVertices[v].x * scale, mesh->mVertices[v].y * scale, mesh->mVertices[v].z * scale);
					vertex[v].normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				}
				vertex[v].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[v].texcoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				if (v==0)
				{
					model->Local.min = vertex[v].position;
					model->Local.max = vertex[v].position;
				}
				else
				{
					model->Local.min.x = std::min(model->Local.min.x, vertex[v].position.x);
					model->Local.min.y = std::min(model->Local.min.y, vertex[v].position.y);
					model->Local.min.z = std::min(model->Local.min.z, vertex[v].position.z);
					model->Local.max.x = std::max(model->Local.max.x, vertex[v].position.x);
					model->Local.max.y = std::max(model->Local.max.y, vertex[v].position.y);
					model->Local.max.z = std::max(model->Local.max.z, vertex[v].position.z);
				}
				for (int k = 0; k < 4; k++)
				{
					vertex[v].boneIndex[k] = 0;
					vertex[v].boneWeight[k] = 0.0f;
				}
			}

			// 2) 讀 Bone + 填權重到 vertex[vId]
			for (unsigned int i = 0; i < mesh->mNumBones; i++)
			{
				aiBone* aibone = mesh->mBones[i];
				std::string boneName = aibone->mName.C_Str();

				int boneIndex;

				// 如果這個骨頭沒出現過，建立資料
				auto it = model->boneIndex.find(boneName);
				if (it == model->boneIndex.end())
				{
					Bone bone;
					bone.name = boneName;
					bone.parentIndex = -1;
					bone.offsetMatrix = AiToXMMATRIX(aibone->mOffsetMatrix);
					bone.finalTransform = XMMatrixIdentity();

					boneIndex = (int)model->bones.size();
					model->bones.push_back(bone);
					model->boneIndex[boneName] = boneIndex;
				}
				else
				{
					boneIndex = it->second;
				}

				// 把這個 bone 的權重寫進對應頂點
				for (unsigned int j = 0; j < aibone->mNumWeights; j++)
				{
					unsigned int vId = aibone->mWeights[j].mVertexId;
					float weight = aibone->mWeights[j].mWeight;

					if (vId >= mesh->mNumVertices) continue; // 保險

					AddBoneWeightToVertex(vertex[vId], boneIndex, weight);
				}
			}
			// ★ End Read Bone & Weights

			// 3) 用「已含骨骼權重」的 vertex 建 VertexBuffer
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(Vertex) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;

			Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &model->VertexBuffer[m]);

			delete[] vertex;
		}
		


		// インデックスバッファ生成
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace* face = &mesh->mFaces[f];

				assert(face->mNumIndices == 3);

				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;

			Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &model->IndexBuffer[m]);

			delete[] index;
		}

	}

	BuildSkeletonHierarchy(model, model->AiScene->mRootNode, -1,XMMatrixIdentity());
	//生成骨骼檔案
	SaveSkeletonAsJSON(model, std::string(FileName));
	g_textureWhite = Texture_Load(L"white.png");

	//テクスチャ読み込み
	for (int i = 0; i < model->AiScene->mNumTextures; i++)
		{

			aiTexture* aitexture = model->AiScene->mTextures[i];

			ID3D11ShaderResourceView* texture;
			ID3D11Resource* resource;


			CreateWICTextureFromMemory(
				Direct3D_GetDevice(),
				Direct3D_GetContext(),
				(const uint8_t*)aitexture->pcData,
				(size_t)aitexture->mWidth,
				&resource,
				&texture
			);
			assert(texture);

			model->Texture[aitexture->mFilename.data] = texture;
	}
	
	//fbx's filebus
	const std::string modelPath(FileName);

	size_t pos = modelPath.find_last_of("/\\");
	std::string directory;

	if (pos!= std::string::npos)
	{
		directory = modelPath.substr(0, pos);
	}
	else
	{
		directory = "";
	}
	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiString filename;
	}
	//material out fbx
	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiString filename;
		aiMaterial* aimaterial = model->AiScene->mMaterials[model->AiScene->mMeshes[m]->mMaterialIndex];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &filename);

		if (filename.length ==0)
		{
			continue;
		}

		if (model->Texture.count(filename.C_Str())) 
		{
			continue;
		}

		ID3D11ShaderResourceView* texture;
		ID3D11Resource* resource;

		std::string texfilename = directory + "/" + filename.C_Str();

		int len = MultiByteToWideChar(CP_UTF8, 0, texfilename.c_str(), -1, nullptr, 0);
		wchar_t* pWideFilename = new wchar_t[len];
		MultiByteToWideChar(CP_UTF8, 0, texfilename.c_str(), -1, pWideFilename, len);

		CreateWICTextureFromFile(
			Direct3D_GetDevice(),
			Direct3D_GetContext(),
			pWideFilename,
			&resource,
			&texture);

		delete[] pWideFilename;

		assert(texture);

		resource->Release();//!!!!!!!!!!!!!!!!!

		model->Texture[filename.C_Str()] = texture;
	}
	
	//build animation
	if (model->AiScene->mNumAnimations > 0)
	{
		aiAnimation* anim = model->AiScene->mAnimations[0];
		model->animation.duration = anim->mDuration;
		model->animation.ticksPerSecond = anim->mTicksPerSecond != 0 ?
			anim->mTicksPerSecond : 25.0;

		for (unsigned int c = 0; c < anim->mNumChannels; c++)
		{
			aiNodeAnim* channel = anim->mChannels[c];
			std::string name = channel->mNodeName.C_Str();

			AnimationChannel& dst = model->animation.channels[name];

			// Position
			for (unsigned int i = 0; i < channel->mNumPositionKeys; i++)
			{
				auto& k = channel->mPositionKeys[i];
				dst.positions.push_back({ k.mTime, XMFLOAT3(k.mValue.x, k.mValue.y, k.mValue.z) });
			}

			// Rotation
			for (unsigned int i = 0; i < channel->mNumRotationKeys; i++)
			{
				auto& k = channel->mRotationKeys[i];
				dst.rotations.push_back({ k.mTime, XMFLOAT4(k.mValue.x, k.mValue.y, k.mValue.z, k.mValue.w) });
			}

			// Scale
			for (unsigned int i = 0; i < channel->mNumScalingKeys; i++)
			{
				auto& k = channel->mScalingKeys[i];
				dst.scales.push_back({ k.mTime, XMFLOAT3(k.mValue.x, k.mValue.y, k.mValue.z) });
			}
		}
	}
	for (unsigned int i = 0; i < model->AiScene->mNumAnimations; i++)
	{
		aiAnimation* anim = model->AiScene->mAnimations[i];
		//ExportAnimation(model,"Throw.anim");
	}
	return model;
}




void ModelRelease(MODEL* model)
{
	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		model->VertexBuffer[m]->Release();
		model->IndexBuffer[m]->Release();
	}

	delete[] model->VertexBuffer;
	delete[] model->IndexBuffer;


	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : model->Texture)
	{
		pair.second->Release();
	}


	aiReleaseImport(model->AiScene);


	delete model;
}

void ModelDraw(MODEL* model, GameObject* gameobject)
{
	XMFLOAT3 gameobjectPos = gameobject->transform.Position;
	XMFLOAT3 gameobjectRot = gameobject->transform.Rotation;
	XMFLOAT3 gameobjectScale = gameobject->transform.Scale;
	Shader3D_Begin();

	
	// プリミティブトポロジ設定
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//world matrix
		//XMMATRIX mtxWorld = XMMatrixIdentity();
	XMMATRIX mtxTrans = XMMatrixTranslation(gameobjectPos.x, gameobjectPos.y, gameobjectPos.z);
	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw
		(XMConvertToRadians(gameobjectRot.x),
		 XMConvertToRadians(gameobjectRot.y),
		 XMConvertToRadians(gameobjectRot.z));
	XMMATRIX mtxScale = XMMatrixScaling(gameobjectScale.x, gameobjectScale.y, gameobjectScale.z);
	XMMATRIX mtxWorld = mtxScale * mtxRot * mtxTrans;
	Shader3D_SetWorldMatrix(mtxWorld);
	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++) {

		if (model->AiScene->mNumTextures)
		{
			aiString texture;
			aiMaterial* aimaterial = model->AiScene->mMaterials[model->AiScene->mMeshes[m]->mMaterialIndex];
			aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);
			if (texture != aiString(""))
			{
				Direct3D_GetContext()->PSSetShaderResources(0, 1, &model->Texture[texture.data]);
			}
		}
		else
		{
			Texture_SetTexture(g_textureWhite);
		}

		aiMaterial* aimaterial = model->AiScene->mMaterials[model->AiScene->mMeshes[m]->mMaterialIndex];
		aiColor3D diffuse;
		aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		Shader3d_SetColor({ diffuse.r, diffuse.g, diffuse.b, 1.0f });



		// 頂点バッファを描画パイプラインに設定
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Direct3D_GetContext()->IASetVertexBuffers(0, 1, &model->VertexBuffer[m], &stride, &offset);
		Direct3D_GetContext()->IASetIndexBuffer(model->IndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// ポリゴン描画命令発行
		Direct3D_GetContext()->DrawIndexed(model->AiScene->mMeshes[m]->mNumFaces * 3, 0, 0);
	}
}

DirectX::XMMATRIX AiToXMMATRIX(const aiMatrix4x4& m)
{
	return DirectX::XMMatrixTranspose(DirectX::XMMATRIX(
		m.a1, m.a2, m.a3, m.a4,
		m.b1, m.b2, m.b3, m.b4,
		m.c1, m.c2, m.c3, m.c4,
		m.d1, m.d2, m.d3, m.d4
	));
}

void BuildSkeletonHierarchy(MODEL* model, aiNode* node, int parentIndex, XMMATRIX parentTransform)
{
	std::string nodeName = node->mName.C_Str();

	XMMATRIX local = AiToXMMATRIX(node->mTransformation);
	XMMATRIX global = parentTransform * local;

	int currentBoneIndex = parentIndex;

	// 這個 node 是否對應到一根骨頭？
	auto it = model->boneIndex.find(nodeName);
	if (it != model->boneIndex.end())
	{
		currentBoneIndex = it->second;
		model->bones[currentBoneIndex].parentIndex = parentIndex;

		model->bones[currentBoneIndex].globalBind = global;
	}

	// 遞迴子節點
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		BuildSkeletonHierarchy(model, node->mChildren[i], currentBoneIndex,global);
	}
}

XMVECTOR Lerp(const XMVECTOR& a, const XMVECTOR& b, float t)
{
	return a + (b - a) * t;
}

void SaveSkeletonAsJSON(const MODEL* model, const std::string& path)
{
	if(model->bones.size()==0)return;

	std::string name = std::string(path);

	// 刪掉路徑
	size_t slashPos = name.find_last_of("/\\");
	if (slashPos != std::string::npos)
		name = name.substr(slashPos + 1);

	// 刪掉副檔名 .fbx
	size_t dotPos = name.find_last_of('.');
	if (dotPos != std::string::npos)
		name = name.substr(0, dotPos);

	// 移除空白
	name.erase(std::remove(name.begin(), name.end(), ' '), name.end());

	// 加上新的副檔名
	name += ".skel";



	std::ofstream file(name, std::ios::binary);
	if (!file)
		return;

	int32_t count = (int32_t)model->bones.size();
	file.write((char*)&count, sizeof(int32_t));

	for (const Bone& b : model->bones)
	{
		// Name
		int32_t len = (int32_t)b.name.size();
		file.write((char*)&len, sizeof(int32_t));
		file.write(b.name.data(), len);

		// Parent
		file.write((char*)&b.parentIndex, sizeof(int32_t));

		// Offset matrix
		DirectX::XMFLOAT4X4 mat;
		DirectX::XMStoreFloat4x4(&mat, b.offsetMatrix);
		file.write((char*)&mat, sizeof(float) * 16);
	}

	file.close();
}

void ExportAnimation(MODEL* model, const std::string& outPath)
{
	const Animation& anim = model->animation;

	std::ofstream file(outPath, std::ios::binary);
	if (!file.is_open()) return;

	// --- duration ---
	file.write((char*)&anim.duration, sizeof(double));
	double arrange = anim.ticksPerSecond * 2.0; //調整動畫速度
	file.write((char*)&arrange, sizeof(double));

	// --- channel count ---
	int32_t channelCount = anim.channels.size();
	file.write((char*)&channelCount, sizeof(int32_t));

	// --- 正確的寫 channel（boneName 在 for 裡） ---
	for (auto& kv : anim.channels)
	{
		const std::string& boneName = kv.first;
		const AnimationChannel& channel = kv.second;

		// boneName
		int32_t nameLen = boneName.size();
		file.write((char*)&nameLen, 4);
		file.write(boneName.c_str(), nameLen);

		// positions
		int32_t posCount = channel.positions.size();
		file.write((char*)&posCount, 4);
		for (auto& p : channel.positions)
		{
			file.write((char*)&p.first, sizeof(double));
			file.write((char*)&p.second, sizeof(XMFLOAT3));
		}

		// rotations
		int32_t rotCount = channel.rotations.size();
		file.write((char*)&rotCount, 4);
		for (auto& r : channel.rotations)
		{
			file.write((char*)&r.first, sizeof(double));
			file.write((char*)&r.second, sizeof(XMFLOAT4));
		}

		// scales
		int32_t scaleCount = channel.scales.size();
		file.write((char*)&scaleCount, 4);
		for (auto& s : channel.scales)
		{
			file.write((char*)&s.first, sizeof(double));
			file.write((char*)&s.second, sizeof(XMFLOAT3));
		}
	}

	file.close();
}
Animation ImportAnimation(const std::string& path)
{
	Animation anim;
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		//std::cout << "Failed to open anim file: " << path << std::endl;
		return anim;
	}

	// Duration & ticksPerSecond
	file.read((char*)&anim.duration, sizeof(double));
	file.read((char*)&anim.ticksPerSecond, sizeof(double));

	// Channel count
	int32_t channelCount = 0;
	file.read((char*)&channelCount, sizeof(int32_t));

	for (int i = 0; i < channelCount; i++)
	{
		AnimationChannel channel;

		// Bone name
		int32_t nameLen;
		file.read((char*)&nameLen, sizeof(int32_t));

		std::string boneName;
		boneName.resize(nameLen);
		file.read(&boneName[0], nameLen);

		// Positions
		int32_t posCount = 0;
		file.read((char*)&posCount, sizeof(int32_t));
		for (int p = 0; p < posCount; p++)
		{
			double t;
			XMFLOAT3 v;
			file.read((char*)&t, sizeof(double));
			file.read((char*)&v, sizeof(XMFLOAT3));
			channel.positions.push_back({ t, v });
		}

		// Rotations
		int32_t rotCount = 0;
		file.read((char*)&rotCount, sizeof(int32_t));
		for (int r = 0; r < rotCount; r++)
		{
			double t;
			XMFLOAT4 q;
			file.read((char*)&t, sizeof(double));
			file.read((char*)&q, sizeof(XMFLOAT4));
			channel.rotations.push_back({ t, q });
		}

		// Scales
		int32_t scaleCount = 0;
		file.read((char*)&scaleCount, sizeof(int32_t));
		for (int s = 0; s < scaleCount; s++)
		{
			double t;
			XMFLOAT3 v;
			file.read((char*)&t, sizeof(double));
			file.read((char*)&v, sizeof(XMFLOAT3));
			channel.scales.push_back({ t, v });
		}

		anim.channels[boneName] = channel;
	}

	file.close();
	return anim;
}
void AddBoneWeightToVertex(Vertex& v, int idx, float w)
{
	for (int i = 0; i < 4; i++) {
		if (v.boneWeight[i] == 0.0f) {
			v.boneIndex[i] = idx;
			v.boneWeight[i] = w;
			return;
		}
	}
}







