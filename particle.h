#ifndef PARTICLE_H
#define PARTICLE_H

#include <DirectXMath.h>

void Particle_Initialize();
void Particle_Finalize();
void Particle_Update(double elapsed_time);
void Particle_Draw();

void Particle_Create(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT4& color,
	float size, double lifeTime);


#endif //PARTICLE_H