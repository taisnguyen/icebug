/*
 * LeastCommonAncestorGTest.cpp
 *
 *  Created on: 06.08.2026
 *      Author: Ian Chen (ianchen3@illinois.edu)
 */

#include <stdexcept>
#include <gtest/gtest.h>
#include <networkit/graph/GraphW.hpp>
#include <networkit/structures/LeastCommonAncestor.hpp>

namespace NetworKit {

class LeastCommonAncestorGTest : public testing::Test {
protected:
    GraphW buildValidTree() {
        GraphW g(7, false, true);
        g.addEdge(0, 1);
        g.addEdge(0, 2);
        g.addEdge(1, 3);
        g.addEdge(1, 4);
        g.addEdge(2, 5);
        g.addEdge(5, 6);
        return g;
    }
};

TEST_F(LeastCommonAncestorGTest, testDisconnected) {
    GraphW g(4, false, true);
    g.addEdge(0, 1);
    g.addEdge(2, 3);
    EXPECT_THROW(LeastCommonAncestor lca(g, 0), std::invalid_argument);
}

TEST_F(LeastCommonAncestorGTest, testCyclic) {
    GraphW g(4, false, true);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 1);
    EXPECT_THROW(LeastCommonAncestor lca(g, 0), std::invalid_argument);
}

TEST_F(LeastCommonAncestorGTest, testPairs) {
    GraphW g = buildValidTree();
    LeastCommonAncestor lca(g, 0);

    EXPECT_EQ(lca.Query(std::vector<node>{3, 4}), 1);
    EXPECT_EQ(lca.Query(std::vector<node>{3, 5}), 0);
    EXPECT_EQ(lca.Query(std::vector<node>{4, 6}), 0);
    EXPECT_EQ(lca.Query(std::vector<node>{1, 4}), 1);
    EXPECT_EQ(lca.Query(std::vector<node>{2, 6}), 2);
    EXPECT_EQ(lca.Query(std::vector<node>{3, 3}), 3);
    EXPECT_EQ(lca.Query(std::vector<node>{0, 5}), 0);
}

TEST_F(LeastCommonAncestorGTest, testTriplets) {
    GraphW g = buildValidTree();
    LeastCommonAncestor lca(g, 0);

    EXPECT_EQ(lca.Query(std::vector<node>{1, 3, 4}), 1);
    EXPECT_EQ(lca.Query(std::vector<node>{2, 5, 6}), 2);
    EXPECT_EQ(lca.Query(std::vector<node>{3, 4, 5}), 0);
    EXPECT_EQ(lca.Query(std::vector<node>{3, 5, 6}), 0);
    EXPECT_EQ(lca.Query(std::vector<node>{4, 4, 1}), 1);
    EXPECT_EQ(lca.Query(std::vector<node>{0, 3, 6}), 0);
}

} /* namespace NetworKit */
