#include <map>
#include <cmath>

#include "graph.h"
#include "aceHeuristics.h"


using namespace ace;

aceHeutistic::aceHeutistic(const string &filename) {
    g = new maoa::Graph(filename);
}

void aceHeutistic::run(int nb_iter, int nb_ants, int beta, float alpha, float q0, float t0) {

    //initialization ants
    ant Ants [nb_ants];
    for(int i = 0; i < nb_ants; i++){
        Ants[i].id = i;
        Ants[i].position = g->depotId();
        Ants[i].capacity = g->capacity();
        Ants[i].visited = 1;
        Ants[i].it = Ants[i].path.begin();
        Ants[i].path.insert(Ants[i].it,Ants[i].position);
    }

    //initialization pheromones
    std::stringstream ss;
    for(int i = 0; i < g->arcNum(); i++){
        for(int j = i+1; j < g->arcNum(); j ++){
            ss << i << j;
            pheromones.insert(std::pair<string,float >(ss.str(),t0));
            ss.str("");
            ss.clear();
        }
    }
    //main loop
    for(int j = 0; j < nb_iter;j++){
        for(int i = 0; i < nb_ants; i ++){
            while (Ants[i].visited < g->nodeNum());
                selectNode(Ants[i],q0,beta);
        }
        updatePheromones();
        evaporatePheromones(alpha, t0);
    }
}

void aceHeutistic::selectNode(ant& a,float q0,int beta) {
    //TODO check if backpack is full
    float q = static_cast<float> (rand()) / static_cast<float> (RAND_MAX);
    std::list<int> nodes = g->getNodesIndexes();
    std::list<int>::iterator it;
    std::list<int>::iterator ita;
    double j = 0;
    int nextNode = -1;
    if (q < q0) {
        for (it = nodes.begin(); it != nodes.end(); ++it) {
            ita = std::find(a.path.begin(), a.path.end(), *it);
            if (ita != a.path.end()) { // node not visited : calculate its value
                if (a.capacity + g->getData(*ita).demand <= g->capacity()) {
                    double tmp = getpheromones(a.position, *ita) * pow(g->getDistance(a.position, *ita), beta);
                    if (tmp > j) {
                        j = tmp;
                        nextNode = *ita;
                    }
                }
            }
        }
        if (nextNode == -1) {
            a.position = 0;
        } else {
            a.position = nextNode;
            //TODO update score
        }
    }else{
        for (it = nodes.begin(); it != nodes.end(); ++it) {
            ita = std::find(a.path.begin(), a.path.end(), *it);
            if (ita != a.path.end()) { // node not visited : calculate its value
                if (a.capacity + g->getData(*ita).demand <= g->capacity()) {
                        //TODO calculate probability
                }
            }
        }
    }
}


void aceHeutistic::updatePheromones() {

}

void aceHeutistic::evaporatePheromones(float alpha, float t0) {
    std::map<string,float>::iterator it;
    for(it = pheromones.begin(); it != pheromones.end(); it++){
        it->second = it->second *(alpha-1) + t0 * alpha;
    }
}

float aceHeutistic::getpheromones(int nodeA, int nodeB) {
    std::stringstream ss;
    ss << nodeA << nodeB;
    std::map<string,float>::iterator it;
    it = pheromones.find(ss.str());
    if(it != pheromones.end()){
        return it->second;
    } else{
        ss.str("");
        ss.clear();
        ss << nodeB << nodeA;
        return pheromones.find(ss.str())->second;

    }
}


