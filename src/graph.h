#ifndef UTILS_H
#define UTILS_H

#include <lemon/list_graph.h>
#include <lemon/dim2.h>

using namespace std;
using namespace lemon;

class VrpReader{
public:	

	void readVrpFiles(string fileName, ListDigraph g);

};

#endif