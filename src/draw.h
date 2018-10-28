#include <fstream>
#include "gnuplot.h"
#include "graph.h"
#include "cw_heuristic.h"

#ifndef PMAOA_DRAW_H
#define PMAOA_DRAW_H

void drawGraph(maoa::Graph &g);
void drawTours(std::vector<maoa::VTour> &tours, maoa::Graph &g);
void drawTours(std::list<maoa::cw::Tour> &tours, maoa::Graph &g);

#endif //PMAOA_DRAW_H
