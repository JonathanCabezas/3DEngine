#include <vector>
#include <algorithm>

#include "Vec3d.cpp"
#include "Mesh.cpp"
#include "Mat4x4.cpp"
#include "Triangle.cpp"
#include "GameEngine.cpp"

const int WIDTH = 640;
const int HEIGHT = 480;

class Video3DEngine : public GameEngine {
    private:
        Mesh mesh;
        Mat4x4 matWorld, matRotX, matRotZ, matTrans, matProj;

        Vec3d vCamera = { 0.0f, 0.0f, 0.0f };

        // Projection Matrix
        float fFovDegrees = 90.0f;
        float fAspectRatio = (float) HEIGHT / (float) WIDTH;
        float fNear = 0.1f;
        float fFar = 1000.0f;

        float fTheta = 0.0f;

    public:
        bool OnCreate() override {
            mesh.LoadFromObjectFile("models/teapot.obj");

            matProj = Mat4x4::MakeProjection(fFovDegrees, fAspectRatio, fNear, fFar);

            return true;
        }

        bool OnUpdate(float fElapsedTime) override {
                renderer.Fill(BLACK);

                fTheta += 1.0f * fElapsedTime;

                matRotX = Mat4x4::MakeRotationX(fTheta);
                matRotZ = Mat4x4::MakeRotationZ(fTheta * 0.5f);
                matTrans = Mat4x4::MakeTranslation(0.0f, 0.0f, 8.0f);

                std::vector<Triangle> vecTrianglesToRaster;

                // Draw Triangles
                for (auto tri : mesh.tris) {
                    Triangle triProjected, triTransformed;

                    matWorld = matRotZ * matRotX * matTrans;

                    triTransformed = matWorld * tri;

                    Vec3d vCameraRay = triTransformed.p[0] - vCamera;
                    Vec3d normal = triTransformed.normal();

                    if (normal.dot(vCameraRay) < 0.0f) {

                        // Illumination
                        Vec3d light_direction = { 0.0f, 0.0f, -1.0f };
                        
                        float dp = normal.dot(light_direction);
                        int b = dp * 255 + 0.5f; // brightness

                        uint32_t shade = b + (b << 8) + (b << 16);
                        triProjected.col = shade;

                        // Project Triangles from 3D --> 2D
                        //triProjected = matProj * triTransformed; // <- This line doesn't work while the 3 following do, WHY ? :o
                        triProjected.p[0] = matProj * triTransformed.p[0];
                        triProjected.p[1] = matProj * triTransformed.p[1];
                        triProjected.p[2] = matProj * triTransformed.p[2];

                        triProjected.p[0] /= triProjected.p[0].w;
                        triProjected.p[1] /= triProjected.p[1].w;
                        triProjected.p[2] /= triProjected.p[2].w;

                        // Scale into view
                        triProjected += Vec3d(1.0f, 1.0f, 0.0f);
                        triProjected *= Vec3d(0.5f * (float) WIDTH, 0.5f * (float) HEIGHT, 1.0f);

                        // Store Triangles for sorting
                        vecTrianglesToRaster.push_back(triProjected);
                    }
                }

                // Sort Triangles from back to front
                sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](Triangle &t1, Triangle &t2) {
                    float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                    float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;

                    return z1 > z2;
                });

                for (auto &triProjected : vecTrianglesToRaster) {
                    // Rasterize Triangle
                    renderer.FillTriangle(triProjected);

                    renderer.DrawTriangle(triProjected, BLACK);
                }

            return true;
        }

};

int main()
{
    Video3DEngine demo;

    if (demo.CreateWindow("3D Demo", WIDTH, HEIGHT)) {
        demo.Start();
    }

    return 0;
}