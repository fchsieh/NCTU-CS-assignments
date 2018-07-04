// compile with -std=c++11
#include <iostream>
#include <cstring>
using namespace std;

class RBTree;

#define RED 0
#define BLACK 1

class TreeNode
{
    friend class RBTree;

  public:
    TreeNode()
    {
        key = 0;
        gender = "";
        height = weight = 0;
        left = right = parent = NULL;
        color = 0;
    }
    TreeNode(int k, string g, int h, int w)
    {
        this->key = k;
        this->gender = g;
        this->height = h;
        this->weight = w;
        left = right = parent = NULL;
        color = RED;
    }
    //int getKey() { return key; }
    inline string getGender() { return gender; }
    inline int getHeight() { return height; }
    inline int getWeight() { return weight; }
    void print_node(); // for debug usage

  private:
    int key;
    string gender;
    int height, weight;
    bool color; // 0 for red, 1 for black
    TreeNode *left, *right, *parent;
};

class RBTree
{
  public:
    RBTree()
    {
        T_NIL = new TreeNode();
        T_NIL->color = BLACK;
        root = T_NIL;
        root->parent = T_NIL;
    }
    bool insert(int key, string gender, int height, int weight);
    TreeNode &operator[](int key);
    TreeNode *Search(int key);

  private:
    TreeNode *root;
    TreeNode *T_NIL;
    void rotateLeft(TreeNode *current);
    void rotateRight(TreeNode *current);
    void insertFixUp(TreeNode *current);
};
