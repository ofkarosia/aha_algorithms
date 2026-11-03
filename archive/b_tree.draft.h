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
