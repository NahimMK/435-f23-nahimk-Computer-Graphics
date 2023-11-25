#include <Eigen/Dense>
#include "kdTree.H"
// Struct to store simulation parameters
struct Params {
    float size;
    float neighbor_radius;
    float num_neighbors;
    float mass;
    float collision;
    float centering;
    float velocity;
    float hunger;
    float damping;
    double dt;
    int length;
};

// Struct to represent a fish in the simulation
struct Fish {
    Eigen::Vector3d position;
    Eigen::Vector3d vel;
    Eigen::Vector3d center;
    Eigen::Vector3d force;
    int neighbors;
    //int kdTreeId;
};

// Struct to represent food in the simulation
struct Food {
    Eigen::Vector3d position;
    Eigen::Vector3d vel;
    int t;
};
/*
class FishTank {
public:
    FishTank(const char* fname);
    void runSimulation();

private:

    void updateNeighborsUsingKDTree();
    void buildKDTree();

    Params params;
    std::vector<Fish> fish;
    std::vector<Food> food;
    kdTree kdTree;
};*/