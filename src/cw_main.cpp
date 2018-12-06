#include "bp_heuristic.h"
#include "cw_heuristic.h"
#include "draw.h"
#include "iterative_descent.h"

int main (int argc, char** argv) {
    if (argc <= 1) {
        std::cout << "Usage:" << std::endl;
        std::cout << "\tcw <filepath> [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "\t-draw, -d\t Draw final solution with gnuplot" << std::endl;
        exit(1);
    }

    string filepath{argv[1]};
    bool drawSolution = false;

    for (int i = 2; i < argc; i++) {
        if (strncmp(argv[i], "-draw", 2) == 0) {
            drawSolution = true;
        }
    }

    maoa::Graph g(filepath);

    std::list<maoa::Tour> tours = maoa::cw::getFeasible(g);
    std::cout << "Number of routes: " << tours.size() << std::endl;
    maoa::idesc::descent(tours, g);

    if (drawSolution) {
        maoa::DrawUtils drawUtils;
        drawUtils.drawTours(tours, g);
    }
}