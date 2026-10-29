#include "shared.h"

typedef struct AVLNode {
    Item item;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

typedef struct {
    AVLNode *root;
    int size;
} AVLTree;

AVLTree new_tree(void);

bool insert_item(const Item *item, AVLTree *tree);

void in_order(AVLNode *node, Fn fun);

void pre_order(AVLNode *node, Fn fun);

void traverse(AVLTree *tree, Fn fun);

void delete_item(const Item *item, AVLTree *tree);
