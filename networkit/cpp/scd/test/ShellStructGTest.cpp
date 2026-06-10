/*
 * ShellStructGTest.cpp
 *
 *  Created on: 06.08.2026
 *      Author: Ian Chen (ianchen3@illinois.edu)
 */

#include <numeric>
#include <vector>
#include <gtest/gtest.h>
#include <networkit/graph/GraphW.hpp>
#include <networkit/scd/ShellStruct.hpp>

namespace NetworKit {

class ShellStructGTest : public ::testing::Test {
protected:
    GraphW getCliqueChain(const std::vector<size_t> &cliqueSizes) {
        size_t totalNodes = 0;
        for (size_t size : cliqueSizes)
            totalNodes += size;

        GraphW g(totalNodes, false, false);
        size_t start = 0;
        for (size_t size : cliqueSizes) {
            for (size_t u = start; u < start + size; ++u)
                for (size_t v = u + 1; v < start + size; ++v)
                    g.addEdge(u, v);
            start += size;
        }

        start = 0;
        for (size_t i = 0; i < cliqueSizes.size() - 1; ++i) {
            start += cliqueSizes[i];
            g.addEdge(start - 1, start);
        }

        return g;
    }

    std::set<node> gtSearch(const std::vector<size_t> &cliques, const std::set<node> &query) {
        std::vector<size_t> boundaries;
        size_t currentSum = 0;
        for (size_t size : cliques) {
            currentSum += size;
            boundaries.push_back(currentSum);
        }

        size_t cMin = cliques.size();
        size_t cMax = 0;

        for (node q : query) {
            size_t cIdx = 0;
            for (size_t i = 0; i < boundaries.size(); ++i) {
                if (q < boundaries[i]) {
                    cIdx = i;
                    break;
                }
            }
            cMin = std::min(cMin, cIdx);
            cMax = std::max(cMax, cIdx);
        }

        size_t bottleneckSize = cliques[cMin];
        for (size_t i = cMin; i <= cMax; ++i)
            bottleneckSize = std::min(bottleneckSize, cliques[i]);

        int left = static_cast<int>(cMin);
        while (left > 0 && cliques[left - 1] >= bottleneckSize)
            left--;

        int right = static_cast<int>(cMax);
        while (right < static_cast<int>(cliques.size()) - 1 && cliques[right + 1] >= bottleneckSize)
            right++;

        size_t vBeg = (left > 0) ? boundaries[left - 1] : 0;
        size_t vEnd = boundaries[right];

        std::set<node> expectedCommunity;
        for (size_t v = vBeg; v < vEnd; ++v) {
            expectedCommunity.insert(static_cast<node>(v));
        }

        return expectedCommunity;
    }

    void verifySingletons(ShellStruct &shell, const std::vector<count> &cliques) {
        count n = std::accumulate(cliques.begin(), cliques.end(), count(0));
        for (node v = 0; v < n; ++v) {
            std::set<node> query = {v};
            std::set<node> et = shell.expandOneCommunity(query);
            std::set<node> gt = gtSearch(cliques, query);

            EXPECT_EQ(et, gt) << "Failed on singleton query node: " << v;
        }
    }

    void verifyPairs(ShellStruct &shell, const std::vector<count> &cliques) {
        count n = std::accumulate(cliques.begin(), cliques.end(), count(0));
        for (node u = 0; u < n; ++u) {
            for (node v = u + 1; v < n; ++v) {
                std::set<node> query = {u, v};
                std::set<node> et = shell.expandOneCommunity(query);
                std::set<node> gt = gtSearch(cliques, query);

                EXPECT_EQ(et, gt) << "Failed on pair nodes: {" << u << ", " << v << "}";
            }
        }
    }
};

TEST_F(ShellStructGTest, SeparatedCores_Singleton) {
    std::vector<count> cliques{4, 3, 4};
    GraphW g = getCliqueChain(cliques);
    ShellStruct shell(g);
    shell.build();
    verifySingletons(shell, cliques);
}

TEST_F(ShellStructGTest, SeparatedCores_Pairs) {
    std::vector<count> cliques{4, 3, 4};
    GraphW g = getCliqueChain(cliques);
    ShellStruct shell(g);
    shell.build();
    verifyPairs(shell, cliques);
}

TEST_F(ShellStructGTest, BridgedCore_Singleton) {
    std::vector<count> cliques{3, 4, 3};
    GraphW g = getCliqueChain(cliques);
    ShellStruct shell(g);
    shell.build();
    verifySingletons(shell, cliques);
}

TEST_F(ShellStructGTest, BridgedCore_Pairs) {
    std::vector<count> cliques{3, 4, 3};
    GraphW g = getCliqueChain(cliques);
    ShellStruct shell(g);
    shell.build();
    verifyPairs(shell, cliques);
}

TEST_F(ShellStructGTest, Medium_Singleton) {
    std::vector<count> cliques{3, 5, 4, 3, 6, 5, 4, 6, 3};
    GraphW g = getCliqueChain(cliques);
    ShellStruct shell(g);
    shell.build();
    verifySingletons(shell, cliques);
}

TEST_F(ShellStructGTest, Medium_Pairs) {
    std::vector<count> cliques{3, 5, 4, 3, 6, 5, 4, 6, 3};
    GraphW g = getCliqueChain(cliques);
    ShellStruct shell(g);
    shell.build();
    verifyPairs(shell, cliques);
}

TEST_F(ShellStructGTest, Large_Singleton) {
    std::vector<count> cliques{3, 4, 5, 6, 5, 4, 3, 5, 7, 9, 11, 9, 7, 5, 3, 4, 5, 6, 4, 3};
    GraphW g = getCliqueChain(cliques);
    ShellStruct shell(g);
    shell.build();
    verifySingletons(shell, cliques);
}

TEST_F(ShellStructGTest, LargePairs) {
    std::vector<count> cliques{3, 4, 5, 6, 5, 4, 3, 5, 7, 9, 11, 9, 7, 5, 3, 4, 5, 6, 4, 3};
    GraphW g = getCliqueChain(cliques);
    ShellStruct shell(g);
    shell.build();
    verifyPairs(shell, cliques);
}

TEST_F(ShellStructGTest, LoadSave) {
    std::vector<count> cliques{3, 4, 5, 6, 5, 4, 3, 5, 7, 9, 11, 9, 7, 5, 3, 4, 5, 6, 4, 3};
    GraphW g = getCliqueChain(cliques);
    ShellStruct shell(g);
    shell.build();
    shell.save("output/shellstruct_comp.parquet", "output/shellstruct_tree.parquet");

    ShellStruct index(g);
    index.load("output/shellstruct_comp.parquet", "output/shellstruct_tree.parquet");
    verifyPairs(index, cliques);
}

} /* namespace NetworKit */
