#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance() const;
    void setBalance(int8_t balance);
    void updateBalance(int8_t diff);
    int getHeight() const;
    void setHeight(int height);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
    int height_;
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent) :
    Node<Key, Value>(key, value, parent), balance_(0), height_(1)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}
template<class Key, class Value>
int AVLNode<Key, Value>::getHeight() const
{
    if (this == NULL) {
        return 0;
    }
    return height_;
}
template<class Key, class Value>
void AVLNode<Key, Value>::setHeight(int height)
{
    height_ = height;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert(const std::pair<const Key, Value>& new_item); // TODO
    virtual void remove(const Key& key);  // TODO
    bool isBalancedTree();
protected:
    virtual void nodeSwap(AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2);

    // Add helper functions here
    void updateParentHeights(AVLNode<Key, Value>* currNode);
    void subtractParentHeights(AVLNode<Key, Value>* currNode);
    bool isBalancedNode(AVLNode<Key, Value>* currNode);
    void rotateRight(AVLNode<Key, Value>* currNode);
    void rotateLeft(AVLNode<Key, Value>* currNode);
    void setBalance(AVLNode<Key, Value>* currNode);
    AVLNode<Key, Value>* getFurthestDown(AVLNode<Key, Value>* currNode);

};

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item)
{
    // TODO
    AVLNode<Key, Value>* toInsert = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    if (this->root_ == nullptr) {
        this->root_ = toInsert;
        toInsert->setHeight(1);
        return;
    }
    AVLNode<Key, Value>* currNode = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    while (currNode != nullptr) {
        parent = currNode;
        if (new_item.first == currNode->getKey()) { //Check for repeats
            currNode->setValue(new_item.second);
            delete toInsert;
            return;
        }
        if (new_item.first < currNode->getKey()) { //Find out where the node goes.
            currNode = static_cast<AVLNode<Key, Value>*>(currNode->getLeft());
        }
        else {
            currNode = static_cast<AVLNode<Key, Value>*>(currNode->getRight());
        }
    }
    if (new_item.first < parent->getKey()) {
        parent->setLeft(toInsert);
    }
    else {
        parent->setRight(toInsert);
    }
    toInsert->setParent(parent);
    toInsert->setHeight(1);
    //Setting heights of parent nodes + balance if needs be
    updateParentHeights(parent);
    setBalance(toInsert);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* toRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (toRemove == nullptr) {
        return;
    }
    AVLNode<Key, Value>* parent = toRemove->getParent();

    if (toRemove->getLeft() != nullptr && toRemove->getRight() != nullptr) { //Case for two children
        AVLNode<Key, Value>* currPredecessor = static_cast<AVLNode<Key, Value>*>(this->predecessor(toRemove));
        if (currPredecessor != nullptr) {
            this->nodeSwap(toRemove, currPredecessor);
        }
    }
    //Everything after accounts for one child, now that we accounted for two children
    AVLNode<Key, Value>* child = nullptr;

    if (toRemove->getLeft() != nullptr) {
        child = static_cast<AVLNode<Key, Value>*>(toRemove->getLeft());
    }
    else {
        child = static_cast<AVLNode<Key, Value>*>(toRemove->getRight());
    }

    if (child != nullptr) {
        child->setParent(toRemove->getParent());
        if (toRemove->getParent() != nullptr) {
            if (toRemove == toRemove->getParent()->getLeft()) {
                toRemove->getParent()->setLeft(child);
            }
            else {
                toRemove->getParent()->setRight(child);
            }
        }
        else {
            this->root_ = child;
        }
    }
    else {
        if (toRemove->getParent() != nullptr) {
            if (toRemove == toRemove->getParent()->getLeft()) {
                parent = toRemove->getParent();
                toRemove->getParent()->setLeft(nullptr);
            }
            else {
                parent = toRemove->getParent();
                toRemove->getParent()->setRight(nullptr);
            }
        }
        else {
            this->root_ = nullptr;
        }
    }
    delete toRemove;
    if (parent != nullptr) { //Sets balance from the bottom node no matter what
        setBalance(parent);
        if (this->root_ == nullptr) {
            return;
        }
        //setBalance(static_cast<AVLNode<Key, Value>*>(this->root_));
        AVLNode<Key, Value>* furthestLeft;
        AVLNode<Key, Value>* furthestRight;
        if (static_cast<AVLNode<Key, Value>*>(this->root_)->getLeft() != nullptr) {
            furthestLeft = getFurthestDown(static_cast<AVLNode<Key, Value>*>(this->root_)->getLeft());
        }
        else {
            furthestLeft = static_cast<AVLNode<Key, Value>*>(this->root_);
        }
        if (static_cast<AVLNode<Key, Value>*>(this->root_)->getRight() != nullptr) {
            furthestRight = getFurthestDown(static_cast<AVLNode<Key, Value>*>(this->root_)->getRight());
        }
        else {
            furthestRight = static_cast<AVLNode<Key, Value>*>(this->root_);
        }
        setBalance(furthestLeft);
        setBalance(furthestRight);
    }
    else {
        if (this->root_ == nullptr) {
            return;
        }
        //setBalance(static_cast<AVLNode<Key, Value>*>(this->root_));
        AVLNode<Key, Value>* furthestLeft;
        AVLNode<Key, Value>* furthestRight;
        if (static_cast<AVLNode<Key, Value>*>(this->root_)->getLeft() != nullptr) {
            furthestLeft = getFurthestDown(static_cast<AVLNode<Key, Value>*>(this->root_)->getLeft());
        }
        else {
            furthestLeft = static_cast<AVLNode<Key, Value>*>(this->root_);
        }
        if (static_cast<AVLNode<Key, Value>*>(this->root_)->getRight() != nullptr) {
            furthestRight = getFurthestDown(static_cast<AVLNode<Key, Value>*>(this->root_)->getRight());
        }
        else {
            furthestRight = static_cast<AVLNode<Key, Value>*>(this->root_);
        }
        setBalance(furthestLeft);
        setBalance(furthestRight);
    }
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::getFurthestDown(AVLNode<Key, Value>* currNode) {
    if (currNode == nullptr) {
        return nullptr;
    }
    if (currNode->getLeft() == nullptr && currNode->getRight() == nullptr) {
        return currNode;
    }
    int leftHeight = currNode->getLeft()->getHeight();
    int rightHeight = currNode->getRight()->getHeight();
    if (leftHeight >= rightHeight) {
        return getFurthestDown(currNode->getLeft());
    }
    else {
        return getFurthestDown(currNode->getRight());
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* currNode) {
    AVLNode<Key, Value>* leftChild = currNode->getLeft();
    AVLNode<Key, Value>* parent = currNode->getParent();

    if (leftChild == nullptr) {
        return;
    }
    currNode->setLeft(leftChild->getRight());
    if (leftChild->getRight() != nullptr) {
        leftChild->getRight()->setParent(currNode);
    }
    leftChild->setRight(currNode);
    currNode->setParent(leftChild);
    if (parent != nullptr) {
        if (parent->getLeft() == currNode) {
            parent->setLeft(leftChild);
        }
        else {
            parent->setRight(leftChild);
        }
    }
    else {
        this->root_ = leftChild;
    }
    leftChild->setParent(parent);

    int currLeftChildHeight = currNode->getLeft()->getHeight();
    int currRightChildHeight = currNode->getRight()->getHeight();

    if (currLeftChildHeight >= currRightChildHeight) {
        currNode->setHeight(currNode->getLeft()->getHeight() + 1);
    }
    else {
        currNode->setHeight(currNode->getRight()->getHeight() + 1);
    }

    int leftChildLeftHeight = leftChild->getLeft()->getHeight();
    int leftChildRightHeight = leftChild->getRight()->getHeight();
    if (leftChildLeftHeight >= leftChildRightHeight) {
        leftChild->setHeight(leftChild->getLeft()->getHeight() + 1);
    }
    else {
        leftChild->setHeight(leftChild->getRight()->getHeight() + 1);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* currNode) {
    AVLNode<Key, Value>* rightChild = currNode->getRight();
    AVLNode<Key, Value>* parent = currNode->getParent();

    if (rightChild == nullptr) {
        return;
    }
    currNode->setRight(rightChild->getLeft());
    if (rightChild->getLeft() != nullptr) {
        rightChild->getLeft()->setParent(currNode);
    }
    rightChild->setLeft(currNode);
    currNode->setParent(rightChild);
    if (parent != nullptr) {
        if (parent->getLeft() == currNode) {
            parent->setLeft(rightChild);
        }
        else {
            parent->setRight(rightChild);
        }
    }
    else {
        this->root_ = rightChild;
    }
    rightChild->setParent(parent);

    int currLeftChildHeight = currNode->getLeft()->getHeight();
    int currRightChildHeight = currNode->getRight()->getHeight();

    if (currLeftChildHeight >= currRightChildHeight) {
        currNode->setHeight(currNode->getLeft()->getHeight() + 1);
    }
    else {
        currNode->setHeight(currNode->getRight()->getHeight() + 1);
    }

    int rightChildLeftHeight = rightChild->getLeft()->getHeight();
    int rightChildRightHeight = rightChild->getRight()->getHeight();
    if (rightChildLeftHeight >= rightChildRightHeight) {
        rightChild->setHeight(rightChild->getLeft()->getHeight() + 1);
    }
    else {
        rightChild->setHeight(rightChild->getRight()->getHeight() + 1);
    }
}
template<class Key, class Value>
void AVLTree<Key, Value>::setBalance(AVLNode<Key, Value>* currNode) { //Sets balance from node all the way to parent
    while (currNode != nullptr) {
        int balanceFactor = (currNode->getLeft()->getHeight()) - (currNode->getRight()->getHeight()); //Find out which way it's unbalanced.
        if (balanceFactor > 1) {
            if (currNode->getLeft()->getLeft()->getHeight() >= currNode->getLeft()->getRight()->getHeight()) {
                rotateRight(currNode);
            }
            else {
                rotateLeft(currNode->getLeft());
                rotateRight(currNode);
            }
        }
        else if (balanceFactor < -1) {
            if (currNode->getRight()->getRight()->getHeight() >= currNode->getRight()->getLeft()->getHeight()) {
                rotateLeft(currNode);
            }
            else {
                rotateRight(currNode->getRight());
                rotateLeft(currNode);
            }
        }
        int leftHeightOfCurr = currNode->getLeft()->getHeight();
        int rightHeightOfCurr = currNode->getRight()->getHeight();

        if (leftHeightOfCurr >= rightHeightOfCurr) {
            currNode->setHeight(1 + leftHeightOfCurr);
        }
        else {
            currNode->setHeight(1 + rightHeightOfCurr);
        }

        currNode = currNode->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateParentHeights(AVLNode<Key, Value>* currNode) { //Update all parents' heights to account for insert
    while (currNode != nullptr) {
        int leftHeight = 0;
        if (currNode->getLeft() != nullptr) {
            leftHeight = currNode->getLeft()->getHeight();
        }
        int rightHeight = 0;
        if (currNode->getRight() != nullptr) {
            rightHeight = currNode->getRight()->getHeight();
        }
        if (leftHeight >= rightHeight) {
            currNode->setHeight(1 + leftHeight);
        }
        if (rightHeight > leftHeight) {
            currNode->setHeight(1 + rightHeight);
        }
        currNode = currNode->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::subtractParentHeights(AVLNode<Key, Value>* currNode) { //Updates all nodes heights to account for any deletion
    while (currNode != nullptr) {
        int leftHeight = 0;
        int rightHeight = 0;

        AVLNode<Key, Value>* leftChild = currNode->getLeft();
        AVLNode<Key, Value>* rightChild = currNode->getRight();
        if (leftChild != nullptr) {
            leftHeight = leftChild->getHeight();
        }

        if (rightChild != nullptr) {
            rightHeight = rightChild->getHeight();
        }
        int newHeight = 0;
        if (leftHeight >= rightHeight) {
            newHeight = 1 + (leftHeight - rightHeight);
        }
        else {
            newHeight = 1 + (rightHeight - leftHeight);
        }

        if (currNode->getHeight() == newHeight) {
            break;
        }
        currNode->setHeight(newHeight);
        currNode = currNode->getParent();
    }
}
template<class Key, class Value>
bool AVLTree<Key, Value>::isBalancedNode(AVLNode<Key, Value>* currNode) {
    if (currNode == nullptr) {
        return true;
    }
    int leftHeight = 0;
    int rightHeight = 0;
    if (currNode->getLeft() != nullptr) {
        leftHeight = currNode->getLeft()->getHeight();
    }
    if (currNode->getRight() != nullptr) {
        rightHeight = currNode->getRight()->getHeight();
    }
    int difference = 0;
    if (leftHeight >= rightHeight) {
        difference = leftHeight - rightHeight;
    }
    else {
        difference = rightHeight - leftHeight;
    }
    if (difference > 1) {
        return false;
    }
    bool isLeftBalanced = isBalancedNode(currNode->getLeft());
    bool isRightBalanced = isBalancedNode(currNode->getRight());
    if (isLeftBalanced == true && isRightBalanced == true) {
        return true;
    }
    else {
        return false;
    }
}

template<class Key, class Value>
bool AVLTree<Key, Value>::isBalancedTree() {
    AVLNode<Key, Value>* rootAVL = static_cast<AVLNode<Key, Value>*>(this->root_);
    bool isTopBalanced = isBalancedNode(rootAVL);
    if (isTopBalanced == true) {
        return true;
    }
    else {
        return false;
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int tempH = n1->getHeight();
    n1->setHeight(n2->getHeight());
    n2->setHeight(tempH);
}


#endif
