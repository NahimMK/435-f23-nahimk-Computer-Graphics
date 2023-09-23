#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
using namespace std;
/*
struct Ray {
    Eigen::Vector3f origin;
    Eigen::Vector3f direction;
};
*/


struct Camera {
    float fromX, fromY, fromZ;
    float atX, atY, atZ;
    float upX, upY, upZ;
    float angle;
    float hither;
    int resX, resY;
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
            getline(file, line); // Read the from line
            cout << line << endl;
            istringstream fromStream(line);
            fromStream >> cmd >> camera.fromX >> camera.fromY >> camera.fromZ;

            getline(file, line); // Read the at line
            istringstream atStream(line);
            atStream >> cmd >> camera.atX >> camera.atY >> camera.atZ;

            getline(file, line); // Read the up line
            istringstream upStream(line);
            upStream >> cmd >> camera.upX >> camera.upY >> camera.upZ;

            getline(file, line); // Read the angle line
            istringstream angleStream(line);
            angleStream >> cmd >> camera.angle;

            getline(file, line); // Read the hither line
            istringstream hitherStream(line);
            hitherStream >> cmd >> camera.hither;

            getline(file, line); // Read the resolution line
            istringstream resStream(line);
            resStream >> cmd >> camera.resX >> camera.resY;
        }else if(line[0] == 'b') {
            istringstream bgStream(line);
            char cmd;
            bgStream >> cmd >> bg.r >> bg.g >> bg.b;
        }else if(line[0] == 'f' && line[1] == ' ') {
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

        cout << "Resolution:" << endl;
        cout << "Height: " << camera.resY << " Width: " << camera.resX << endl;
    }

    unsigned char pixels[camera.resY][camera.resX][3];
    for (int y = 0; y < camera.resY; y++) {
        for (int x = 0; x < camera.resX; x++) {
            pixels[y][x][0] = static_cast<unsigned char>(bg.r * 255);
            pixels[y][x][1] = static_cast<unsigned char>(bg.g * 255);
            pixels[y][x][2] = static_cast<unsigned char>(bg.b * 255);
        }
    }

    FILE *f = fopen("output.ppm", "wb");
    fprintf(f, "P6\n%d %d\n%d\n", camera.resX, camera.resY, 255);
    fwrite(pixels, 1, camera.resY * camera.resX * 3, f);
    fclose(f);
    return 0;
}