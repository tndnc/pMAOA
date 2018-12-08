#include "graph.h"
#include "ace_heuristic.h"
#include "draw.h"
#include "iterative_descent.h"

int main(int argc, char** argv) {
    maoa::Graph g("../data/A/A-n80-k10.vrp");
    maoa::DrawUtils drawUtils;
    //drawUtils.drawGraph(g);
    ace::ace_heutistic ace("../data/A/A-n80-k10.vrp");
    std::list<maoa::Tour> tours = ace.run(20,20,2.3,0.1,0.2,0.00001);
    for (auto & c : tours) {
       c.print();
    }

    //drawUtils.drawTours(tours, g);
    maoa::idesc::descent(tours, g);
    //drawUtils.drawTours(tours, g);

//    maoa::Graph g("../data/A/A-n62-k8.vrp");
//    g.print();
//    drawGraph(g);
//    std::vector<maoa::VTour> tours = constructClusters(g);
//
//    for (auto & c : tours) {
//        c.print();
//    }

//    drawTours(tours, g);
//    std::list<maoa::cw::Tour> tours = maoa::cw::constructTours(g);
//    std::cout << "Number of routes: " << tours.size() << std::endl;
//    drawTours(tours, g);
}