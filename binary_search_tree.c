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
    return (Tree) {
        .root = NULL,
        .size = 0,
    };
}

Node *create_node(const Item *item) {
    Node *node = malloc(sizeof(Node));

    if (node != NULL) {
        node->item = *item;
        node->left = NULL;
        node->right = NULL;
    }

    return node;
}

static bool insert(const Item *item, Node **ppn) {
    if (*ppn == NULL) {
        *ppn = create_node(item);
        return *ppn != NULL;
    }

    Node *node = *ppn;

    if (*item < node->item) {
        return insert(item, &node->left);
    } else if (*item > node->item) {
        return insert(item, &node->right);
    }

    return false;
}

bool insert_item(const Item *item, Tree *tree) {
    bool res = insert(item, &tree->root);

    if (res) {
        tree->size++;
    }

    return res;
}

typedef struct {
    Node *parent;
    Node *child;
} Pair;

static Pair seek_item(const Item *item, const Tree *tree) {
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

        break;
    }

    return look;
}

static Pair seek_min_node(Node *root) {
    Pair look = {
        .parent = NULL,
        .child = root,
    };

    if (root == NULL) {
        return look;
    }

    while (look.child->left != NULL) {
        look.parent = look.child;
        look.child = look.child->left;
    }

    return look;
}

static void delete_node(Node **ppn) {
    if (*ppn == NULL) {
        return;
    }

    Node *node = *ppn;

    if (node->left == NULL) {
        *ppn = node->right;
        free(node);
        return;
    }

    if (node->right == NULL) {
        *ppn = node->left;
        free(node);
        return;
    }

    Pair look = seek_min_node(node->right);
    Node *successor = look.child;

    if (look.parent != NULL) {
        look.parent->left = successor->right;
        successor->right = node->right;
    }

    successor->left = node->left;
    *ppn = successor;

    free(node);
}

void delete_item(const Item *item, Tree *tree) {
    Pair look = seek_item(item, tree);

    if (look.child == NULL) {
        return;
    }

    if (look.parent == NULL) {
        delete_node(&tree->root);
    } else if (look.parent->left == look.child) {
        delete_node(&look.parent->left);
    } else {
        delete_node(&look.parent->right);
    }

    tree->size--;
}

void in_order(const Node *node, Fn fun) {
    if (node != NULL) {
        in_order(node->left, fun);
        fun(node->item);
        in_order(node->right, fun);
    }
}

void pre_order(const Node *node, Fn fun) {
    if (node != NULL) {
        fun(node->item);
        pre_order(node->left, fun);
        pre_order(node->right, fun);
    }
}

void traverse(const Tree *tree, Fn fun) {
    if (tree->root != NULL) {
        pre_order(tree->root, fun);
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

void delete_all(Tree *tree) {
    delete_all_nodes(tree->root);
    tree->root = NULL;
    tree->size = 0;
}
