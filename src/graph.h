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

		double getDistance(const lemon::FullGraph::Node &v1, const lemon::FullGraph::Node &v2) const {
            NodeData d1 = _nodeMap[v1];
            NodeData d2 = _nodeMap[v2];
            return _eucDist(d1, d2);
        }
        double getDistance(int n1, int n2) const {
            return getDistance(_g(n1), _g(n2));
		}
        void print() const;
		int vehiclesNum() const { return _vehicles;	}
		float capacity() const { return _Q; }
		int nodeNum() const { return _g.nodeNum(); }
		int depotId() const { return _depotId; }
		lemon::FullGraph::Node operator()(int nodeId) const {
		    return _g(nodeId);
		}
		float getDemand(lemon::FullGraph::Node u) const {
		    return _nodeMap[u].demand;
		}
		float getDemand(int i) {
		    return getDemand(_g(i));
		}
		NodeData getData(lemon::FullGraph::Node u) const {
		    return _nodeMap[u];
		}
		NodeData getData(int nodeId) const {
		    lemon::FullGraph::Node u = _g(nodeId);
		    return getData(u);
		}
		std::list<int> getNodesIndexes() const {
			std::list<int> nodesIndexes;
			std::list<int>::iterator it;
			for (lemon::FullGraph::NodeIt n(_g); n != lemon::INVALID; ++n) {
				nodesIndexes.insert(it, _g.id(n));
				++it;
			}
			return  nodesIndexes;
		}
		int arcNum() const {
			return _g.arcNum();
		};

	private:
		lemon::FullGraph _g;
		lemon::FullGraph::NodeMap<NodeData> _nodeMap;
		int _depotId;
		float _Q;
		int _vehicles;

		double _eucDist(NodeData &d1, NodeData &d2) const {
		    return sqrt(pow(d1.x - d2.x, 2) + pow(d1.y - d2.y, 2));
		}
	};

	struct VTour {
	private:
		std::vector<int> cities;
	public:
		float capacity;
		VTour() : capacity(0) {}
		int getCity(unsigned long idx) { return cities.at(idx); }
		unsigned long size() const { return cities.size(); }
		void addCity(int nodeId, float c) {
			cities.push_back(nodeId);
			capacity += c;
		}
		void print() const {
			for (int c : cities) {
				std::cout << c << " ";
			}
			std::cout << std::endl << "Capacity: " << capacity << std::endl;
			std::cout << "Size: " << cities.size() << std::endl;
		}
	};
}

#endif