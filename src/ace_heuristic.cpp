#include <map>
#include <cmath>
#include <random>

#include "graph.h"
#include "ace_heuristic.h"
#include "limits"

namespace ace {

    ace_heutistic::ace_heutistic(const string &filename) {
        g = new maoa::Graph(filename);
    }

    std::list<maoa::Tour> ace_heutistic::run(int nb_iter, int nb_ants, float beta, float alpha, float q0, float t0) {

        //std::cout<<"Running, graph has "<< g->nodeNum() <<" nodes\n";         //initialization ants

        ant Ants[nb_ants];
        for (int i = 0; i < nb_ants; i++) {
            Ants[i].id = i;
        }
        resetAnts(nb_ants, Ants);

        //std::cout<<"Done initializing ants\n";         //initialization pheromones

        std::stringstream ss;
        for (int i = 0; i < g->nodeNum(); i++) {
            for (int j = i + 1; j < g->nodeNum(); j++) {
                ss << i << j;
                pheromones.insert(std::pair<string, float>(ss.str(), t0));
                ss.str("");
                ss.clear();
            }
        }

        //std::cout<<"Done initializing Pheromones\n";         //data for selecting best ant

        float dst = std::numeric_limits<float>::max();
        int bestAnt = 0;
        std::list<int> bestPath;

        std::list<int>::iterator itm;
        for (int j = 0; j < nb_iter; j++) {
            for (int i = 0; i < nb_ants; i++) {
                while (Ants[i].visited < g->nodeNum()) {
                    selectNode(Ants[i], q0, beta);
                }
                //adding this so that the ant always return to start
                itm = Ants[i].path.begin();
                itm = itm++;
                if(*itm != 0){
                    Ants[i].distance += g->getDistance(Ants[i].position, g->depotId());
                    Ants[i].position = g->depotId();
                    itm = Ants[i].path.begin();
                    Ants[i].path.insert(itm, 0);
                }
                //making the distance longer if you use too many vehicules
                int cpt = 0;
                for (itm = Ants[i].path.begin(); itm != Ants[i].path.end(); ++itm) {
                    if(*itm == 0){
                        cpt++;
                    }
                }
                if(cpt > g->vehiclesNum()+1){
                    //std::cout << "malus"<< "\n";
                    Ants[i].distance = std::numeric_limits<float>::max();;
                }
                if (dst > Ants[i].distance) {
                    bestAnt = Ants[i].id;
                    bestPath = Ants[i].path;
                    dst = Ants[i].distance;
                    //std::cout << "\n new best ant found ;"<< dst << "\n";         //getting the best ant
                }
            }
            updatePheromones(Ants[bestAnt], alpha);
            evaporatePheromones(alpha, t0);

            if(j !=  nb_iter-1){
                resetAnts(nb_ants,Ants);
            }
        }

        std::cout << "best Ant :"<< Ants[bestAnt].id  << " Distance :" << dst << "\n";

        std::list<maoa::Tour> clusters;
        maoa::Tour currentCluster;
        const int depotId = g->depotId();
        std::list<int>::iterator itbp;
        for(itbp = ++bestPath.begin(); itbp != bestPath.end(); itbp ++){
            if(*itbp == 0){
                clusters.push_back(currentCluster);
                currentCluster = maoa::Tour();
            }else{
                currentCluster.addCity(*itbp, g->getDemand(*itbp));
            }
        }
        return clusters;

    }

    void ace_heutistic::selectNode(ant &a, float q0, int beta) {
        float q =  getRandom();
        std::list<int>::iterator ita;
        std::list<int>::iterator it;
        double j = 0;
        int nextNode = -1;
        if (q < q0) {
            for (int i = 0; i < g->nodeNum(); ++i) {
                ita = std::find(a.path.begin(), a.path.end(), i);
                if (ita == a.path.end() && *ita != a.position) { // node not visited : calculate its value
                    if (a.capacity - g->getData(i).demand >= 0) {
                        double tmp = getpheromones(a.position, i) * pow((1 / g->getDistance(a.position, i))*1, beta);
                        if (tmp > j) {
                            j = tmp;
                            nextNode = i;
                        }
                    }
                }
            }
            if (nextNode == -1) {
                //std::cout << 0 << " ";
                a.distance += g->getDistance(a.position, g->depotId());
                a.position = 0;
                a.capacity =  100;
                it = a.path.begin();
                a.path.insert(it, 0);
                return;
            } else {
                //std::cout << nextNode << " ";
                a.distance += g->getDistance(a.position, g->depotId());
                a.position = nextNode;
                it = a.path.begin();
                a.capacity -= g->getData(nextNode).demand;
                a.visited ++;
                a.path.insert(it, nextNode);
                return;
            }
        } else {
            // first loop calculating the sum of all paths
            float sumPaths = 0;
            for (int i = 0; i != g->nodeNum(); i++) {
                ita = std::find(a.path.begin(), a.path.end(), i);
                if (ita == a.path.end() && *ita != a.position) { // node not visited : calculate its value
                    if (a.capacity - g->getData(i).demand >= 0) {
                        sumPaths += getpheromones(a.position, i) * pow((1 / g->getDistance(a.position, i)) * 1, beta);
                    }
                }
            }
            // finding the best path
            float rnd = getRandom();
            std::map<double, int> proba;
            for (int i = 0; i < g->nodeNum(); ++i) {
                ita = std::find(a.path.begin(), a.path.end(), i);
                if (ita == a.path.end() && *ita != a.position) { // node not visited : calculate its value
                    if (a.capacity - g->getData(i).demand >= 0) {
                        double tmp = (getpheromones(a.position, i) * pow(1 / g->getDistance(a.position, i), beta)) /
                                     sumPaths;
                        proba.insert(std::pair<double, int>(tmp, i));
                    }
                }
            }
            if(proba.empty()){
                //std::cout << 0 << " ";
                a.distance += g->getDistance(a.position, g->depotId());
                a.position = 0;
                a.capacity =  100;
                it = a.path.begin();
                a.path.insert(it, 0);
                return;
            }
            std::map<double, int>::iterator itproba;
            float totpop = 0;
            for (itproba = proba.begin(); itproba != proba.end(); ++itproba) {
                totpop += itproba->first;
                if (totpop > rnd) {
                    a.distance += g->getDistance(a.position, itproba->second);
                    a.capacity -= g->getData(itproba->second).demand;
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
        //std::cout << " updating pheromones \n";
        //TODO itrttre da,ns lautre sens
        int prevPosition = a.position;
        std::list<int>::iterator it;
        for (it = ++a.path.begin(); it != a.path.end(); ++it) {
            addpheromones(prevPosition, *it, a.distance,alpha);
            prevPosition = *it;
        }
    }

    void ace_heutistic::evaporatePheromones(float alpha, float t0) {
        //std::cout << " evaporate pheromones : ";
        std::map<string, float>::iterator phit;
        for (phit = pheromones.begin(); phit != pheromones.end(); phit++) {
            //std::cout << phit->second *100000<< "|";
            float tmp = (phit->second * (1 - alpha )) + (t0 * alpha);
            phit->second = tmp;
        }
        //std::cout << " -\n";
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
            //TODO test values
            it->second = ((1 - alpha) * it->second) + (alpha * (1 / value));
        } else {
            ss.str("");
            ss.clear();
            ss << nodeB << nodeA;
            pheromones.find(ss.str())->second += ((1 - alpha) * it->second) + (alpha * 1 / value);

        }
    }

    void ace_heutistic::resetAnts(int nb_ants, ant Ants[]) {
        for (int i = 0; i < nb_ants; i++) {
            Ants[i].position = g->depotId();
            Ants[i].capacity = g->capacity();
            Ants[i].visited = 1;
            Ants[i].path.clear();
            Ants[i].it = Ants[i].path.begin();
            Ants[i].path.insert(Ants[i].it, Ants[i].position);
            Ants[i].distance = 0;
        }
    }

    float ace_heutistic::getRandom() {
        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<> dist(0,1);
        return dist(e2);
    }
}