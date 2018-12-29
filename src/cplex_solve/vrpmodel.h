#ifndef PMAOA_VRPMODEL_H
#define PMAOA_VRPMODEL_H

#include <ilcplex/ilocplex.h>
#include <unordered_map>
#include "../graph.h"
#include "unionfind.h"

#define epsilon 0.00001

namespace maoa {
namespace cplex {

    typedef std::unordered_map<int, IloNumVar> _var_map;
    typedef std::pair<int, int> _edge_t;

    /*!
     * This abstract class allows manipulation of a Capacitated Vehicle Routing
     * Problem (CVRP) with Concert technology.
     *
     * Provides virtual methods for:
     * - Creating the problem's variables.
     * - Setting the minimizing objective.
     * - Creating the problem's constraints.
     */
    class VrpModel {
    public:
        /*!
         * Constructor.
         * @param g Graph on which the model is based.
         */
        explicit VrpModel(const maoa::Graph &g);
        /*! Creates the decision variables needed to solve the problem. */
        virtual void createVars() =0;
        /*! Sets the minimizing objective. */
        virtual void setObjective();
        /*! Creates all the problem's constraints and adds them to the model. */
        virtual void createConstraints() =0;
        /*! Adds a mip start to cplex */
        void addMIPStart(std::list<maoa::Tour> &startSolution);
        /*! Writes the model in LP format to the file specified. */
        void exportModel(const char* filename) const {
            _cplex.exportModel(filename);
        }
        /*! Starts the CPLEX solving process. Returns CPLEX Status when done. */
        IloBool solve() { return _cplex.solve(); }
        /*! Get solution value */
        IloNum getSolutionValue() { return _cplex.getObjValue(); }
        /*! Get solution status */
        IloAlgorithm::Status getStatus() { return _cplex.getStatus(); }
        /*! When a solution is found, this function can be used to draw the
         * solution with gnuplot.
         */
        void drawSolution();
        /*!
         * Returns a unique numeric index for an edge in the form (i,j).
         * @param i The first node.
         * @param j The second node.
         * @return The corresponding index.
         */
        virtual int edgeToIdx(int i, int j) const =0;
        /*! Returns the pair of node indices (i,j) corresponding to an index
         * obtained via \p edgeToIdx.
         * @param idx The index.
         * @return The corresponding pair (i,j).
         */
        virtual _edge_t idxToEdge(int idx) const =0;
    protected:
        IloEnv _env;
        IloModel _model;
        IloObjective _obj;
        IloCplex _cplex;
        _var_map _x_vars;
        const maoa::Graph &_g;
    };

    /*!
     * Implementation of \p VrpModel for the oriented case. Each decision var x
     * corresponds to a directed edge (i -> j). This class must be derived with
     * additional constraints in order to avoid sub-tours in the final solution.
     */
    class VrpModelDirected : public VrpModel {
    public:
        explicit VrpModelDirected(const Graph &g) : VrpModel(g) {};
        void createVars() override;
        void createConstraints() override;
        int edgeToIdx(int i, int j) const override {
            return i * _g.nodeNum() + j;
        }
        _edge_t idxToEdge(int idx) const override {
            return std::make_pair(idx / _g.nodeNum(), idx % _g.nodeNum());
        }
    };

    /*! Implementation of \p VrpModelDirected. Uses Miller-Tucker-Zemlin
     * constraints to eliminate sub-tours. Also uses a MinCut algorithm for
     * Lazy and User CPLEX cuts.
     */
    class VrpModelDirectedMTZ : public VrpModelDirected {
    public:
        explicit VrpModelDirectedMTZ(const Graph& g);
        void createVars() override;
        void createConstraints() override;
    private:
        _var_map _w_vars;
        void _create_MTZVars();
    };

    /*!
     * Implementation of \p VrpModelDirected. Uses Cycle cuts constraints to
     * eliminate sub-tours and for the capacity constraints.
     */
    class VrpModelDirectedCut : public VrpModelDirected {
    public:
        explicit VrpModelDirectedCut(const Graph &g);
    };

} // namespace cplex
} // namespace maoa

#endif //PMAOA_VRPMODEL_H
