#ifndef _TRIANGLE
#define _TRIANGLE

#include "Vec3d.cpp"
#include <stdint.h>

struct Triangle {
    Vec3d p[3];
    uint32_t col;

    Triangle() {
        col = 0;
    }

    Triangle(Vec3d a, Vec3d b, Vec3d c) {
        p[0] = a;
        p[1] = b;
        p[2] = c;
        col = 0;
    }

    Triangle operator +(const Vec3d& b) const {
        Triangle r;

        r.col = col;
        r.p[0] = p[0] + b;
        r.p[1] = p[1] + b;
        r.p[2] = p[2] + b;

        return r;
    }

    Triangle& operator +=(const Vec3d& b) {
        p[0] += b;
        p[1] += b;
        p[2] += b;

        return *this;
    }

    Triangle& operator *=(const Vec3d& b) {
        p[0] *= b;
        p[1] *= b;
        p[2] *= b;

        return *this;
    }
    
    Vec3d normal() const {
        Vec3d normal;
        Vec3d line1 = p[1] - p[0];
        Vec3d line2 = p[2] - p[0];

        normal = line1.cross(line2);
        normal.normalize();

        return normal;
    }

    Vec3d center() const {
        Vec3d r;

        r.x = (p[0].x + p[1].x + p[2].x) / 3;
        r.y = (p[0].y + p[1].y + p[2].y) / 3;
        r.z = (p[0].z + p[1].z + p[2].z) / 3;

        return r;
    }
};

#endif