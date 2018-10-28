#include <vector>
#include <random>

#include "bp_heuristic.h"

namespace maoa {

    std::vector<VTour> constructClusters(Graph &g) {

        std::vector<VTour> clusters;
        VTour currentCluster;
        const int nodeNum = g.nodeNum();
        const int depotId = g.depotId();
        int startingPoint;

        std::random_device rd; // used to obtain a seed for the rng.
        std::mt19937 gen(rd()); // seeding generator with rd.
        std::uniform_int_distribution<> dis(0, nodeNum - 1); // create distribution with interval [0,nodeNum-1]

        do {
            startingPoint = dis(gen);
        } while (startingPoint == depotId);

        std::cout << "Starting point: " << startingPoint << std::endl;

        lemon::FullGraph::Node s = g(startingPoint);
        currentCluster.addCity(startingPoint, g.getDemand(s));

        std::list<int> nodeIds(nodeNum);
        std::iota(nodeIds.begin(), nodeIds.end(), 0);
        // Sorting by decreasing distance
        nodeIds.sort([&](int a, int b) {
            return g.getDistance(a, startingPoint) > g.getDistance(b, startingPoint);
        });

        auto it = nodeIds.end();
        while (!nodeIds.empty()) {
            it = std::prev(it);
            int nodeId = (*it);
            if (nodeId == depotId || nodeId == startingPoint) {
                // Discard depot
                nodeIds.erase(it, std::next(it));
                continue;
            }

            std::cout << "Considering node id: " << nodeId << ". Distance to sp = "
                                                              << g.getDistance(nodeId, startingPoint) << std::endl;
            lemon::FullGraph::Node u = g(nodeId);
            if (currentCluster.capacity + g.getDemand(u) <= g.capacity()) {
                // Add city to cluster
                currentCluster.addCity(nodeId, g.getDemand(u));
                std::cout << "[Node " << nodeId << "] Added to cluster" << std::endl;
                // Erase city from unvisited cities.
                nodeIds.erase(it, std::next(it));
            }
            else {
                // Current node could not be added to current cluster. Two possibilities:
                //
                // 1. Cluster is full and no city can be added (this is the case if all cities have been considered).
                // A new cluster must be created in this case and the cities must be resorted.
                //
                // 2. A city could be added but we haven't considered it yet. We simply move the iterator.
                if (it == nodeIds.begin()) {
                    std::cout << "Cluster is full" << std::endl;
                    clusters.push_back(currentCluster);
                    currentCluster = VTour();
                    // Reset iterator to the end of the list
                    startingPoint = (*--nodeIds.end());
                    std::cout << "New starting point: " << startingPoint << std::endl;
                    s = g(startingPoint);
                    currentCluster.addCity(startingPoint, g.getDemand(s));
                    nodeIds.erase(--nodeIds.end(), nodeIds.end());
                    // Sort cities according to new starting point
                    nodeIds.sort([&](int a, int b) {
                        return g.getDistance(a, startingPoint) > g.getDistance(b, startingPoint);
                    });
                    it = nodeIds.end();
                }
            }
        }

        clusters.push_back(currentCluster);
        return clusters;
    }
}

