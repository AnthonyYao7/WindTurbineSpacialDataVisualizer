//
// Created by antho on 7/4/2023.
//
#include <iostream>
#include <vector>
#include <random>
#include <array>
#include <memory>
#include <algorithm>

#include <gtest/gtest.h>
#include "k-d_tree.h"

#define NUM_ITERATIONS 1
#define TEST_POINTS 100000
#define TEST_DIMS 2
#define NUM_QUERIES 1

TEST(RangeQueryTest, KDTreeTests)
{
    std::mt19937 gen(1);

    for (int i = 0; i < NUM_ITERATIONS; i++)
    {
        std::unique_ptr<KD_Tree<double, TEST_DIMS>> tree;

        std::vector<std::array<double, TEST_DIMS>> randomVectors;
        randomVectors.reserve(TEST_POINTS);

        std::uniform_real_distribution<double> valueDistr(0.0, 1.0);
        for (int c = 0; c < TEST_POINTS; ++c) {
            std::array<double, TEST_DIMS> vec{};
            for (int j = 0; j < TEST_DIMS; ++j) {
                vec[j] = valueDistr(gen);
            }
            randomVectors.push_back(vec);
        }

        tree = std::make_unique<KD_Tree<double, TEST_DIMS>>(randomVectors);


        for (int q = 0; q < NUM_QUERIES; q++)
        {
            std::array<std::array<double, 2>, TEST_DIMS> query{};

            for (int d = 0; d < TEST_DIMS; d++)
            {
                double first = valueDistr(gen);
                double second = valueDistr(gen);
                double temp = first;
                first = std::min(first, second);
                second = std::max(temp, second);
                query[d] = {first, second};
            }

            auto q_results = tree->range_query(query);

            std::vector<std::array<double, TEST_DIMS>> brute_force_results;

            for (auto& point : randomVectors)
            {
                for (int d1 = 0; d1 < TEST_DIMS; d1++)
                {
                    if (not (point[d1] >= query[d1][0] and point[d1] <= query[d1][1]))
                        break;
                    if (d1 == (TEST_DIMS - 1))
                        brute_force_results.push_back(point);
                }
            }

            std::sort(brute_force_results.begin(), brute_force_results.end(), [] (std::array<double, TEST_DIMS>& a, std::array<double, TEST_DIMS>& b) { return a[0] < b[0]; });
            std::sort(q_results.begin(), q_results.end(), [] (std::array<double, TEST_DIMS>& a, std::array<double, TEST_DIMS>& b) { return a[0] < b[0]; });

            EXPECT_EQ(brute_force_results.size(), q_results.size());

            if (brute_force_results.size() != q_results.size()) continue;

            for (int r = 0; r < brute_force_results.size(); r++)
            {
                EXPECT_EQ(brute_force_results[r], q_results[r]);
            }

        }
    }
}
