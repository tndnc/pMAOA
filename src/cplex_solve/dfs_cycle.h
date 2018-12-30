#ifndef PMAOA_DFS_CYCLE_H
#define PMAOA_DFS_CYCLE_H

#include <set>
#include "../graph.h"
#include "vrpmodel.h"

/*!
 * This class can be used to detect and retrieve a cycle in a directed graph.
 * It is used in conjunction with a \p VrpModel who must provide a way to
 * index the arcs of the graph. \p reset() must be called between each search.
 * The underlying algorithm is a Depth-First-Search performed with recursion.
 */
class FindCycleUtil {
public:
    /*!
     * Constructor.
     * @param nodeNum Number of nodes in the graph.
     * @param a Boolean map that specifies which arcs are present in the graph.
     * @param m \p VrpModel used to index the arcs.
     */
    explicit FindCycleUtil(
            int nodeNum,
            std::unordered_map<int, bool> &a,
            maoa::cplex::VrpModel &m) : _arcs(&a), _m(&m), _nodeNum(nodeNum),
                                        _cycleFound(false), _cycleStart(-1)
    {
        reset();
    }
    /*!
     * Initializes all structures used during the search. Must be called
     * between each search.
     */
    void reset() {
        _cycleFound = false;
        _cycleStart = -1;
        _visited = std::make_unique<bool[]>(static_cast<size_t>(_nodeNum));
        _recursionStack = std::make_unique<bool[]>
                (static_cast<size_t>(_nodeNum));

        for (int i = 0; i < _nodeNum; i++) {
            _visited[i] = false;
            _recursionStack[i] = false;
        }
    }
    /*!
     * Searches and retrieves a cycle.
     * @param cycle container for the cycle to be found.
     * @return \a True if a cycle was found. \a False otherwise.
     */
    bool getCycle(std::set<int> &cycle) {
        for (int u = 0; u < _nodeNum; u++) {
            if (findCycle(-1, u, cycle)) {
                return true;
            }
        }
        return false;
    }
private:
    int _nodeNum, _cycleStart;
    bool _cycleFound;
    std::unordered_map<int, bool> *_arcs;
    maoa::cplex::VrpModel *_m;
    std::unique_ptr<bool[]> _visited, _recursionStack;

    /*!
     * Auxiliary recursive function used for DFS.
     * @param u Node to visit.
     * @param cycle Container for the cycle.
     * @return \a True if a cycle was found while visiting \a u. \a False
     * otherwise.
     */
    bool findCycle(int parent, int u, std::set<int> &cycle) {
        if (_visited[u]) return false;

        _visited[u] = true;
        _recursionStack[u] = true;

        for (int v = 0; v < _nodeNum; v++) {
            if (v == parent) continue;
            if (u == v) continue;

            int idx = _m->edgeToIdx(u, v);
            if ((*_arcs)[idx]) { // v is adjacent to u.
                if(!_visited[v] && findCycle(u, v, cycle)) {
                    if (v == _cycleStart) {
                        // Recursion has reached the start of the cycle.
                        _cycleFound = false;
                    }
                    if (_cycleFound) {
                        cycle.emplace(v);
                    }
                    return true;
                } else if (_recursionStack[v]) {
                    // Found start of cycle.
                    _cycleStart = v;
                    _cycleFound = true;
                    cycle.emplace(v);
                    return true;
                }
            }
        }

        _recursionStack[u] = false;
        return false;
    }
};

#endif //PMAOA_DFS_CYCLE_H
