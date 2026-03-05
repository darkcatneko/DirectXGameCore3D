#ifndef PARTICLE_H
#define PARTICLE_H
#include "DirectXMath.h"
#include <vector>

class Particle {
public:
	Particle(float id,DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel, float time);
	void update(float deltaTime);
	void render() const;
	bool isAlive() const { return nowlifeTime < totalLifeTime; }
	float texId;
	float totalLifeTime;
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 velocity;
	float nowlifeTime;
};

class Emitter {
public:
	Emitter(Particle* particle,DirectX::XMFLOAT3 pos, float emitterLifeTime, int maxParticles);
	void update(float deltaTime);
	void render() const;
	void emit(int count);

private:
	Particle prototypeParticle;
	std::vector<Particle> particles;
	DirectX::XMFLOAT3 position;
	float EmitterLifeTime;
	int maxParticles;
};
#endif 
