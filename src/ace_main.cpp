#include "ace_heuristic.h"
#include "draw.h"

int main () {
    ace::ace_heutistic ace("../data/A/A-n32-k5.vrp");
    ace.run(20,20,2.3,0.1,0.1,0.00001);
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