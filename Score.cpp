#include "Score.h"
#include "texture.h"
#include "sprite.h"


static unsigned int g_score = 0;
static int g_Digit = 1;
static float g_x = 0.0f, g_y = 0.0f;
static int g_ScoreTexId = -1;
static constexpr int SCORE_FONT_WIDTH = 48;
static constexpr int SCORE_FONT_Height = 48;

static void drawNumber(float x, float y, int number);

void Score_Initialize(float x, float y, int digit)
{
	g_score = 0;
	g_Digit = digit;
	g_x = x;
	g_y = y;

	g_ScoreTexId = Texture_Load(L"Number.png");
}

void Score_Finalize()
{
}

void Score_Draw()
{
	unsigned int temp = g_score;
	for (int i = 0; i < g_Digit; i++)
	{
		int n = temp % 10;

		float x = g_x + (g_Digit - i - 1) * SCORE_FONT_WIDTH;
		drawNumber(x, g_y, n);
		temp /= 10;
	}
}

unsigned int Score_GetScore()
{
	return g_score;
}

void Score_AddScore(int score)
{
	g_score += score;
}

void Score_Reset()
{
	g_score = 0;
}

void drawNumber(float x, float y, int number)
{
	Sprite_Draw_UV_UI(g_ScoreTexId, x, y, SCORE_FONT_WIDTH, SCORE_FONT_Height, 1.0f, 0.1f, number, 0);
}
