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

        struct Tour {
            float capacity;
            std::list<int> cities;

            Tour() : capacity(0) {}
            void addCity(int i, float c) {
                cities.push_back(i);
                capacity += c;
            }
            int getFirstCity() const {
                return *cities.begin();
            }
            int getLastCity() const {
                return *--cities.end();
            }
            void print() const {
                std::cout << "Capacity: " << capacity << std::endl;
                std::cout << "0 ";
                for (int a : cities) {
                    std::cout << a << " ";
                }
                std::cout << "0" << std::endl;
            }
            void merge(Tour * t2) {
                cities.splice(cities.end(), t2->cities);
                capacity += t2->capacity;
            }
        };

        std::list<Tour> constructTours(Graph & g);
    }
}

#endif //PMAOA_CW_HEURISTIC_H
