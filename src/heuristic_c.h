#include <vector>

#include "graph.h"

#ifndef PMAOA_CHEURISTIC_H
#define PMAOA_CHEURISTIC_H

namespace maoa {

    struct Tour {
    private:
        std::vector<int> cities;
    public:
        float capacity;
        Tour() : capacity(0) {}
        int getCity(int idx) { return cities.at(idx); }
        unsigned long size() const { return cities.size(); }
        void addCity(int nodeId, float c) {
            cities.push_back(nodeId);
            capacity += c;
        }
        void print() const {
            for (int c : cities) {
                std::cout << c << " ";
            }
            std::cout << std::endl << "Capacity: " << capacity << std::endl;
        }
    };

    std::vector<Tour> constructClusters(Graph & g);
    void drawClusters(std::vector<Tour> & clusters, Graph & g);
}

#endif //PMAOA_CHEURISTIC_H
