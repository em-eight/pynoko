#include "math/Matrix.hpp"

#include <cmath>

namespace bolt {
namespace math {

void Matrix34f::setRotation(float r, float p, float y) {
    float cr = std::cos(r);
    float sr = std::sin(r);
    float cp = std::cos(p);
    float sp = std::sin(p);
    float cy = std::cos(y);
    float sy = std::sin(y);

    data[0][0] = cy * cp;
    data[0][1] = cy * sp * sr - sy * cr;
    data[0][2] = cy * sp * cr + sy * sr;

    data[1][0] = sy * cp;
    data[1][1] = sy * sp * sr + cy * cr;
    data[1][2] = sy * sp * cr - cy * sr;

    data[2][0] = -sp;
    data[2][1] = cp * sr;
    data[2][2] = cp * cr;
}

Vector3f Matrix34f::operator*(const Vector3f& other) const {
    Vector3f ret;
    ret.x = this->data[0][0] * other.x + this->data[0][1] * other.y + this->data[0][2] * other.z + this->data[0][3];
    ret.y = this->data[1][0] * other.x + this->data[1][1] * other.y + this->data[1][2] * other.z + this->data[1][3];
    ret.z = this->data[2][0] * other.x + this->data[2][1] * other.y + this->data[2][2] * other.z + this->data[2][3];
    return ret;
}

void Matrix44f::setRotation(float r, float p, float y) {
    float cr = std::cos(r);
    float sr = std::sin(r);
    float cp = std::cos(p);
    float sp = std::sin(p);
    float cy = std::cos(y);
    float sy = std::sin(y);

    data[0][0] = cy * cp;
    data[0][1] = cy * sp * sr - sy * cr;
    data[0][2] = cy * sp * cr + sy * sr;

    data[1][0] = sy * cp;
    data[1][1] = sy * sp * sr + cy * cr;
    data[1][2] = sy * sp * cr - cy * sr;

    data[2][0] = -sp;
    data[2][1] = cp * sr;
    data[2][2] = cp * cr;
}

void Matrix44f::setPerspective(float fovy, float aspect, float zNear, float zFar) {
    setZero();
    float tanHalfFovy = std::tan(fovy / 2);
    data[0][0] = 1 / (aspect * tanHalfFovy);
    data[1][1] = 1 / tanHalfFovy;
    data[2][2] = -(zFar + zNear) / (zFar - zNear);
    data[2][3] = -(2 * zFar * zNear) / (zFar - zNear);
    data[3][2] = -1;
}

void Matrix44f::setLookAt(const Vector3f& pos, const Vector3f& focus, const Vector3f& up) {
    Vector3f X, Y, Z;

    Z = pos - focus;
    Z.normalize();
    X = up.cross(Z);
    X.normalize();
    Y = Z.cross(X);
    Y.normalize();

    data[0][0] = X.x;
    data[0][1] = X.y;
    data[0][2] = X.z;
    data[1][0] = Y.x;
    data[1][1] = Y.y;
    data[1][2] = Y.z;
    data[2][0] = Z.x;
    data[2][1] = Z.y;
    data[2][2] = Z.z;

    data[0][3] = -pos.dot(X);
    data[1][3] = -pos.dot(Y);
    data[2][3] = -pos.dot(Z);

    data[3][0] = 0;
    data[3][1] = 0;
    data[3][2] = 0;
    data[3][3] = 1.0f;
}

Matrix44f Matrix44f::operator*(const Matrix44f& other) const {
    Matrix44f ret;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ret.data[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                ret.data[i][j] += this->data[i][k] * other.data[k][j];
            }
        }
    }
    return ret;
}

Vector4f Matrix44f::operator*(const Vector4f& other) const {
    Vector4f ret;
    for (int i = 0; i < 4; i++) {
        ret.data[i] = 0;
        for (int j = 0; j < 4; j++) {
            ret.data[i] += this->data[i][j] * other.data[j];
        }
    }
    return ret;
}

} // math
} // bolt
