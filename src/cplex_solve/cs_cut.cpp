#include "cs_cut.h"

//void findMinCut(IloCplex &cplex, maoa::Graph &g, std::unordered_map<int, IloNumVar> &x) {

//    maoa::Graph::EdgeMap<int> capacities(g);
//    maoa::Graph::NodeMap<bool> minCut(g);
//    int nodeNum = g.nodeNum();
//    for (int i = 0; i < nodeNum; i++) {
//        for (int j = 0; j < nodeNum; j++) {
//            int idx = i * nodeNum + j;
//            maoa::Graph::Edge e = g.edge(g(i), g(j));
//            capacities[e] = 1;
////            capacities[e] = (cplex.getValue(x[idx]) > epsilon) ? 1 : 0;
//        }
//    }
//
//    lemon::NagamochiIbaraki<maoa::Graph, maoa::Graph::EdgeMap<int>> mc(g, capacities);
//    mc.run();
//    double minCutValue = mc.minCutMap(minCut);
//    std::cout << minCutValue << std::endl;
//}
