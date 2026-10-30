#include "shared.h"

typedef struct Node {
    Item item;
    int height;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct {
    Node *root;
    int size;
} Tree;

Tree new_tree(void);

bool insert_item(const Item *item, Tree *tree);

void in_order(Node *node, Fn fun);

void pre_order(Node *node, Fn fun);

void traverse(Tree *tree, Fn fun);

bool delete_item(const Item *item, Tree *tree);
