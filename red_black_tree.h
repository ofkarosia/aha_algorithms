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
    int size;
} Tree;

typedef Fn(Func, const Node *);

Tree new_tree(void);

bool insert_item(const Item *item, Tree *tree);

void pre_order(const Node *node, Func fun);

void in_order(const Node *node, Func fun);

void traverse(const Tree *tree, Func fun);
