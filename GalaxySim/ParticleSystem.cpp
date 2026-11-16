#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(int poolSize) {
	particlePool = std::vector<Particle>(poolSize, Particle());
}
ParticleSystem::Particle::Particle(Vector3 ipos, Vector3 ivel, float imass) {
	pos = ipos;
	vel = ivel;
	acc = Vector3();
	mass = imass;
}
ParticleSystem::ParticleGroup::ParticleGroup(ParticleSystem* isystem, std::vector<int> iindies) {
	system = isystem;
	indies = iindies;
}
void ParticleSystem::ParticleGroup::addNewParticle(Particle p) {
	indies.push_back(system->particlePool.size());
	system->particlePool.push_back(p);
}
void ParticleSystem::ParticleGroup::addIndex(int i) {
	indies.push_back(i);
}
void ParticleSystem::ParticleGroup::UpdatePos(float t, float universeSize) {
	for (int i : indies) {
		Particle* p = &system->particlePool[i];
		p->pos += p->vel * t / universeSize;
	}
}
void ParticleSystem::ParticleGroup::UpdateVel(float t) {
	for (int i : indies) {
		Particle* p = &system->particlePool[i];
		p->vel += p->acc * t;
	}
}
void ParticleSystem::ParticleGroup::Force(float f) {
	for (int i : indies) {
		Particle* p = &system->particlePool[i];
		p->acc += f;
	}
}
void ParticleSystem::ParticleGroup::Draw(Engine* e, float contrast, int mode) {
	std::vector<sf::Vector3f> positions = std::vector<sf::Vector3f>();
	std::vector<sf::Color> colors = std::vector<sf::Color>();
	for (int i : indies) {
		Particle* p = &system->particlePool[i];
		positions.push_back(sf::Vector3f(p->pos.x, p->pos.y, p->pos.z));
		float hue;
		if (mode == 0) {
			hue = p->vel.getLength();
		}
		else {
			hue = p->acc.getLength() * 100;
		}
		colors.push_back(sf::Color(HSVtoR(hue * contrast, 1, 1), HSVtoG(hue * contrast, 1, 1), HSVtoB(hue * contrast, 1, 1)));
	}
	e->drawPointCloud(positions, colors);
}