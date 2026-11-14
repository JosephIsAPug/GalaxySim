#ifndef MY_FUNCTIONS_H // Include guard to prevent multiple inclusions
#define MY_FUNCTIONS_H

void Rnd_SetSeed(float seed);
void Rnd_RndSeed();
float Rnd_Float(float min_val, float max_val);
int Rnd_Int(int min_val, int max_val);

float Clamp(float value, float min, float max);

float Dist(float x, float y, float min = 0.001);
int Sign(float v);

int HSVtoR(float h, float s, float v);
int HSVtoG(float h, float s, float v);
int HSVtoB(float h, float s, float v);
class Vector3 {
public:
	float x;
	float y;
	float z;
	Vector3(float ix, float iy, float iz);
};
#endif