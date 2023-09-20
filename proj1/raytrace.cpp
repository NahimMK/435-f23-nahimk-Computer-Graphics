#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;

const int WIDTH = 512; //Change later to parse from nff resolution
const int HEIGHT = 512;

struct Camera {
    float fromX, fromY, fromZ;
    float atX, atY, atZ;
    float upX, upY, upZ;
    float angle;
    float hither;
};

struct Background {
    float r, g, b;
};

struct Material {
    float r, g, b;
};

struct Triangle {
    vector<float> vertices;
    Material material;
};

bool parseNFF(const string& filename, Camera& camera, Background& bg, vector<Triangle>& triangles){
    ifstream file(filename);
    if(!file.is_open()) {
        cerr << "Failed to open NFF file" << endl;
        return false;
    }

    string line;
    while(getline(file, line)) {
        istringstream iss(line);
        char cmd;
        if(line[0] == 'v') {
            iss >> cmd >> camera.fromX >> camera.fromY >> camera.fromZ;
            iss >> cmd >> camera.atX >> camera.atY >> camera.atZ;
            iss >> cmd >> camera.upX >> camera.upY >> camera.upZ;
            iss >> cmd >> camera.angle;
            iss >> cmd >> camera.hither;
        }else if(line[0] == 'b') {
            iss >> cmd >> bg.r >> bg.g >> bg.b;
        }else if(line[0] == 'f') {
            Material material;
            iss >> cmd >> material.r >> material.g >> material.b;
        }else if(line[0] == 'p') {
            Triangle triangle;
            iss >> cmd;
            float vertex;
            while (iss >> vertex) {
                triangle.vertices.push_back(vertex);
            }
            triangles.push_back(triangle);
        }
    }
    file.close();
    return true;
}

int main() {
    Camera camera;
    Background bg;
    vector<Triangle> triangles;

    if(parseNFF("input.nff", camera, bg, triangles)){

    }
    return 0;
}