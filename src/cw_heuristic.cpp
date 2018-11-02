#include "cw_heuristic.h"

namespace maoa {
    namespace cw {

        std::list<Saving> _computeSavings(Graph &g) {
            std::list<Saving> savings;
            double lambda = 1; // TODO: change this parameter

            int nodeNum, i, j;
            int depotId = g.depotId();
            double ci0, c0j, cij;
            nodeNum = g.nodeNum();
            for (i = 0; i < nodeNum; i++) {
                for (j = 0; j < nodeNum; j++) {
                    if (i == j) continue;
                    if (i == depotId || j == depotId) continue;
                    ci0 = g.getDistance(i, depotId);
                    c0j = g.getDistance(j, depotId);
                    cij = g.getDistance(i, j);
                    savings.emplace_back(i, j, ci0 + c0j - (lambda * cij));
                }
            }
            savings.sort([&](Saving &s1, Saving &s2) {
                return s1.saving > s2.saving;
            });

            return savings;
        }

        std::list<Tour> constructTours(Graph &g) {

            // TODO: Too many savings computed. Could be reduced to nlogn. The construction then needs to check the
            // routes beginning-ending by (i,0)-(0,j) and (j,0)-(0,i).
            std::list<Saving> savings = _computeSavings(g);
            int nodeNum, i, j;
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
    }
}

