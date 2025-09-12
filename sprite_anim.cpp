#include "sprite_anim.h"
#include "sprite.h"
#include "Texture.h"
#include "GameWindow.h"
#include "DirectXMath.h"
#include <string.h>

static int g_PatternMax = 13;
static int g_PatternNum = 0;
int anim_texid = -1;
double frame = 0.0;



static constexpr int ANIM_PATTERN_MAX = 256;
static AnimPatternData g_animdata_data[ANIM_PATTERN_MAX];
static constexpr int ANIM_PLAY_MAX = 512;
static AnimPlayData g_ANIM_PLAY[ANIM_PLAY_MAX];

static constexpr double ROBOT_ONEPIC = 32.0f / 512.0f;

void SpriteAnim_Initialize()
{
	for (int i = 0; i < ANIM_PLAY_MAX; i++)
	{
		g_ANIM_PLAY[i].m_IsStopped = false;
	}
}

void SpriteAnim_Finitialize()
{
	g_PatternNum = (g_PatternNum + 1) % g_PatternMax;
}

void SpriteAnim_Update(double elapsed_time)
{
	/*if (frame >= 0.1)
	{
		g_PatternNum = (g_PatternNum + 1) % g_PatternMax;
		frame = 0;
	}*/
	//frame += elapsed_time;
	for (int i = 0; i < ANIM_PLAY_MAX; i++)
	{
		if (g_ANIM_PLAY[i].m_patternId < 0)continue;
		AnimPatternData* p_patterndata = &g_animdata_data[g_ANIM_PLAY[i].m_patternId];
		if (g_ANIM_PLAY[i].pass_frame >= p_patterndata->anim_speed)
		{
			g_ANIM_PLAY[i].m_PatternNum++;
			if (g_ANIM_PLAY[i].m_PatternNum >= p_patterndata->m_PatternMax)
			{
				if (p_patterndata->m_IsLoop)
				{
					g_ANIM_PLAY[i].m_PatternNum = 0;
				}
				else
				{
					g_ANIM_PLAY[i].m_PatternNum = p_patterndata->m_PatternMax - 1;
					g_ANIM_PLAY[i].m_IsStopped = true;
				}

			}
			g_ANIM_PLAY[i].pass_frame -= p_patterndata->anim_speed;

		}
		g_ANIM_PLAY[i].pass_frame += elapsed_time;
	}

}

void SpriteAnim_Draw(int animeID, float posx, float poxy, XMFLOAT4 color)
{
	Texture_SetTexture(g_animdata_data[g_ANIM_PLAY[animeID].m_patternId].m_TextureId);
	Sprite_Draw_Move_STRUCT(posx, poxy,
		g_animdata_data[g_ANIM_PLAY[animeID].m_patternId].m_Sprite_Size.x, g_animdata_data[g_ANIM_PLAY[animeID].m_patternId].m_Sprite_Size.y,
		g_animdata_data[g_ANIM_PLAY[animeID].m_patternId].m_Pattern_Size.x, g_animdata_data[g_ANIM_PLAY[animeID].m_patternId].m_Pattern_Size.y,
		g_ANIM_PLAY[animeID].m_PatternNum % g_animdata_data[g_ANIM_PLAY[animeID].m_patternId].m_PatternHMax, g_animdata_data[g_ANIM_PLAY[animeID].m_patternId].m_uv_patternStartPos_y + g_ANIM_PLAY[animeID].m_PatternNum / g_animdata_data[g_ANIM_PLAY[animeID].m_patternId].m_PatternHMax, g_animdata_data[g_ANIM_PLAY[animeID].m_patternId].m_uv_patternStartPos_x, g_animdata_data[g_ANIM_PLAY[animeID].m_patternId].gamma, color);
	/*Texture_SetTexture(Texture_Load(L"Explosion.png"));
	Sprite_Draw_Move_STRUCT(0, 0, 250, 250, 0.25f, 0.25f, 0, 0, 0);*/
}

//void SpriteRobotDraw()
//{
//	Texture_SetTexture(g_animdata_data[0].m_TextureId);
//	//float onepic = 32.0f / 512.0f;
//	if (return_face() == TRUE)
//	{
//		Sprite_Draw_Move_STRUCT(return_vector().x, return_vector().y,
//			g_animdata_data[0].m_Sprite_Size.x, g_animdata_data[0].m_Sprite_Size.y,
//			g_animdata_data[0].m_Pattern_Size.x, g_animdata_data[0].m_Pattern_Size.y,
//			g_ANIM_PLAY[0].m_PatternNum, 0, g_animdata_data[0].m_uv_patternStartPos_x);
//
//	}
//	else
//	{
//		Sprite_Draw_Move_STRUCT(return_vector().x, return_vector().y,
//			g_animdata_data[0].m_Sprite_Size.x, g_animdata_data[0].m_Sprite_Size.y,
//			g_animdata_data[0].m_Pattern_Size.x, g_animdata_data[0].m_Pattern_Size.y,
//			g_ANIM_PLAY[0].m_PatternNum, 1, g_animdata_data[0].m_uv_patternStartPos_x);
//	}
//}

int AnimDataInitialize(const wchar_t* name, int patternMax, int patternHMax, int patternstart_x, int patternstart_y, DirectX::XMFLOAT2 StartPosition, DirectX::XMFLOAT2 Pattern_Size, DirectX::XMFLOAT2 Sprite_Size, bool isloop, double animation_speed,float gamma)
{
	for (int i = 0; i < ANIM_PATTERN_MAX; i++)
	{
		if (g_animdata_data[i].m_TextureId >= 0)continue;
		g_animdata_data[i].m_TextureId = Texture_Load(name);
		g_animdata_data[i].m_PatternMax = patternMax;
		g_animdata_data[i].m_PatternHMax = patternHMax;
		g_animdata_data[i].anim_speed = animation_speed;
		g_animdata_data[i].m_uv_patternStartPos_x = patternstart_x;
		g_animdata_data[i].m_uv_patternStartPos_y = patternstart_y;
		g_animdata_data[i].m_Pattern_Size = { Pattern_Size.x,Pattern_Size.y };
		g_animdata_data[i].m_Sprite_Size = { Sprite_Size.x,Sprite_Size.y };
		g_animdata_data[i].m_StartPosition = { StartPosition.x,StartPosition.y };
		g_animdata_data[i].m_IsLoop = isloop;
		g_animdata_data[i].gamma = gamma;
		//SpriteAnim_CreatePlayer(i, { StartPosition.x,StartPosition.y });
		//g_ANIM_PLAY[i].m_patternId = i;
		//g_ANIM_PLAY[i].now_vertex = { StartPosition.x,StartPosition.y };
		return i;

	}
	return -1;
}

int SpriteAnim_CreatePlayer(int anim_pattern_id, DirectX::XMFLOAT2 startpos)
{
	for (int i = 0; i < ANIM_PLAY_MAX; i++)
	{
		if (g_ANIM_PLAY[i].m_patternId >= 0)continue;
		g_ANIM_PLAY[i].m_patternId = anim_pattern_id;
		g_ANIM_PLAY[i].pass_frame = 0.0f;
		g_ANIM_PLAY[i].m_PatternNum = 0;
		g_ANIM_PLAY[i].now_vertex = { startpos.x,startpos.y };
		g_ANIM_PLAY[i].m_IsStopped = false;
		return i;
	}

}

bool SpriteAnim_IsStopped(int index)
{
	return g_ANIM_PLAY[index].m_IsStopped;
}

void SpriteAnim_DestroyPlayer(int index)
{
	g_ANIM_PLAY[index].m_patternId = -1;
}



