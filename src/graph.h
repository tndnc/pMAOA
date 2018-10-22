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
            return eucDist(d1, d2);
        }
        double getDistance(int n1, int n2) const {
            return getDistance(g(n1), g(n2));
		}
        void print() const;
		void draw();
		int vehiclesNum() const { return vehicles;	}
		float capacity() const { return Q; }
		int nodeNum() const { return g.nodeNum(); }
		int depotId() const { return g.id(depot); }
		lemon::FullGraph::Node operator()(int nodeId) const {
		    return g(nodeId);
		}
		float getDemand(lemon::FullGraph::Node u) const {
		    return nodeMap[u].demand;
		}
		NodeData getData(lemon::FullGraph::Node u) const {
		    return nodeMap[u];
		}
		NodeData getData(int nodeId) const {
		    lemon::FullGraph::Node u = g(nodeId);
		    return getData(u);
		}
	private:
		lemon::FullGraph g;
		lemon::FullGraph::NodeMap<NodeData> nodeMap;
		lemon::FullGraph::Node depot;
		float Q;
		int vehicles;

		double eucDist(NodeData & d1, NodeData & d2) const {
		    return sqrt(pow(d1.x - d2.x, 2) + pow(d1.y - d2.y, 2));
		}
	};
}

#endif