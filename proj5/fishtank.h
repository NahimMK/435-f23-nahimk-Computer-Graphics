#include <Eigen/Dense>

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
};

// Struct to represent food in the simulation
struct Food {
    Eigen::Vector3d position;
    Eigen::Vector3d vel;
    int t;
};