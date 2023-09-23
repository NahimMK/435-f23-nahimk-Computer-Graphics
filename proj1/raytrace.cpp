//CMSC 435 Computer Graphics
//Rendering Triangles Using Raytracing
//Nahim Kamruzzaman
//9.23.24
//IMPORTANT: PLEASE RUN THE ./raytrace A COUPLE OF TIMES (Like 4 or 5 or 10)
//I'm not sure why but sometimes one of the triangle protrudes out or is very big.
//This is a program that parses and NFF file and renders triangles
//using raytracing and the barycentric intersection method.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;

//struct for rays
struct Ray {
    Vector3f origin;     // Starting point of ray
    Vector3f direction;  // Direction ray points
};

//struct for the camera, eye, origin
struct Camera {
    float fromX, fromY, fromZ;
    float atX, atY, atZ;
    float upX, upY, upZ;
    float angle;
    float hither;
    int resX, resY;
};

//struct for background color
struct Background {
    float r, g, b;
};

//struct for triangle color
struct Material {
    float r, g, b;
};

//struct to hold triangle vertices and has a material property
struct Triangle {
    vector<float> vertices;
    Material material;
};

//function to parse the nff file 
bool parseNFF(const string& filename, Camera& camera, Background& bg, vector<Triangle>& triangles){
    //opens file
    ifstream file(filename);
    if(!file.is_open()) {
        cerr << "Failed to open NFF file" << endl;
        return false;
    }

    string line;
    //goes through nff file line by line and checks first char
    while(getline(file, line)) {
        if (line[0] == 'v') {
            string cmd; //place holder for command string/char
            getline(file, line); //read the from line
            istringstream fromStream(line); //shortform iss
            fromStream >> cmd >> camera.fromX >> camera.fromY >> camera.fromZ; //parses and stores values

            getline(file, line);
            istringstream atStream(line);
            atStream >> cmd >> camera.atX >> camera.atY >> camera.atZ;

            getline(file, line);
            istringstream upStream(line);
            upStream >> cmd >> camera.upX >> camera.upY >> camera.upZ;

            getline(file, line);
            istringstream angleStream(line);
            angleStream >> cmd >> camera.angle;

            getline(file, line);
            istringstream hitherStream(line);
            hitherStream >> cmd >> camera.hither;

            getline(file, line);
            istringstream resStream(line);
            resStream >> cmd >> camera.resX >> camera.resY;
        }else if(line[0] == 'b') {
            istringstream bgStream(line);
            char cmd; //place holder for command string/char
            bgStream >> cmd >> bg.r >> bg.g >> bg.b; //parses and stores values for background
        } else if (line[0] == 'f') {
            istringstream matStream(line);
            char cmd; //place holder for command string/char
            Material material;
            matStream >> cmd >> material.r >> material.g >> material.b; //parses and stores values for triangle color

            //declare a triangle object
            Triangle triangle;

            triangle.material = material; //assign triangle color

            //parse and store vertices of this triangle
            while (getline(file, line) && line[0] == 'p') {
                istringstream triStream(line);
                char cmd;
                float vertex;
                triStream >> cmd;
                while (triStream >> vertex) {
                    triangle.vertices.push_back(vertex);
                }
                triangles.push_back(triangle);
            }
        } else if (line[0] == 'p') {
            //parse the triangle vertices with the material
            Triangle triangle;
            istringstream triStream(line);
            char cmd; //place holder for command string/char
            triStream >> cmd;
            //stores vertices for each triangle
            for (int i = 0; i < 3; i++) {
                float vertexX, vertexY, vertexZ;
                getline(file, line);
                istringstream vertexStream(line);
                vertexStream >> vertexX >> vertexY >> vertexZ;
                triangle.vertices.push_back(vertexX);
                triangle.vertices.push_back(vertexY);
                triangle.vertices.push_back(vertexZ);
            }

            // Assign the material color to the triangle
            triangle.material = triangles.back().material;

            triangles.push_back(triangle);
        }
    }
    //closes the file
    file.close();
    return true;
}

//function generates a ray from the camera and through the pixel
Ray generateRay(int x, int y, const Camera& camera) {
    //creates ray object
    Ray ray;
    //sets origin of the ray (where camera is in space)
    ray.origin = Vector3f(camera.fromX, camera.fromY, camera.fromZ);

    //calculate the camera coordinate system
    Vector3f forward = (Vector3f(camera.atX, camera.atY, camera.atZ) - ray.origin).normalized();
    Vector3f right = forward.cross(Vector3f(camera.upX, camera.upY, camera.upZ)).normalized();
    Vector3f up = right.cross(forward).normalized();

    //calculate the ray direction based on pixel coordinates
    float aspectRatio = static_cast<float>(camera.resX) / static_cast<float>(camera.resY);
    float pixelX = (2.0f * (x + 0.5f) / camera.resX - 1.0f) * aspectRatio * tan(camera.angle / 2.0f);
    float pixelY = (1.0f - 2.0f * (y + 0.5f) / camera.resY) * tan(camera.angle / 2.0f);

    //calculate the ray direction in world space (transformation)
    ray.direction = (forward + pixelX * right + pixelY * up).normalized();

    return ray;
}

//ray-triangle intersection method using barycentric coordinates
bool intersectTriangle(const Ray& ray, const Triangle& triangle, Vector3f& intersectionPoint) {
    //stores edges
    const Vector3f& v0 = Vector3f(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]);
    const Vector3f& v1 = Vector3f(triangle.vertices[3], triangle.vertices[4], triangle.vertices[5]);
    const Vector3f& v2 = Vector3f(triangle.vertices[6], triangle.vertices[7], triangle.vertices[8]);
    
    //calculates the edges of the triangles
    Vector3f e1 = v1 - v0;
    Vector3f e2 = v2 - v0;
    Vector3f h = ray.direction.cross(e2);
    float a = e1.dot(h);

    //checks if ray is parallel to triangle plane
    if (a > -0.00001 && a < 0.00001) {
        return false;
    }

    float f = 1.0f / a;
    Vector3f s = ray.origin - v0;
    float u = f * s.dot(h);

    //checks if the intersection point is not in triangle
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    Vector3f q = s.cross(e1);
    float v = f * ray.direction.dot(q);

    //checks if intersection point is outside the triangle
    if (v < 0.0f || u + v > 1.0f) {
        return false; 
    }

    float t = f * e2.dot(q);

    //checks if intersection point is valid
    if (t > 0.00001) {
        intersectionPoint = ray.origin + ray.direction * t;
        return true;
    }

    //this is if the intersection point is behind the camera
    return false;
}

int main() {
    Camera camera;
    Background bg;
    vector<Triangle> triangles;
    
    //parses the NFF file and stores the camera, background, and triangle properties
    if (parseNFF("tetra-3.nff", camera, bg, triangles)) {
        //for debugging and checking values parsed are correct
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

    //dynamically allocate array for image
    unsigned char* pixels;
    //use resolution for height and width
    pixels = new unsigned char[camera.resY * camera.resX * 3];
    //loops through each pixel in array
    for (int y = 0; y < camera.resY; y++) {
        for (int x = 0; x < camera.resX; x++) {
            int index = (y * camera.resX + x) * 3; // Calculate the index

            //generates a ray from the camera through the pixel
            Ray ray = generateRay(x, y, camera);

            //initialize the pixel color to background color
            Vector3f pixelColor(bg.r, bg.g, bg.b);

            //checks for intersections with triangles
            for (const Triangle& triangle : triangles) {
                Vector3f intersectionPoint;
                if (intersectTriangle(ray, triangle, intersectionPoint)) {
                    //sets the pixel color to the triangle's material color if there's an intersection
                    pixelColor = Vector3f(triangle.material.r, triangle.material.g, triangle.material.b);
                }
            }

            //assigns the pixel color to the pixels in the image
            pixels[index] = static_cast<unsigned char>(pixelColor.x() * 255);
            pixels[index + 1] = static_cast<unsigned char>(pixelColor.y() * 255);
            pixels[index + 2] = static_cast<unsigned char>(pixelColor.z() * 255);
        }
    }

    //write to a ppm file (as given in project description)
    FILE* f = fopen("output.ppm", "wb");
    fprintf(f, "P6\n%d %d\n%d\n", camera.resX, camera.resY, 255);
    fwrite(pixels, 1, camera.resY * camera.resX * 3, f);
    fclose(f);
    
    //frees dynamically allocated memory
    delete[] pixels;

    return 0;
}