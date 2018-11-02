#include "graph.h"
#include "bp_heuristic.h"
#include "draw.h"
#include "iterative_descent.h"

int main(int argc, char** argv) {
    if (argc <= 1) {
        std::cout << "Usage:" << std::endl;
        std::cout << "\tbp <filepath> [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "\t-db\t Draw solution before iterative descent (with gnuplot)" << std::endl;
        std::cout << "\t-da\t Draw solution after iterative descent (with gnuplot)" << std::endl;
        exit(1);
    }

    string filepath{argv[1]};
    bool drawSolutionBefore = false;
    bool drawSolutionAfter = false;

    for (int i = 2; i < argc; i++) {
        if (strncmp(argv[i], "-db", 3) == 0) {
            drawSolutionBefore = true;
        }
        if (strncmp(argv[i], "-da", 3) == 0) {
            drawSolutionAfter = true;
        }
    }

    maoa::Graph g(filepath);

    std::list<maoa::Tour> tours = maoa::bp::constructClusters(g);
    std::cout << "Number of tours constructed: " << tours.size() << std::endl;
    if (tours.size() >= 10) {
        for (maoa::Tour &t : tours) {
            t.print();
        }
    }

    if (drawSolutionBefore) {
        maoa::DrawUtils drawUtils;
        drawUtils.drawTours(tours, g);
    }

    maoa::idesc::descent(tours, g);

    if (tours.size() >= 10) {
        for (maoa::Tour &t : tours) {
            t.print();
        }
    }

    if (drawSolutionAfter) {
        maoa::DrawUtils drawUtils;
        drawUtils.drawTours(tours, g);
    }
}