//CMSC 435 Computer Graphics
//Creating a rasterization based renderer.
//Nahim Kamruzzaman
//10.22.23
//IMPORTANT: If you want to run the program twice, run make clean before you make again.
//Please look at my functions as I think they all make a good amount of sense based on what
//we learned in class, I just couldn't get the pixels to change colors.
//Partial credit?

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <Eigen/Dense>

//define vertex data structure
struct Vertex {
    float x, y, z;
};
//define material data structure
struct Color {
    float r, g, b;
};

//for patches
struct Patch {
    std::vector<Vertex> vertices;
    std::vector<Color> colors;
    std::vector<Eigen::Vector3f> normals;
};

//for background color
struct Background {
    Color color;
};

//for the view
struct View {
    Vertex from, at, up;
    float angle, hither;
    int resolution_x, resolution_y;
    Background background;
};

//for lighting
struct Light {
    Vertex position;
};

//using Eigens image so I don't have to allocate and deallocate
using Image = Eigen::Matrix<Color, Eigen::Dynamic, Eigen::Dynamic>;

//for fragment processing
struct Fragment {
    int x, y;
    Vertex position;
    Color color;
};

//apply a transformation matrix to a vertex
//this function takes a vertex and a transformation matrix and returns a new vertex
Vertex applyTransformation(const Vertex& vertex, const Eigen::Matrix4f& transformationMatrix) {
    //convert the 3D vertex to a 4D homogeneous vector
    Eigen::Vector4f v(vertex.x, vertex.y, vertex.z, 1.0);
    //apply the transformation matrix to the vertex
    Eigen::Vector4f transformed = transformationMatrix * v;
    //convert the resulting 4D vector back to a 3D vertex
    return {transformed.x(), transformed.y(), transformed.z()};
}

//perform vertex processing, transforming vertices of a patch
//this function takes a patch and a transformation matrix and applies the transformation to each vertex in the patch
void vertexProcessing(Patch& patch, const Eigen::Matrix4f& transformationMatrix) {
    for (Vertex& vertex : patch.vertices) {
        //apply the transformation to each vertex in the patch
        vertex = applyTransformation(vertex, transformationMatrix);
    }
}

//check if a vertex is inside a clipping plane
//this function determines if a vertex is inside a clipping plane defined by the equation Ax + By + Cz + D >= 0
bool isInsideClip(const Vertex& vertex, float A, float B, float C, float D) {
    //calculate the result of the plane equation
    float result = A * vertex.x + B * vertex.y + C * vertex.z + D;
    //return true if the vertex is inside the clipping plane (result is positive)
    return (result >= 0.0);
}

//clips patches against a plane defined by the equation Ax + By + Cz + D = 0
void clipAgainstPlane(std::vector<Patch>& patches, float A, float B, float C, float D) {
    std::vector<Patch> clippedPatches;
    
    for (const Patch& patch : patches) {
        std::vector<Vertex> insideVertices;
        std::vector<Vertex> outsideVertices;
        
        //check if each vertex of the patch is inside or outside the clipping plane
        for (const Vertex& vertex : patch.vertices) {
            if (isInsideClip(vertex, A, B, C, D)) {
                insideVertices.push_back(vertex);
            } else {
                outsideVertices.push_back(vertex);
            }
        }
        
        //debug information: Number of patches before and after clipping
        std::cout << "Patches before clipping: " << patches.size() << std::endl;
        std::cout << "Inside vertices: " << insideVertices.size() << std::endl;
        std::cout << "Outside vertices: " << outsideVertices.size() << std::endl;
        
        if (insideVertices.size() == 3) {
            clippedPatches.push_back(patch);
        } else if (insideVertices.size() == 1) {

            Vertex intersection1, intersection2;

            float t1, t2;

            //calculate intersection points with the clipping plane
            intersection1.x = insideVertices[0].x + t1 * (outsideVertices[0].x - insideVertices[0].x);
            intersection1.y = insideVertices[0].y + t1 * (outsideVertices[0].y - insideVertices[0].y);
            intersection1.z = insideVertices[0].z + t1 * (outsideVertices[0].z - insideVertices[0].z);

            intersection2.x = insideVertices[0].x + t2 * (outsideVertices[1].x - insideVertices[0].x);
            intersection2.y = insideVertices[0].y + t2 * (outsideVertices[1].y - insideVertices[0].y);
            intersection2.z = insideVertices[0].z + t2 * (outsideVertices[1].z - insideVertices[0].z);

            //create two new patches from the intersection points
            Patch newPatch1, newPatch2;
            newPatch1.vertices = {insideVertices[0], intersection1, intersection2};
            newPatch2.vertices = {insideVertices[0], intersection2, outsideVertices[1]};

            clippedPatches.push_back(newPatch1);
            clippedPatches.push_back(newPatch2);
        } else if (insideVertices.size() == 2) {

            Vertex intersection;

            float t;

            //calculate intersection point with the clipping plane
            intersection.x = insideVertices[0].x + t * (outsideVertices[0].x - insideVertices[0].x);
            intersection.y = insideVertices[0].y + t * (outsideVertices[0].y - insideVertices[0].y);
            intersection.z = insideVertices[0].z + t * (outsideVertices[0].z - insideVertices[0].z);

            //create a new patch from the intersection point
            Patch newPatch;
            newPatch.vertices = {insideVertices[0], insideVertices[1], intersection};

            clippedPatches.push_back(newPatch);
        }
        
        //debug information: Number of patches after clipping
        std::cout << "Clipped patches: " << clippedPatches.size() << std::endl;
    }

    patches = clippedPatches;
}

//clips patches against all six clipping planes (view frustum)
void clipScene(std::vector<Patch>& patches, const View& view) {
    
    std::vector<std::tuple<float, float, float, float>> clippingPlanes = {
        {1.0, 0.0, 0.0, 0.0},    //right plane
        {-1.0, 0.0, 0.0, view.resolution_x},  //left plane
        {0.0, 1.0, 0.0, 0.0},    //top plane
        {0.0, -1.0, 0.0, view.resolution_y}, //bottom plane
        {0.0, 0.0, 1.0, 0.0},    //near plane
        {0.0, 0.0, -1.0, 1000.0} //far plane
    };

    //iterate through all six clipping planes and perform patch clipping
    for (const auto& plane : clippingPlanes) {
        float A, B, C, D;
        std::tie(A, B, C, D) = plane;
        clipAgainstPlane(patches, A, B, C, D);
    }
}

//gets the bounds 
void getBoundingBox(const Patch& patch, int& xMin, int& xMax, int& yMin, int& yMax) {
    //initialize the bounding box coordinates with the first vertex
    xMin = xMax = static_cast<int>(patch.vertices[0].x);
    yMin = yMax = static_cast<int>(patch.vertices[0].y);

    //iterate through the vertices to find the minimum and maximum coordinates
    for (const Vertex& vertex : patch.vertices) {
        int x = static_cast<int>(vertex.x);
        int y = static_cast<int>(vertex.y);

        //update xMin, xMax, yMin, and yMax if necessary
        xMin = std::min(xMin, x);
        xMax = std::max(xMax, x);
        yMin = std::min(yMin, y);
        yMax = std::max(yMax, y);
    }
}

//checks if a point (x, y) is inside a triangle defined by the vertices of a patch.
bool isInsideTriangle(int x, int y, const Patch& patch) {
    const Vertex& v0 = patch.vertices[0];
    const Vertex& v1 = patch.vertices[1];
    const Vertex& v2 = patch.vertices[2];

    //calculate the denominator for barycentric coordinates.
    float denominator = ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y));

    //check if the denominator is zero, which means the triangle is degenerate.
    if (denominator == 0) {
        return false; // Not a valid triangle.
    }

    //calculate alpha and beta using barycentric coordinates.
    float alpha = ((v1.y - v2.y) * (x - v2.x) + (v2.x - v1.x) * (y - v2.y)) / denominator;
    float beta = ((v2.y - v0.y) * (x - v2.x) + (v0.x - v2.x) * (y - v2.y)) / denominator;

    //check if the point is inside the triangle (alpha and beta are in the range [0, 1]).
    return (alpha >= 0.0 && beta >= 0.0 && alpha + beta <= 1.0);
}

//interpolates color within a triangle at a specific (x, y) position.
Color interpolateColor(int x, int y, const Patch& patch) {
    //retrieve vertices and their corresponding colors.
    const Vertex& v0 = patch.vertices[0];
    const Vertex& v1 = patch.vertices[1];
    const Vertex& v2 = patch.vertices[2];
    const Color& c0 = patch.colors[0];
    const Color& c1 = patch.colors[1];
    const Color& c2 = patch.colors[2];

    //calculate the denominator for barycentric coordinates.
    float denominator = ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y));

    //check if the denominator is zero, which means the triangle is degenerate.
    if (denominator == 0) {
        //return black color in case of a degenerate triangle.
        Color black;
        black.r = 0;
        black.g = 0;
        black.b = 0;
        return black;
    }

    //calculate barycentric coordinates (alpha, beta, gamma).
    float alpha = ((v1.y - v2.y) * (x - v2.x) + (v2.x - v1.x) * (y - v2.y)) / denominator;
    float beta = ((v2.y - v0.y) * (x - v2.x) + (v0.x - v2.x) * (y - v2.y)) / denominator;
    float gamma = 1.0 - alpha - beta;

    // Interpolate the color based on barycentric coordinates.
    Color interpolatedColor;
    interpolatedColor.r = alpha * c0.r + beta * c1.r + gamma * c2.r;
    interpolatedColor.g = alpha * c0.g + beta * c1.g + gamma * c2.g;
    interpolatedColor.b = alpha * c0.b + beta * c1.b + gamma * c2.b;

    return interpolatedColor;
}

//calculates barycentric coordinates for a fragment within a triangle defined by vertices v0, v1, and v2.
void calculateBarycentricCoordinates(const Fragment& fragment, const Vertex& v0, const Vertex& v1, const Vertex& v2, float& alpha, float& beta, float& gamma) {
    //calculate the full area of the triangle and areas relative to the fragment.
    float fullTriangleArea = 0.5f * ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y));
    float area0 = 0.5f * ((v1.y - v2.y) * (fragment.position.x - v2.x) + (v2.x - v1.x) * (fragment.position.y - v2.y));
    float area1 = 0.5f * ((v2.y - v0.y) * (fragment.position.x - v2.x) + (v0.x - v2.x) * (fragment.position.y - v2.y));
    float area2 = 0.5f * ((v0.y - v1.y) * (fragment.position.x - v0.x) + (v1.x - v0.x) * (fragment.position.y - v0.y));

    //calculate the barycentric coordinates (alpha, beta, gamma) based on the areas.
    alpha = area0 / fullTriangleArea;
    beta = area1 / fullTriangleArea;
    gamma = area2 / fullTriangleArea;
}

//calculates phong shading (caused seg fault)
Color calculatePhongShading(const Fragment& fragment, const Patch& patch, const Light& light) {

    Color flatColor;
    flatColor.r = 1.0f;
    flatColor.g = 1.0f;
    flatColor.b = 1.0f;

    return flatColor;
}
/*
Color calculatePhongShading(const Fragment& fragment, const Patch& patch, const Light& light) {
    //check if the patch has at least one vertex
    if (patch.vertices.size() < 1) {
        Color black;
        black.r = 0;
        black.g = 0;
        black.b = 0;
        return black;
    }

    //calculate the direction from the fragment to the light source
    Eigen::Vector3f fragmentPosition(fragment.position.x, fragment.position.y, fragment.position.z);
    Eigen::Vector3f lightPosition(light.position.x, light.position.y, light.position.z);
    Eigen::Vector3f lightDirection = (lightPosition - fragmentPosition).normalized();

    //check if the patch has at least one normal vector
    if (patch.normals.size() < 1) {
        Color black;
        black.r = 0;
        black.g = 0;
        black.b = 0;
        return black;
    }

    Eigen::Vector3f normal0(patch.normals[0].x(), patch.normals[0].y(), patch.normals[0].z());
    normal0.normalize(); 

    //calculate the reflection direction
    Eigen::Vector3f reflectionDirection = (-lightDirection + 2 * normal0.dot(lightDirection) * normal0).normalized();

    if (patch.colors.size() < 1) {
        Color black;
        black.r = 0;
        black.g = 0;
        black.b = 0;
        return black;
    }

    Color patchColor = patch.colors[0];

    float diffuseIntensity = std::max(0.0f, normal0.dot(lightDirection));
    float specularIntensity = std::pow(std::max(0.0f, reflectionDirection.dot(fragmentPosition.normalized())), 10); // Shininess factor: 10

    Color phongColor;
    phongColor.r = patchColor.r * (0.1f + 0.9f * (diffuseIntensity + specularIntensity));
    phongColor.g = patchColor.g * (0.1f + 0.9f * (diffuseIntensity + specularIntensity));
    phongColor.b = patchColor.b * (0.1f + 0.9f * (diffuseIntensity + specularIntensity));

    return phongColor;
}
*/

//processes fragments and fills the image with a flat color for each fragment.
void fragmentProcessing(const Patch& patch, Image& image, const Color& flatColor) {
    for (const Vertex& vertex : patch.vertices) {
        int x = static_cast<int>(vertex.x);
        int y = static_cast<int>(vertex.y);
        if (x >= 0 && x < image.cols() && y >= 0 && y < image.rows()) {
            //fill the image with the specified flat color at the (x, y) position.
            image(y, x) = flatColor;
        }
    }
}

//rasterizes a patch into the image and interpolates colors for pixels inside the triangle.
void rasterizePatch(const Patch& patch, Image& image, const View& view) {
    for (const Vertex& vertex : patch.vertices) {
        int x = static_cast<int>(vertex.x);
        int y = static_cast<int>(vertex.y);
        if (x >= 0 && x < view.resolution_x && y >= 0 && y < view.resolution_y) {
            Fragment fragment;
            fragment.x = x;
            fragment.y = y;
            fragment.position = vertex;

            if (isInsideTriangle(x, y, patch)) {
                //interpolate color for the pixel inside the triangle and update the image.
                Color interpolatedColor = interpolateColor(x, y, patch);
                image(y, x) = interpolatedColor;
            }
        }
    }
}

//rasterizes the entire scene composed of patches into the image.
void rasterizeScene(const View& view, const std::vector<Patch>& patches, Image& image) {
    for (const Patch& patch : patches) {
        int xMin, xMax, yMin, yMax;
        getBoundingBox(patch, xMin, xMax, yMin, yMax);

        float fullTriangleArea = 0.5f * ((patch.vertices[1].y - patch.vertices[2].y) * (patch.vertices[0].x - patch.vertices[2].x) + (patch.vertices[2].x - patch.vertices[1].x) * (patch.vertices[0].y - patch.vertices[2].y));

        for (int x = std::max(0, xMin); x < std::min(view.resolution_x, xMax); ++x) {
            for (int y = std::max(0, yMin); y < std::min(view.resolution_y, yMax); ++y) {
                if (isInsideTriangle(x, y, patch)) {
                    float area0 = 0.5f * ((patch.vertices[1].y - patch.vertices[2].y) * (x - patch.vertices[2].x) + (patch.vertices[2].x - patch.vertices[1].x) * (y - patch.vertices[2].y));
                    float area1 = 0.5f * ((patch.vertices[2].y - patch.vertices[0].y) * (x - patch.vertices[2].x) + (patch.vertices[0].x - patch.vertices[2].x) * (y - patch.vertices[2].y));
                    float area2 = 0.5f * ((patch.vertices[0].y - patch.vertices[1].y) * (x - patch.vertices[0].x) + (patch.vertices[1].x - patch.vertices[0].x) * (y - patch.vertices[0].y));
                    float alpha = area0 / fullTriangleArea;
                    float beta = area1 / fullTriangleArea;
                    float gamma = area2 / fullTriangleArea;

                    Color interpolatedColor;
                    interpolatedColor.r = alpha * patch.colors[0].r + beta * patch.colors[1].r + gamma * patch.colors[2].r;
                    interpolatedColor.g = alpha * patch.colors[0].g + beta * patch.colors[1].g + gamma * patch.colors[2].g;
                    interpolatedColor.b = alpha * patch.colors[0].b + beta * patch.colors[1].b + gamma * patch.colors[2].b;

                    if (x >= 0 && x < view.resolution_x && y >= 0 && y < view.resolution_y) {
                        //update the image with the interpolated color.
                        image(y, x) = interpolatedColor;
                    }
                }
            }
        }
    }
}

//outputs image as a ppm file
void savePPMImage(const Image& image, const std::string& filename) {
    std::ofstream ppmFile(filename);
    if (!ppmFile.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    //write the PPM header
    ppmFile << "P6\n" << image.cols() << " " << image.rows() << "\n255\n";

    //write the image data
    for (int y = 0; y < image.rows(); ++y) {
        for (int x = 0; x < image.cols(); ++x) {
            Color color = image(y, x);
            ppmFile.put(static_cast<char>(color.r * 255));
            ppmFile.put(static_cast<char>(color.g * 255));
            ppmFile.put(static_cast<char>(color.b * 255));
        }
    }

    ppmFile.close();
}

//renders entire scene
void renderScene(const View& view, std::vector<Patch>& patches, const std::vector<Light>& lights) {
    //std::cout << "Number of patches at beginning of render: " << patches.size() << std::endl;

    clipScene(patches, view);

    std::cout << "Number of patches after clipping: " << patches.size() << std::endl;

    Image image(view.resolution_x, view.resolution_y);
    image.setConstant(view.background.color);

    //std::cout << "Made it after drawing background: " << patches.size() << std::endl;

    for (const Light& light : lights) {
        std::cout << "Made it into for loop with light sources " << patches.size() << std::endl;
        rasterizeScene(view, patches, image);
        std::cout << "Rasterization complete" << std::endl;

        for (Patch& patch : patches) {
            for (Vertex& vertex : patch.vertices) {

            }
            Color flatColor; //choose a flat color for your patches
            flatColor.r = 1.0; //set red component
            flatColor.g = 1.0; //set green component
            flatColor.b = 1.0; //set blue component
            //std::cout << "Right before fragment processing." << std::endl;
            fragmentProcessing(patch, image, flatColor);
            //std::cout << "Fragment shading complete" << std::endl;
        }
    }

    //save the rendered image to a file or display it as needed
    savePPMImage(image, "output.ppm");
}

//parses the nff file
std::vector<Patch> parse_nff_file(const std::string& file_path, View& view, std::vector<Light>& lights) {
    std::ifstream nff_file(file_path);
    std::string line;
    std::vector<Patch> patches;
    Background background;

    while (std::getline(nff_file, line)) {
        if (line[0] == 'v') {
            std::string cmd; //place holder for command string/char
            getline(nff_file, line); //read the from line
            std::istringstream fromStream(line); //shortform iss
            fromStream >> cmd >> view.from.x >> view.from.y >> view.from.z; //parses and stores values

            getline(nff_file, line); //read the from line
            std::istringstream atStream(line); //shortform iss
            atStream >> cmd >> view.at.x >> view.at.y >> view.at.z; //parses and stores values

            getline(nff_file, line); //read the from line
            std::istringstream upStream(line); //shortform iss
            upStream >> cmd >> view.up.x >> view.up.y >> view.up.z; //parses and stores values

            getline(nff_file, line);
            std::istringstream angleStream(line);
            angleStream >> cmd >> view.angle;

            getline(nff_file, line);
            std::istringstream hitherStream(line);
            hitherStream >> cmd >> view.hither;

            getline(nff_file, line);
            std::istringstream resStream(line);
            resStream >> cmd >> view.resolution_x >> view.resolution_y;

            std::cout << view.from.x << " " << view.from.y << " " << view.from.z
                        << view.at.x << " " << view.at.y << " " << view.at.z
                        << view.up.x << " " << view.up.y << " " << view.up.z
                        << view.angle << " " << view.hither << " " 
                        << view.resolution_x << " " << view.resolution_y
             << std::endl;
        }
        else if (line[0] == 'b') {
            std::istringstream bgStream(line);
            char cmd;
            bgStream >> cmd >> background.color.r >> background.color.g >> background.color.b;
            view.background = background; 
            std::cout << background.color.r << " " << background.color.g << " " << background.color.b << std::endl;
        } else if (line[0] == 'l') {
            std::istringstream lightStream(line);
            char cmd;
            Light light;
            lightStream >> cmd>> light.position.x >> light.position.y >> light.position.z;
            std::cout << light.position.x << " " << light.position.y << " " << light.position.z << std::endl;
            lights.push_back(light);
        } else if (line[0] == 'f') {
            std::istringstream colorStream(line);
            char cmd;
            Color color;
            colorStream >> cmd >> color.r >> color.g >> color.b;
            if (!patches.empty()) {
                patches.back().colors.push_back(color); 
            }
            std::cout << color.r << " " << color.g << " " << color.b << std::endl;
        } else if (line[0] == 'p' && line[1] != 'p') {
            std::istringstream polyStream(line);
            char cmd;
            int numVertices;
            polyStream >> cmd >> numVertices;
            Patch patch;
            for (int i = 0; i < numVertices; ++i) {
                Vertex vertex;
                getline(nff_file, line);
                std::istringstream vertexStream(line);
                vertexStream >> vertex.x >> vertex.y >> vertex.z;
                patch.vertices.push_back(vertex);
                std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
            }
            patches.push_back(patch);
        } else if (line[0] == 'p' && line[1] == 'p') {
            
            std::istringstream ppStream(line);
            char cmd;
            int numVertices = 3;

            Patch patch;

            for (int i = 0; i < numVertices; ++i) {
                Vertex vertex;
                Eigen::Vector3f normal;

                getline(nff_file, line);
                std::istringstream vertexStream(line);
                vertexStream >> vertex.x >> vertex.y >> vertex.z;
                vertexStream >> normal.x() >> normal.y() >> normal.z();

                patch.vertices.push_back(vertex);
                patch.normals.push_back(normal);

                std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
                std::cout << normal.x() << " " << normal.y() << " " << normal.z() << std::endl;
            }
            patches.push_back(patch);
        }
    }
    nff_file.close();
    return patches;
}

int main() {
    View view;
    std::vector<Light> lights;

    //parse the NFF file and obtain scene data
    std::vector<Patch> patches = parse_nff_file("teapot-3.nff", view, lights);
    std::cout << "Number of patches: after parse" << patches.size() << std::endl;
    //render the scene
    renderScene(view, patches, lights);

    return 0;
}
