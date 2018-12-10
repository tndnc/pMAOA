#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "graph.h"


int edgeToIdx(int i, int j, int max) {
    if (i > j) return i * max + j;
    else return j * max + i;
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
        if (i != depotId) {
            // Create MTZ Variables
            w[i] = IloNumVar(env, 0.0, g.capacity(), ILOFLOAT);
            std::ostringstream varName;
            varName << "w_" << i;
            w[i].setName(varName.str().c_str());
        }

        for (int j = i + 1; j < nodeNum; j++) {
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
    for (int i = 0; i < nodeNum; i++) {
        if (i == depotId) continue;
        int idx = edgeToIdx(depotId, i, nodeNum);
        cst_1 += x[idx];
    }
    cst_1.setName("cst_(1)-(2)");
    CC.add(cst_1 <= 2 * vehiclesNum);

    // Constraints (3)-(4)
    for (int i = 0; i < nodeNum; i++) {
        if (i == depotId) continue;
        IloExpr cst_2(env);
        for (int j = 1; j < nodeNum; j++) {
            if (i == j or j == depotId) continue;
            int idx = edgeToIdx(i, j, nodeNum);
            cst_2 += x[idx];
        }
        CC.add(cst_2 == 2);
    }

    // Constraints (5)
    for (int i = 0; i < nodeNum; i++) {
        if (i == depotId) continue;
        for (int j = i + 1; j < nodeNum; j++) {
            if (j == depotId) continue;

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

    if (!cplex.solve()) {
        env.error() << "Failed to optimize LP" << std::endl;
        exit(1);
    }

    env.out() << "Solution status = " << cplex.getStatus() << std::endl;
    env.out() << "Solution value  = " << cplex.getObjValue() << std::endl;

    return 0;
}