#include "shared.h"

typedef struct Node {
    Item item;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct {
    Node *root;
    int size;
} Tree;

Tree new_tree(void);

Node *create_node(const Item *item);

bool insert_item(const Item *item, Tree *tree);

bool delete_item(const Item *item, Tree *tree);

void in_order(const Node *node, Fn fun);

void pre_order(const Node *node, Fn fun);

void traverse(const Tree *tree, Fn fun);

void delete_all(Tree *tree);
