#include <map>
#include <cmath>

#include "graph.h"
#include "ace_heuristic.h"

namespace ace {

    ace_heutistic::ace_heutistic(const string &filename) {
        g = new maoa::Graph(filename);
    }

    void ace_heutistic::run(int nb_iter, int nb_ants, int beta, float alpha, float q0, float t0) {

        std::cout<<"Running\n";         //initialization ants

        ant Ants[nb_ants];
        for (int i = 0; i < nb_ants; i++) {
            Ants[i].id = i;
            Ants[i].position = g->depotId();
            Ants[i].capacity = g->capacity();
            Ants[i].visited = 1;
            Ants[i].it = Ants[i].path.begin();
            Ants[i].path.insert(Ants[i].it, Ants[i].position);
            Ants[i].score = 0;
            Ants[i].distance = 0;
            //TODO no need for two variables just distance
        }

        std::cout<<"Done initializing ants\n";         //initialization pheromones

        std::stringstream ss;
        for (int i = 0; i < g->arcNum(); i++) {
            for (int j = i + 1; j < g->arcNum(); j++) {
                ss << i << j;
                pheromones.insert(std::pair<string, float>(ss.str(), t0));
                ss.str("");
                ss.clear();
            }
        }

        std::cout<<"Done initializing Pheromones\n";         //data for selecting best ant

        float score = 0;
        int bestAnt = 0;
        std::cout << "started Main loop\n";         //main loop
        for (int j = 0; j < nb_iter; j++) {
            for (int i = 0; i < nb_ants; i++) {
                while (Ants[i].visited < g->nodeNum()) {
                    //std::cout << "test\n";
                    selectNode(Ants[i], q0, beta);
                    //std::cout << "test\n";
                }
                std::cout << "getting the best ant \n";         //getting the best ant
                if (score <= Ants[i].score) {
                    bestAnt = Ants[i].id;
                }
            }
            updatePheromones(Ants[bestAnt], alpha);
            evaporatePheromones(alpha, t0);
        }
        for(int i = 0; i < nb_ants; i++) {
            if (score <= Ants[i].score) {
                bestAnt = Ants[i].id;
            }
        }
        std::cout << bestAnt;
    }

    void ace_heutistic::selectNode(ant &a, float q0, int beta) {
        //TODO add negative score if too many vehicles
        std::cout << "node selection for ant " << a.id << ":";
        float q =  static_cast<float> (rand()) / static_cast<float> (RAND_MAX);
        std::list<int>::iterator ita;
        std::list<int>::iterator it;
        double j = 0;
        int nextNode = -1;
        if (q < q0) {
            for (int i = 0; i != g->nodeNum(); i++) {
                ita = std::find(a.path.begin(), a.path.end(), i);
                if (ita != a.path.end()) { // node not visited : calculate its value
                    if (a.capacity + g->getData(*ita).demand <= g->capacity()) {
                        double tmp = getpheromones(a.position, *ita) * pow(1 / g->getDistance(a.position, *ita), beta);
                        if (tmp > j) {
                            j = tmp;
                            nextNode = *ita;
                        }
                    }
                }
            }
            if (nextNode == -1) {
                a.distance += g->getDistance(a.position, g->depotId());
                a.score += 1 / (a.distance); //updating score
                a.position = 0;
                it = a.path.begin();
                a.path.insert(it, 0);
                return;
            } else {
                a.distance += g->getDistance(a.position, g->depotId());
                a.score += 1 / (a.distance); //updating score
                a.position = nextNode;
                it = a.path.begin();
                a.visited ++;
                a.path.insert(it, nextNode);
                return;
            }
        } else {
            //TODO turn it into probability
            // first loop calculating the sum of all paths
            float sumPaths = 0;
            for (int i = 0; i != g->nodeNum(); i++) {
                ita = std::find(a.path.begin(), a.path.end(), i);
                if (ita != a.path.end()) { // node not visited : calculate its value
                    if (a.capacity + g->getData(*ita).demand <= g->capacity()) {
                        sumPaths += getpheromones(a.position, *ita) * pow(1 / g->getDistance(a.position, *ita), beta);
                    }
                }
            }
            // finding the best path
            float rnd = static_cast<float> (rand()) / static_cast<float> (RAND_MAX);
            std::map<double, int> proba;
            for (int i = 0; i != g->nodeNum(); i++) {
                ita = std::find(a.path.begin(), a.path.end(), i);
                if (ita != a.path.end()) { // node not visited : calculate its value
                    if (a.capacity + g->getData(*ita).demand <= g->capacity()) {
                        proba.insert(std::pair<double, int>((getpheromones(a.position, *ita) *
                                                             pow(1 / g->getDistance(a.position, *ita), beta) /
                                                             sumPaths), *ita));
                    }
                }
            }
            std::map<double, int>::iterator itproba;
            for (itproba = proba.begin(); itproba != proba.end(); ++itproba) {
                if (itproba->first > rnd) {
                    a.distance += g->getDistance(a.position, itproba->second);
                    a.score += 1 / (a.distance); //updating score
                    a.position = itproba->second;
                    it = a.path.begin();
                    a.visited ++;
                    a.path.insert(it,itproba->second);
                    return;
                }
            }
        }
    }


    void ace_heutistic::updatePheromones(ant &a, float alpha) {
        int prevPosition = 0;
        std::list<int>::iterator it;
        for (it = ++a.path.begin(); it != a.path.end(); ++it) {
            addpheromones(prevPosition, *it, alpha, a.distance);
        }
    }

    void ace_heutistic::evaporatePheromones(float alpha, float t0) {
        std::map<string, float>::iterator phit;
        for (phit = pheromones.begin(); phit != pheromones.end(); phit++) {
            phit->second = phit->second * (alpha - 1) + t0 * alpha;
        }
    }

    float ace_heutistic::getpheromones(int nodeA, int nodeB) {
        std::stringstream ss;
        ss << nodeA << nodeB;
        std::map<string, float>::iterator it;
        it = pheromones.find(ss.str());
        if (it != pheromones.end()) {
            return it->second;
        } else {
            ss.str("");
            ss.clear();
            ss << nodeB << nodeA;
            return pheromones.find(ss.str())->second;

        }
    }

    void ace_heutistic::addpheromones(int nodeA, int nodeB, float value, float alpha) {
        std::stringstream ss;
        ss << nodeA << nodeB;
        std::map<string, float>::iterator it;
        it = pheromones.find(ss.str());
        if (it != pheromones.end()) {
            it->second = ((1 - alpha) * it->second) + (alpha * 1 / value);
        } else {
            ss.str("");
            ss.clear();
            ss << nodeB << nodeA;
            pheromones.find(ss.str())->second += ((1 - alpha) * it->second) + (alpha * 1 / value);

        }
    }
}