#ifndef SPRITE_H
#define SPRITE_H

#include <d3d11.h>
#include <DirectXMath.h>
#include "sprite_anim.h"
using namespace DirectX;
// 頂点構造体
struct Vertex
{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT4 color;
	XMFLOAT2 uv;
};
void gamma_function(float gamma);
void Sprite_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Sprite_Finalize(void);
void Sprite_Begin(void);
void Sprite_Draw(int textureID, float dx, float dy, float sp_w, float sp_h, float gamma = 1.0f, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });
void Sprite_Draw_Rotate(int textureID, float dx, float dy, float sp_w, float sp_h, float angle, float gamma = 1.0f, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });
void Sprite_Draw_Y_Percentage(int textureID, float dx, float dy, float sp_w, float sp_h, float percent, float gamma = 1.0f, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });
void Sprite_Draw_time(float dx, float dy, float sp_w, float sp_h);
void Sprite_Draw_UV(int textureID, float dx, float dy, float sp_w, float sp_h, float uv_h, float uv_w, float row, float column);
void Sprite_Draw_UV_UI(int textureID, float dx, float dy, float sp_w, float sp_h, float uv_h, float uv_w, float row, float column, float gamma = 1.0f, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });
void Sprite_Draw_time_UV(float dx, float dy, float sp_w, float sp_h, float uv_h, float uv_w, float row, float column);
void Sprite_Draw_time_UV(float dx, float dy, float sp_w, float sp_h, float uv_h, float uv_w, float row, float column, bool face_right);//0618轉圖片方向
void Sprite_Draw_Move_STRUCT(float dx, float dy, float sp_w, float sp_h, float uv_w, float uv_h, float row, float column, float startpos_x, float gamma, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });
void Sprite_Draw_Move_Rotate_STRUCT(AnimPlayData& playdata, float dx, float dy, float sp_w, float sp_h, float angle, float uv_w, float uv_h, float row, float column, float startpos_x);
#endif