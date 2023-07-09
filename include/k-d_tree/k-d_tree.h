//
// Created by antho on 7/4/2023.
//

#ifndef DSA_P3_K_D_TREE_H
#define DSA_P3_K_D_TREE_H

#include <array>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <stack>

#include "tree_utils.h"


// implementation of k-d tree without support for insertion and deletion, i.e., only tree creation from some points and searching
template <typename T, u64 K>
class KD_Tree
{
public:

    struct KDTNode
    {
        std::array<T, K> point;
        bool contains_point;
        u64 dim{};
        T plane;
        KDTNode* left;
        KDTNode* right;

        KDTNode(const std::array<T, K>& p) : point(p), left(nullptr), right(nullptr), contains_point(true) {}
        KDTNode(u64 d, T pl) : dim(d) , plane(pl), contains_point(false) {}
        KDTNode(const std::array<T, K>& p, u64 d, T pl) : point(p), dim(d), plane(pl), left(nullptr), right(nullptr), contains_point(true) {}
    };

private:

    KDTNode* root = nullptr;

public:

    KDTNode* create_tree(
            typename std::vector<std::array<T, K>>::iterator start,
            typename std::vector<std::array<T, K>>::iterator stop,
            u64 dim)
    {
        if (start == stop)
            return nullptr;

        if (start + 1 == stop)
        {
            return new KDTNode(*start, dim, (*start)[dim]);
        }

        // we sort to find the median and to avoid having to copy elements around
        sort(start, stop, [&](const std::array<T, K> &a, const std::array<T, K> &b) { return a[dim] < b[dim]; });

        u64 length = stop - start;
        KDTNode* subtree_root = nullptr, *left = nullptr, *right = nullptr;

        if (length % 2 == 0)
        {
            auto f = start + length / 2 - 1, s = start + length / 2;
            subtree_root = new KDTNode(dim, ((*f)[dim] + (*s)[dim]) / 2.);
            left = create_tree(start, s, (dim + 1) % K);
            right = create_tree(s, stop, (dim + 1) % K);
        }
        else
        {
            auto m = start + length / 2;
            subtree_root = new KDTNode(*m, dim, (*m)[dim]);
            left = create_tree(start, m, (dim + 1) % K);
            right = create_tree(m + 1, stop, (dim + 1) % K);
        }

        subtree_root->left = left;
        subtree_root->right = right;
        return subtree_root;
    }

    KD_Tree(const std::vector<std::array<T, K>>& points)
    {
        std::vector<std::array<T, K>> a = points;
        root = create_tree(a.begin(), a.end(), 0);
    }

    void destructor_helper(KDTNode* node)
    {
        if (node == nullptr) return;

        destructor_helper(node->left);
        destructor_helper(node->right);
        delete node;
    }

    ~KD_Tree()
    {
        destructor_helper(root);
    }

    std::vector<std::array<T, K>> range_query(std::array<std::array<T, 2>, K>& range)
    {
        std::vector<std::array<T, K>> ret;
        std::stack<KDTNode*> cs;
        cs.push(root);
        KDTNode* cur;
        u64 cur_dim;

        while (!cs.empty())
        {
            cur = cs.top();
            cs.pop();

            if (cur == nullptr)
                continue;

            cur_dim = cur->dim;

            if (cur->plane <= range[cur_dim][1])
            {
                cs.push(cur->right);
            }

            if (cur->plane >= range[cur_dim][0])
            {
                cs.push(cur->left);
            }

            if (cur->contains_point)
            {
                for (int d = 0; d < K; d++)
                {
                    if (not (cur->point[d] >= range[d][0] and cur->point[d] <= range[d][1]))
                        break;
                    if (d == (K - 1))
                        ret.push_back(cur->point);
                }
            }
//            if (cur->contains_point and (cur->point[cur_dim] >= range[cur_dim][0] and cur->point[cur_dim] <= range[cur_dim][1]))
//            {
//                ret.push_back(cur->point);
//            }
        }

        return ret;
    }

    KDTNode* get_root() { return root; }
};


#endif //DSA_P3_K_D_TREE_H
