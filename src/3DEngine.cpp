#include <vector>
#include <algorithm>

#include "Vec3d.cpp"
#include "Mesh.cpp"
#include "Mat4x4.cpp"
#include "Triangle.cpp"
#include "GameEngine.cpp"

const int WIDTH = 1024;
const int HEIGHT = 960;

class Video3DEngine : public GameEngine {
    private:
        Mesh mesh;
        Mat4x4 matWorld, matRotX, matRotZ, matTrans, matProj;

        Vec3d vCamera;
        Vec3d vLookDir;
        Vec3d vel;

        // Projection Matrix
        float fFovDegrees;
        float fAspectRatio;
        float fNear;
        float fFar;

        float fTheta;

    public:
        bool OnCreate() override {
            fFovDegrees = 90.0f;
            fAspectRatio = (float) HEIGHT / (float) WIDTH;
            fNear = 0.1f;
            fFar = 1000.0f;

            fTheta = 0.0f;

            mesh.LoadFromObjectFile("models/teapot.obj");

            vCamera = { 0.0f, 0.0f, 0.0f };
            vLookDir = { 0.0f, 0.0f, 1.0f };
            matProj = Mat4x4::MakeProjection(fFovDegrees, fAspectRatio, fNear, fFar);

            return true;
        }

        bool OnKeyPressed(SDL_Keycode kc) {
            switch(kc) {
                case SDLK_LEFT:
                    vel.x -= 1;
                    break;
                case SDLK_RIGHT:
                    vel.x += 1;
                    break;
                case SDLK_UP:
                    vel.y -= 1;
                    break;
                case SDLK_DOWN:
                    vel.y += 1;
                    break;
                default:
                    break;
            }

            return true;
        }

        bool OnKeyReleased(SDL_Keycode kc) {
            switch(kc) {
                case SDLK_LEFT:
                    if (vel.x < 0)
                        vel.x = 0;
                    break;
                case SDLK_RIGHT:
                    if (vel.x > 0)
                        vel.x = 0;
                    break;
                case SDLK_UP:
                    if (vel.y < 0)
                        vel.y = 0;
                    break;
                case SDLK_DOWN:
                    if (vel.y > 0)
                        vel.y = 0;
                    break;
                default:
                    break;
            }

            return true;
        }

        bool OnUpdate(float fElapsedTime) override {
                // Move camera
                vCamera.x += vel.x * fElapsedTime;
                vCamera.y += vel.y * fElapsedTime;
                vCamera.z += vel.z * fElapsedTime;

                renderer.Fill(BLACK);

                fTheta += 1.0f * fElapsedTime;

                matRotX = Mat4x4::MakeRotationX(fTheta * 0.5f);
                matRotZ = Mat4x4::MakeRotationZ(fTheta);
                matTrans = Mat4x4::MakeTranslation(0.0f, 0.0f, 8.0f);
                matWorld = matRotZ * matRotX * matTrans;
                
                Vec3d vUp = { 0.0f, 1.0f, 0.0f };
                Vec3d vTarget = vCamera + vLookDir;

                Mat4x4 matCamera = Mat4x4::PointAt(vCamera, vTarget, vUp);

                // Make view matrix from camera
                Mat4x4 matView = Mat4x4::QuickInverse(matCamera);

                std::vector<Triangle> vecTrianglesToRaster;
                std::vector<std::pair<Vec3d, Vec3d>> linesToDraw;

                // Drawing axis
                float axisLength = 100000.0f;
                Vec3d origin = { 0.0f, 0.0f, 0.0f };
                Vec3d xDir = { 1.0f, 0.0f, 0.0f };
                Vec3d yDir = { 0.0f, 1.0f, 0.0f };
                Vec3d zDir = { 0.0f, 0.0f, 1.0f };
                origin = matWorld * origin;
                xDir = matWorld * xDir * axisLength;
                yDir = matWorld * yDir * axisLength;
                zDir = matWorld * zDir * axisLength;
                linesToDraw.push_back({ origin, xDir });
                linesToDraw.push_back({ origin, yDir });
                linesToDraw.push_back({ origin, zDir });

                // Draw Triangles
                for (auto tri : mesh.tris) {
                    Triangle triProjected, triTransformed, triViewed;

                    triTransformed = matWorld * tri;

                    // Calculating Camera Rays to see if the triangle is visible
                    Vec3d vCameraRay = triTransformed.p[0] - vCamera;
                    Vec3d normal = triTransformed.normal();

                    if (normal.dot(vCameraRay) < 0.0f) {
                        // Adding normals to draw list
                        Vec3d p1 = triTransformed.center();
                        Vec3d p2 = p1 + normal;
                        //linesToDraw.push_back({ p1, p2 });

                        // Illumination
                        Vec3d light_direction = { 0.0f, 0.0f, -1.0f };
                        light_direction.normalize();
                        
                        float dp = normal.dot(light_direction);
                        int b = dp * 255 + 0.5f; // brightness

                        uint32_t shade = (b << 16) + (b << 8) + b;
                        triTransformed.col = shade;

                        // Store Triangles for sorting
                        vecTrianglesToRaster.push_back(triTransformed);
                    }
                }

                // Sort Triangles from back to front
                sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](Triangle &t1, Triangle &t2) {
                    float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                    float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;

                    return z1 > z2;
                });

                for (auto &triangle : vecTrianglesToRaster) {
                    // Convert World Space --> View Space
                    triangle = matView * triangle;

                    // Projecting into 2D View
                    triangle = matProj * triangle;

                    triangle.p[0] /= triangle.p[0].w;
                    triangle.p[1] /= triangle.p[1].w;
                    triangle.p[2] /= triangle.p[2].w;

					// X/Y are inverted so put them back
                    triangle *= { -1.0f, -1.0f, 0.0f };

                    // Scale into view
                    triangle += Vec3d(1.0f, 1.0f, 0.0f);
                    triangle *= Vec3d(0.5f * (float) WIDTH, 0.5f * (float) HEIGHT, 1.0f);

                    // Rasterize Triangle
                    renderer.FillTriangle(triangle);
                    //renderer.DrawTriangle(triangle, RED);
                }

                for (auto &line : linesToDraw) {
                    Vec3d p1 = line.first;
                    Vec3d p2 = line.second;

                    // Convert World Space --> View Space
                    p1 = matView * p1;
                    p2 = matView * p2;

                    // Projecting into 2D View
                    p1 = matProj * p1;
                    p2 = matProj * p2;
                    p1 /= p1.w;
                    p2 /= p2.w;

					// X/Y are inverted so put them back
                    p1 *= { -1.0f, -1.0f, 0.0f };
                    p2 *= { -1.0f, -1.0f, 0.0f };

                    // Scale into view
                    p1 += Vec3d(1.0f, 1.0f, 0.0f);
                    p2 += Vec3d(1.0f, 1.0f, 0.0f);
                    p1 *= Vec3d(0.5f * (float) WIDTH, 0.5f * (float) HEIGHT, 1.0f);
                    p2 *= Vec3d(0.5f * (float) WIDTH, 0.5f * (float) HEIGHT, 1.0f);

                    //renderer.DrawLine(p1, p2, GREEN);
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