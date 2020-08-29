#ifndef _MAT4X4
#define _MAT4X4

#include "Triangle.cpp"
#include "Vec3d.cpp"

struct Mat4x4 {
    float m[4][4] = { 0 };

    static Mat4x4 MakeIdentity() {
        Mat4x4 mat;

        mat.m[0][0] = 1;
        mat.m[1][1] = 1;
        mat.m[2][2] = 1;
        mat.m[3][3] = 1;

        return mat;
    }

    static Mat4x4 MakeRotationX(float fAngleRad) {
        Mat4x4 mat;

        mat.m[0][0] = 1;
        mat.m[1][1] = cosf(fAngleRad);
        mat.m[1][2] = sinf(fAngleRad);
        mat.m[2][1] = -sinf(fAngleRad);
        mat.m[2][2] = cosf(fAngleRad);
        mat.m[3][3] = 1;

        return mat;
    }

    static Mat4x4 MakeRotationY(float fAngleRad) {
        Mat4x4 mat;

        mat.m[0][0] = cosf(fAngleRad);
        mat.m[0][2] = sinf(fAngleRad);
        mat.m[1][1] = 1;
        mat.m[2][0] = -sinf(fAngleRad);
        mat.m[2][2] = cosf(fAngleRad);
        mat.m[3][3] = 1;

        return mat;
    }
    
    static Mat4x4 MakeRotationZ(float fAngleRad) {
        Mat4x4 mat;

        mat.m[0][0] = cosf(fAngleRad);
        mat.m[0][1] = sinf(fAngleRad); // switch both signs there to invert rotation
        mat.m[1][0] = -sinf(fAngleRad);
        mat.m[1][1] = cosf(fAngleRad);
        mat.m[2][2] = 1;
        mat.m[3][3] = 1;

        return mat;
    }

    static Mat4x4 MakeTranslation(float x, float y, float z) {
        Mat4x4 mat;

        mat.m[0][0] = 1;
        mat.m[1][1] = 1;
        mat.m[2][2] = 1;
        mat.m[3][3] = 1;
        mat.m[3][0] = x;
        mat.m[3][1] = y;
        mat.m[3][2] = z;

        return mat;
    }

    static Mat4x4 MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar) {
        Mat4x4 mat;

        float fFovRad = 1.0 / tanf(fFovDegrees * 0.5 / 180.0 * 3.14159);

        mat.m[0][0] = fAspectRatio * fFovRad;
        mat.m[1][1] = fFovRad;
        mat.m[2][2] = fFar / (fFar - fNear);
        mat.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        mat.m[2][3] = 1.0;

        return mat;
    }

    static Mat4x4 PointAt(const Vec3d &pos, const Vec3d &target, const Vec3d up) {
        // Calculate new Forward direction
        Vec3d newForward = target - pos;
        newForward.normalize();

        // Calculate new Up direction
        Vec3d a = newForward * up.dot(newForward);
        Vec3d newUp = up - a;
        newUp.normalize();

        // New Right direction is easy, its just cross product
		Vec3d newRight = newUp.cross(newForward);

        // Construct Dimensioning and Translation Matrix	
		Mat4x4 mat;

		mat.m[0][0] = newRight.x;
        mat.m[0][1] = newRight.y;
        mat.m[0][2] = newRight.z;

		mat.m[1][0] = newUp.x;
        mat.m[1][1] = newUp.y;
        mat.m[1][2] = newUp.z;

		mat.m[2][0] = newForward.x;
        mat.m[2][1] = newForward.y;
        mat.m[2][2] = newForward.z;

		mat.m[3][0] = pos.x;
        mat.m[3][1] = pos.y;
        mat.m[3][2] = pos.z;
        mat.m[3][3] = 1;

		return mat;
    }

    static Mat4x4 QuickInverse(Mat4x4 &m) // Only for Rotation/Translation Matrices
	{
		Mat4x4 mat;

		mat.m[0][0] = m.m[0][0];
        mat.m[0][1] = m.m[1][0];
        mat.m[0][2] = m.m[2][0];

		mat.m[1][0] = m.m[0][1];
        mat.m[1][1] = m.m[1][1];
        mat.m[1][2] = m.m[2][1];

		mat.m[2][0] = m.m[0][2];
        mat.m[2][1] = m.m[1][2];
        mat.m[2][2] = m.m[2][2];

		mat.m[3][0] = -(m.m[3][0] * mat.m[0][0] + m.m[3][1] * mat.m[1][0] + m.m[3][2] * mat.m[2][0]);
		mat.m[3][1] = -(m.m[3][0] * mat.m[0][1] + m.m[3][1] * mat.m[1][1] + m.m[3][2] * mat.m[2][1]);
		mat.m[3][2] = -(m.m[3][0] * mat.m[0][2] + m.m[3][1] * mat.m[1][2] + m.m[3][2] * mat.m[2][2]);
        mat.m[3][3] = 1;

		return mat;
	}

    Mat4x4 operator *(const Mat4x4& b) const {
        Mat4x4 mat;

        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                for (int k = 0; k < 4; ++k)
                    mat.m[i][j] += m[i][k] * b.m[k][j];

        return mat;
    }

    Vec3d operator * (const Vec3d& b) const {
        Vec3d r;

        r.x = b.x * m[0][0] + b.y * m[1][0] + b.z * m[2][0] + m[3][0];
        r.y = b.x * m[0][1] + b.y * m[1][1] + b.z * m[2][1] + m[3][1];
        r.z = b.x * m[0][2] + b.y * m[1][2] + b.z * m[2][2] + m[3][2];
        r.w = b.x * m[0][3] + b.y * m[1][3] + b.z * m[2][3] + m[3][3];

        return r;
    }

    Triangle operator * (const Triangle& b) const {
        Triangle r;

        r.col = b.col;

        r.t[0] = b.t[0];
        r.t[1] = b.t[1];
        r.t[2] = b.t[2];

        r.p[0] = *this * b.p[0];
        r.p[1] = *this * b.p[1];
        r.p[2] = *this * b.p[2];

        return r;
    }
};

#endif