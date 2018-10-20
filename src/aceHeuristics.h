#ifndef PMAOA_ACEHEURISTIC_H
#define PMAOA_ACEHEURISTIC_H

#include <string>
#include <sstream>
#include <iostream>

#include "graph.h"

using std::string;

namespace ace{

    struct ant{
        int id;
        std::list<int> visited;
        std::list<int> path;
        float capacity;
        int position;
        int score;

        string to_string() const {
            std::stringstream ss;
            ss << "Ant #" << id << "\n";
            return ss.str();
        };

    };

    class aceHeutistic {
    public:
        explicit aceHeutistic(const string & filename);
        void run(int nb_iter, int nb_ants, float beta, float alpha, float q0, float t0);
    private:
        void selectNode(ant);
        void updatePheromones();
        void evaporatePheromones();
        maoa::Graph * g;
    };
