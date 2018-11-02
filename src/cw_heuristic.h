#ifndef PMAOA_CW_HEURISTIC_H
#define PMAOA_CW_HEURISTIC_H

#include "graph.h"

namespace maoa {
    namespace cw {
        struct Saving {
            Saving(int i, int j, double s) : i(i), j(j), saving(s) {};
            int i, j;
            double saving;
        };

        std::list<Tour> constructTours(Graph & g);
    }
}

#endif //PMAOA_CW_HEURISTIC_H
