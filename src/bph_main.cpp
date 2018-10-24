#include "bp_heuristic.h"
#include "cw_heuristic.h"
#include "draw.h"

int main () {
    maoa::Graph g("../data/A/A-n32-k5.vrp");
//    g.print();
//    drawGraph(g);
//    std::vector<maoa::Tour> solution = constructClusters(g);

//    for (auto & c : solution) {
//        c.print();
//    }
//
//    drawTours(solution, g);
    maoa::cw::constructTours(g);
}