#include "particle.h"
#include "texture.h"
#include "sprite.h"
#include "direct3d.h"
using namespace DirectX;


struct Particle {
	XMFLOAT2 position;
	XMFLOAT4 color;
	float size;
	double lifeTime;
	double birthTime; //‚O‚¾‚Á‚½‚çŽg‚Á‚Ä‚¢‚È‚¢
};

static constexpr unsigned int PARTICLE_MAX = 4096;
static Particle g_Paticles[PARTICLE_MAX]{};
static int g_ParticleTexId = -1;
static double g_Time = 0.0;

void Particle_Initialize() {
	for (Particle& p : g_Paticles) {
		p.birthTime = 0.0;
	}
	g_Time = 0.0;

	g_ParticleTexId = Texture_Load(L"effect000.png");
}

void Particle_Finalize() {
}

void Particle_Update(double elapsed_time) {
	for (Particle& p : g_Paticles)
	{
		if (p.birthTime == 0.0) continue;

		p.birthTime -= elapsed_time;
		//double time = g_Time - p.birthTime;
		if (p.birthTime <= 0) {
			p.birthTime = 0.0; //Žõ–½‚ªs‚«‚½
		}
	}

	//ŽžŠÔXV
	g_Time += elapsed_time;
}

void Particle_Draw() {


	for (const Particle& p : g_Paticles) {
		if (p.birthTime == 0.0) continue;

		float screen_x = p.position.x;
		float screen_y = p.position.y;
		//Žõ–½‚Ì“à¶‚«‚½ŽžŠÔ
		float time_ratio = (float)(p.birthTime / p.lifeTime);
		float size = p.size * (time_ratio);
		float half_size = size * 0.5f;
		XMFLOAT4 color = p.color;
		color.w = p.color.w * time_ratio;

		Sprite_Draw(g_ParticleTexId, screen_x - half_size, screen_y - half_size, size, size, 1.0f, color);
	}

}

void Particle_Create(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT4& color, float size, double lifeTime) {
	for (Particle& p : g_Paticles) {
		//‹ó‚«—Ìˆæ‚ð’T‚·
		if (p.birthTime != 0.0) continue;

		p.birthTime = lifeTime;
		p.lifeTime = lifeTime;
		p.color = color;
		p.position = position;
		p.size = size;

		break;
	}
}
