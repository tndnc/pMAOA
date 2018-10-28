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
        float score;
        float distance;

        string to_string() const {
            std::stringstream ss;
            ss << "Ant #" << id << "\n";
            return ss.str();
        };
    };

    class ace_heutistic {
    public:
        explicit ace_heutistic(const string &filename);

        void run(int nb_iter, int nb_ants, int beta, float alpha, float q0, float t0);

        std::map<string, float> pheromones;
    private:
        void selectNode(ant &a, float q0, int beta);

        void updatePheromones(ant &a, float alpha);

        void evaporatePheromones(float alpha, float t0);

        float getpheromones(int nodeA, int nodeB);

        void addpheromones(int nodeA, int nodeB, float value, float alpha);

        maoa::Graph *g;
    };
}
#endif