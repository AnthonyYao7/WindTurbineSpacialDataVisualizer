#include <iostream>
#include <vector>
#include <array>

#include "k-d_tree/k-d_tree.h"

using namespace std;
#define COUNT 10

typedef KD_Tree<double, 2>::KDTNode Node;

void print2DUtil(Node* root, int space)
{
    // Base case
    if (root == NULL)
        return;

    // Increase distance between levels
    space += COUNT;

    // Process right child first
    print2DUtil(root->right, space);

    // Print current node after space
    // count
    cout << endl;
    for (int i = COUNT; i < space; i++)
        cout << " ";
    cout << (root->contains_point ? to_string(root->point[0]) + " " + to_string(root->point[1]) : "") << " " << root->plane << " " << root->dim << "\n";

    // Process left child
    print2DUtil(root->left, space);
}

// Wrapper over print2DUtil()
void print2D(Node* root)
{
    // Pass initial space count as 0
    print2DUtil(root, 0);
}

int main()
{
    vector<array<double, 2>> points = {{ 0.7726914985597692 , 0.7755390708672442 },//
                                       { 0.7455338323568447 , 0.32162748467685165 },//
                                       { 0.8090022694940407 , 0.09814634301417269 },//
                                       { 0.962155906628329 , 0.3903571327081561 },//
                                       { 0.09791475835241492 , 0.16639053361257072 },
                                       { 0.43184344891685766 , 0.06854055267925951 },//
                                       { 0.9549559743718948 , 0.24678514345444025 },
                                       { 0.7176965024161037 , 0.49169206488839223 },//
                                       { 0.2392238876896211 , 0.8041312208024288 },//
                                       { 0.4178457900456979 , 0.6721843663643722 },};//

    KD_Tree<double, 2> tree(points);

    print2D(tree.get_root());
    cout << endl;
    array<array<double, 2>, 2> query = {{{0., 1.}, {0., 1.}}};
    vector<array<double, 2>> queried = tree.range_query(query);

    for (auto & i : queried)
    {
        cout << i[0] << " " << i[1] << endl;
    }

    return 0;
}
