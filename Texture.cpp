#include "Texture.h"
#include <corecrt_wstring.h>
#include <string>
#include "direct3d.h"
#include "WICTextureLoader11.h"
using namespace DirectX;

static constexpr int TEXTURE_MAX = 512;//texture管理最大値

struct Texture
{
	std::wstring  filename;
	unsigned int width;
	unsigned int height;
	ID3D11ShaderResourceView* g_pTextureView;
	ID3D11Resource* pTexture;

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
		t.g_pTextureView = nullptr;
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
		if (g_Textures[i].g_pTextureView)continue;


		////テキスチャーの読み込み
		//TexMetadata metadata;
		//ScratchImage image;

		std::wstring filename = std::wstring(L"resource/") + std::wstring(pFilename);
		//HRESULT hr = LoadFromWICFile(filename.c_str(), WIC_FLAGS_NONE, &metadata, image);

		HRESULT hr;

		hr = CreateWICTextureFromFile(g_pDevice, g_pContext, filename.c_str(), &g_Textures[i].pTexture, &g_Textures[i].g_pTextureView);

		ID3D11Texture2D* pTexture = (ID3D11Texture2D*)g_Textures[i].pTexture;
		D3D11_TEXTURE2D_DESC t2desc;
		pTexture->GetDesc(&t2desc);
		g_Textures[i].width = t2desc.Width;
		g_Textures[i].height = t2desc.Height;

		if (FAILED(hr))
		{
			MessageBoxW(nullptr, L"NO FILE", pFilename, MB_OK);
			return -1;
		}
		g_Textures[i].filename = pFilename;
		
		return i;
	}
	return -1;
}

void Texture_AllRelease()
{
	for (Texture& t : g_Textures)
	{
		t.filename.clear();
		SAFE_RELEASE(t.g_pTextureView);
	}
}

void Texture_SetTexture(int texid, int slot)
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
	g_pContext->PSSetShaderResources(slot, 1, &g_Textures[texid].g_pTextureView);
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
