#include <random>
#include <numeric>
#include "cw_heuristic.h"

namespace maoa {
    namespace cw {

        /*!
         * Computes the savings list for the graph `g`. For each pair of cities (i,j), the associated saving value is
         * the gain from including a link (i,j) to a route in a CVRP solution (s_ij = c_i0 + c0j + c_ij).
         * @param g input graph.
         * @return the savings list.
         */
        std::list<Saving> computeSavings(Graph &g) {
            std::list<Saving> savings;
            double lambda = 1; // TODO: change this parameter

            int nodeNum, i, j;
            int depotId = g.depotId();
            double ci0, c0j, cij;
            nodeNum = g.nodeNum();
            for (i = 0; i < nodeNum; i++) {
                // TODO: change j to i + 1
                for (j = 0; j < nodeNum; j++) {
                    if (i == j) continue;
                    if (i == depotId || j == depotId) continue;
                    ci0 = g.getDistance(i, depotId);
                    c0j = g.getDistance(j, depotId);
                    cij = g.getDistance(i, j);
                    savings.emplace_back(i, j, ci0 + c0j - (lambda * cij));
                }
            }
            savings.sort([&](Saving &s1, Saving &s2) { return s1.saving > s2.saving; });
            return savings;
        }

        std::list<Saving> _updateSavings(std::list<Saving> inSavings) {
            // IN: Must be sorted

            std::list<Saving> outSavings;

            std::random_device rd;     // only used once to initialise (seed) engine
            std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
            std::uniform_int_distribution<int> u_int(1,5); // guaranteed unbiased
            std::uniform_real_distribution<double> u_real;

            while (!inSavings.empty()) {
                int T = u_int(rng);
                if (T > inSavings.size()) T = inSavings.size();
                auto endIter = inSavings.begin();
                std::advance(endIter, T);
                double savingSum = std::accumulate(inSavings.begin(), endIter, 0, [&](double lhs, Saving &s) {
                    return lhs + s.saving;
                });

                std::vector<double> cumSumVector;
                cumSumVector.resize(T);
                auto beginIter = inSavings.begin();
                double cumSum = 0;
                for (int i = 0; i < T; i++) {
                    cumSum += (beginIter->saving / savingSum);;
                    cumSumVector[i] = cumSum;
                    beginIter++;
                }

                double r = u_real(rng);
                int chosenSavingIdx = 0;
                while (cumSumVector[chosenSavingIdx] < r) {
                    chosenSavingIdx++;
                }

                beginIter = inSavings.begin();
                std::advance(beginIter, chosenSavingIdx);
                outSavings.push_back(*beginIter);

                inSavings.erase(beginIter, std::next(beginIter));
            }

            return outSavings;
        }

        std::list<Tour> constructTours(Graph &g, std::list<Saving> &savings) {

            int nodeNum, i;
            int depotId = g.depotId();
            nodeNum = g.nodeNum();

            // Create n initial routes of the form (depot, i, depot).
            std::list<Tour> routes;
            for (i = 0; i < nodeNum; i++) {
                if (i == depotId) continue;
                Tour r;
                r.addCity(i, g.getDemand(i));
                routes.push_back(r);
            }

            // For each saving, find two routes that can be merged.
            auto s_it = savings.begin();
            Tour *route1, *route2;
            std::list<Tour>::iterator r_it;
            int firstNode, lastNode, erase_index;
            while (s_it != savings.end()) {
                Saving& s = *s_it;
                route1 = nullptr;
                route2 = nullptr;

                r_it = routes.begin();
                i = 0;
                erase_index = 0;
                while (r_it != routes.end()) {
                    // Check if there exists two routes with (i,0) and (0,j).
                    firstNode = r_it->getFirstCity();
                    lastNode = r_it->getLastCity();
                    if (firstNode == s.j) {
                        route2 = &(*r_it);
                        erase_index = i;
                    }
                    if (lastNode == s.i) {
                        route1 = &(*r_it);
                    }
                    r_it = std::next(r_it);
                    i++;
                }

                if (route1 != nullptr && route2 != nullptr && route1 != route2) {
                    if (route1->capacity + route2->capacity <= g.capacity()) {
                        route1->merge(route2);

                        // Delete empty route after merge
                        auto re_it = routes.begin();
                        std::advance(re_it, erase_index);
                        routes.erase(re_it, ++re_it);
                    }
                }

                s_it++;
            }

            return routes;
        }

        /*!
         * Returns the total distance between a string `t` of cities, with the depot of the graph `g` as starting and
         * ending point.
         * @param t list of consecutive cities.
         * @param g graph containing the cities.
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
         * Returns the total distance of a solution.
         * @param tours list of tours
         * @param g graph containing the tours.
         * @return the total cost of the solution (sum of tour distances) as a double.
         */
        double getTotalCost(std::list<Tour> &tours, const Graph &g) {
            double totalCost = 0;
            for (Tour &t : tours) {
                totalCost += getTotalTourDistance(t.cities, g);
            }
            return totalCost;
        }

        std::list<Tour> getFeasible(Graph &g) {
            int nbTries = 0;
            double bestCost = INFINITY;
            std::list<Saving> savings = computeSavings(g);
            std::list<Saving> bestSavings;

            while (nbTries < 1000) {
                nbTries += 1;
                std::list<Tour> tours = constructTours(g, savings);
                if (tours.size() <= g.vehiclesNum()) {
                    std::cout << "Number of tries before feasible: " << nbTries << std::endl;
                    std::cout << "Best cost: " << bestCost << std::endl;
                    return tours;
                }
                double totalCost = getTotalCost(tours, g);
                if (totalCost < bestCost) {
                    bestCost = totalCost;
                    bestSavings = savings;
                    std::cout << "Improvement: " << bestCost << std::endl;
                }
                savings = _updateSavings(bestSavings);
            }

            std::cout << "1000 iterations and no feasible solution" << std::endl;
        }
    }
}

