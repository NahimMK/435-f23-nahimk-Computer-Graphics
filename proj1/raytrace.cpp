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
        if (line[0] == 'v') {
            string cmd;
            getline(file, line); // Read the 'from' line
            cout << line << endl;
            istringstream fromStream(line);
            fromStream >> cmd >> camera.fromX >> camera.fromY >> camera.fromZ;

            getline(file, line); // Read the 'at' line
            istringstream atStream(line);
            atStream >> cmd >> camera.atX >> camera.atY >> camera.atZ;

            getline(file, line); // Read the 'up' line
            istringstream upStream(line);
            upStream >> cmd >> camera.upX >> camera.upY >> camera.upZ;

            getline(file, line); // Read the 'angle' line
            istringstream angleStream(line);
            angleStream >> cmd >> camera.angle;

            getline(file, line); // Read the 'hither' line
            istringstream hitherStream(line);
            hitherStream >> cmd >> camera.hither;
        }else if(line[0] == 'b') {
            istringstream bgStream(line);
            char cmd;
            bgStream >> cmd >> bg.r >> bg.g >> bg.b;
        }else if(line[0] == 'f') {
            istringstream matStream(line);
            char cmd;
            Material material;
            matStream >> cmd >> material.r >> material.g >> material.b;
        }else if(line[0] == 'p') {
            istringstream triStream(line);
            char cmd;
            Triangle triangle;
            triStream >> cmd;
            float vertex;
            while (triStream >> vertex) {
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

    if(parseNFF("tetra-3.nff", camera, bg, triangles)){
        cout << "Camera:" << endl;
        cout << "From: " << camera.fromX << " " << camera.fromY << " " << camera.fromZ << endl;
        cout << "At: " << camera.atX << " " << camera.atY << " " << camera.atZ << endl;
        cout << "Up: " << camera.upX << " " << camera.upY << " " << camera.upZ << endl;
        cout << "Angle: " << camera.angle << endl;
        cout << "Hither: " << camera.hither << endl;

        cout << "Background:" << endl;
        cout << "R: " << bg.r << " G: " << bg.g << " B: " << bg.b << endl;
    }
    return 0;
}