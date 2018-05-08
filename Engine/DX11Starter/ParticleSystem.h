#pragma once
#include <DirectXMath.h>
#include <DirectXHelpers.h>
#include <stdio.h>
#include <vector>

using namespace DirectX;

class ParticleSystem
{

	struct StickConstraint
	{
		StickConstraint(uint32_t pA, uint32_t pB)
		{
			this->particleA = pA;
			this->particleB = pB;
		}
		uint32_t particleA;
		uint32_t particleB;
	};


	static const uint32_t PARTICLE_DIM = 32;
	static const uint32_t NUM_PARTICLES = PARTICLE_DIM * PARTICLE_DIM;
	static const uint32_t NUM_ITERATIONS = 8;
	XMFLOAT3 m_pos[NUM_PARTICLES];
	XMFLOAT3 m_oldPos[NUM_PARTICLES];
	XMFLOAT3 m_acceleration[NUM_PARTICLES];
	XMFLOAT3 m_vGravity;

	XMFLOAT3 m_EdgeConstraint[PARTICLE_DIM];
	std::vector<StickConstraint*> m_StickConstraint;
	float m_restLength;

public:
	ParticleSystem();
	~ParticleSystem();
	void Update(float dt);
	void Init();
	XMFLOAT3& GetParticlesPos(uint32_t ii);
	XMFLOAT3* GetEdge() { return m_EdgeConstraint; }
private:
	void Verlet(float dt);
	void StatisfyConstraints();
	void AccumulateForces();
};

