#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int getHeight(Node* currNode) {
    if (currNode == nullptr) {
        return 0;
    }
    int leftHeight = getHeight(currNode->left);
    int rightHeight = getHeight(currNode->right);

    if (leftHeight >= rightHeight) {
        return leftHeight + 1;
    }
    if (rightHeight >= leftHeight) {
        return rightHeight + 1;
    }
    return 0;
}


bool equalPaths(Node* root)
{
    // Add your code below
    if (root == nullptr) { //Base case
        return true;
    }
    if (root->right == nullptr) {
        if (root->left != nullptr && getHeight(root->left) == 1) {
            return true;
        }
    }
    if (root->left == nullptr) {
        if (root->right != nullptr && getHeight(root->right) == 1) {
            return true;
        }
    }

    if (getHeight(root->left) == getHeight(root->right)) {
        return true;
    }
    else {
        return false;
    }
}

