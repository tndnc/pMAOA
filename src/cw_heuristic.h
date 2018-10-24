#ifndef PMAOA_CW_HEURISTIC_H
#define PMAOA_CW_HEURISTIC_H

#include "graph.h"

namespace maoa {
    namespace cw {
        struct Saving {
            int i, j;
            double saving;
        };

        std::vector<Tour> constructTours(Graph & g);
    }
}

#endif //PMAOA_CW_HEURISTIC_H
