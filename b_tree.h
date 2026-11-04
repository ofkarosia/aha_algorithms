#include "shared.h"

typedef struct Node {
    Item *keys;
    int key_count;
    bool leaf;
    struct Node **children;
} Node;

typedef struct {
    Node *root;
    int m;
    int size;
} Tree;

Tree new_tree(int m);

bool insert_item(const Item *item, Tree *tree);

typedef bool (*VisitFn)(const Item *item, void *ctx);

void traverse(const Tree *tree, VisitFn visit, void *ctx);
