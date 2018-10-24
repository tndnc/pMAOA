#include <fstream>
#include "gnuplot.h"
#include "graph.h"

#ifndef PMAOA_DRAW_H
#define PMAOA_DRAW_H

void drawGraph(maoa::Graph &g);
void drawTours(std::vector<maoa::Tour> &tours, maoa::Graph &g);

#endif //PMAOA_DRAW_H
