#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the height as a data member, plus
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
    int getHeight () const;
    void setHeight (int height);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int height_;
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), height_(1)
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
* A getter for the height of a AVLNode.
*/
template<class Key, class Value>
int AVLNode<Key, Value>::getHeight() const
{
    return height_;
}

/**
* A setter for the height of a AVLNode.
*/
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
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
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
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void singleLeft(AVLNode<Key, Value>* z, AVLNode<Key, Value>* y, AVLNode<Key, Value>* x);
    void singleRight(AVLNode<Key, Value>* z, AVLNode<Key, Value>* y, AVLNode<Key, Value>* x);
    void doubleLeft(AVLNode<Key, Value>* z, AVLNode<Key, Value>* y, AVLNode<Key, Value>* x);
    void doubleRight(AVLNode<Key, Value>* z, AVLNode<Key, Value>* y, AVLNode<Key, Value>* x);
    void insertFix(AVLNode<Key, Value>* child, AVLNode<Key, Value>* parent);
    void removeFix(AVLNode<Key, Value>* node);
    void rotate(AVLNode<Key, Value>* z, AVLNode<Key, Value>* y, AVLNode<Key, Value>* x);
};

template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    //Check if the key already exists
    Node<Key, Value>* n = BinarySearchTree<Key, Value>::internalFind(new_item.first);
    bool alreadyExists = false;
    if (n != nullptr) alreadyExists = true;

    //If the key already existed, BST Insert will override its value, and we are done
    BinarySearchTree<Key, Value>::insert(new_item);
    if (alreadyExists == true) return;

    //BST Insert creates a non-AVL node, so replace the node created with an equivelent AVL Node
    n = BinarySearchTree<Key, Value>::internalFind(new_item.first);
    AVLNode<Key, Value>* avl_n = new AVLNode<Key, Value>(new_item.first, new_item.second, static_cast<AVLNode<Key, Value>*>(n->getParent()));
    avl_n->setLeft(n->getLeft());
    avl_n->setRight(n->getRight());

    //Place this AVL node in the correct spot in the tree
    if (n->getLeft() != nullptr) n->getLeft()->setParent(avl_n);
    if (n->getRight() != nullptr) n->getRight()->setParent(avl_n);
    if (n->getParent() != nullptr && n->getParent()->getLeft() == n) n->getParent()->setLeft(avl_n);
    else if (n->getParent() != nullptr) n->getParent()->setRight(avl_n);

    if (BinarySearchTree<Key,Value>::root_ == n) BinarySearchTree<Key,Value>::root_ = avl_n;
    delete n;   //Delete the non-AVL Node

    //Check if the parent's height changed after the insertion
    if (avl_n->getParent() == nullptr) return;
    int oldParentHeight = avl_n->getParent()->getHeight();

    int parentLeftHeight = 0;
    int parentRightHeight = 0;
    if (avl_n->getParent()->getLeft() != nullptr) parentLeftHeight = avl_n->getParent()->getLeft()->getHeight();
    if (avl_n->getParent()->getRight() != nullptr) parentRightHeight = avl_n->getParent()->getRight()->getHeight();

    int newParentHeight = std::max(parentLeftHeight, parentRightHeight) + 1;
    if (newParentHeight == oldParentHeight) return;

    //If the parent's height changed, call insertFix
    avl_n->getParent()->setHeight(newParentHeight);
    insertFix(avl_n, avl_n->getParent());
}

template<class Key, class Value>
void AVLTree<Key,Value>::insertFix(AVLNode<Key, Value>* child, AVLNode<Key, Value>* parent)
{
    if (parent == nullptr || parent->getParent() == nullptr) return;
    AVLNode<Key, Value>* grandparent = parent->getParent();

    //Check if grandparent's height changed
    int oldGPHeight = grandparent->getHeight();
    int gpLeftHeight = 0;
    if (grandparent->getLeft() != nullptr) gpLeftHeight = grandparent->getLeft()->getHeight();
    int gpRightHeight = 0;
    if (grandparent->getRight() != nullptr) gpRightHeight = grandparent->getRight()->getHeight();

    int newGPHeight = std::max(gpLeftHeight, gpRightHeight) + 1;
    if (oldGPHeight == newGPHeight) return;
    grandparent->setHeight(newGPHeight);    //Update grandparent's height

    //If the grandparent is balanced, go up in the tree
    bool grandparentBalanced = (abs(gpLeftHeight - gpRightHeight) <= 1);
    if (grandparentBalanced) insertFix(parent, grandparent);

    //If grandparent is not balanced, perform the appropriate rotation
    else{
        AVLNode<Key,Value>* z = grandparent;
        AVLNode<Key, Value>* y = nullptr;
        AVLNode<Key, Value>* x = nullptr;
        rotate(z,y,x);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotate(AVLNode<Key, Value>* z, AVLNode<Key, Value>* y, AVLNode<Key, Value>* x)
{
    //Get the height of z's children
    int zLeftHeight = 0;
    int zRightHeight = 0;
    if (z->getLeft() != nullptr) zLeftHeight = z->getLeft()->getHeight();
    if (z->getRight() != nullptr) zRightHeight = z->getRight()->getHeight();

    //y is z's child with greater height
    bool y_is_left = true;
    if (zLeftHeight > zRightHeight) y = z->getLeft();
    else {
        y = z->getRight();
        y_is_left = false;
    }

    //Get the height of y's children
    int y_left = 0;
    int y_right = 0;
    if (y == nullptr) return;
    if (y->getLeft() != nullptr) y_left = y->getLeft()->getHeight();
    if (y->getRight() != nullptr) y_right = y->getRight()->getHeight();

    //Break ties with a single rotation
    bool singleRotation = true;
    if (y_left == y_right){
        if (y_is_left == true) x = y->getLeft();
        else x = y->getRight();
    }

    //Choose child with greater height and determine if zig-zig or zig-zag
    else if (y_left > y_right){
        x = y->getLeft();
        if (y_is_left == false) singleRotation = false;
    }
    else{
        x = y->getRight();
        if (y_is_left == true) singleRotation = false;
    }
    
    //Perform the appropriate rotation
    if (singleRotation && y_is_left) singleRight(z, y, x);  //Zig-zig right
    else if (singleRotation) singleLeft(z, y, x);   //Zig-zig left
    else if (y_is_left) doubleRight(z, y, x);   //Zig-zag right
    else doubleLeft(z, y, x);   //Zig-zag left
}

template<class Key, class Value>
void AVLTree<Key, Value>::singleLeft(AVLNode<Key, Value>* z, AVLNode<Key, Value>* y, AVLNode<Key, Value>* x)
{
    //z's parent should have y as its child
    if (BinarySearchTree<Key,Value>::root_ == z) BinarySearchTree<Key,Value>::root_ = y;
    AVLNode<Key, Value>* z_parent = z->getParent();
    if (z_parent != nullptr){
        if (z_parent->getLeft() == z) z_parent->setLeft(y);
        else z_parent->setRight(y);
    }

    //y will adpot z as its child and z will adopt y's orphan if y has a left child
    AVLNode<Key, Value>* y_left = y->getLeft();
    y->setLeft(z);
    z->setParent(y);
    y->setParent(z_parent);
    z->setRight(y_left);
    if (y_left != nullptr) y_left->setParent(z);
    
    //Update Heights of x, z, and y. Update y last since it is the parent of x and z
    if (x != nullptr){
        int x_leftH = 0;
        int x_rightH = 0;
        if (x->getLeft() != nullptr) x_leftH = x->getLeft()->getHeight();
        if (x->getRight() != nullptr) x_rightH = x->getRight()->getHeight();
        x->setHeight(std::max(x_leftH, x_rightH) + 1);
    }
    
    if (z != nullptr){
        int z_leftH = 0;
        int z_rightH = 0;
        if (z->getLeft() != nullptr) z_leftH = z->getLeft()->getHeight();
        if (z->getRight() != nullptr) z_rightH = z->getRight()->getHeight();
        z->setHeight(std::max(z_leftH, z_rightH) + 1);
    }
    
    if (y != nullptr){
        int y_leftH = 0;
        int y_rightH = 0;
        if (y->getLeft() != nullptr) y_leftH = y->getLeft()->getHeight();
        if (y->getRight() != nullptr) y_rightH = y->getRight()->getHeight();
        y->setHeight(std::max(y_leftH, y_rightH) + 1);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::singleRight(AVLNode<Key, Value>* z, AVLNode<Key, Value>* y, AVLNode<Key, Value>* x)
{
    //z's parent should have y as its child
    if (BinarySearchTree<Key,Value>::root_ == z) BinarySearchTree<Key,Value>::root_ = y;
    AVLNode<Key, Value>* z_parent = z->getParent();
    if (z_parent != nullptr){
        if (z_parent->getLeft() == z) z_parent->setLeft(y);
        else z_parent->setRight(y);
    }

    //y will adpot z as its child and z will adopt y's orphan if y has a right child
    z->setParent(y);
    y->setParent(z_parent);
    AVLNode<Key, Value>* y_right = y->getRight();
    y->setRight(z);
    z->setLeft(y_right);
    if (y_right) y_right->setParent(z);

    //Update Heights of x, z, and y. Update y last since it is the parent of x and y
    if (x != nullptr){
        int x_leftH = 0;
        int x_rightH = 0;
        if (x->getLeft() != nullptr) x_leftH = x->getLeft()->getHeight();
        if (x->getRight() != nullptr) x_rightH = x->getRight()->getHeight();
        x->setHeight(std::max(x_leftH, x_rightH) + 1);
    }
    
    if (z != nullptr){
        int z_leftH = 0;
        int z_rightH = 0;
        if (z->getLeft() != nullptr) z_leftH = z->getLeft()->getHeight();
        if (z->getRight() != nullptr) z_rightH = z->getRight()->getHeight();
        z->setHeight(std::max(z_leftH, z_rightH) + 1);
    }
    
    if (y != nullptr){
        int y_leftH = 0;
        int y_rightH = 0;
        if (y->getLeft() != nullptr) y_leftH = y->getLeft()->getHeight();
        if (y->getRight() != nullptr) y_rightH = y->getRight()->getHeight();
        y->setHeight(std::max(y_leftH, y_rightH) + 1);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::doubleLeft(AVLNode<Key, Value>* z, AVLNode<Key, Value>* y, AVLNode<Key, Value>* x)
{
    singleRight(y, x, x->getLeft());    //Right rotate on y
    singleLeft(z,x,y);  //Left rotate on z
}

template<class Key, class Value>
void AVLTree<Key, Value>::doubleRight(AVLNode<Key, Value>* z, AVLNode<Key, Value>* y, AVLNode<Key, Value>* x)
{
    singleLeft(y, x, x->getRight());    //Left rotate on y
    singleRight(z, x, y);   //Right rotate on z
}

template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key,Value>* n = static_cast<AVLNode<Key,Value>*>(BinarySearchTree<Key,Value>::internalFind(key));
    if (n == nullptr) return;

    //Get the node's parent and child pointers before it is deleted
    AVLNode<Key, Value>* left = n->getLeft();
    AVLNode<Key, Value>* right = n->getRight();
    AVLNode<Key, Value>* parent = n->getParent();

    //Check if the node is the root and whether it is a left or right child
    bool n_is_left = true;
    bool n_is_root = false;
    if (parent != nullptr && parent->getLeft() != n) n_is_left = false;
    else if (parent == nullptr){
        n_is_root = true;
        n_is_left = false;
    } 

    //Remove the node unless it has two children, in which case we need to swap with successor before removing
    if (left == nullptr || right == nullptr) parent = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key,Value>::removeNode(n));

    //Leaf node
    if (left == nullptr && right == nullptr){
        if (n_is_root) BinarySearchTree<Key,Value>::root_ = nullptr;
    } 

    //Only has a right child (promote the child)
    else if (left == nullptr && right != nullptr){
        if (n_is_root) BinarySearchTree<Key,Value>::root_ = right;
        else right->setParent(parent);
        if (n_is_left) parent->setLeft(right);
        else if (!n_is_root) parent->setRight(right);
    }

    //Only has a left child (promote the child)
    else if (right == nullptr && left != nullptr){
        if (n_is_root) BinarySearchTree<Key,Value>::root_ = left;
        else left->setParent(parent);
        if (n_is_left) parent->setLeft(left);
        else if (!n_is_root) parent->setRight(left);
    }

    //Has two children, so swap with successor
    else{  
        AVLNode<Key, Value>* s = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key,Value>::successor(n));
        //If the successor is a leaf, swap and delete
        if (s->getLeft() == nullptr && s->getRight() == nullptr){
            nodeSwap(n, s);
            parent = static_cast<AVLNode<Key,Value>*>(BinarySearchTree<Key,Value>::removeNode(n));
        }

        //If the successor has a right child, promote the right child after swapping
        else{
            nodeSwap(s, n);
            AVLNode<Key, Value>* rightNode = n->getRight();
            bool n_isLeft = false;
            if (n->getParent() != nullptr && n->getParent()->getLeft() == n) n_isLeft = true;

            AVLNode<Key, Value>* parentNode = static_cast<AVLNode<Key,Value>*>(BinarySearchTree<Key,Value>::removeNode(n));
            if (n_isLeft == true) parentNode->setLeft(rightNode);
            else parentNode->setRight(rightNode);
            rightNode->setParent(parentNode);
            parent = parentNode;
        }
    }

    //Balance the tree
    removeFix(parent);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node)
{
    if (node == nullptr) return;    

    //Update the height of the node
    int oldHeight = node->getHeight();
    int leftHeight = 0;
    int rightHeight = 0;
    if (node->getLeft() != nullptr) leftHeight = node->getLeft()->getHeight();
    if (node->getRight() != nullptr) rightHeight = node->getRight()->getHeight();
    
    int newHeight = std::max(leftHeight, rightHeight) + 1;
    node->setHeight(newHeight);

    //Check if this node is balanced
    bool balanced = (std::abs(leftHeight - rightHeight) <= 1);
    if (balanced){
        if (newHeight != oldHeight) removeFix(node->getParent());   //If the node is balanced, keep checking up the tree
    }

    //If the node is not balanced, perform a rotation, then call removeFix up the tree
    else{
        AVLNode<Key,Value>* z = node;
        AVLNode<Key, Value>* y = nullptr;

        //get y, which is z's child with greater height
        bool y_is_left = true;
        if (leftHeight > rightHeight) y = node->getLeft();
        else {
            y = node->getRight();
            y_is_left = false;
        }

        //Get x (y's child with greater height)
        AVLNode<Key, Value>* x = nullptr;
        int y_left = 0;
        int y_right = 0;
        if (y->getLeft() != nullptr) y_left = y->getLeft()->getHeight();
        if (y->getRight() != nullptr) y_right = y->getRight()->getHeight();

        //Break ties with a single rotation
        bool singleRotation = true;
        if (y_left == y_right){
            if (y_is_left == true) x = y->getLeft();
            else x = y->getRight();
        }
    
        //Choose child with greater height and determine if zig-zig or zig-zag
        else if (y_left > y_right){
            x = y->getLeft();
            if (y_is_left == false) singleRotation = false;
        }
        else{
            x = y->getRight();
            if (y_is_left == true) singleRotation = false;
        }

        //After a zig-zig rotation, call removeFix on y's parent
        if (singleRotation && y_is_left){
            singleRight(z, y, x);
            removeFix(y->getParent());
        } 
        else if (singleRotation){
            singleLeft(z, y, x);
            removeFix(y->getParent());
        } 
        
        //After a zig-zag rotation, call removeFix on x's parent
        else if (y_is_left){
            doubleRight(z, y, x);
            removeFix(x->getParent());
        } 
        else{
            doubleLeft(z, y, x);
            removeFix(x->getParent());
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int tempH = n1->getHeight();
    n1->setHeight(n2->getHeight());
    n2->setHeight(tempH);
}

#endif
