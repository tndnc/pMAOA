#include "cw_heuristic.h"

namespace maoa {
    namespace cw {

        std::vector<Tour> constructTours(Graph &g) {

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
                    Saving s{};
                    ci0 = g.getDistance(i, depotId);
                    c0j = g.getDistance(j, depotId);
                    cij = g.getDistance(i, j);
                    s.i = i;
                    s.j = j;
                    s.saving = ci0 + c0j - (lambda * cij);
                    savings.push_back(s);
                }
            }
            savings.sort([&](Saving &s1, Saving &s2) {
                return s1.saving > s2.saving;
            });

            // Create n initial routes of the form (depot, i, depot).
            std::list<std::list<int>> routes;
            for (i = 0; i < nodeNum; i++) {
                if (i == depotId) continue;
                std::list<int> r;
                r.push_back(depotId);
                r.push_back(i);
                r.push_back(depotId);
                routes.push_back(r);
            }

            // For each saving, find two routes that can be merged.
            auto it = savings.begin();
            while (it != savings.end()) {


                --it;
            }

            std::vector<Tour> tours;
            return tours;
        }
    }
}

