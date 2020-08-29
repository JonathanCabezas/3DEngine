struct Vec2d {
    float u, v;

    Vec2d() {
        u = v = 0;
    }
    
    Vec2d(float a, float b) {
        u = a;
        v = b;
    }

    Vec2d operator +(const Vec2d& b) const {
        Vec2d r;

        r.u = u + b.u;
        r.v = v + b.v;

        return r;
    }

    Vec2d operator -(const Vec2d& b) const {
        Vec2d r;

        r.u = u - b.u;
        r.v = v - b.v;

        return r;
    }

    Vec2d operator *(float b) const {
        Vec2d r;

        r.u = u * b;
        r.v = v * b;

        return r;
    }
};