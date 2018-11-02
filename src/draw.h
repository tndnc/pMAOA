#include <fstream>
#include "gnuplot.h"
#include "graph.h"
#include "cw_heuristic.h"

#ifndef PMAOA_DRAW_H
#define PMAOA_DRAW_H

namespace maoa {
    class DrawUtils {
    private:
        gnuplot plot;
    public:
        void drawGraph(Graph &g);
        void drawTours(std::list<Tour> &tours, Graph &g);
    };
}

#endif //PMAOA_DRAW_H
