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


struct  Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 texcoord;
};
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
			}

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

			//Read bone
			for (unsigned int i = 0; i < mesh->mNumBones; i++)
			{
				aiBone* aibone = mesh->mBones[i];
				std::string boneName = aibone->mName.C_Str();

				// 如果這個骨頭沒出現過，建立資料
				if (!model->boneIndex.count(boneName))
				{
					Bone bone;
					bone.name = boneName;
					bone.parentIndex = -1;
					bone.offsetMatrix = AiToXMMATRIX(aibone->mOffsetMatrix);
					bone.finalTransform = XMMatrixIdentity();

					model->boneIndex[boneName] = model->bones.size();
					model->bones.push_back(bone);
				}
			}
			//End read bone
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

	BuildSkeletonHierarchy(model, model->AiScene->mRootNode, -1);
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
		ExportAnimation(anim, model, std::string(FileName));
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
	Shader3D_Begin();

	
	// プリミティブトポロジ設定
	//g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//world matrix
		//XMMATRIX mtxWorld = XMMatrixIdentity();
	XMMATRIX mtxTrans = XMMatrixTranslation(gameobjectPos.x, gameobjectPos.y, gameobjectPos.z);
	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(gameobjectRot.x, gameobjectRot.y, gameobjectRot.z);
	XMMATRIX mtxScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
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
	return DirectX::XMMATRIX(
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4
	);
}

void BuildSkeletonHierarchy(MODEL* model, aiNode* node, int parentIndex)
{
	std::string n = node->mName.C_Str();

	if (model->boneIndex.count(n))
	{
		int index = model->boneIndex[n];
		model->bones[index].parentIndex = parentIndex;

		parentIndex = index;
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
		BuildSkeletonHierarchy(model, node->mChildren[i], parentIndex);
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

void ExportAnimation(aiAnimation* anim, MODEL* model, const std::string& outPath)
{

	std::string name = std::string(outPath);

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
	name += ".anim";


	std::ofstream file(name, std::ios::binary);

	// 1. 動畫名稱
	std::string animName = anim->mName.length > 0 ?
		anim->mName.C_Str() : "Animation";

	int32_t nameLen = animName.size();
	file.write((char*)&nameLen, 4);
	file.write(animName.c_str(), nameLen);

	// 2. 動畫持續秒數
	float duration = (float)anim->mDuration;
	float ticksPerSecond =
		(float)(anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 30.0f);

	float durationSeconds = duration / ticksPerSecond;
	file.write((char*)&durationSeconds, 4);

	// 3. 寫入 bone 數量（與 Skeleton 對應）
	int32_t boneCount = (int32_t)model->bones.size();
	file.write((char*)&boneCount, 4);

	// 4. 每個 bone 的 keyframes
	for (int b = 0; b < boneCount; b++)
	{
		std::string boneName = model->bones[b].name;
		aiNodeAnim* channel = nullptr;

		// 找對應的 channel
		for (unsigned int c = 0; c < anim->mNumChannels; c++)
		{
			if (boneName == anim->mChannels[c]->mNodeName.C_Str())
			{
				channel = anim->mChannels[c];
				break;
			}
		}

		if (!channel)
		{
			int32_t zero = 0;
			file.write((char*)&zero, 4);
			continue;
		}

		int keyCount = std::max({
	(int)channel->mNumPositionKeys,
	(int)channel->mNumRotationKeys,
	(int)channel->mNumScalingKeys
			});

		file.write((char*)&keyCount, 4);

		for (int k = 0; k < keyCount; k++)
		{
			// 時間：以 Position Keys 的時間為主
			int posIndex = std::min(k, (int)channel->mNumPositionKeys - 1);
			float t = (float)channel->mPositionKeys[posIndex].mTime / ticksPerSecond;
			file.write((char*)&t, 4);

			// Translation
			aiVector3D p = channel->mPositionKeys[posIndex].mValue;
			float T[3] = { p.x, p.y, p.z };
			file.write((char*)T, sizeof(T));

			// Rotation
			int rotIndex = std::min(k, (int)channel->mNumRotationKeys - 1);
			aiQuaternion q = channel->mRotationKeys[rotIndex].mValue;
			float R[4] = { q.x, q.y, q.z, q.w };
			file.write((char*)R, sizeof(R));

			// Scale
			int sIndex = std::min(k, (int)channel->mNumScalingKeys - 1);
			aiVector3D sc = channel->mScalingKeys[sIndex].mValue;
			float S[3] = { sc.x, sc.y, sc.z };
			file.write((char*)S, sizeof(S));
		}
	}

	file.close();
}





