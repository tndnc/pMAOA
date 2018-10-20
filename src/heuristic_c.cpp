#include <vector>
#include <random>

#include "heuristic_c.h"

std::vector<Tour> constructSolution(maoa::Graph & g) {

    std::vector<Tour> solution;
    std::vector<int> startingPoints;
    int nodeNum = g.nodeNum();
    int vehiclesNum = g.vehiclesNum();
    int i = 0;

    std::random_device rd; // used to obtain a seed for the rng.
    std::mt19937 gen(rd()); // seed generator with rd.
    std::uniform_int_distribution<> dis(0, nodeNum-1); // create distribution with interval [0,nodeNum-1]
    while (i  < vehiclesNum) {
        startingPoints.push_back(dis(gen)); // add random starting city
        i++;
    }

    for (auto c : startingPoints) {
        std::cout << c << std::endl;
    }

    return solution;
}

