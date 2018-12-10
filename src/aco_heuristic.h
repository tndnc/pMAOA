#ifndef PMAOA_ACEHEURISTIC_H
#define PMAOA_ACEHEURISTIC_H

#include <string>
#include <sstream>
#include <iostream>
#include <map>


#include "graph.h"

using std::string;

namespace ace {

    struct ant {
        int id, visited;
        std::list<int> path;
        std::list<int>::iterator it;
        float capacity;
        int position;
        float distance;

        string to_string() const {
            std::stringstream ss;
            ss << "Ant #" << id << "\n";
            return ss.str();
        };
    };

    class aco_heuristic {
    public:
        explicit aco_heuristic(const string &filename);

        std::list<maoa::Tour> run(int nb_iter, int nb_ants, float beta, float alpha, float q0, float t0);

        std::map<string, float> pheromones;
    private:
        void selectNode(ant &a, float q0, int beta);

        void updatePheromones(ant &a, float alpha);

        void evaporatePheromones(float alpha, float t0);

        float getpheromones(int nodeA, int nodeB);

        void addpheromones(int nodeA, int nodeB, float value, float alpha);

        void resetAnts(int nb_ants, ant Ants[]);

        float getRandom();

        maoa::Graph *g;
    };
}
#endif