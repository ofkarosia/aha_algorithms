#include "shared.h"
#include <stdbool.h>

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

bool add_item(const Item *item, Tree *tree);

typedef struct Pair {
    Node *parent;
    Node *child;
} Pair;

Pair seek_item(const Item *item, Tree *tree);

bool delete_item(const Item *item, Tree *tree);

int item_count(const Tree *tree);

void traverse(const Tree *tree, void (*fun)(Item));

void delete_all_items(Tree *tree);

bool is_empty(const Tree *tree);
