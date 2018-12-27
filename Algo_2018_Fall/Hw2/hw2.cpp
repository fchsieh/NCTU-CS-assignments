#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

void ReadFile();
void RBInsert(fstream &, vector<int> &);
void RBDelete(fstream &, vector<int> &);

// ========= Build RB tree ========
class RBTree;

#define RED 0
#define BLACK 1

class TreeNode {
    friend class RBTree;

   public:
    TreeNode() {
        key = 0;
        left = right = parent = NULL;
        color = 0;
    }
    TreeNode(int k) {
        this->key = k;
        left = right = parent = NULL;
        color = RED;
    }

   private:
    int key;
    bool color;  // 0 for red, 1 for black
    TreeNode *left, *right, *parent;
};

class RBTree {
   public:
    RBTree() {
        T_NIL = new TreeNode();
        T_NIL->color = BLACK;
        root = T_NIL;
        root->parent = T_NIL;
    }
    void insert(int key);
    void del(int key);
    TreeNode &operator[](int key);
    TreeNode *Search(int key);
    TreeNode *minimum(TreeNode *key);
    void print(fstream &outfile, TreeNode *node);
    TreeNode *GetRoot() { return root; }

   private:
    TreeNode *root;
    TreeNode *T_NIL;
    void rotateLeft(TreeNode *x);
    void rotateRight(TreeNode *y);
    void insertFixUp(TreeNode *z);
    void transplant(TreeNode *u, TreeNode *v);
    void deleteFixUp(TreeNode *z);
};

void RBTree::rotateLeft(TreeNode *x) {
    TreeNode *y = x->right;
    x->right = y->left;
    if (y->left != T_NIL) y->left->parent = x;
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

void RBTree::rotateRight(TreeNode *y) {
    TreeNode *x = y->left;
    y->left = x->right;
    if (x->right != T_NIL) x->right->parent = y;
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

void RBTree::insert(int k) {
    TreeNode *y = T_NIL;
    TreeNode *x = root;
    TreeNode *insert_node = new TreeNode(k);

    while (x != T_NIL) {
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
}

void RBTree::insertFixUp(TreeNode *z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            TreeNode *y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotateLeft(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateRight(z->parent->parent);
            }
        } else  // same as THEN clause after IF, with right and left exchanged
        {
            TreeNode *y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
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

void RBTree::transplant(TreeNode *u, TreeNode *v) {
    if (u->parent == T_NIL) {
        root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

void RBTree::del(int k) {
    TreeNode *z = Search(k);
    TreeNode *y = z;
    TreeNode *x;
    bool y_original_color = y->color;
    if (z->left == T_NIL) {
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == T_NIL) {
        x = z->left;
        transplant(z, z->left);
    } else {
        y = minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (y_original_color == BLACK) {
        deleteFixUp(x);
    }
}

void RBTree::deleteFixUp(TreeNode *x) {
    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            TreeNode *w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateLeft(x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rotateRight(w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rotateLeft(x->parent);
                x = root;
            }
        } else {
            TreeNode *w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateRight(x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rotateLeft(w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rotateRight(x->parent);
                x = root;
            }
        }
    }
    x->color = BLACK;
}

TreeNode *RBTree::Search(int search_key) {
    TreeNode *current = root;
    // do binary search
    while (current->key != search_key && current != T_NIL) {
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

TreeNode &RBTree::operator[](int search_key) { return *Search(search_key); }

TreeNode *RBTree::minimum(TreeNode *x) {
    while (x->left != T_NIL) {
        x = x->left;
    }
    return x;
}

void RBTree::print(fstream &outfile, TreeNode *node) {
    if (node == T_NIL) {
        return;
    }

    print(outfile, node->left);
    // ========= Print data =========
    outfile << "key: " << node->key << " parent: ";
    if (node->parent != T_NIL) {
        outfile << node->parent->key;
    } else {
        outfile << " ";
    }
    outfile << " color: ";
    if (node->color == RED) {
        outfile << "red";
    } else if (node->color == BLACK) {
        outfile << "black";
    }
    outfile << "\r\n";
    // ===============================
    print(outfile, node->right);
}

// ================================

RBTree rbTree;

int main() {
    ReadFile();
    return 0;
}

void ReadFile() {
    fstream infile("input.txt", infile.in);
    fstream outfile("output.txt", outfile.out);

    int case_count = 0;
    int used_method;
    string insert;

    // Number of works
    infile >> case_count;

    for (int i = 0; i < case_count; i++) {
        vector<int> rb_tree_data;
        // Insert or delete
        // ========= Read data row ============
        infile >> used_method;
        infile >> ws >> ws;
        getline(infile, insert);
        stringstream ss(insert);
        int n;
        while (ss >> n) {
            rb_tree_data.push_back(n);
        }

        // ========= Method 1: Insert =========
        if (used_method == 1) {
            RBInsert(outfile, rb_tree_data);
        }
        // ====================================

        // ========= Method 2: Delete =========
        else if (used_method == 2) {
            RBDelete(outfile, rb_tree_data);
        }
        // ====================================
        rbTree.print(outfile, rbTree.GetRoot());
    }
}

void RBInsert(fstream &outfile, vector<int> &rb_tree_data) {
    outfile << "Insert: ";
    for (int i = 0; i < rb_tree_data.size(); i++) {
        if (i != rb_tree_data.size() - 1) {
            outfile << rb_tree_data[i] << ", ";
        } else {
            outfile << rb_tree_data[i] << "\r\n";
        }
        rbTree.insert(rb_tree_data[i]);
    }
}

void RBDelete(fstream &outfile, vector<int> &rb_tree_data) {
    outfile << "Delete: ";
    for (int i = 0; i < rb_tree_data.size(); i++) {
        if (i != rb_tree_data.size() - 1) {
            outfile << rb_tree_data[i] << ", ";
        } else {
            outfile << rb_tree_data[i] << "\r\n";
        }
        rbTree.del(rb_tree_data[i]);
    }
}
