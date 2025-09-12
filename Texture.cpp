#include "Texture.h"
#include <corecrt_wstring.h>
#include <string>
#include "DirectXTex.h"[
#include "direct3d.h"]
using namespace DirectX;

static constexpr int TEXTURE_MAX = 512;//texture管理最大値

struct Texture
{
	std::wstring  filename;
	ID3D11ShaderResourceView* g_pTexture;
	unsigned int width;
	unsigned int height;

};
static Texture g_Textures[TEXTURE_MAX];
static unsigned int g_SetTextureIndex = -1;
// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;


void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	for (Texture& t : g_Textures)
	{
		t.g_pTexture = nullptr;
	}
	g_SetTextureIndex = -1;
	g_pDevice = pDevice;
	g_pContext = pContext;
}

void Texture_Finalize(void)
{
	Texture_AllRelease();
}

int Texture_Load(const wchar_t* pFilename)
{
	//既に読み込みんだファイルは読み込みまない
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Textures[i].filename == pFilename)
		{
			return i;
		}
	}

	//空いている管理領域を探す
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Textures[i].g_pTexture)continue;

		//テキスチャーの読み込み
		TexMetadata metadata;
		ScratchImage image;

		std::wstring filename = std::wstring(L"resource/") + std::wstring(pFilename);
		HRESULT hr = LoadFromWICFile(filename.c_str(), WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr))
		{
			MessageBoxW(nullptr, L"NO FILE", pFilename, MB_OK);
			return -1;
		}
		g_Textures[i].filename = pFilename;
		g_Textures[i].width = metadata.width;
		g_Textures[i].height = metadata.height;
		hr = CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, &g_Textures[i].g_pTexture);
		return i;
	}
	return -1;
}

void Texture_AllRelease()
{
	for (Texture& t : g_Textures)
	{
		t.filename.clear();
		SAFE_RELEASE(t.g_pTexture);
	}
}

void Texture_SetTexture(int texid)
{
	if (texid < 0)
	{
		return;
	}
	/*if (g_SetTextureIndex == texid)
	{
		return;
	}*/
	g_SetTextureIndex = texid;
	g_pContext->PSSetShaderResources(0, 1, &g_Textures[texid].g_pTexture);
}

unsigned int Texture_Width(int texid)
{
	if (texid < 0)
	{
		return 0;
	}
	return g_Textures[texid].width;
}

unsigned int Texture_Height(int texid)
{
	if (texid < 0)
	{
		return 0;
	}
	return g_Textures[texid].height;
}
