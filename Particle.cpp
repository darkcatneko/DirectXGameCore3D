#include "Particle.h"
#include "Billboard.h"

Particle::Particle(float id, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel, float time)
	: texId(id)
	, position(pos)
	, velocity(vel)
	, nowlifeTime(0.0f)
	, totalLifeTime(time)
{
}

void Particle::update(float deltaTime)
{
	position.x += velocity.x * deltaTime;
	position.y += velocity.y * deltaTime;
	position.z += velocity.z * deltaTime;
	nowlifeTime += deltaTime;
}

void Particle::render() const
{
	Billboard_Draw(texId, position, 1-(nowlifeTime/totalLifeTime), 1-(nowlifeTime / totalLifeTime));
}

Emitter::Emitter(Particle* particle, DirectX::XMFLOAT3 pos, float emitterLifeTime, int maxParticles) :
	prototypeParticle(*particle),
	position(pos),
	EmitterLifeTime(emitterLifeTime),
	maxParticles(maxParticles)
{
}

void Emitter::update(float deltaTime)
{
	for (auto& p : particles)
	{
		p.update(deltaTime);
	}

	// 移除死亡粒子
	particles.erase(
		std::remove_if(
			particles.begin(),
			particles.end(),
			[](const Particle& p) { return !p.isAlive(); }
		),
		particles.end()
	);
}

void Emitter::render() const
{
	for (const auto& p : particles)
	{
		p.render();
	}
}

void Emitter::emit(int count)
{
	for (int i = 0; i < count; ++i)
	{
		auto Rand01 = []() {
			return static_cast<float>(rand()) / RAND_MAX;
			};

		DirectX::XMFLOAT3 vel(
			Rand01() * 2.0f - 1.0f,
			Rand01() * 2.0f - 1.0f,
			Rand01() * 2.0f - 1.0f
		);
		if (particles.size() >= maxParticles) {
			break; // 达到最大粒子数，停止发射
		}
		particles.emplace_back(
			prototypeParticle.texId,
			position,
			vel,
			prototypeParticle.totalLifeTime
		);
	}
}
