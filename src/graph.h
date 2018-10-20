#ifndef UTILS_H
#define UTILS_H

#include <lemon/full_graph.h>
#include <cmath>

using std::string;

namespace maoa {

	struct NodeData {
		float x, y, demand;

		string to_string() const {
		    std::stringstream ss;
		    ss << "Node x=" << x << " y=" << y << " d=" << demand;
			return ss.str();
		}
	};

	class Graph {
	public:
        explicit Graph(const string & filename);

		double getDistance(const lemon::FullGraph::Node & v1, const lemon::FullGraph::Node & v2) const {
            NodeData d1 = nodeMap[v1];
            NodeData d2 = nodeMap[v2];
            return sqrt(pow(d1.x - d2.x, 2) + pow(d1.y - d2.y, 2));
        }
        void print() const;
		void draw();
		inline int vehiclesNum() { return vehicles;	}
		inline float capacity() { return Q; }
		inline int nodeNum() { return g.nodeNum(); }
	private:
		lemon::FullGraph g;
		lemon::FullGraph::NodeMap<NodeData> nodeMap;
		lemon::FullGraph::Node depot;
		float Q;
		int vehicles;
	};
}

#endif