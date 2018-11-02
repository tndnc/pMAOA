#include "graph.h"

#ifndef PMAOA_ITERATIVE_DESCENT_H
#define PMAOA_ITERATIVE_DESCENT_H

namespace maoa {
    namespace idesc {
        double getTotalTourDistance(std::list<int> &t, const Graph &g);
        double getTotalCost(std::list<Tour> &tours, const Graph &g);
        bool improve2opt(std::list<Tour> &tours, const Graph &g);
        bool improveByRelocate(std::list<Tour> &tours, const Graph &g);
        bool improveByExchange(std::list<Tour> &tours, const Graph &g);
        void descent(std::list<Tour> &tours, const Graph &g);
    }
}

#endif //PMAOA_ITERATIVE_DESCENT_H
