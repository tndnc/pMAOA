#include <vector>

#include "graph.h"

#ifndef PMAOA_CHEURISTIC_H
#define PMAOA_CHEURISTIC_H

struct Tour {
    std::vector<int> cities;
};

std::vector<Tour> constructSolution(maoa::Graph & g);

#endif //PMAOA_CHEURISTIC_H
