/*==============================================================================

  ‹ó‚Ì•`‰æ[sky.cpp]
														 Author : Harada Ren
														 Date   : 2025/11/21
--------------------------------------------------------------------------------

==============================================================================*/
#include "sky.h"
using namespace DirectX;
#include "Model_Static.h"
#include "shader3d_unlit.h"

static MODEL_STATIC* g_pModelSky{ nullptr };
static XMFLOAT3 g_Position{};

void Sky_Initialize(){
	g_pModelSky = Model_Static_Load("sky.fbx", 100.0f, true);
}

void Sky_Finalize(){
	Model_Static_Release(g_pModelSky);
}

void Sky_SetPosition(const DirectX::XMFLOAT3& position){
	g_Position = position;
}

void Sky_Draw(){
	Shader3dUnlit_Begin();
	ModelUnlitDraw(g_pModelSky, XMMatrixTranslationFromVector(XMLoadFloat3(&g_Position)));
}
