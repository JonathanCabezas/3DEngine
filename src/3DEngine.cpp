#include <vector>
#include <algorithm>
#include <list>

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

        // Camera
        Vec3d vCamera;
        Vec3d vLookDir;
        Vec3d vForward;
        Vec3d vVel;

        // Movement
        float fYaw;
        float fYawVel;
        float fForwardSpeed;
        float fYawSpeed;
        float fLateralSpeed;

        // Projection Matrix
        float fFovDegrees;
        float fAspectRatio;
        float fNear;
        float fFar;

        float fTheta;

    public:
        bool OnCreate() override {
            fFovDegrees = 90;
            fAspectRatio = (float) HEIGHT / (float) WIDTH;
            fNear = 0.1f;
            fFar = 1000;

            fTheta = 0;

            mesh.LoadFromObjectFile("models/teapot.obj");

            // Initializing Movement
            fYaw = 0;
            fYawVel = 0;
            fForwardSpeed = 8;
            fYawSpeed = 2;
            fLateralSpeed = 8;

            // Initializing Camera and Projection
            vCamera = { 0, 0, 0 };
            vLookDir = { 0, 0, 1 };
            matProj = Mat4x4::MakeProjection(fFovDegrees, fAspectRatio, fNear, fFar);

            return true;
        }

        bool OnKeyPressed(SDL_Keycode kc) {
            switch(kc) {
                case SDLK_LEFT:
                    vVel.x = -fLateralSpeed;
                    break;
                case SDLK_RIGHT:
                    vVel.x = fLateralSpeed;
                    break;
                case SDLK_UP:
                    vVel.y = fLateralSpeed;
                    break;
                case SDLK_DOWN:
                    vVel.y = -fLateralSpeed;
                    break;

                case SDLK_z:
                    vVel.z = fForwardSpeed;
                    break;
                case SDLK_s:
                    vVel.z = -fForwardSpeed;
                    break;
                case SDLK_q:
                    fYawVel = fYawSpeed;
                    break;
                case SDLK_d:
                    fYawVel = -fYawSpeed;
                    break;
                default:
                    break;
            }

            return true;
        }

        bool OnKeyReleased(SDL_Keycode kc) {
            switch(kc) {
                case SDLK_LEFT:
                    vVel.x = 0;
                    break;
                case SDLK_RIGHT:
                    vVel.x = 0;
                    break;
                case SDLK_UP:
                    vVel.y = 0;
                    break;
                case SDLK_DOWN:
                    vVel.y = 0;
                    break;
                    
                case SDLK_z:
                    vVel.z = 0;
                    break;
                case SDLK_s:
                    vVel.z = 0;
                    break;
                case SDLK_q:
                    fYawVel = 0;
                    break;
                case SDLK_d:
                    fYawVel = 0;
                    break;
                default:
                    break;
            }

            return true;
        }

        bool OnUpdate(float fElapsedTime) override {
                // Move camera
                vCamera.x += vVel.x * fElapsedTime;
                vCamera.y += vVel.y * fElapsedTime;
                vCamera += vLookDir * vVel.z * fElapsedTime;
                fYaw += fYawVel * fElapsedTime;

                renderer.Fill(BLACK);

                fTheta += 1 * fElapsedTime;

                matRotX = Mat4x4::MakeRotationX(fTheta * 0.5);
                matRotZ = Mat4x4::MakeRotationZ(fTheta);
                matTrans = Mat4x4::MakeTranslation(0, 0, 8);
                matWorld = matRotZ * matRotX * matTrans;
                
                Vec3d vUp = { 0, 1, 0 };
                Vec3d vTarget = { 0, 0, 1 };
                Mat4x4 matCameraRot = Mat4x4::MakeRotationY(fYaw);
                vLookDir = matCameraRot * vTarget;
                vTarget = vCamera + vLookDir;

                Mat4x4 matCamera = Mat4x4::PointAt(vCamera, vTarget, vUp);

                // Make view matrix from camera
                Mat4x4 matView = Mat4x4::QuickInverse(matCamera);

                std::vector<Triangle> vecTrianglesToRaster;
                std::vector<std::pair<Vec3d, Vec3d>> linesToDraw;

                // Drawing axis
                float axisLength = 2;
                Vec3d origin = { 0, 0, 0 };
                Vec3d xDir = { 1, 0, 0 };
                Vec3d yDir = { 0, 1, 0 };
                Vec3d zDir = { 0, 0, 1 };
                origin = matWorld * origin;
                xDir = matWorld * (xDir * axisLength);
                yDir = matWorld * (yDir * axisLength);
                zDir = matWorld * (zDir * axisLength);
                //Vec3d vCameraRay = origin - vCamera;

                //linesToDraw.push_back({ origin, xDir });
                //linesToDraw.push_back({ origin, yDir });
                //linesToDraw.push_back({ origin, zDir });

                // Draw Triangles
                for (auto tri : mesh.tris) {
                    Triangle triProjected, triTransformed, triViewed;

                    triTransformed = matWorld * tri;

                    // Calculating Camera Rays to see if the triangle is visible
                    Vec3d vCameraRay = triTransformed.p[0] - vCamera;
                    Vec3d normal = triTransformed.normal();

                    if (normal.dot(vCameraRay) < 0.0f) {
                        // Adding normals to draw list
                        //Vec3d p1 = triTransformed.center();
                        //Vec3d p2 = p1 + normal * 2;
                        //linesToDraw.push_back({ p1, p2 });

                        // Illumination
                        Vec3d light_direction = { 0, 1, -1 };
                        light_direction.normalize();
                        
                        float dp = std::max(0.1f, normal.dot(light_direction));
                        int b = dp * 255 + 0.5; // brightness

                        uint32_t shade = (b << 16) + (b << 8) + b;
                        triTransformed.col = shade;

                        // Convert World Space --> View Space
                        triViewed = matView * triTransformed;

                        // Clip Viewed Triangle against near plane, this could form two additional
                        // additional triangles. 
                        int nClippedTriangles = 0;
                        Triangle clipped[2];
                        nClippedTriangles = triViewed.clipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, clipped[0], clipped[1]);

                        for (int n = 0; n < nClippedTriangles; n++) {
                            triProjected = matProj * clipped[n];
                            triProjected.p[0] /= triProjected.p[0].w;
                            triProjected.p[1] /= triProjected.p[1].w;
                            triProjected.p[2] /= triProjected.p[2].w;

                            // Scale into view
                            triProjected += Vec3d(1, 1, 0);
                            triProjected *= Vec3d(0.5 * (float) WIDTH, 0.5 * (float) HEIGHT, 1);

                            // Store Triangles for sorting
                            vecTrianglesToRaster.push_back(triProjected);
                        }
                    }
                }

                // Sort Triangles from back to front
                sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](Triangle &t1, Triangle &t2) {
                    float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3;
                    float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3;

                    return z1 > z2;
                });

                for (auto &triToRaster : vecTrianglesToRaster) {
                    // Clip triangles against all four screen edges, this could yield
                    // a bunch of triangles, so create a queue that we traverse to 
                    //  ensure we only test new triangles generated against planes
                    Triangle clipped[2];
                    std::list<Triangle> listTriangles;

                    // Add initial triangle
                    listTriangles.push_back(triToRaster);
                    int nNewTriangles = 1;

                    for (int p = 0; p < 4; p++) {
                        int nTrisToAdd = 0;
                        while (nNewTriangles > 0) {
                            // Take triangle from front of queue
                            Triangle test = listTriangles.front();
                            listTriangles.pop_front();
                            nNewTriangles--;

                            // Clip it against a plane. We only need to test each 
                            // subsequent plane, against subsequent new triangles
                            // as all triangles after a plane clip are guaranteed
                            // to lie on the inside of the plane. I like how this
                            // comment is almost completely and utterly justified
                            switch (p) {
                                case 0:	nTrisToAdd = test.clipAgainstPlane({ 0, 0, 0 }, { 0, 1, 0 }, clipped[0], clipped[1]); break;
                                case 1:	nTrisToAdd = test.clipAgainstPlane({ 0, (float)HEIGHT - 1, 0 }, { 0, -1, 0 }, clipped[0], clipped[1]); break;
                                case 2:	nTrisToAdd = test.clipAgainstPlane({ 0, 0, 0 }, { 1, 0, 0 }, clipped[0], clipped[1]); break;
                                case 3:	nTrisToAdd = test.clipAgainstPlane({ (float)WIDTH - 1, 0, 0 }, { -1, 0, 0 }, clipped[0], clipped[1]); break;
                            }

                            // Clipping may yield a variable number of triangles, so
                            // add these new ones to the back of the queue for subsequent
                            // clipping against next planes
                            for (int w = 0; w < nTrisToAdd; w++)
                                listTriangles.push_back(clipped[w]);
                        }
                        nNewTriangles = listTriangles.size();
                    }


                    // Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
                    for (auto &triangle : listTriangles) {
                        // Rasterize Triangle
                        renderer.FillTriangle(triangle);
                        renderer.DrawTriangle(triangle, RED);
                    }
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

                    // Scale into view
                    p1 += Vec3d(1, 1, 0);
                    p2 += Vec3d(1, 1, 0);
                    p1 *= Vec3d(0.5 * (float) WIDTH, 0.5 * (float) HEIGHT, 1);
                    p2 *= Vec3d(0.5 * (float) WIDTH, 0.5 * (float) HEIGHT, 1);

                    renderer.DrawLine(p1, p2, GREEN);
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