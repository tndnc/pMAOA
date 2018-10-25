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
        Ants[i].score = 0;
        Ants[i].distance = 0;
        //TODO no need for two variables just distance
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
    //data for selecting best ant
    float score = 0;
    int bestAnt = 0;
    //main loop
    for(int j = 0; j < nb_iter;j++){
        for(int i = 0; i < nb_ants; i ++){
            while (Ants[i].visited < g->nodeNum()){
                selectNode(Ants[i],q0,beta);
            }
            //getting best ant
            if(score <= Ants[i].score ){
                bestAnt = Ants[i].id;
            }
        }
        updatePheromones(Ants[bestAnt],alpha);
        evaporatePheromones(alpha, t0);
    }
}

void aceHeutistic::selectNode(ant& a,float q0,int beta) {
    //TODO add negative score if too many vehicles
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
                    double tmp = getpheromones(a.position, *ita) * pow(1/g->getDistance(a.position, *ita), beta);
                    if (tmp > j) {
                        j = tmp;
                        nextNode = *ita;
                    }
                }
            }
        }
        if (nextNode == -1) {
            a.distance += g->getDistance(a.position, g->depotId());
            a.score += 1/(a.distance); //updating score
            a.position = 0;
            it = a.path.begin();
            a.path.insert(it,0);
        } else {
            a.distance += g->getDistance(a.position, g->depotId());
            a.score += 1/(a.distance); //updating score
            a.position = nextNode;
            it = a.path.begin();
            a.path.insert(it,nextNode);
        }
    }else{
        //TODO turn it into probability
        // first loop calculating the sum of all paths
        float sumPaths = 0;
        for (it = nodes.begin(); it != nodes.end(); ++it) {
            ita = std::find(a.path.begin(), a.path.end(), *it);
            if (ita != a.path.end()) { // node not visited : calculate its value
                if (a.capacity + g->getData(*ita).demand <= g->capacity()) {
                        sumPaths += getpheromones(a.position, *ita) * pow(1/g->getDistance(a.position, *ita), beta);
                }
            }
        }
        // finding the best path
        for (it = nodes.begin(); it != nodes.end(); ++it) {
            ita = std::find(a.path.begin(), a.path.end(), *it);
            if (ita != a.path.end()) { // node not visited : calculate its value
                if (a.capacity + g->getData(*ita).demand <= g->capacity()) {
                    double tmp = (getpheromones(a.position, *ita) * pow(1/g->getDistance(a.position, *ita), beta))/sumPaths;
                }
            }
        }
    }
}


void aceHeutistic::updatePheromones(ant& a,float alpha) {
    int prevPosition = 0;
    std::list<int>::iterator it;
    for (it = ++a.path.begin(); it != a.path.end(); ++it) {
        addpheromones(prevPosition, *it, alpha, a.distance);
    }
}

void aceHeutistic::evaporatePheromones(float alpha, float t0){
    std::map<string,float>::iterator phit;
    for(phit = pheromones.begin(); phit != pheromones.end(); phit++){
        phit->second = phit->second *(alpha-1) + t0 * alpha;
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

void aceHeutistic::addpheromones(int nodeA, int nodeB,float value, float alpha){
    std::stringstream ss;
    ss << nodeA << nodeB;
    std::map<string,float>::iterator it;
    it = pheromones.find(ss.str());
    if(it != pheromones.end()){
        it->second = ((1-alpha) * it->second) + (alpha * 1/value);
    } else{
        ss.str("");
        ss.clear();
        ss << nodeB << nodeA;
        pheromones.find(ss.str())->second += ((1-alpha) * it->second) + (alpha * 1/value);

    }
}

