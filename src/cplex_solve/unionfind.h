#ifndef PMAOA_UNIONFIND_H
#define PMAOA_UNIONFIND_H

#include <vector>

class UnionFind {
public:
    explicit UnionFind(int size) {
        _parent.reserve(static_cast<unsigned long>(size));
        _rank.reserve(static_cast<unsigned long>(size));
        for (int i = 0; i < size; i++) {
            _parent[i] = i;
            _rank[i] = i;
        }
    }
    int find(int x) {
        if (_parent[x] != x) {
            _parent[x] = find(_parent[x]);
        }
        return _parent[x];
    }
    void merge(int x, int y) {
        int x_parent = find(x);
        int y_parent = find(y);

        if (x_parent == y_parent) return;

        if (_rank[x_parent] < _rank[y_parent]) {
            _parent[x_parent] = y_parent;
        } else {
            _parent[y_parent] = x_parent;
            if (_rank[x_parent] == _rank[y_parent]) {
                _rank[x_parent] += 1;
            }
        }
    }
private:
    std::vector<int> _parent;
    std::vector<int> _rank;
};


#endif //PMAOA_UNIONFIND_H
