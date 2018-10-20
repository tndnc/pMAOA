#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>

#include "graph.h"

using std::string;
using namespace maoa;

Graph::Graph(const string &filename) : nodeMap(g) {

    std::ifstream infile;
    infile.open(filename);

    if (!infile) {
        std::cout << "Unable to open file : " << filename << std::endl;
        exit(1);
    }

    string line;
    string data[3];
    int i;

    bool readingCoord = false;
    bool readingDemand = false;
    bool readingDepot = false;

    while (getline(infile, line)) {
        if (line.find("NODE_COORD_SECTION") != string::npos) {
            readingCoord = true;
        }
        else if (line.find("DEMAND_SECTION") != string::npos) {
            readingCoord = false;
            readingDemand = true;
        }
        else if (line.find("DEPOT_SECTION") != string::npos) {
            readingDemand = false;
            readingDepot = true;
        }
        else if(line.find("VEHICLES") != string::npos or line.find("trucks") != string::npos){
            std::regex r ("[[:digit:]]+");
            std::smatch s;
            std::regex_search(line,s,r);
            std::cout << "Vehicles "<< s[0] << "\n";
            vehicles = stoi(s[0]);

        }
        else {
            // Extract data from the line. There is at most three strings that matter to us for each line.
            std::stringstream lineStream(line);
            string temp;
            i = 0;
            while (!lineStream.eof() && i < 3) {
                lineStream >> data[i];
                i++;
            }
            if (data[0] == "DIMENSION") {
                g.resize(stoi(data[2]));
                continue;
            }
            if (data[0] == "CAPACITY") {
                capacity = stof(data[2]);
                continue;
            }
            if (readingCoord) {
                // Add Node to graph.
                int nodeIndex = stoi(data[0]) - 1;
                lemon::FullGraph::Node u = g(nodeIndex);
                nodeMap[u].x = stof(data[1]);
                nodeMap[u].y = stof(data[2]);
            }
            else if (readingDemand) {
                lemon::FullGraph::Node u = g(stoi(data[0]) - 1);
                nodeMap[u].demand = stof(data[1]);
            }
            else if (readingDepot) {
                depot = g(stoi(data[0]) - 1);
                break;
            }
        }
    }
}

void Graph::print() const {
    int nodeNum = g.nodeNum();
    int i;
    for (i = 0; i < nodeNum; i++) {
        lemon::FullGraph::Node u = g(i);
        std::cout << nodeMap[u].to_string() << std::endl;
    }
    std::cout << "Depot: " << g.id(depot) << std::endl;
    std::cout << "Capacity: " << capacity << std::endl;
}

int main () {
    Graph g("../data/A/A-n80-k10.vrp");
    g.print();
}