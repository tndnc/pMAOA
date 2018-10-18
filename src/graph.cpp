#include "graph.h"
#include<cstring>
#include<sstream>
#include<fstream>
#include<iostream>

#include <lemon/list_graph.h>
#include <lemon/dim2.h>

using namespace std;
using namespace lemon;

/*
void C_Graph::read_undirected_complete_TSP(istream & fic){



*/
void VrpReader::readVrpFiles(string fileName, ListDigraph g){

  ifstream inFile;
  inFile.open(fileName);
  if ( !inFile ) {
      cout << "Unable to open file : " << fileName << "\n";
    exit(1);
  }

  string line;
  ListDigraph::NodeMap<dim2::Point<int>> coord(g);
  bool readCoord = false;
  bool readSection = false;
  int data [3] = { };
  while ( getline(inFile, line) ) {
    if(readCoord){
      if(line.find("DEMAND_SECTION") != string::npos){
        readCoord = false;
        readSection = true;
        continue;
        }
      stringstream stream(line);
      ListDigraph::Node u = g.addNode();
      for (int i = 0; i < 3;i++){
        int n;
        stream >> n;
        if(i == 1){
          coord[u].x = n;
        }else if(i == 2){
          coord[u].y = n;
        }
      }
    }
    if(line.find("NODE_COORD_SECTION") != string::npos)
      readCoord = true;
    if(line.find("DEPOT_SECTION") != string::npos)
      readSection = false;  
  }
}

int main(int argc,char**argv){
  ListDigraph g;
  VrpReader vr;
  //VrpReader *vr = new VrpReader();
  vr.readVrpFiles(argv[1],g);
}