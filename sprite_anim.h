#include "d3d11.h"
#include "DirectXMath.h"
#ifndef SPRITE_ANIM
#define SPRITE_ANIM

struct AnimPatternData
{
	int m_TextureId = -1; //sprite id
	int m_PatternMax = 13;//sprite pattern max
	int m_PatternHMax = 0;
	//int m_StartPosition = 0;//sprite start pos(橫條)
	DirectX::XMFLOAT2 m_StartPosition = { 0,0 };//sprite start pos(橫條)
	DirectX::XMFLOAT2 m_Pattern_Size = { 0.0f,0.0f };//sptire pattern Size
	DirectX::XMFLOAT2 m_Sprite_Size = { 0.0f,0.0f };
	double anim_speed = 0.1;
	int m_uv_patternStartPos_x = 0;
	int m_uv_patternStartPos_y = 0;
	bool m_IsLoop = TRUE;
	float gamma;
};
struct AnimPlayData
{
	int m_patternId = -1;
	double pass_frame = 0.0;
	int m_PatternNum = 0;
	DirectX::XMFLOAT2 now_vertex;
	bool m_IsStopped;
};
void SpriteAnim_Initialize();
void SpriteAnim_Finitialize();
void SpriteAnim_Update(double elapsed_time);
void SpriteAnim_Draw(int animeID, float posx, float poxy, DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });
//void SpriteRobotDraw();
int AnimDataInitialize(const wchar_t* name, int patternMax, int patternHMax, int patternstart_x, int patternstart_y, DirectX::XMFLOAT2 StartPosition, DirectX::XMFLOAT2 Pattern_Size, DirectX::XMFLOAT2 Sprite_Size, bool isloop = true, double animation_speed = 0.1, float gamma=1.0f);
int SpriteAnim_CreatePlayer(int anim_pattern_id, DirectX::XMFLOAT2 startpos);

bool SpriteAnim_IsStopped(int index);
void SpriteAnim_DestroyPlayer(int index);
#endif // !SPRITE_ANIM

