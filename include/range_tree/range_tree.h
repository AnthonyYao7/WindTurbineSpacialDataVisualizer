//
// Created by anthony on 7/6/23.
//

#ifndef DSA_P3_RANGE_TREE_H
#define DSA_P3_RANGE_TREE_H

#include <vector>
#include <array>
#include <algorithm>
#include <queue>

#include "tree_utils.h"


template <typename T, typename V, u64 K, u64 P = K>
// istg dont manually set the value of P or i will punch u, let me do my clever templating
// T is the type of the point
// V is the type of the value
// K represents the dimensionality of the range tree
// P represents the dimensionality of the overall range tree this range tree may be an orthogonal tree of
// The use of P rather than just K is very handy in a lot of cases
class RangeTree
{
public:

    struct RTNode
    {
        RangeTree<T, V, K - 1, P>* orthogonal = nullptr; // the auxiliary tree for this node, contains all the descendents of this node but sorted by the next dimension
        RTNode* left = nullptr;
        RTNode* right = nullptr;

        std::array<T, P> point;

        T key;
        T st_min;
        T st_max;

        V val;

        explicit RTNode(T v) : key(v) {}
        RTNode(T k, const std::array<T, P>& p, V v) : key(k), point(p), val(v) {}
    };

private:
    RTNode* root;

public:
    RTNode* get_root()
    {
        return root;
    }

    RTNode* create_tree(typename std::vector<std::pair<std::array<T, P>, V>>::iterator begin, typename std::vector<std::pair<std::array<T, P>, V>>::iterator end)
    {
        if (begin == end)
            return nullptr;

        if (begin + 1 == end) // reached a leaf node
        {
            auto cur = new RTNode((*begin).first[P - K], begin->first, (*begin).second); // construct the leaf node
            cur->st_min = (*begin).first[P - K];
            cur->st_max = (*begin).first[P - K];

            auto orthogonal_tree = new RangeTree<T, V, K - 1, P>(begin, end); // make auxiliary
            cur->orthogonal = orthogonal_tree;

            return cur;
        }

        u64 length = end - begin;

        RTNode* subtree_root = nullptr, *left = nullptr, *right = nullptr;

        typename std::vector<std::pair<std::array<T, P>, V>>::iterator m = begin + length / 2; // the median
        if (length % 2 == 0)
            m--;

        subtree_root = new RTNode((*m).first[P - K], m->first, m->second); // constructs the current node with the median data
        subtree_root->st_min = (*begin).first[P - K];
        subtree_root->st_max = (*(end - 1)).first[P - K];

        left = create_tree(begin, m + 1); // create left and right children
        right = create_tree(m + 1, end);

        subtree_root->left = left;
        subtree_root->right = right;

        auto orthogonal_tree = new RangeTree<T, V, K - 1, P>(begin, end); // create auxiliary tree on the next dimension
        subtree_root->orthogonal = orthogonal_tree;

        return subtree_root;
    }

    RangeTree() = default;

    // ctor, calls create_tree
    RangeTree(typename std::vector<std::pair<std::array<T, P>, V>>::iterator begin, typename std::vector<std::pair<std::array<T, P>, V>>::iterator end)
    {
        if (begin == end)
            return;

        std::vector<std::pair<std::array<T, P>, V>> sorted_points(begin, end);
        // sort the points, only need to sort once per dimension
        std::sort(sorted_points.begin(), sorted_points.end(), [] (const std::pair<std::array<T, P>, V>& a, const std::pair<std::array<T, P>, V>& b) { return a.first[P - K] < b.first[P - K]; });

        root = create_tree(sorted_points.begin(), sorted_points.end());
    }

    
    std::vector<V> range_query(const std::array<std::array<T, 2>, P>& range) const
    {
        std::vector<V> ret;

        std::queue<RTNode*> q; // bfs
        q.push(root);

        while (not q.empty())
        {
            auto cur = q.front();
            q.pop();

            if (cur == nullptr)
                continue;

            // subtree is completely contained within the range, now recurse and perform range query on the next dimension
            if (cur->st_min > range[P - K][0] and cur->st_max < range[P - K][1])
            {
                auto results = cur->orthogonal->range_query(range);
                ret.insert(ret.end(), results.begin(), results.end());
            }
            else // subtree overlaps with the range, need to go further down the tree
            {
                if (range[P - K][0] < cur->st_max and range[P - K][0] > cur->st_min ||
                    range[P - K][1] < cur->st_max and range[P - K][1] > cur->st_min)
                {
                    q.push(cur->left);
                    q.push(cur->right);
                }
            }
        }

        return ret;
    }
};


// template specialization for the base case K = 1
template <typename T, typename V, u64 P>
class RangeTree<T, V, 1, P>
{
public:
    struct RTNode
    {
        RTNode* left = nullptr; // notice how it does not have an auxiliary tree
        RTNode* right = nullptr;

        std::array<T, P> point;

        T key;
        T st_min;
        T st_max;

        V val;

        explicit RTNode(T v) : key(v) {}
        RTNode(T k, const std::array<T, P>& p, V v) : key(k), point(p), val(v) {}
    };

private:
    RTNode* root;

public:
    RTNode* get_root()
    {
        return root;
    }

    // same stuff as the other create_tree method except it does not create an auxiliary tree since this is the last dimension
    RTNode* create_tree(typename std::vector<std::pair<std::array<T, P>, V>>::iterator begin, typename std::vector<std::pair<std::array<T, P>, V>>::iterator end)
    {
        if (begin == end)
            return nullptr;

        if (begin + 1 == end) // leaf node
        {
            auto cur = new RTNode((*begin).first[P - 1], begin->first, begin->second);
            cur->st_min = (*begin).first[P - 1];
            cur->st_max = (*begin).first[P - 1];
            return cur;
        }

        u64 length = end - begin;

        RTNode* subtree_root = nullptr, *left = nullptr, *right = nullptr;

        typename std::vector<std::pair<std::array<T, P>, V>>::iterator m = begin + length / 2; // median
        if (length % 2 == 0)
            m--;

        subtree_root = new RTNode((*m).first[P - 1], m->first, m->second);
        subtree_root->st_min = (*begin).first[P - 1];
        subtree_root->st_max = (*(end - 1)).first[P - 1];

        left = create_tree(begin, m + 1); // children, no auxiliary tree
        right = create_tree(m + 1, end);

        subtree_root->left = left;
        subtree_root->right = right;

        return subtree_root;
    }

    RangeTree() = default;

    // ctor
    RangeTree(typename std::vector<std::pair<std::array<T, P>, V>>::iterator begin, typename std::vector<std::pair<std::array<T, P>, V>>::iterator end)
    {
        if (begin == end)
            return;

        std::vector<std::pair<std::array<T, P>, V>> sorted_points(begin, end);
        std::sort(sorted_points.begin(), sorted_points.end(), [] (const std::pair<std::array<T, P>, V>& a, const std::pair<std::array<T, P>, V>& b) { return a.first[P - 1] < b.first[P - 1]; });

        root = create_tree(sorted_points.begin(), sorted_points.end());
    }

    std::vector<V> range_query(const std::array<std::array<T, 2>, P>& range) const
    {
        std::vector<V> ret;

        std::queue<RTNode*> q;
        q.push(root);

        while (not q.empty())
        {
            auto cur = q.front();
            q.pop();

            if (cur == nullptr)
                continue;

            // subtree is completely contained within the range, place all values in this subtree in the return array
            // this is different from the other definition of range query, since there is no auxiliary tree to recurse on here
            if (cur->st_min > range[P - 1][0] and cur->st_max < range[P - 1][1])
            {
                std::queue<RTNode*> cq;
                cq.push(cur);

                while (not cq.empty()) // quick bfs inside the bfs
                {
                    auto cur_node = cq.front();
                    cq.pop();

                    if (cur_node == nullptr)
                        continue;

                    if (cur_node->left == nullptr and cur_node->right == nullptr)
                        ret.push_back(cur_node->val);

                    cq.push(cur_node->left);
                    cq.push(cur_node->right);
                }
            }
            else // subtree overlaps with the range, need to go further down the tree
            {
                if (range[P - 1][0] < cur->st_max and range[P - 1][0] > cur->st_min ||
                    range[P - 1][1] < cur->st_max and range[P - 1][1] > cur->st_min)
                {
                    q.push(cur->left);
                    q.push(cur->right);
                }
            }
        }

        return ret;
    }
};

#endif //DSA_P3_RANGE_TREE_H
