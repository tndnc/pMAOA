#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../graph.h"
#include "cs_cut.h"
#include "../gnuplot.h"

typedef std::unordered_map<int, IloNumVar>& x_var_ref_type;

int edgeToIdx(int i, int j, int max) {
    return i * max + j;
}

void drawCplexSolution(x_var_ref_type x, IloCplex &cplex, maoa::Graph &g) {
    gnuplot plot;
    int color[3] {233, 30, 99};
    std::ofstream outfile;
    outfile.open("../points.dat");

    int nodeNum = g.nodeNum();
    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            if (i == j) continue;
            int idx = edgeToIdx(i, j, nodeNum);
            if (cplex.getValue(x[idx]) > epsilon) {
                maoa::NodeData d1 = g.getData(i);
                maoa::NodeData d2 = g.getData(j);
                outfile << d1.x << ' ' << d1.y << ' ' << color[0] << ' ' << color[1] << ' ' << color[2] << std::endl;
                outfile << d2.x << ' ' << d2.y << ' ' << color[0] << ' ' << color[1] << ' ' << color[2] << std::endl;
                outfile << std::endl;
            }
        }
    }

    outfile.close();

    plot("rgb(r, g, b) = int(r)*65536 + int(g)*256 + int(b)");
    plot("plot \"../points.dat\" using 1:2:(rgb($3, $4, $5)) with linespoints pt 3 ps 1 lc rgb variable notitle");
}



ILOUSERCUTCALLBACK2(UsercutVRPMinCut, maoa::Graph&, g, x_var_ref_type, x) {
    std::cout << "starting user callback: ";
    maoa::Graph::EdgeMap<double> capacities(g);
    maoa::Graph::NodeMap<bool> minCutMap(g);
    int nodeNum = g.nodeNum();
    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            if (i == j) continue;
            int idx = edgeToIdx(i, j, nodeNum);
            maoa::Graph::Edge e = g.edge(g(i), g(j));
            capacities[e] = getValue(x[idx]);
        }
    }
    std::cout << "capacities done: ";

    lemon::NagamochiIbaraki<maoa::Graph, maoa::Graph::EdgeMap<double>> mc(g, capacities);
    mc.run();
    double value = mc.minCutMap(minCutMap);
    std::cout << "mincut = " << value << ": ";
    IloExpr cutCstr(getEnv());
    int cutSize = 0;
    for (int i = 0; i < nodeNum; i++) {
        if (minCutMap[g(i)]) {
            cutSize += 1;
            for (int j = 0; j < nodeNum; j++) {
                if (i == j) continue;
                if (not minCutMap[g(j)]) {
                    int idx = edgeToIdx(i, j, nodeNum);
                    cutCstr += x[idx];
                }
            }
        }
    }
    std::cout << "cutSize = " << cutSize << ": ";
//    if (cutSize >= 2) {
        add(cutCstr >= 1, IloCplex::UseCutForce);
        std::cout << "added cut";
//    }
    std::cout << std::endl;
}

ILOLAZYCONSTRAINTCALLBACK2(LazyCutVRPMinCut, maoa::Graph&, g, x_var_ref_type, x) {
    std::cout << "starting lazy callback" << std::endl;
    maoa::Graph::EdgeMap<double> capacities(g);
    maoa::Graph::NodeMap<bool> minCutMap(g);
    int nodeNum = g.nodeNum();
    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            if (i == j) continue;
            int idx = edgeToIdx(i, j, nodeNum);
            maoa::Graph::Edge e = g.edge(g(i), g(j));
            capacities[e] = getValue(x[idx]);
            std::cout << i << " " << j << ": " << capacities[e] << std::endl;
        }
    }

    lemon::NagamochiIbaraki<maoa::Graph, maoa::Graph::EdgeMap<double>> mc(g, capacities);
    mc.run();
    mc.minCutMap(minCutMap);
    IloExpr cutCstr(getEnv());
    int cutSize = 0;
    for (int ii = 0; ii < nodeNum; ii++) {
        if (minCutMap[g(ii)]) {
            cutSize += 1;
            for (int jj = 0; jj < nodeNum; jj++) {
                if (ii == jj) continue;
                if (not minCutMap[g(jj)]) {
                    int idx = edgeToIdx(ii, jj, nodeNum);
                    cutCstr += x[idx];
                }
            }
        }
    }
    std::cout << "Cut Size = " << cutSize << std::endl;
//    if (cutSize >= 2) {
        add(cutCstr >= 1, IloCplex::UseCutForce);
//    }
}

int main (int argc, char** argv) {

    if (argc <= 1) {
        std::cout << "Usage:" << std::endl;
        std::cout << "\tcplex_solve <filepath> [options]" << std::endl;
//        std::cout << "Options:" << std::endl;
//        std::cout << "\t-db\t Draw solution before iterative descent (with gnuplot)" << std::endl;
//        std::cout << "\t-da\t Draw solution after iterative descent (with gnuplot)" << std::endl;
        exit(1);
    }

    string filepath{argv[1]};
    maoa::Graph g(filepath);

    // ------------
    // CPLEX INIT
    // ------------
    IloEnv env;
    IloModel model(env);

    // ------------
    // VARIABLES AND OBJ
    // ------------

    IloObjective obj=IloAdd(model, IloMinimize(env));
    std::unordered_map<int, IloNumVar> x;
    std::unordered_map<int, IloNumVar> w;

    int nodeNum = g.nodeNum();
    int vehiclesNum = g.vehiclesNum();
    int depotId = g.depotId();

    for (int i = 0; i < nodeNum; i++) {
        // Create MTZ Variables
        w[i] = IloNumVar(env, 0.0, g.capacity(), ILOFLOAT);
        std::ostringstream wName;
        wName << "w_" << i;
        w[i].setName(wName.str().c_str());

        for (int j = 0; j < nodeNum; j++) {
            if (i == j) continue;

            int idx = edgeToIdx(i, j, nodeNum);
            x[idx] = IloNumVar(env, 0.0, 1.0, ILOINT);
            // Set variable name
            std::ostringstream varName;
            varName << "x_" << i << "_" << j;
            x[idx].setName(varName.str().c_str());
            // Set obj coefficient
            double distance = g.getDistance(i, j);
            obj.setLinearCoef(x[idx], distance);
        }
    }

    //////////////
    //////  CST
    //////////////

    IloRangeArray CC(env);

    // Constraint (1)-(2)
    IloExpr cst_1(env);
    IloExpr cst_2(env);
    for (int i = 0; i < nodeNum; i++) {
        if (i == depotId) continue;
        int idx1 = edgeToIdx(depotId, i, nodeNum);
        int idx2 = edgeToIdx(i, depotId, nodeNum);
        cst_1 += x[idx1];
        cst_2 += x[idx2];
    }
    CC.add(cst_1 <= vehiclesNum);
    CC.add(cst_2 <= vehiclesNum);

    // Constraints (3)
    for (int i = 0; i < nodeNum; i++) {
        if (i == depotId) continue;
        IloExpr cst_3(env);
        for (int j = 1; j < nodeNum; j++) {
            if (i == j or j == depotId) continue;
            int idx = edgeToIdx(i, j, nodeNum);
            cst_3 += x[idx];
        }
        CC.add(cst_3 == 1);
    }
    // Constraints (4)
    for (int i = 0; i < nodeNum; i++) {
        if (i == depotId) continue;
        IloExpr cst_4(env);
        for (int j = 1; j < nodeNum; j++) {
            if (i == j or j == depotId) continue;
            int idx = edgeToIdx(j, i, nodeNum);
            cst_4 += x[idx];
        }
        CC.add(cst_4 == 1);
    }

    // Constraints (5)
    for (int i = 0; i < nodeNum; i++) {
//        if (i == depotId) continue;
        for (int j = 0; j < nodeNum; j++) {
//            if (j == depotId or i == j) continue;
            if (j == i) continue;
            // MTZ Constraint
            IloExpr cst_mtz(env);
            cst_mtz += w[i] - w[j];
            float d_i = g.getDemand(i);
            int idx = edgeToIdx(i, j, nodeNum);
            IloExpr rhs(env);
            rhs += d_i - (g.capacity() + d_i) * (1 - x[idx]);
            CC.add(cst_mtz - rhs >= 0);
        }
    }

    model.add(CC);

    ///////////
    //// RESOLUTION
    //////////

    IloCplex cplex(model);
    cplex.exportModel("model.lp");

//    cplex.use(UsercutVRPMinCut(env, g, x));
    cplex.use(LazyCutVRPMinCut(env, g, x));

    if (!cplex.solve()) {
        env.error() << "Failed to optimize LP" << std::endl;
        exit(1);
    }


    env.out() << "Solution status = " << cplex.getStatus() << std::endl;
    env.out() << "Solution value  = " << cplex.getObjValue() << std::endl;

    // draw solution
    drawCplexSolution(x, cplex, g);

    return 0;
}