#ifndef _TRIANGLE
#define _TRIANGLE

#include "Vec2d.cpp"
#include "Vec3d.cpp"
#include <stdint.h>

struct Triangle {
    Vec3d p[3];
    Vec2d t[3];
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

        r.t[0] = t[0];
        r.t[1] = t[1];
        r.t[2] = t[2];

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

    int clipAgainstPlane(Vec3d plane_p, Vec3d plane_n, Triangle &out_tri1, Triangle &out_tri2)
        {
            // Make sure plane normal is indeed normal
            plane_n.normalize();

            // Return signed shortest distance from point to plane, plane normal must be normalised
            auto dist = [&](Vec3d &p)
            {
                return (plane_n.dot(p) - plane_n.dot(plane_p));
            };

            // Create two temporary storage arrays to classify points either side of plane
            // If distance sign is positive, point lies on "inside" of plane
            Vec3d* inside_points[3];
            Vec3d* outside_points[3];
            int nInsidePointCount = 0;
            int nOutsidePointCount = 0;

            Vec2d* inside_tex[3];
            Vec2d* outside_tex[3];
            int nInsideTexCount = 0;
            int nOutsideTexCount = 0;
            
            // To store the parameter to get to the intersection
            float par;

            // Get signed distance of each point in triangle to plane
            float d0 = dist(p[0]);
            float d1 = dist(p[1]);
            float d2 = dist(p[2]);

            if (d0 >= 0) {
                inside_points[nInsidePointCount++] = &p[0];
                inside_tex[nInsideTexCount++] = &t[0];
            } else {
                outside_points[nOutsidePointCount++] = &p[0];
                outside_tex[nOutsideTexCount++] = &t[0];
            } if (d1 >= 0) {
                inside_points[nInsidePointCount++] = &p[1];
                inside_tex[nInsideTexCount++] = &t[1];
            } else {
                outside_points[nOutsidePointCount++] = &p[1];
                outside_tex[nOutsideTexCount++] = &t[1];
            } if (d2 >= 0) {
                inside_points[nInsidePointCount++] = &p[2];
                inside_tex[nInsideTexCount++] = &t[2];
            } else {
                outside_points[nOutsidePointCount++] = &p[2];
                outside_tex[nOutsideTexCount++] = &t[2];
            }

            // Now classify triangle points, and break the input triangle into 
            // smaller output triangles if required. There are four possible
            // outcomes...

            if (nInsidePointCount == 0)
            {
                // All points lie on the outside of plane, so clip whole triangle
                // It ceases to exist

                return 0; // No returned triangles are valid
            }

            if (nInsidePointCount == 3)
            {
                // All points lie on the inside of plane, so do nothing
                // and allow the triangle to simply pass through
                out_tri1 = *this;

                return 1; // Just the one returned original triangle is valid
            }

            if (nInsidePointCount == 1 && nOutsidePointCount == 2)
            {
                // Triangle should be clipped. As two points lie outside
                // the plane, the triangle simply becomes a smaller triangle

                // Copy appearance info to new triangle
                out_tri1.col =  col;
                //out_tri1.col =  0x0000ff;

                // The inside point is valid, so keep that...
                out_tri1.p[0] = *inside_points[0];
                out_tri1.t[0] = *inside_tex[0];

                // but the two new points are at the locations where the 
                // original sides of the triangle (lines) intersect with the plane
                out_tri1.p[1] = Vec3d::IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], par);
                out_tri1.t[1] = *inside_tex[0] + (*outside_tex[0] - *inside_tex[0]) * par;

                out_tri1.p[2] = Vec3d::IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], par);
                out_tri1.t[2] = *inside_tex[0] + (*outside_tex[1] - *inside_tex[0]) * par;

                return 1; // Return the newly formed single triangle
            }

            if (nInsidePointCount == 2 && nOutsidePointCount == 1)
            {
                // Triangle should be clipped. As two points lie inside the plane,
                // the clipped triangle becomes a "quad". Fortunately, we can
                // represent a quad with two new triangles

                // Copy appearance info to new triangles
                out_tri1.col =  col;
                out_tri2.col =  col;
                //out_tri1.col =  0x008000;
                //out_tri2.col =  0xcf0000

                // The first triangle consists of the two inside points and a new
                // point determined by the location where one side of the triangle
                // intersects with the plane
                out_tri1.p[0] = *inside_points[0];
                out_tri1.t[0] = *inside_tex[0];

                out_tri1.p[1] = *inside_points[1];
                out_tri1.t[1] = *inside_tex[1];

                out_tri1.p[2] = Vec3d::IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], par);
                out_tri1.t[2] = *inside_tex[0] + (*outside_tex[0] - *inside_tex[0]) * par;

                // The second triangle is composed of one of he inside points, a
                // new point determined by the intersection of the other side of the 
                // triangle and the plane, and the newly created point above
                out_tri2.p[0] = *inside_points[1];
                out_tri1.t[0] = *inside_tex[0];

                out_tri2.p[1] = Vec3d::IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], par);
                out_tri1.t[1] = *inside_tex[1] + (*outside_tex[0] - *inside_tex[1]) * par;

                out_tri2.p[2] = out_tri1.p[2];
                out_tri1.t[2] = out_tri1.t[2];

                return 2; // Return two newly formed triangles which form a quad
            }

            return 0;
        }
};

#endif