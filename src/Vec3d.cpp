#ifndef _VEC3D
#define _VEC3D

#include <math.h>

struct Vec3d {
    float x, y, z, w;

    Vec3d() {
        x = y = z = 0;
        w = 1;
    }

    Vec3d(float a, float b, float c) {
        x = a;
        y = b;
        z = c;
        w = 1;
    }

    float length() const {
        return sqrtf(x * x + y * y + z * z);
    }

    Vec3d normalized() const {
        float l = length();

        if (l != 0)
            return *this / l;

        return *this;
    }

    void normalize() {
        float l = length();

        if (l != 0) {
            x /= l;
            y /= l;
            z /= l;
        }
    }

    float angle(const Vec3d& b) const {
        return acosf((x * b.x + y * b.y + z * b.z) / length() * b.length());
    }

    float dot(const Vec3d& b) const {
        return (x * b.x + y * b.y + z * b.z);
    }

    Vec3d cross(const Vec3d& b) const {
        return { y * b.z - z * b.y,
                 z * b.x - x * b.z,
                 x * b.y - y * b.x };
    }

    Vec3d& operator +=(const Vec3d& b) {
        x += b.x;
        y += b.y;
        z += b.z;

        return *this;
    }

    Vec3d& operator -=(const Vec3d& b) {
        x -= b.x;
        y -= b.y;
        z -= b.z;

        return *this;
    }

    Vec3d& operator *=(const Vec3d& b) {
        x *= b.x;
        y *= b.y;
        z *= b.z;

        return *this;
    }

    Vec3d& operator /=(const Vec3d& b) {
        x /= b.x;
        y /= b.y;
        z /= b.z;

        return *this;
    }

    Vec3d& operator +=(const float b) {
        x += b;
        y += b;
        z += b;

        return *this;
    }

    Vec3d& operator -=(const float b) {
        x -= b;
        y -= b;
        z -= b;

        return *this;
    }

    Vec3d& operator *=(const float b) {
        x *= b;
        y *= b;
        z *= b;

        return *this;
    }

    Vec3d& operator /=(const float b) {
        x /= b;
        y /= b;
        z /= b;

        return *this;
    }

    Vec3d operator +(const Vec3d& b) const {
        Vec3d r;
        r.x = x + b.x;
        r.y = y + b.y;
        r.z = z + b.z;

        return r;
    }

    Vec3d operator -(const Vec3d& b) const {
        Vec3d r;
        r.x = x - b.x;
        r.y = y - b.y;
        r.z = z - b.z;

        return r;
    }

    Vec3d operator +(const float b) const {
        Vec3d r;
        r.x = x + b;
        r.y = y + b;
        r.z = z + b;

        return r;
    }

    Vec3d operator -(const float b) const {
        Vec3d r;
        r.x = x - b;
        r.y = y - b;
        r.z = z - b;

        return r;
    }

    Vec3d operator *(const float b) const {
        Vec3d r;
        r.x = x * b;
        r.y = y * b;
        r.z = z * b;

        return r;
    }

    Vec3d operator /(const float b) const {
        Vec3d r;
        r.x = x / b;
        r.y = y / b;
        r.z = z / b;

        return r;
    }
};

#endif