#include "ParticleSystem.h"



ParticleSystem::ParticleSystem()
{
}


ParticleSystem::~ParticleSystem()
{
	for (int i = 0; i < m_StickConstraint.size(); i++)
	{
		if (m_StickConstraint[i]) delete m_StickConstraint[i];
	}
}

void ParticleSystem::Update(float dt)
{
	//printf("dt: %f\n", dt);
	AccumulateForces();
	Verlet(dt);
	StatisfyConstraints();
}

void ParticleSystem::Init()
{
	m_vGravity = XMFLOAT3(0.f, -0.50f, 0.f);
	const float length = 1.0f;
	const float size = length / (static_cast<float>(PARTICLE_DIM - 1.0f));
	float height = .50f;

	//build vertices 
	uint32_t ii = 0;
	for (uint32_t zz = 0; zz < PARTICLE_DIM; ++zz) {
		for (uint32_t xx = 0; xx < PARTICLE_DIM; ++xx) {
			m_pos[ii] = XMFLOAT3(
				static_cast<float>(xx) * size - (length / 2),
				height,
				static_cast<float>(zz) * size - (length / 2)
			);
			m_oldPos[ii] = m_pos[ii];
			ii++;

		}
	}

	//build stick constraint 
	m_restLength = size;
	//save edge constraint 
	for (uint32_t xx = 0; xx < PARTICLE_DIM; xx++) {
		m_EdgeConstraint[xx] = m_pos[xx];
	}

	//build stick constraints (horizontal sticks)
	for (uint32_t zz = 0; zz < PARTICLE_DIM; zz++)
	{
		for (uint32_t xx = 0; xx < PARTICLE_DIM - 1; xx++)
		{
			uint32_t particleA = xx + PARTICLE_DIM * zz;
			uint32_t particleB = particleA + 1;

			StickConstraint* pStick = new StickConstraint(particleA, particleB);
			m_StickConstraint.push_back(pStick);
		}
	}
	//build stick constraints (vertical sticks)
	for (uint32_t xx = 0; xx < PARTICLE_DIM; xx++)
	{
		for (uint32_t zz = 0; zz < PARTICLE_DIM - 1; zz++)
		{
			uint32_t p0 = xx + PARTICLE_DIM * zz;
			uint32_t p1 = p0 + PARTICLE_DIM;

			StickConstraint* pStick = new StickConstraint(p0, p1);
			m_StickConstraint.push_back(pStick);
		}
	}
}

XMFLOAT3 & ParticleSystem::GetParticlesPos(uint32_t ii)
{
	return m_pos[ii];
}

void ParticleSystem::Verlet(float dt)
{
	float f1 = 1.0f;
	float f2 = 1.0f;

	for (int i = 0; i < NUM_PARTICLES; i++) 
	{
		XMFLOAT3& pos = m_pos[i];
		XMFLOAT3 temp = pos;
		XMFLOAT3& oldPos = m_oldPos[i];
		XMFLOAT3& acceleration = m_acceleration[i];
		//Integrate old pos and new pos with acceleration 
		pos.x += f1 * pos.x - f2 * oldPos.x + acceleration.x * dt * dt;
		pos.y += f1 * pos.y - f2 * oldPos.y + acceleration.y * dt * dt;
		pos.z += f1 * pos.z - f2 * oldPos.z + acceleration.z * dt * dt;

		oldPos = temp;
		m_acceleration[i] = XMFLOAT3(0.f, 0.0f, 0.f);
	}
}

void ParticleSystem::StatisfyConstraints()
{
	//statisfy edge constraint c1
	for (uint32_t xx = 0; xx < PARTICLE_DIM; xx++)
	{
		m_pos[xx] = m_EdgeConstraint[xx];
	}
	for (int j = 0; j < NUM_ITERATIONS; j++)
	{
		//statisfy c2 (stick constraints)
		for(auto it = m_StickConstraint.begin(); it != m_StickConstraint.end(); ++it)
		{
			StickConstraint* pStick = *it;
			XMFLOAT3& x1 = m_pos[pStick->particleA];
			XMFLOAT3& x2 = m_pos[pStick->particleB];

			XMFLOAT3 delta;
			delta.x = x2.x - x1.x;
			delta.y = x2.y - x1.y;
			delta.z = x2.z - x1.z;

			float deltalength = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
			float diff = (deltalength - m_restLength) / deltalength;
			//float diff = delta * (1 - m_restLength / deltalength);

			x1.x += delta.x * 0.5f * diff;
			x1.y += delta.y * 0.5f * diff;
			x1.z += delta.z * 0.5f * diff;

			x2.x -= delta.x * 0.5f * diff;
			x2.y -= delta.y * 0.5f * diff;
			x2.z -= delta.z * 0.5f * diff;
		}

		//staisfy sphere constraint
		for (uint32_t ii = 0; ii < NUM_PARTICLES; ii++)
		{
			//Calc P' = Center + ContactNormal * Radius
			float bias = 0.010f;
			float sphereRadius = 0.20f + bias;
			XMFLOAT3 sphereCenter = XMFLOAT3(0.f, 0.f, 0.f);
			XMFLOAT3& p = m_pos[ii];
			XMFLOAT3 contactNormal;

			contactNormal.x = p.x - sphereCenter.x;
			contactNormal.y = p.y - sphereCenter.y;
			contactNormal.z = p.z - sphereCenter.z;

			float deltaLength = sqrtf(
				contactNormal.x * contactNormal.x + 
				contactNormal.y * contactNormal.y + 
				contactNormal.z * contactNormal.z);

			if (deltaLength < sphereRadius)
			{
				contactNormal.x /= deltaLength;
				contactNormal.y /= deltaLength;
				contactNormal.z /= deltaLength;

				m_pos[ii].x = sphereCenter.x + (sphereRadius * contactNormal.x);
				m_pos[ii].y = sphereCenter.y + (sphereRadius * contactNormal.y);
				m_pos[ii].z = sphereCenter.z + (sphereRadius * contactNormal.z);
			}
		}
	}
}

void ParticleSystem::AccumulateForces()
{
	for (int i = 0; i < NUM_PARTICLES; i++) 
	{
		m_acceleration[i] = m_vGravity;

	}
}
