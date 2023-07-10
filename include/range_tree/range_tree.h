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


template <typename T, u64 K, u64 P = K>
// istg dont manually set the value of P or i will punch u, let me do my clever templating
// K represents the dimensionality of the range tree
// P represents the dimensionality of the overall range tree this range tree may be an orthogonal tree of
// The use of P rather than just K is very handy in a lot of cases
class RangeTree
{
public:

    struct RTNode
    {
        RangeTree<T, K - 1, P>* orthogonal = nullptr;
        RTNode* left = nullptr;
        RTNode* right = nullptr;

        std::array<T, P> point;

        T val;
        T st_min;
        T st_max;

        explicit RTNode(T v) : val(v) {}
        RTNode(T v, const std::array<T, P>& p) : val(v), point(p) {}
    };

private:
    RTNode* root;

public:
    RTNode* get_root()
    {
        return root;
    }

    RTNode* create_tree(typename std::vector<std::array<T, P>>::iterator begin, typename std::vector<std::array<T, P>>::iterator end)
    {
        if (begin == end)
            return nullptr;

        if (begin + 1 == end)
        {
            auto cur = new RTNode((*begin)[P - K], *begin);
            cur->st_min = (*begin)[P - K];
            cur->st_max = (*begin)[P - K];

            auto orthogonal_tree = new RangeTree<T, K - 1, P>(begin, end);
            cur->orthogonal = orthogonal_tree;

            return cur;
        }

        u64 length = end - begin;

        RTNode* subtree_root = nullptr, *left = nullptr, *right = nullptr;

        typename std::vector<std::array<T, P>>::iterator m = begin + length / 2;
        if (length % 2 == 0)
            m--;

        subtree_root = new RTNode((*m)[P - K], *m);
        subtree_root->st_min = (*begin)[P - K];
        subtree_root->st_max = (*(end - 1))[P - K];

        left = create_tree(begin, m + 1);
        right = create_tree(m + 1, end);

        subtree_root->left = left;
        subtree_root->right = right;

        auto orthogonal_tree = new RangeTree<T, K - 1, P>(begin, end);
        subtree_root->orthogonal = orthogonal_tree;

        return subtree_root;
    }

    RangeTree(typename std::vector<std::array<T, P>>::iterator begin, typename std::vector<std::array<T, P>>::iterator end)
    {
        if (begin == end)
            return;

        std::vector<std::array<T, P>> sorted_points(begin, end);
        std::sort(sorted_points.begin(), sorted_points.end(), [] (const std::array<T, P>& a, const std::array<T, P>& b) { return a[P - K] < b[P - K]; });

        root = create_tree(sorted_points.begin(), sorted_points.end());
    }

    
    std::vector<std::array<T, P>> range_query(std::array<std::array<T, 2>, P>& range)
    {
        std::vector<std::array<T, P>> ret;

        std::queue<RTNode*> q;
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
template <typename T, u64 P>
class RangeTree<T, 1, P>
{
public:
    struct RTNode
    {
        RTNode* left = nullptr;
        RTNode* right = nullptr;

        std::array<T, P> point;

        T val;
        T st_min;
        T st_max;

        explicit RTNode(T v) : val(v) {}
        RTNode(T v, const std::array<T, P>& p) : val(v), point(p) {}
    };

private:
    RTNode* root;

public:
    RTNode* get_root()
    {
        return root;
    }

    RTNode* create_tree(typename std::vector<std::array<T, P>>::iterator begin, typename std::vector<std::array<T, P>>::iterator end)
    {
        if (begin == end)
            return nullptr;

        if (begin + 1 == end)
        {
            auto cur = new RTNode((*begin)[P - 1], *begin);
            cur->st_min = (*begin)[P - 1];
            cur->st_max = (*begin)[P - 1];

            return cur;
        }

        u64 length = end - begin;

        RTNode* subtree_root = nullptr, *left = nullptr, *right = nullptr;

        typename std::vector<std::array<T, P>>::iterator m = begin + length / 2;
        if (length % 2 == 0)
            m--;

        subtree_root = new RTNode((*m)[P - 1], *m);
        subtree_root->st_min = (*begin)[P - 1];
        subtree_root->st_max = (*(end - 1))[P - 1];

        left = create_tree(begin, m + 1);
        right = create_tree(m + 1, end);

        subtree_root->left = left;
        subtree_root->right = right;

        return subtree_root;
    }

    RangeTree(typename std::vector<std::array<T, P>>::iterator begin, typename std::vector<std::array<T, P>>::iterator end)
    {
        if (begin == end)
            return;

        std::vector<std::array<T, P>> sorted_points(begin, end);
        std::sort(sorted_points.begin(), sorted_points.end(), [] (const std::array<T, P>& a, const std::array<T, P>& b) { return a[P - 1] < b[P - 1]; });

        root = create_tree(sorted_points.begin(), sorted_points.end());
    }

    std::vector<std::array<T, P>> range_query(std::array<std::array<T, 2>, P>& range)
    {
        std::vector<std::array<T, P>> ret;

        std::queue<RTNode*> q;
        q.push(root);

        while (not q.empty())
        {
            auto cur = q.front();
            q.pop();

            if (cur == nullptr)
                continue;

            // subtree is completely contained within the range, now recurse and perform range query on the next dimension
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
                        ret.push_back(cur_node->point);

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



//template <typename T, u64 K>
//class RangeTree
//{
//public:
//    struct RTNode
//    {
//        RangeTree<T, K - 1>* orthogonal = nullptr;
//        RTNode* left = nullptr;
//        RTNode* right = nullptr;
//
//        std::array<T, K> point;
//        T val;
//
//        RTNode(T v) : val(v) {}
//    };
//
//private:
//    RTNode* root;
//
//public:
//    RTNode* create_tree(typename std::vector<std::array<T, P>>::iterator begin, typename std::vector<std::array<T, P>>::iterator end, u64 dim)
//    {
//        if (begin == end)
//            return nullptr;
//
//        if (begin + 1 == end)
//        {
//            auto cur = new RTNode((*begin)[dim]);
//
//            if (K != 1) // this range tree has sub range trees
//            {
//                auto orthogonal_tree = new RangeTree<T, K - 1>(begin, end);
//                cur->orthogonal = orthogonal_tree;
//            }
//
//            return cur;
//        }
//
//        u64 length = end - begin;
//
//        RTNode* subtree_root = nullptr, *left = nullptr, *right = nullptr;
//
//
//        if (length % 2 == 0)
//        {
//            auto f = begin + length / 2 - 1, s = begin + length / 2;
//            subtree_root = new RTNode(((*f)[dim] + (*s)[dim]) / 2.);
//            left = create_tree(begin, s, dim);
//            right = create_tree(s, end, dim);
//        }
//        else
//        {
//            auto m = begin + length / 2;
//            subtree_root = new RTNode((*m)[dim]);
//            left = create_tree(begin, m, dim);
//            right = create_tree(m + 1, end, dim);
//        }
//
//        subtree_root->left = left;
//        subtree_root->right = right;
//
//        if (K != 1)
//        {
//            auto orthogonal_tree = new RangeTree<T, K - 1>(begin, end);
//            subtree_root->orthogonal = orthogonal_tree;
//        }
//
//        return subtree_root;
//    }
//
//    // the length of the inner vector must be constant
//    RangeTree(typename std::vector<std::array<T, P>>::iterator begin, typename std::vector<std::array<T, P>>::iterator end)
//    {
//        if (begin == end)
//            return;
//        std::vector<std::array<T, P>>& sorted_points(begin, end);
//        std::sort(sorted_points.begin(), sorted_points.end(), [] (const std::array<T, P>& a, const std::array<T, P>& b) { return a[a.size() - K] < b[a.size() - K]; });
//
//        root = create_tree(sorted_points.begin(), sorted_points.end(), sorted_points[0].size() - K);
//    }
//
//};


#endif //DSA_P3_RANGE_TREE_H
