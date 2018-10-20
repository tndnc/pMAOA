#include "graph.h"
#include "aceHeuristics.h"


using namespace ace;

aceHeutistic::aceHeutistic(const string &filename) {
    g = new maoa::Graph(filename);
}

void aceHeutistic::run(int nb_iter, int nb_ants, float beta, float alpha, float q0, float t0) {

    //initialization
    ant Ants [nb_ants];
    for(int i = 0; i < nb_ants; i++){
        Ants[i].id = i;
        Ants[i].position = 0;
        Ants[i].capacity = g->capacity();
    }

    for(int j = 0; j < nb_iter;j++){
        for(int i = 0; i < nb_ants; i ++){
            while (Ants[i].visited.size() < g->nodeNum());
                selectNode(Ants[i]);
        }
        updatePheromones();
        evaporatePheromones();
    }
}

void aceHeutistic::selectNode(ant) {

}

void aceHeutistic::updatePheromones() {

}

void aceHeutistic::evaporatePheromones() {

}


