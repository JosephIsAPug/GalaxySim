#include <random>
#include "Mis.h"

std::random_device rd; // Obtain a random seed from the OS
std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

void Rnd_SetSeed(float seed) {
    gen = std::mt19937(seed);
}
void Rnd_RndSeed() {
    gen = std::mt19937(rd());
}
float Rnd_Float(float min_val, float max_val) {

    std::uniform_real_distribution<> dis(min_val, max_val); // Define the range

    return dis(gen); // Return a random float within the specified range
}
int Rnd_Int(int min_val, int max_val) {

    std::uniform_real_distribution<> dis(min_val, max_val); // Define the range

    return dis(gen); // Return a random float within the specified range
}

float Clamp(float value, float min, float max) {
    return std::max(std::min(value, max), min);
}

float Dist(float x, float y, float min) {
    return sqrtf(fmaxf(powf(x, 2) + powf(y, 2), min));
}

int Sign(float v) {
    if (v < 0)
        return -1;
    if (v > 0)
        return 1;
    return 0;
}

int HSVtoR(float h, float s, float v) {
    return fminf(fmaxf(sin(h) - 1 + v * 1.5, 0), 1) * 255;
}
int HSVtoG(float h, float s, float v) {
    return fminf(fmaxf(sin(h + 2 * 3.1415 / 3) - 1 + v * 1.5, 0), 1) * 255;
}
int HSVtoB(float h, float s, float v) {
    return fminf(fmaxf(sin(h + 4 * 3.1415 / 3) - 1 + v * 1.5, 0), 1) * 255;
}

Vector3::Vector3(float ix, float iy, float iz) {
    x = ix;
    y = iy;
    z = iz;
}
float Vector3::getLength() {
    return std::hypot(x, y, z);
}
void Vector3::Normalize() {
    float length = getLength();
    x /= length;
    y /= length;
    z /= length;
}
Vector3 Vector3::operator+(const Vector3& other) {
    return Vector3(x + other.x, y + other.y, z + other.z);
}
void Vector3::operator+=(const Vector3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
}
Vector3 Vector3::operator-(const Vector3& other) {
    return Vector3(x - other.x, y - other.y, z - other.z);
}
void Vector3::operator-=(const Vector3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
}
Vector3 Vector3::operator*(float fac) {
    return Vector3(x * fac, y * fac, z * fac);
}
void Vector3::operator*=(float fac) {
    x *= fac;
    y *= fac;
    z *= fac;
}
Vector3 Vector3::operator/(float fac) {
    return Vector3(x / fac, y / fac, z / fac);
}
void Vector3::operator/=(float fac) {
    x /= fac;
    y /= fac;
    z /= fac;
}