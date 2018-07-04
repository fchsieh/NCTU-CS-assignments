// compile with -std=c++11
#include "RBTree.h"

inline void RBTree::rotateLeft(TreeNode *x)
{
    TreeNode *y = x->right;
    x->right = y->left;
    if (y->left != T_NIL)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == T_NIL)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

inline void RBTree::rotateRight(TreeNode *y)
{
    TreeNode *x = y->left;
    y->left = x->right;
    if (x->right != T_NIL)
        x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == T_NIL)
        root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;
}

bool RBTree::insert(int k, string g, int h, int w)
{
    if (Search(k) == NULL) // NULL: not in tree, need to insert
    {
        TreeNode *y = T_NIL;
        TreeNode *x = root;
        TreeNode *insert_node = new TreeNode(k, g, h, w);

        while (x != T_NIL)
        {
            y = x;
            if (insert_node->key < x->key)
                x = x->left;
            else
                x = x->right;
        }
        insert_node->parent = y;
        if (y == T_NIL)
            root = insert_node;
        else if (insert_node->key < y->key)
            y->left = insert_node;
        else
            y->right = insert_node;
        insert_node->left = T_NIL;
        insert_node->right = T_NIL;
        insert_node->color = RED;
        insertFixUp(insert_node);
        return true;
    }
    else // already in tree, return false
        return false;
}

inline void RBTree::insertFixUp(TreeNode *z)
{
    while (z->parent->color == RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            TreeNode *y = z->parent->parent->right;
            if (y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    z = z->parent;
                    rotateLeft(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateRight(z->parent->parent);
            }
        }
        else // same as THEN clause after IF, with right and left exchanged
        {
            TreeNode *y = z->parent->parent->left;
            if (y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    rotateRight(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateLeft(z->parent->parent);
            }
        }
    }
    root->color = BLACK;
}

inline TreeNode *RBTree::Search(int search_key)
{
    TreeNode *current = root;
    // do binary search
    while (current->key != search_key && current != T_NIL)
    {
        if (search_key < current->key)
            current = current->left;
        else
            current = current->right;
    }
    if (current->key == search_key)
        return current;
    else
        return NULL;
}

TreeNode &RBTree::operator[](int search_key)
{
    return *Search(search_key);
}

void TreeNode::print_node() // for debug
{
    if (this != NULL)
    {
        cout << "Key: " << key << "\n";
        cout << "Gender: " << gender << "\n";
        cout << "Height: " << height << "\n";
        cout << "Weight: " << weight << "\n";
    }
    else
        cout << "NULL\n";
}