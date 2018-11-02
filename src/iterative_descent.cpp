#include "iterative_descent.h"

namespace maoa {
    namespace idesc {

        /*!
         * Returns the total distance between a string of city `t` with the depot of
         * the graph `g` as starting and ending point.
         * @param t list of consecutive cities.
         * @param g graph containing the cities of `t`.
         * @return the total distance as a double.
         */
        double getTotalTourDistance(std::list<int> &t, const Graph &g) {
            double tourDistance = g.getDistance(g.depotId(), *t.begin());
            auto c_it = ++t.begin();
            while (c_it != t.end()) {
                tourDistance += g.getDistance(*std::prev(c_it), *c_it);
                c_it++;
            }
            tourDistance += g.getDistance(*--c_it, g.depotId());
            return tourDistance;
        }

        /*!
         * Returns the total distance of a solution, that is a list of tours.
         * @param tours list of tours
         * @param g graph containing the tours.
         * @return the total cost of the solution (that is the sum if tour distances) as a double.
         */
        double getTotalCost(std::list<Tour> &tours, const Graph &g) {
            double totalCost = 0;
            for (Tour &t : tours) {
                totalCost += getTotalTourDistance(t.cities, g);
            }
            return totalCost;
        }

        /*!
         * Improves a list of tours by exploring the 2-opt neighborhood of each tour. For each tour, the arcs between
         * four cities are deleted and the cities are reconnected so as to construct a new different tour. If the new
         * tour has a total distance inferior to the orginal, the new tour is kept.
         * @param tours list of tours to improve.
         * @param g graph containing the tours.
         * @return a boolean indicating if a change was made to any of the tours.
         */
        bool improve2opt(std::list<Tour> &tours, const Graph &g) {
            bool changeMade = false;

            int firstNodeIdx, secondNodeIdx;
            long t_size;

            for (Tour &t : tours) {
                t_size = t.cities.size();
                // Compute original tour cost (before improvements).
                double currentTourCost = getTotalTourDistance(t.cities, g);
                double newTourCost;
                std::list<int>::iterator c_it;

                firstNodeIdx = -1;
                while (firstNodeIdx < t_size - 1) {

                    secondNodeIdx = firstNodeIdx + 1;
                    while (secondNodeIdx < t_size) {

                        // Construct new tour according to firstNode and secondNode.
                        std::list<int> new_tour;
                        // 1. Append every node before `firstNode`.
                        int constructIdx;
                        c_it = t.cities.begin();
                        for (constructIdx = 0; constructIdx <= firstNodeIdx; constructIdx++) {
                            new_tour.push_back(*c_it);
                            c_it++;
                        }
                        // 2. Add from `secondNode` to `firstNode` in reverse order
                        int advanceCounter = (firstNodeIdx == -1) ? secondNodeIdx : secondNodeIdx-firstNodeIdx-1;
                        std::advance(c_it, advanceCounter);
                        for (constructIdx = secondNodeIdx; constructIdx > firstNodeIdx; constructIdx--) {
                            new_tour.push_back(*c_it);
                            c_it--;
                        }
                        // 3. Add rest of route in correct order.
                        std::advance(c_it, secondNodeIdx-firstNodeIdx+1);
                        for (constructIdx = secondNodeIdx + 1; constructIdx < t_size; constructIdx++) {
                            new_tour.push_back(*c_it);
                            c_it++;
                        }

                        newTourCost = getTotalTourDistance(new_tour, g);
                        if (newTourCost < currentTourCost) {
                            changeMade = true;
                            currentTourCost = newTourCost;
                            t.cities.swap(new_tour);
                            firstNodeIdx = -1;
                            secondNodeIdx = 0;
                        } else {
                            secondNodeIdx++;
                        }
                    }
                    firstNodeIdx++;
                }
            }
            return changeMade;
        }

        /*!
         * Improves a list of tours by relocating a city from one tour to another. For every city, the procedure tries
         * to relocate it in a new tour at the best position. The first relocation that improves the total cost is
         * implemented.
         * @param tours list of tours to improve.
         * @param g graph containing the tours.
         * @return a boolean indicating if a change was made to any of the tours.
         */
        bool improveByRelocate(std::list<Tour> &tours, const Graph &g) {
            bool changeMade = false;
            std::list<Tour>::iterator t_it1, t_it2;

            begin:
            // Check all pairs of tours.
            for (t_it1 = tours.begin(); t_it1 != --tours.end(); t_it1++) {
                for (t_it2 = std::next(t_it1); t_it2 != tours.end(); t_it2++) {

                    // Check if a node of tour1 can be relocated inside tour2.
                    auto c_it1 = t_it1->cities.begin();
                    while (c_it1 != t_it1->cities.end()) {
                        int currentT1 = *c_it1;
                        float demandT1 = g.getDemand(currentT1);
                        // Check capacity of tour2.
                        if (t_it2->capacity + demandT1 > g.capacity()) {
                            c_it1++;
                            continue;
                        }

                        // If c_it1 is first node in tour, then previous node is depot.
                        int prevT1 = (c_it1 == t_it1->cities.begin()) ? g.depotId() : *std::prev(c_it1);
                        // If c_it1 is last node in tour, then next node is depot.
                        int nextT1 = (c_it1 == --t_it1->cities.end()) ? g.depotId() : *std::next(c_it1);
                        double gain_fromRemove = g.getDistance(prevT1, currentT1) + g.getDistance(currentT1, nextT1)
                                                 - g.getDistance(prevT1, nextT1);

                        // Search for the best place to put currentT1 in tour2.
                        auto c_it2 = t_it2->cities.begin();
                        while (c_it2 != t_it2->cities.end()) {
                            int prevT2 = (c_it2 == t_it2->cities.begin()) ? g.depotId() : *std::prev(c_it2);
                            int currentT2 = *c_it2;

                            double loss_fromAdd = g.getDistance(prevT2, currentT2) - g.getDistance(prevT2, currentT1)
                                                  - g.getDistance(currentT1, currentT2);

                            double totalGain = gain_fromRemove + loss_fromAdd;
                            if (totalGain > 0) {
                                // Implement changes.
                                t_it2->cities.insert(c_it2, currentT1);
                                t_it2->capacity += demandT1;
                                t_it1->cities.erase(c_it1);
                                t_it1->capacity -= demandT1;
                                changeMade = true;
                                // Reset search.
                                goto begin;
                            }

                            c_it2++;
                        }
                        c_it1++;
                    }
                }
            }
            return changeMade;
        }

        /*!
         * Improves a list of tours by exchanging two cities between tours. For every pair of city, if the exchange is
         * possible and improves the distance of the combined tours, it is implemented.
         * @param tours list of tours to improve.
         * @param g graph containing the tours.
         * @return a boolean indicating if a change was made to any of the tours.
         */
        bool improveByExchange(std::list<Tour> &tours, const Graph &g) {
            bool changeMade = false;
            std::list<Tour>::iterator t_it1, t_it2;

            // Check all pairs of tours.
            for (t_it1 = tours.begin(); t_it1 != --tours.end(); t_it1++) {
                for (t_it2 = std::next(t_it1); t_it2 != tours.end(); t_it2++) {
                }
            }

            return changeMade;
        }

        /*!
         * Performs an iterative improvement procedure. The tours are improved with several procedure in the following
         * order: 2-opt neighborhood, relocating of one city, exchange of city. The process is retried as long as one
         * of the procedure is successful in improving the solution cost.
         * @param tours list of tours to improve.
         * @param g graph containing the tours.
         */
        void descent(std::list<Tour> &tours, const Graph &g) {
            bool changeMade;
            do {
                changeMade = false;
                changeMade = changeMade || improve2opt(tours, g);
                changeMade = changeMade || improveByRelocate(tours, g);
                changeMade = changeMade || improveByExchange(tours, g);
                double totalCost = getTotalCost(tours, g);
                std::cout << "Total cost is: " << totalCost << std::endl;
            } while (changeMade);
        }
    }
}

