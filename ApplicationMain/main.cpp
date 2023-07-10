#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include "range_tree/range_tree.h"

using namespace std;
#define COUNT 10


template <u64 K, u64 P>
void print2DUtil(typename RangeTree<double, K, P>::RTNode* root, int space)
{
    if (root == NULL)
        return;

    space += COUNT;

    print2DUtil<K, P>(root->right, space);

    cout << endl;
    for (int i = COUNT; i < space; i++)
        cout << " ";

    cout << root->point[0] << " " << root->point[1] << endl;

    print2DUtil<K, P>(root->left, space);
}

template <u64 K, u64 P>
void print2D(typename RangeTree<double, K, P>::RTNode* root)
{
    print2DUtil<K, P>(root, 0);
}

int main()
{
    vector<array<double, 2>> points = {{0.2, 0.2}, {0.8, 0.4}, {0.5, 0.5}, {0.2, 0.4}, {0.2, 0.8}};
    array<array<double, 2>, 2> query = {{{0., 1.}, {0.3, 0.6}}};

    auto tree = make_unique<RangeTree<double, 2>>(points.begin(), points.end());

    print2D<2, 2>(tree->get_root());

    queue<RangeTree<double, 2>::RTNode*> q;
    q.push(tree->get_root());

    while (not q.empty())
    {
        auto cur = q.front();
        q.pop();

        if (cur == nullptr)
            continue;

        cout << cur->point[0] << " " << cur->point[1] << "sugma" << endl;
        print2D<1, 2>(cur->orthogonal->get_root());

        q.push(cur->left);
        q.push(cur->right);
    }

    auto results = tree->range_query(query);

    cout << "Num results: " << results.size() << endl;



    return 0;
}
