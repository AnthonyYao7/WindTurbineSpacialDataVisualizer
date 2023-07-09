//
// Created by anthony on 7/6/23.
//

#ifndef DSA_P3_RANGE_TREE_H
#define DSA_P3_RANGE_TREE_H

#include <vector>
#include <array>
#include <algorithm>

#include "tree_utils.h"


template <typename T, u64 K>
class RangeTree
{
public:
    struct RTNode
    {
        RangeTree<T, K - 1>* orthogonal = nullptr;
        RTNode* left = nullptr;
        RTNode* right = nullptr;

        T val;

        RTNode(T v) : val(v) {}
    };

private:
    RTNode* root;

public:
    RTNode* create_tree(typename std::vector<std::vector<T>>::iterator begin, typename std::vector<std::vector<T>>::iterator end, u64 dim)
    {
        if (begin == end)
            return nullptr;

        if (begin + 1 == end)
        {
            auto cur = new RTNode((*begin)[dim]);

            if (K != 1) // this range tree has sub range trees
            {
                auto orthogonal_tree = new RangeTree<T, K - 1>(begin, end);
                cur->orthogonal = orthogonal_tree;
            }

            return cur;
        }

        u64 length = end - begin;

        RTNode* subtree_root = nullptr, *left = nullptr, *right = nullptr;


        if (length % 2 == 0)
        {
            auto f = begin + length / 2 - 1, s = begin + length / 2;
            subtree_root = new RTNode(((*f)[dim] + (*s)[dim]) / 2.);
            left = create_tree(begin, s, dim);
            right = create_tree(s, end, dim);
        }
        else
        {
            auto m = begin + length / 2;
            subtree_root = new RTNode((*m)[dim]);
            left = create_tree(begin, m, dim);
            right = create_tree(m + 1, end, dim);
        }

        subtree_root->left = left;
        subtree_root->right = right;

        if (K != 1)
        {
            auto orthogonal_tree = new RangeTree<T, K - 1>(begin, end);
            subtree_root->orthogonal = orthogonal_tree;
        }

        return subtree_root;
    }

    // the length of the inner vector must be constant
    RangeTree(typename std::vector<std::vector<T>>::iterator begin, typename std::vector<std::vector<T>>::iterator end)
    {
        if (begin == end)
            return;
        std::vector<std::vector<T>>& sorted_points(begin, end);
        std::sort(sorted_points.begin(), sorted_points.end(), [] (const std::vector<T>& a, const std::vector<T>& b) { return a[a.size() - K] < b[a.size() - K]; });

        root = create_tree(sorted_points.begin(), sorted_points.end(), sorted_points[0].size() - K);
    }
};


#endif //DSA_P3_RANGE_TREE_H
