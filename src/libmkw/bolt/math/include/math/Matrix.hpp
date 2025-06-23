#pragma once

#include "Vector.hpp"

namespace bolt {
namespace math {

struct Matrix34f {
    float data[3][4];

    // immutable element accessor
    const float operator()(int x, int y) const { return data[x][y]; }
    // mutable element accessor
    float& operator()(int x, int y) { return data[x][y]; }
    
    void setZero() {
        data[0][0] = data[0][1] = data[0][2] = data[0][3] = 0;
        data[1][0] = data[1][1] = data[1][2] = data[1][3] = 0;
        data[2][0] = data[2][1] = data[2][2] = data[2][3] = 0;
    }
    void setTranslation(float x, float y, float z) {
        data[0][3] = x;
        data[1][3] = y;
        data[2][3] = z;
    }
    void setRotation(float roll, float pitch, float yaw);
    Vector3f operator*(const Vector3f& other) const;
};

struct Matrix44f {
    float data[4][4];

    // immutable element accessor
    const float operator()(int x, int y) const { return data[x][y]; }
    // mutable element accessor
    float& operator()(int x, int y) { return data[x][y]; }
    
    void setZero() {
        data[0][0] = data[0][1] = data[0][2] = data[0][3] = 0;
        data[1][0] = data[1][1] = data[1][2] = data[1][3] = 0;
        data[2][0] = data[2][1] = data[2][2] = data[2][3] = 0;
        data[3][0] = data[3][1] = data[3][2] = data[3][3] = 0;
    }
    void setIdentity() {
        setZero();
        data[0][0] = 1;
        data[1][1] = 1;
        data[2][2] = 1;
        data[3][3] = 1;
    }
    void setTranslation(float x, float y, float z) {
        data[0][3] = x;
        data[1][3] = y;
        data[2][3] = z;
    }
    void setRotation(float roll, float pitch, float yaw);
    void makeHomogeneous() {
        data[3][0] = 0.0;
        data[3][1] = 0.0;
        data[3][2] = 0.0;
        data[3][3] = 1.0;
    }
    void setPerspective(float fovy, float aspect, float zNear, float zFar);
    void setLookAt(const Vector3f& pos, const Vector3f& focus, const Vector3f& up);
    Matrix44f operator*(const Matrix44f& other) const;
    Vector4f operator*(const Vector4f& other) const;
};

} // math
} // bolt
