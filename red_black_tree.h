#include "shared.h"

typedef enum {
    RED,
    BLACK
} Color;

typedef struct Node {
    Item item;
    Color color;
    struct Node *left;
    struct Node *right;
    struct Node *parent;
} Node;

typedef struct {
    Node *root;
    Node nil;
    int size;
} Tree;

typedef Fn(Func, const Node *);

Tree *new_tree(void);

bool insert_item(const Item *item, Tree *tree);

void pre_order(const Node *node, const Tree *tree, Func fun);

void in_order(const Node *node, const Tree *tree, Func fun);

void traverse(const Tree *tree, Func fun);

void iterate(const Tree *tree, Func fun);
