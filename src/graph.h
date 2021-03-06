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

	class Graph : public lemon::FullGraph{
	public:
        explicit Graph(const string & filename);

		double getDistance(const lemon::FullGraph::Node &v1, const lemon::FullGraph::Node &v2) const {
            NodeData d1 = _nodeMap[v1];
            NodeData d2 = _nodeMap[v2];
            return _eucDist(d1, d2);
        }
        double getDistance(int n1, int n2) const {
            return getDistance(this->operator()(n1), this->operator()(n2));
		}
        void print() const;
		int vehiclesNum() const { return _vehicles;	}
		float capacity() const { return _Q; }
		int depotId() const { return _depotId; }
		float getDemand(lemon::FullGraph::Node u) const {
		    return _nodeMap[u].demand;
		}
		float getDemand(int i) const {
		    return getDemand(this->operator()(i));
		}
		NodeData getData(lemon::FullGraph::Node u) const {
		    return _nodeMap[u];
		}
		NodeData getData(int nodeId) const {
		    lemon::FullGraph::Node u = this->operator()(nodeId);
		    return getData(u);
		}

	private:
		lemon::FullGraph::NodeMap<NodeData> _nodeMap;
		int _depotId;
		float _Q;
		int _vehicles;

		double _eucDist(NodeData &d1, NodeData &d2) const {
		    return sqrt(pow(d1.x - d2.x, 2) + pow(d1.y - d2.y, 2));
		}
	};

	struct Tour {
		float capacity;
		std::list<int> cities;

		Tour() : capacity(0) {}
		void addCity(int i, float c) {
			cities.push_back(i);
			capacity += c;
		}
		int getFirstCity() const {
			return *cities.begin();
		}
		int getLastCity() const {
			return *--cities.end();
		}
		void print() const {
			std::cout << "Capacity: " << capacity << std::endl;
			std::cout << "0 ";
			for (int a : cities) {
				std::cout << a << " ";
			}
			std::cout << "0" << std::endl;
		}
		void merge(Tour * t2) {
			cities.splice(cities.end(), t2->cities);
			capacity += t2->capacity;
		}
	};
}

#endif