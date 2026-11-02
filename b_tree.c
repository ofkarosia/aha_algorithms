#include "b_tree.h"

Tree new_tree(int m)  {
    return (Tree) {
        .root = NULL,
        .m = m,
        .size = 0
    };
}

static Node *create_node(const Item *item, int m) {
    Node *node = malloc(sizeof(Node));

    if (node == NULL) {
        return NULL;
    }

    node->key_count = m - 1;
    node->keys = malloc(sizeof(Item) * (m - 1));

    if (node->keys == NULL) {
        free(node);
        return NULL;
    }

    node->children = malloc(sizeof(Node *) * m);

    if (node->children == NULL) {
        free(node->keys);
        free(node);
        return NULL;
    }

    return node;
}

static bool insert(const Item *item, Node **ppn, Tree *tree) {
    if (ppn == NULL) {
        return;
    }

    if (*ppn == NULL) {
        *ppn = create_node(item, tree->m);
        return *ppn != NULL;
    }

    Node *node = tree->root;

    
}

bool insert_item(const Item *item, Tree *tree) {
    
}