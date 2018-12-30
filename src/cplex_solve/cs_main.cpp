#include <iostream>
#include "../graph.h"
#include "../cw_heuristic.h"
#include "../iterative_descent.h"
#include "vrpmodel.h"
#include "dfs_cycle.h"


int main (int argc, char** argv) {

    if (argc <= 1) {
        std::cout << "Usage:" << std::endl;
        std::cout << "\tcplex_solve <filepath> [options]" << std::endl;
        std::cout << "\t-d\t Draw solution (with gnuplot)" << std::endl;
        exit(1);
    }

    std::string filepath{argv[1]};
    maoa::Graph g(filepath);

    std::list<maoa::Tour> startSolution = maoa::cw::getFeasible(g);
    maoa::idesc::descent(startSolution, g);

    maoa::cplex::VrpModelUndirectedCut vrpModel(g);

    vrpModel.createVars();
    vrpModel.setObjective();
    vrpModel.createConstraints();
    vrpModel.addMIPStart(startSolution);
    vrpModel.exportModel("model.lp");
    if (not vrpModel.solve()) {
        std::cerr << "Failed to optimize LP" << std::endl;
        exit(1);
    }
    std::cout << "Status = " << vrpModel.getStatus() << std::endl;
    std::cout << "Solution value = " << vrpModel.getSolutionValue() << std::endl;
    vrpModel.drawSolution();

}