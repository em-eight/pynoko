#pragma once

#include <cmath>

namespace math {
struct Vector3f {
    float x;
    float y;
    float z;

    Vector3f() = default;

    Vector3f(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vector3f operator+(const Vector3f& other) const {
        Vector3f res;
        res.x = x + other.x;
        res.y = y + other.y;
        res.z = z + other.z;
        return res;
    }

    Vector3f operator-() {
        this->x = -x;
        this->y = -y;
        this->z = -z;
        return *this;
    }

    Vector3f operator-(const Vector3f& other) const {
        Vector3f res;
        res.x = x - other.x;
        res.y = y - other.y;
        res.z = z - other.z;
        return res;
    }

    Vector3f operator*(float scalar) const {
        Vector3f res;
        res.x = x * scalar;
        res.y = y * scalar;
        res.z = z * scalar;
        return res;
    }

    float dot(const Vector3f& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3f cross(const Vector3f& other) const {
        Vector3f res;
        res.x = y * other.z - z * other.y;
        res.y = z * other.x - x * other.z;
        res.z = x * other.y - y * other.x;
        return res;
    }

    float length() const { return std::sqrt(x * x + y * y + z * z); }

    Vector3f normalize() {
        float magnitude = length() + 1e-8f;
        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
        return *this;
    }

    Vector3f normalize() const {
        Vector3f tmp = *this;
        return tmp.normalize();
    }

};
} // math
