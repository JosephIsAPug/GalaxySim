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
	std::vector<data> particles = std::vector<data>();
	std::vector<int> removeI = std::vector<int>();

	void spawnGalaxy(int pNum, float cx, float cy, float cz, float cvx, float cvy, float cvz, float size) {
		for (int i = 0; i < pNum; i++) {
			float z = Rnd_Float(-1, 1);
			float r = sqrt(1 - z * z);
			float a = Rnd_Float(0, 2 * 3.1415);
			float x = cos(a) * r;
			float y = sin(a) * r;
			float l = powf(Rnd_Float(0.02, 1), 1.0 / 0.8);
			x *= l * 1;
			y *= l * 0.4;
			z *= l * 1;
			float orbitSpeed = sqrt(powf(l, 1.0 / 0.8) * pNum / (std::hypot(x, y, z) * size)) * 0.0015;
			float vx = -z;
			float vy = 0;
			float vz = x;
			float speed = std::hypot(vx, vy, vz);
			vx /= speed / orbitSpeed;
			vy /= speed / orbitSpeed;
			vz /= speed / orbitSpeed;
			vx += Rnd_Float(-1, 1) * 0.03;
			vy += Rnd_Float(-1, 1) * 0.06;
			vz += Rnd_Float(-1, 1) * 0.03;
			addParticle(x * size + cx, y * size + cy, z * size + cz, vx + cvx, vy + cvy, vz + cvz);//Rnd_Float(-1, 1), Rnd_Float(-1, 1));
		}
	}
	void clear() {
		particles.clear();
	}
	void addSystem(ParticleSystem* s) {
		for (data p : s->particles)
			particles.push_back(p);
	}
	void addParticle(float x, float y, float z, float vx, float vy, float vz, float m = 1) {
		particles.push_back(data(particles.size(), x, y, z, vx, vy, vz, m));
	}
	void updatePos(int i, float universeSize = 1, float t = 1) {
		data* p = &particles[i];

		p->x += p->vx * t / universeSize;
		p->y += p->vy * t / universeSize;
		p->z += p->vz * t / universeSize;
	}
	void updateVel(int i, float t = 1) {
		data* p = &particles[i];

		/*
		const float max = 5;
		float a = std::hypot(p->ax, p->ay, p->az);
		if (a > max) {
			p->ax /= a / max;
			p->ay /= a / max;
			p->az /= a / max;
		}
		*/

		p->vx += p->ax * t;
		p->vy += p->ay * t;
		p->vz += p->az * t;
	}
	void damp(int i, float f) {
		data* p = &particles[i];

		p->vx *= f;
		p->vy *= f;
		p->vz *= f;
	}
	float calcTimestep(float f, float min=0.1, float max=10) {
		float maxDelta = 0;
		for (int i2 = 0; i2 < particles.size(); i2++) {
			data* p = &particles[i2];

			float a = std::hypot(p->ax, p->ay, p->az);
			if (a > maxDelta)
				maxDelta = a;
		}

		return fminf(fmaxf(f / maxDelta, min * f), max * f);
	}
	void confine(float x1, float y1, float z1, float x2, float y2, float z2, float removeChance = 0) {
		for (int i = 0; i < particles.size(); i++) {
			data* p = &(particles[i]);
			//p->x = fmaxf(fminf(p->x, x2), x1);
			//p->y = fmaxf(fminf(p->y, y2), y1);
			bool b = false;
			if (p->x < x1) {
				b = true;
			}
			if (p->x > x2) {
				b = true;
			}
			if (p->y < y1) {
				b = true;
			}
			if (p->y > y2) {
				b = true;
			}
			if (p->z < z1) {
				b = true;
			}
			if (p->z > z2) {
				b = true;
			}
			if (b && Rnd_Int(0, 1.0 / (removeChance * std::hypot(p->x, p->y, p->z))) == 0) {
				remove(i);
				//removeI.push_back(i);
			}
		}
	}
	void combine(int i, int i2) {
		data* p = &particles[i];
		data* p2 = &particles[i2];

		if (!p->active || !p2->active)
			return;

		float avx = (p2->vx * p2->mass + p->vx * p->mass) / (p2->mass + p->mass);
		float avy = (p2->vy * p2->mass + p->vy * p->mass) / (p2->mass + p->mass);
		float avz = (p2->vz * p2->mass + p->vz * p->mass) / (p2->mass + p->mass);

		float ax = (p2->x * p2->mass + p->x * p->mass) / (p2->mass + p->mass);
		float ay = (p2->y * p2->mass + p->y * p->mass) / (p2->mass + p->mass);
		float az = (p2->z * p2->mass + p->z * p->mass) / (p2->mass + p->mass);

		p->x = ax;
		p->y = ay;
		p->z = az;

		p->vx = avx;
		p->vy = avy;
		p->vz = avz;

		p->mass += p2->mass;
		p2->active = false;
		remove(i2);
		//std::cout << p->mass << std::endl;
	}
	void remove(int i) {
		removeI.push_back(i);
	}
	void RemoveAll() {
		sort(removeI.begin(), removeI.end());
		int pri = -1;
		for (int i = removeI.size() - 1; i >= 0; i--) { // reverse order prevents memory error
			//std::cout << removeI[i] << " : " << i << std::endl;
			if (removeI[i] == pri)
				//std::cout << "removed " << i << std::endl;
				removeI.erase(removeI.begin() + i); // remove duplicets
			pri = removeI[i];
		}
		for (int i = removeI.size() - 1; i >= 0; i--) { // reverse order prevents memory error
			//std::cout << removeI[i] << std::endl;
			if (removeI[i] > particles.size() - 1)
				continue;
			particles.erase(particles.begin() + removeI[i]);
		}
		removeI.clear();
	}
	void Resize(float factor) {
		for (int i = 0; i < particles.size(); i++) {
			data* p = &(particles[i]);

			p->x /= factor;
			p->y /= factor;
			p->z /= factor;
		}
		/*
		float minX = 1;
		float minY = 1;
		float minZ = 1;
		float maxX = -1;
		float maxY = -1;
		float maxZ = -1;
		for (int i = 0; i < particles.size(); i++) {
			data* p = &(particles[i]);

			minX = fminf(minX, p->x);
			minY = fminf(minY, p->y);
			minZ = fminf(minZ, p->z);

			maxX = fmaxf(maxX, p->x);
			maxY = fmaxf(maxY, p->y);
			maxZ = fmaxf(maxZ, p->z);
		}
		float factor = fmaxf(abs(minX), abs(maxX));
		factor = fmaxf(factor, fmaxf(abs(minY), abs(maxY)));
		factor = fmaxf(factor, fmaxf(abs(minZ), abs(maxZ)));
		//factor = powf(factor, 0.1);
		std::cout << factor << std::endl;
		for (int i = 0; i < particles.size(); i++) {
			data* p = &(particles[i]);

			p->x /= factor;
			p->y /= factor;
			p->z /= factor;
		}
		return factor;
		*/
	}
	void Center(float x, float y, float z) {
		for (int i = 0; i < particles.size(); i++) {
			data* p = &(particles[i]);

			p->x -= x;
			p->y -= y;
			p->z -= z;
		}
	}
	void Draw(Engine* engine, float colorHueShift=3, int mode=0) {//(sf::RenderWindow* window, float offsetX, float offsetY, float offsetZ, float rot, float drawSize = 1, float color = 3) {
		std::vector<sf::Vector3f> points = std::vector<sf::Vector3f>();
		std::vector<sf::Color> colors = std::vector<sf::Color>();
		for (int i = 0; i < particles.size(); i++) {
			data* p = &(particles[i]);
			points.push_back({ p->x, p->y, p->z });
			float hue;
			if (mode == 0)
				hue = std::hypot(p->vx, p->vy, p->vz);
			else
				hue = std::hypot(p->ax, p->ay, p->az);
			hue = log(hue + 1);
			colors.push_back(sf::Color(HSVtoR(hue * colorHueShift, 1, 1), HSVtoG(hue * colorHueShift, 1, 1), HSVtoB(hue * colorHueShift, 1, 1), 255));
		}
		engine->drawPointCloud(points, colors);
	}
	void randomisePositions(int num) {
		for (int i = 0; i < num; i++) {
			data* p = &(particles[(i + Rnd_Int(0, particles.size())) % particles.size()]);
			p->x = Rnd_Float(0, 1);
			p->y = Rnd_Float(0, 1);
			//p->vx = 0;//Rnd_Float(-1, 1);
			//p->vy = 0;//Rnd_Float(-1, 1);
		}
	}
};
