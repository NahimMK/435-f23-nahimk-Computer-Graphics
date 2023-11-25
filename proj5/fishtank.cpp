//CMSC 435 Computer Graphics
//Boids Fishtank Simulation
//Nahim Kamruzzaman
//11.25.23
//A program that writes an output file that performs a simple crowd boid simulation
//inside a fish tank. THE SIMULATION DOES NOT INCLUDE FOOD, ONLY FISH
//ATTEMPTED TO USE KDTREE BUT COULDN'T FIGURE IT OUT
//ATTEMPT IS IN COMMENTS, PARTIAL CREDIT?

#include <fstream>
#include <cmath>
#include <vector>
#include <Eigen/Dense>
#include "fishtank.h"

// Const for calculations
const int FRAMES_PER_SEC = 30;
const double EPSILON = 0.00001;
const double BOUND_X = 0.5;
const double BOUND_Y = 0.25;
const double BOUND_Z = 0.125;

// Function to parse input from sample.in file
void parseInput(char *fname, Params &params, std::vector<Fish> &fish, std::vector<Food> &food) {
    char cmd;
    unsigned int nfish, nfood;
    std::ifstream in(fname, std::ios::in);

    // Read simulation parameters from the input file
    in >> params.size >> params.neighbor_radius >> params.num_neighbors
       >> params.mass >> params.collision >> params.centering
       >> params.velocity >> params.hunger >> params.damping
       >> params.dt >> params.length;

    // Read fish information from the input file
    in >> nfish;
    for (unsigned int i = 0; i < nfish; i++) {
        Fish f;
        in >> cmd >> f.position[0] >> cmd >> f.position[1] >> cmd >> f.position[2] >> cmd;
        in >> cmd >> f.vel[0] >> cmd >> f.vel[1] >> cmd >> f.vel[2] >> cmd;
        fish.push_back(f);
    }
    // No food in simulation, just parsing
    // Read food information from the input file
    in >> nfood;
    for (unsigned int i = 0; i < nfood; i++) {
        Food f;
        in >> cmd >> f.position[0] >> cmd >> f.position[1] >> cmd >> f.position[2] >> cmd;
        in >> cmd >> f.vel[0] >> cmd >> f.vel[1] >> cmd >> f.vel[2] >> cmd;
        in >> f.t;
        food.push_back(f);
    }
}

// Function to check if fish are outside the simulation bounds and flip their velocity if needed
void checkBounds(std::vector<Fish> &fish) {
    for (auto &f : fish) {
        if (f.position[0] > BOUND_X || f.position[0] < -BOUND_X ||
            f.position[1] > BOUND_Y || f.position[1] < -BOUND_Y ||
            f.position[2] > BOUND_Z || f.position[2] < -BOUND_Z)
            f.vel *= -1;
    }
}

// Function to write the simulation output to sample.out file
void writeOutput(std::ofstream &out, const std::vector<Fish> &fish, const std::vector<Food> &food) {
    out << fish.size() << std::endl;
    for (const auto &f : fish) {
        out << "[" << f.position[0] << "," << f.position[1] << "," << f.position[2] << "] ";
        out << "[" << f.vel[0] << "," << f.vel[1] << "," << f.vel[2] << "]" << std::endl;
    }
    out << food.size() << std::endl;
    for (const auto &f : food) {
        out << "[" << f.position[0] << "," << f.position[1] << "," << f.position[2] << "]" << std::endl;
    }
}
/*
FishTank::FishTank(const char* fname) {
    parseInput(fname, params, fish, food);
    buildKDTree();
}

void FishTank::runSimulation() {
    int nframes = FRAMES_PER_SEC * params.length;
    std::ofstream out("sample.out");
    out << nframes << std::endl;

    for (int f = 0; f < nframes; f++) {
        for (auto &f : fish) {
            f.center << 0, 0, 0;
            f.force << 0, 0, 0;
        }

        updateNeighborsUsingKDTree();

        for (auto &f : fish) {
            f.vel *= params.damping;
            f.position += (f.vel) * params.dt;
        }

        checkBounds();
        writeOutput(out, fish, food);
    }

    out.close();
}

void FishTank::updateNeighborsUsingKDTree() {
    for (auto &f : fish) {
        std::vector<int> neighbors = kdTree.queryRadius(f.position, params.neighbor_radius);

        for (const auto &neighborId : neighbors) {
            const Fish &neighbor = fish[neighborId];

            if (&f != &neighbor) {
                f.neighbors++;
                f.center += neighbor.position;

                double denom = (f.position - neighbor.position).norm();
                if (denom > EPSILON) {
                    f.force += (neighbor.vel - f.vel) * params.velocity + params.collision * (f.position - neighbor.position) / pow(denom, 3.0);
                }
            }
        }
        f.force += params.centering * ((f.center / f.neighbors) - f.position);
    }
}

void FishTank::buildKDTree() {
    std::vector<Eigen::Vector3d> fishPositions;

    for (const auto &f : fish) {
        fishPositions.push_back(f.position);
    }

    kdTree.buildTree(fishPositions);
}

*/

// Main simulation loop
int main(int argc, char *argv[]) {
    char *fname = argv[1];
    std::ofstream out;
    Params params;
    std::vector<Fish> fish;
    std::vector<Food> food;

    parseInput(fname, params, fish, food);

    int nframes = FRAMES_PER_SEC * params.length;
    out.open("sample.out");
    out << nframes << std::endl;

    for (int f = 0; f < nframes; f++) {

        // Initialize center and force vectors for each fish
        for (auto &f : fish) {
            f.center << 0, 0, 0;
            f.force << 0, 0, 0;
        }

        // Compute forces and update positions for each fish
        for (auto &f : fish) {
            for (auto &c : fish) {
                if (&f != &c) {
                    if ((f.position - c.position).norm() < params.neighbor_radius) {
                        f.neighbors++;
                        f.center += c.position;

                        double denom = (f.position - c.position).norm();
                        if (denom > EPSILON)
                            f.force += (c.vel - f.vel) * params.velocity + params.collision * (f.position - c.position) / pow(denom, 3.0);
                    }
                }
            }
            // Update force
            f.force += params.centering * ((f.center / f.neighbors) - f.position);
        }

        // Update velocities and positions for each fish
        for (auto &f : fish) {
            f.vel *= params.damping; // Apply mass-proportional damping
            f.position += (f.vel) * params.dt; // Update position based on velocity
        }

        // Check if fish are outside the simulation bounds and update output file
        checkBounds(fish);
        writeOutput(out, fish, food);
    }
    out.close();

    return 0;
}