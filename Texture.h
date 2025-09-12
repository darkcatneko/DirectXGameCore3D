
/************************************************************
 *                                                          *
 *       |\---\|                                            *
 *       | o_o |  「			眠い				」      *
 *															*
 *                                                          *
 *   Project : First        DirectX
 *   Author  : 賴鈺翔 (Yu-Hsiang Lai)                       *
 *   Created : 2025/06/13                                   *
 *   Purpose :
 *                                                          *
 ************************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d11.h>

void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Texture_Finalize(void);

int Texture_Load(const wchar_t* pFilename);

void Texture_AllRelease();

void Texture_SetTexture(int texid);

unsigned int Texture_Width(int texid);
unsigned int Texture_Height(int texid);

#endif