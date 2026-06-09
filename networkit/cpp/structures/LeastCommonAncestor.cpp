#include <networkit/graph/GraphTools.hpp>
#include <networkit/structures/LeastCommonAncestor.hpp>

namespace NetworKit {

LeastCommonAncestor::LeastCommonAncestor(const Graph &tree, node root) : g(&tree) {
    count n = tree.numberOfNodes();
    count m = tree.numberOfEdges();

    if (!tree.isDirected())
        throw std::invalid_argument("Input graph must be directed");
    if (m != n - 1)
        throw std::invalid_argument("Input graph is not a tree");

    position.assign(tree.upperNodeIdBound(), none);
    count visited_nodes = 0;

    auto dfs = [&](auto &self, node u, node p, int64_t depth) -> void {
        visited_nodes++;
        position[u] = euler_tour.size();
        euler_tour.push_back(u);
        depths.push_back(depth);

        tree.forNeighborsOf(u, [&](node v) {
            if (v != p) {
                self(self, v, u, depth + 1);
                euler_tour.push_back(u);
                depths.push_back(depth);
            }
        });
    };

    dfs(dfs, root, none, 0);
    if (visited_nodes != n)
        throw std::invalid_argument("Input graph is not connected.");
    rmq = Aux::RangeMinimumQuery(depths);
}

node LeastCommonAncestor::Query(std::span<const node> nodes) {
    if (nodes.empty())
        return none;

    index left = none, right = 0;
    for (node v : nodes) {
        index pos = position[v];
        if (pos < left)
            left = pos;
        if (pos >= right)
            right = pos + 1;
    }

    index min_idx = rmq.Query(left, right);
    return euler_tour[min_idx];
}

} // namespace NetworKit
