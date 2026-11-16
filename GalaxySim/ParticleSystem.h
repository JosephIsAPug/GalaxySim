#pragma once
#include <vector>
#include <algorithm>

#include <SFML/Graphics.hpp>

#include "data.h"
#include "Mis.h"
#include "SFML functions.h"

class ParticleSystem
{
public:
	class Particle {
	public:
		Vector3 pos;
		Vector3 vel;
		Vector3 acc;
		float mass;
		Particle(Vector3 pos = Vector3(), Vector3 vel = Vector3(), float mass = 1);
	};
	class ParticleGroup {
	public:
		ParticleSystem* system;
		std::vector<int> indies;
		ParticleGroup(ParticleSystem* system, std::vector<int> indies = std::vector<int>());
		void addNewParticle(Particle p);
		void addIndex(int i);
		void UpdatePos(float time, float domainSize);
		void UpdateVel(float time);
		void Force(float force);
		void Draw(Engine* e, float contrast, int drawMode);
	};

	std::vector<Particle> particlePool;

	ParticleSystem(int poolSize = 1000);
};
