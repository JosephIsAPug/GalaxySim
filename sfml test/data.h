#pragma once
#include <iostream>
#include "Mis.h"
struct data
{
	int index = 0;
	bool active = true;

	float x = 0;
	float y = 0;
	float z = 0;

	float vx = 0;
	float vy = 0;
	float vz = 0;

	float ax = 0;
	float ay = 0;
	float az = 0;

	float mass;
	data() = default;
	data(int ci, float cx, float cy, float cz, float cvx, float cvy, float cvz, float cmass) {
		index = ci;
		x = cx;
		y = cy;
		z = cz;
		vx = cvx;
		vy = cvy;
		vz = cvz;
		mass = cmass;
	}
	void collide(data* p2) {
		float ax = (p2->vx * p2->mass + vx * mass) / (p2->mass + mass);
		float ay = (p2->vy * p2->mass + vy * mass) / (p2->mass + mass);
		float az = (p2->vz * p2->mass + vz * mass) / (p2->mass + mass);

		float dx = (p2->vx - vx);
		float dy = (p2->vy - vy);
		float dz = (p2->vz - vz);

		float relVel = std::hypot(dx, dy, dz);

		float theta = Rnd_Float(0, 2 * 3.1415);
		float gamma = Rnd_Float(-3.1415 / 2, 3.1415 / 2);

		float z = sinf(gamma);
		float r = sqrt(1 - z * z);

		float x = cosf(theta) * r;
		float y = sinf(theta) * r;

		x *= relVel * (p2->mass + mass) / 2;
		y *= relVel * (p2->mass + mass) / 2;
		z *= relVel * (p2->mass + mass) / 2;

		vx = ax - x / mass;
		vy = ay - y / mass;
		vz = ay - z / mass;

		p2->vx = ax + x / p2->mass;
		p2->vy = ay + y / p2->mass;
		p2->vz = ay + z / p2->mass;
	}
};
