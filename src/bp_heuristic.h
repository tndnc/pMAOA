#include <vector>

#include "graph.h"

#ifndef PMAOA_BPHEURISTIC_H
#define PMAOA_BPHEURISTIC_H

namespace maoa {
    namespace bp {
        std::list<Tour> constructClusters(Graph &g);
    }
}

#endif //PMAOA_CHEURISTIC_H
