#include "io.h"
#include <iostream>
#include <vector>
#include <Eigen/Dense>

void laplacianSmoothing(std::vector<Eigen::Vector3d> &pts, const std::vector<Tri> &triangles, double stepsize, int niterations) {
    for (int iter = 0; iter < niterations; iter++) {
        std::vector<Eigen::Vector3d> newPts(pts.size(), Eigen::Vector3d::Zero());
        std::vector<int> count(pts.size(), 0);

        // Calculate Laplacian and count incident edges for each vertex
        for (const Tri &tri : triangles) {
            for (int i = 0; i < 3; i++) {
                int v0 = tri[i];
                int v1 = tri[(i + 1) % 3];
                int v2 = tri[(i + 2) % 3];
                newPts[v0] += pts[v1] + pts[v2];
                count[v0] += 2;
            }
        }

        // Update vertex positions
        for (int i = 0; i < pts.size(); i++) {
            if (count[i] > 0) {
                newPts[i] /= count[i];  // Calculate the average Laplacian
                pts[i] += stepsize * (newPts[i] - pts[i]);  // Update the vertex position
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: smoothing input.obj output.obj stepsize niterations" << std::endl;
        return 1;
    }

    char* inputFileName = argv[1];
    char* outputFileName = argv[2];
    double stepsize = std::stod(argv[3]);
    int niterations = std::stoi(argv[4]);

    std::vector<Eigen::Vector3d> pts;
    std::vector<Tri> triangles;

    if (!readObjFile(inputFileName, pts, triangles)) {
        std::cerr << "Failed to read input obj file." << std::endl;
        return 1;
    }

    laplacianSmoothing(pts, triangles, stepsize, niterations);

    writeObjFile(outputFileName, pts, triangles);

    std::cout << "Smoothing completed. Result saved to " << outputFileName << std::endl;

    return 0;
}