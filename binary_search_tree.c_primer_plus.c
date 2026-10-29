#include <stdbool.h>
#include <stdlib.h>
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


Tree new_tree(void) {
    (Tree) {
        .root = NULL,
        .size = 0
    };
} 

static bool create_node_and_attach(const Item *item, Node **ppn) {
    *ppn = malloc(sizeof(Node));

    if (*ppn != NULL) {
        (*ppn)->item = *item;
        (*ppn)->left = NULL;
        (*ppn)->right = NULL;
        return true;
    }

    return false;
}

bool add_item(const Item *item, Tree *tree) {
    if (tree->root == NULL) {
        bool res = create_node_and_attach(item, &tree->root);

        if (res) {
            tree->size++;
        }

        return res;
    }

    Node *node = tree->root;

    // Alternative implementation: recursion
    while (true) {
        if (*item < node->item) {
            if (node->left != NULL) {
                node = node->left;
                continue;
            }

            bool res = create_node_and_attach(item, &node->left);

            if (res) {
                tree->size++;
            }

            return res;
        }

        if (*item > node->item) {
            if (node->right != NULL) {
                node = node->right;
                continue;
            }

            bool res = create_node_and_attach(item, &node->right);

            if (res) {
                tree->size++;
            }

            return res;
        }

        return false;
    }
}

typedef struct Pair {
    Node *parent;
    Node *child;
} Pair;

Pair seek_node(const Item *item, Tree *tree) {
    Pair look = {
        .parent = NULL,
        .child = tree->root,
    };

    if (tree->root == NULL) {
        return look;
    }

    while (look.child != NULL) {
        if (*item < look.child->item) {
            look.parent = look.child;
            look.child = look.child->left;
            continue;
        }

        if (*item > look.child->item) {
            look.parent = look.child;
            look.child = look.child->right;
            continue;
        }

        return look;
    }
}

static void delete_node(Node **ppn) {
    Node *pn_free = *ppn;

    if ((*ppn)->left == NULL) {
        *ppn = (*ppn)->right;
        free(pn_free);
        return;
    }

    if ((*ppn)->right == NULL) {
        *ppn = (*ppn)->left;
        free(pn_free);
        return;
    }

    Node *node = pn_free->left;
    Node *right = pn_free->right;

    free(pn_free);
    *ppn = node;

    while (true) {
        if (node->right != NULL) {
            node = node->right;
            continue;
        }

        node->right = right;
        return;
    }
}

bool delete_item(const Item *item, Tree *tree) {
    Pair look = seek_node(item, tree);

    if (look.child == NULL) {
        return false;
    }

    if (look.parent->left == look.child) {
        delete_node(&look.parent->left);
    } else {
        delete_node(&look.parent->right);
    }

    tree->size--;
    return true;
}

int item_count(const Tree *tree) {
    return tree->size;
}

void in_order(const Node *node, void (*fun)(Item)) {
    if (node != NULL) {
        in_order(node->left, fun);
        fun(node->item);
        in_order(node->right, fun);
    }
}

void traverse(const Tree *tree, void (*fun)(Item)) {
    if (tree->root != NULL) {
        in_order(tree->root, fun);
    }
}

static void delete_all_nodes(Node *node) {
    if (node != NULL) {
        delete_all_nodes(node->left);
        Node *right = node->right;
        free(node);
        delete_all_nodes(right);
    }
}

void delete_all_items(Tree *tree) {
    if (tree->root != NULL) {
        delete_all_nodes(tree->root);
        tree->root = NULL;
        tree->size = 0;
    }
}

bool is_empty(const Tree *tree) {
    return tree->size == 0;
}
