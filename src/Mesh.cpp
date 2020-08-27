#ifndef _MESH
#define _MESH

#include <stdint.h>
#include <vector>

#include <string>
#include <fstream>
#include <sstream>

#include "Vec3d.cpp"
#include "Triangle.cpp"

struct Mesh {
    std::vector<Triangle> tris;

    bool LoadFromObjectFile(std::string sFilename) {
        std::ifstream f(sFilename);
        if (!f.is_open())
            return false;

        // Local cache of verts
        std::vector<Vec3d> verts;

        while (!f.eof()) {
            char line[128];
            f.getline(line, 128);

            std::stringstream s;
            s << line;

            char junk;
            
            if (line[0] == 'v') {
                Vec3d v;
                s >> junk >> v.x >> v.y >> v.z;
                verts.push_back(v);
            }

            if (line[0] == 'f') {
                int f[3];
                s >> junk >> f[0] >> f[1] >> f[2];
                tris.push_back( { verts[f[0] - 1], verts [f[1] - 1], verts[f[2] - 1] });
            }
        }

        return 1;
    }
};

#endif