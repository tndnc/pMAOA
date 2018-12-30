#include <lemon/nagamochi_ibaraki.h>
#include "vrpmodel.h"
#include "../gnuplot.h"
#include "dfs_cycle.h"

maoa::cplex::VrpModel::
VrpModel(const maoa::Graph &g) : _model(_env), _g(g), _cplex(_model) {
    _obj = IloAdd(_model, IloMinimize(_env));
//    _cplex.setParam(IloCplex::TiLim, 500);
}

void maoa::cplex::VrpModel::setObjective() {
    for (const auto& x : _x_vars) {
        _edge_t e = idxToEdge(x.first);
        double distance = _g.getDistance(e.first, e.second);
        _obj.setLinearCoef(x.second, distance);
    }
}

void maoa::cplex::VrpModel::drawSolution() {
    gnuplot plot;
    int color[3] {233, 30, 99};
    std::ofstream outfile;
    outfile.open("../points.dat");

    for (const auto& x : _x_vars) {
        if (_cplex.getValue(x.second) <= 1 - epsilon) continue;
        _edge_t edge = idxToEdge(x.first);
        NodeData d1 = _g.getData(edge.first);
        NodeData d2 = _g.getData(edge.second);
        outfile << d1.x << ' ' << d1.y << ' ' << color[0] << ' ' << color[1]
                << ' ' << color[2] << std::endl;
        outfile << d2.x << ' ' << d2.y << ' ' << color[0] << ' ' << color[1]
                << ' ' << color[2] << std::endl;
        outfile << std::endl;
    }

    outfile.close();

    plot("rgb(r, g, b) = int(r)*65536 + int(g)*256 + int(b)");
    plot("plot \"../points.dat\" using 1:2:(rgb($3, $4, $5)) with linespoints "
         "pt 3 ps 1 lc rgb variable notitle");
}

void maoa::cplex::VrpModel::
addMIPStart(std::list<maoa::Tour> &startSolution) {
    IloNumVarArray startVar(_env);
    IloNumArray startVal(_env);

    for (const auto& tour: startSolution) {
        // First arc from depot to start of tour
        int idx = edgeToIdx(_g.depotId(), tour.getFirstCity());
        startVar.add(_x_vars[idx]);
        startVal.add(1);
        // Arcs of tour
        for (auto it = tour.cities.begin(); it != --tour.cities.end(); it++) {
            idx = edgeToIdx(*it, *std::next(it));
            startVar.add(_x_vars[idx]);
            startVal.add(1);
        }
        // Last arc from end of tour to depot.
        idx = edgeToIdx(tour.getLastCity(), _g.depotId());
        startVar.add(_x_vars[idx]);
        startVal.add(1);
    }
    _cplex.addMIPStart(startVar, startVal);
    startVal.end();
    startVar.end();
}

void maoa::cplex::VrpModelDirected::
createVars() {
    int nodeNum = _g.nodeNum();

    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            if (i == j) continue;

            int idx = edgeToIdx(i, j);
            _x_vars[idx] = IloNumVar(_env, 0, 1, ILOBOOL);
            std::ostringstream varName;
            varName << "x_" << i << "_" << j;
            _x_vars[idx].setName(varName.str().c_str());
        }
    }
}

void maoa::cplex::VrpModelDirected::
createConstraints() {
    int nodeNum = _g.nodeNum();
    int vehiclesNum = _g.vehiclesNum();
    int depotId = _g.depotId();
    IloRangeArray CC(_env);

    // Constraints (1)-(2)
    IloExpr cst_1(_env);
    IloExpr cst_2(_env);
    for (int i = 0; i < nodeNum; i++) {
        if (i == depotId) continue;
        int idx1 = edgeToIdx(0, i);
        int idx2 = edgeToIdx(i, 0);
        cst_1 += _x_vars[idx1];
        cst_2 += _x_vars[idx2];
    }
    CC.add(cst_1 <= vehiclesNum);
    CC.add(cst_2 <= vehiclesNum);

    // Constraints (3)
    for (int i = 0; i < nodeNum; i++) {
        if (i == depotId) continue;
        IloExpr cst_3(_env);
        for (int j = 0; j < nodeNum; j++) {
            if (i == j) continue;

            int idx = edgeToIdx(i, j);
            cst_3 += _x_vars[idx];
        }
        CC.add(cst_3 == 1);
    }
    // Constraints (4)
    for (int i = 0; i < nodeNum; i++) {
        if (i == depotId) continue;
        IloExpr cst_4(_env);
        for (int j = 0; j < nodeNum; j++) {
            if (i == j) continue;

            int idx = edgeToIdx(j, i);
            cst_4 += _x_vars[idx];
        }
        CC.add(cst_4 == 1);
    }

    _model.add(CC);
}

void maoa::cplex::VrpModelDirectedMTZ::
createConstraints() {
    VrpModelDirected::createConstraints();

    // Constraints MTZ
    int depotId = _g.depotId();
    float capacity = _g.capacity();
    for (const auto& x : _x_vars) {
        _edge_t edge = idxToEdge(x.first);

        if (edge.first == depotId or edge.second == depotId) continue;

        IloExpr lhs(_env);
        IloExpr rhs(_env);
        float di = _g.getDemand(edge.first);
        lhs = _w_vars[edge.first] - _w_vars[edge.second];
        rhs = di - (capacity + di) * (1 - x.second);
        _model.add(lhs - rhs >= 0);
    }
}

void maoa::cplex::VrpModelDirectedMTZ::
_create_MTZVars() {
    int nodeNum = _g.nodeNum();
    int depotId = _g.depotId();
    float capacity = _g.capacity();

    for (int i = 0; i < nodeNum; i++) {
        if (i == depotId) continue;
        _w_vars[i] = IloNumVar(_env, _g.getDemand(i), capacity, ILOFLOAT);
        std::ostringstream varName;
        varName << "w_" << i;
        _w_vars[i].setName(varName.str().c_str());
    }
}

void maoa::cplex::VrpModelDirectedMTZ::
createVars() {
    VrpModelDirected::createVars();
    _create_MTZVars();
}

ILOUSERCUTCALLBACK3(UsercutVRPMinCut,
                    maoa::cplex::VrpModelDirectedMTZ&, m,
                    const maoa::Graph&, _g,
                    maoa::cplex::_var_map&, x) {

    maoa::Graph::EdgeMap<double> capacities(_g);
    maoa::Graph::NodeMap<bool> minCutMap(_g);
    int nodeNum = _g.nodeNum();
    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            if (i == j) continue;
            int idx = m.edgeToIdx(i, j);
            maoa::Graph::Edge e = _g.edge(_g(i), _g(j));
            capacities[e] = getValue(x[idx]);
        }
    }

    lemon::NagamochiIbaraki<maoa::Graph,
            maoa::Graph::EdgeMap<double>> mc(_g, capacities);
    mc.init();
    for (int i = 0; i < nodeNum-1; i++) { mc.processNextPhase(); }
    mc.minCutMap(minCutMap);
    IloExpr cutCstr(_env);
    int cutSize = 0;
    for (int i = 0; i < nodeNum; i++) {
        if (minCutMap[_g(i)]) {
            cutSize += 1;
            for (int j = 0; j < nodeNum; j++) {
                if (i == j) continue;
                if (not minCutMap[_g(j)]) {
                    int idx = m.edgeToIdx(i, j);
                    cutCstr += x[idx];
                }
            }
        }
    }
    if (cutSize >= 2) {
        add(cutCstr >= 1, IloCplex::UseCutForce);
    }
}

ILOLAZYCONSTRAINTCALLBACK3(LazycutVRPMinCut,
                    maoa::cplex::VrpModelDirectedMTZ&, m,
                    const maoa::Graph&, _g,
                    maoa::cplex::_var_map&, x) {

    maoa::Graph::EdgeMap<double> capacities(_g);
    maoa::Graph::NodeMap<bool> minCutMap(_g);
    int nodeNum = _g.nodeNum();
    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            if (i == j) continue;
            int idx = m.edgeToIdx(i, j);
            maoa::Graph::Edge e = _g.edge(_g(i), _g(j));
            capacities[e] = getValue(x[idx]);
        }
    }

    lemon::NagamochiIbaraki<maoa::Graph,
            maoa::Graph::EdgeMap<double>> mc(_g, capacities);
    mc.init();
    for (int i = 0; i < nodeNum-1; i++) { mc.processNextPhase(); }
    mc.minCutMap(minCutMap);
    IloExpr cutCstr(_env);
    int cutSize = 0;
    for (int i = 0; i < nodeNum; i++) {
        if (minCutMap[_g(i)]) {
            cutSize += 1;
            for (int j = 0; j < nodeNum; j++) {
                if (i == j) continue;
                if (not minCutMap[_g(j)]) {
                    int idx = m.edgeToIdx(i, j);
                    cutCstr += x[idx];
                }
            }
        }
    }
    if (cutSize >= 2) {
        add(cutCstr >= 1, IloCplex::UseCutForce);
    }
}

maoa::cplex::VrpModelDirectedMTZ::
VrpModelDirectedMTZ(const maoa::Graph &g) : VrpModelDirected(g) {
    _cplex.use(LazycutVRPMinCut(_env, *this, _g, _x_vars));
    _cplex.use(UsercutVRPMinCut(_env, *this, _g, _x_vars));
}

ILOLAZYCONSTRAINTCALLBACK4(VrpCycleCut,
                           maoa::cplex::VrpModel&, m,
                           const maoa::Graph&, _g,
                           maoa::cplex::_var_map&, x,
                           int, coeff) {

    int nodeNum = _g.nodeNum();
    int depotId = _g.depotId();
    std::unordered_map<int, bool> edgesInSolution;
    std::vector<int> depotEdges;
    for (const auto& var : x) {
        maoa::cplex::_edge_t edge = m.idxToEdge(var.first);
        // Discard edges that link the depot because we only want to find a
        // cycle that does not go through it.
        if (getValue(var.second) >= 1 - epsilon) {
            if (edge.first == depotId or edge.second == depotId) {
                // Save edges from and to depot node.
                depotEdges.push_back(var.first);
                edgesInSolution[var.first] = false;
            }
            else {
                edgesInSolution[var.first] = true;
            }
        }
    }
    FindCycleUtil fcu(nodeNum, edgesInSolution, m);
    std::set<int> cycle;
    bool cycleFound = fcu.getCycle(cycle);
    if (cycleFound) {
        int demandSum = 0;
        IloExpr cycleCstr(getEnv());
        for (auto &node : cycle) {
            demandSum += _g.getDemand(node);
            for (int v = 0; v < nodeNum; v++) {
                if (cycle.find(v) != cycle.end()) continue;

                int idx = m.edgeToIdx(node, v);
                cycleCstr += x[idx];
            }
        }
        add(cycleCstr >= coeff * ceil(demandSum / _g.capacity()),
            IloCplex::UseCutPurge);
        return;
    }

    // Add edges from and to depot node
    for (const auto& depotEdgeIdx : depotEdges) {
        edgesInSolution[depotEdgeIdx] = true;
    }
    // Detect cycles that pass through the depot and that violate the capacity
    // constraint. When a cycle is found: delete its edges from edgesInSolution
    // and try to find a new one.
    cycle.clear();
    fcu.reset();
    while(fcu.getCycle(cycle)) {
        int demandSum = 0;
        IloExpr cycleCstr(getEnv());
        for (auto &node : cycle) {

            if (node == depotId) continue;
            demandSum += _g.getDemand(node);
            for (int v = 0; v < nodeNum; v++) {
                int idx = m.edgeToIdx(node, v);
                if (cycle.find(v) != cycle.end()) {
                    // clear current cycle from search.
                    edgesInSolution[idx] = false;
                    if (v != depotId) continue;
                }
                cycleCstr += x[idx];
            }
        }
        if (demandSum > _g.capacity()) {
            add(cycleCstr >= coeff * ceil(demandSum / _g.capacity()),
                IloCplex::UseCutForce);
        }
        cycle.clear();
        fcu.reset();
    }
}

maoa::cplex::VrpModelDirectedCut::
VrpModelDirectedCut(const maoa::Graph &g) : VrpModelDirected(g) {
    _cplex.use(VrpDirectedCycleCut(_env, *this, _g, _x_vars, 1));
}

maoa::cplex::VrpModelUndirectedCut::
VrpModelUndirectedCut(const maoa::Graph &g) : VrpModel(g) {
    _cplex.use(VrpDirectedCycleCut(_env, *this, _g, _x_vars, 2));
}

void maoa::cplex::VrpModelUndirectedCut::
createVars() {
    int nodeNum = _g.nodeNum();

    for (int i = 0; i < nodeNum; i++) {
        for (int j = i + 1; j < nodeNum; j++) {

            int idx = edgeToIdx(i, j);
            _x_vars[idx] = IloNumVar(_env, 0, 1, ILOBOOL);
            std::ostringstream varName;
            varName << "x_" << i << "_" << j;
            _x_vars[idx].setName(varName.str().c_str());
        }
    }
}

void maoa::cplex::VrpModelUndirectedCut::
createConstraints() {
    int nodeNum = _g.nodeNum();
    int vehiclesNum = _g.vehiclesNum();
    int depotId = _g.depotId();
    IloRangeArray CC(_env);

    // Constraints (1)-(2)
    IloExpr cst_12(_env);
    for (int i = 0; i < nodeNum; i++) {
        if (i == depotId) continue;
        int idx = edgeToIdx(0, i);
        cst_12 += _x_vars[idx];
    }
    CC.add(cst_12 <= 2 * vehiclesNum);

    // Constraints (3)
    for (int i = 0; i < nodeNum; i++) {
        if (i == depotId) continue;
        IloExpr cst_3(_env);
        for (int j = 0; j < nodeNum; j++) {
            if (i == j) continue;

            int idx = edgeToIdx(i, j);
            cst_3 += _x_vars[idx];
        }
        CC.add(cst_3 == 2);
    }

    _model.add(CC);
}
