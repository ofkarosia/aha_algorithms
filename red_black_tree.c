#include "shared.h"
#include "red_black_tree.h"

Tree new_tree(void) {
    return (Tree) {
        .root = NULL,
        .size = 0,
    };
}

static Color get_color(const Node *node) {
    if (node == NULL) {
        return BLACK;
    }

    return node->color;
}

static Node *create_node(const Item *item) {
    Node *node = malloc(sizeof(Node));

    if (node == NULL) {
        return node;
    }

    node->color = RED;
    node->item = *item;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    
    return node;
}

/*
 *       y                 x
 *      / \               / \
 *     x   C      ->     A   y
 *    / \                   / \
 *   A   B?                B?  C
 */
static void rotate_right(Node *y, Tree *tree) {
    Node *x = y->left;
    Node *subtree_B = x->right;
    y->left = subtree_B;
    
    if (subtree_B != NULL) {
        subtree_B->parent = y;
    }

    x->parent = y->parent;
    
    if (y->parent == NULL) {
        tree->root = x;
    } else if (y->parent->left == y) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    
    x->right = y;
    y->parent = x;
}

/*
 *     x                     y
 *    / \                   / \
 *   A   y        ->       x   C
 *      / \               / \
 *     B?  C             A   B?
 */
static void rotate_left(Node *x, Tree *tree) {
    Node *y = x->right;
    Node *subtree_B = y->left;
    x->right = subtree_B;
    
    if (subtree_B != NULL) {
        subtree_B->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == NULL) {
        tree->root = y;
    } else if (x->parent->left == x) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

static void insert_fixup(Node *node, Tree *tree) {
    while (get_color(node->parent) == RED) {
        if (node->parent == node->parent->parent->left) {
            Node *uncle = node->parent->parent->right;

            if (get_color(uncle) == RED) {
                uncle->color = BLACK;
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
                continue;
            }

            if (node->parent->right == node) {
                // LR
                node = node->parent;
                rotate_left(node, tree);
            }

            // LL
            node->parent->color = BLACK;
            node->parent->parent->color = RED;
            rotate_right(node->parent->parent, tree);
        } else {
            Node *uncle = node->parent->parent->left;

            if (get_color(uncle) == RED) {
                uncle->color = BLACK;
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
                continue;
            }

            if (node->parent->left == node) {
                // RL
                node = node->parent;
                rotate_right(node, tree);
            }

            // RR
            node->parent->color = BLACK;
            node->parent->parent->color = RED;
            rotate_left(node->parent->parent, tree);
        }
    }

    tree->root->color = BLACK;
}

static bool insert(const Item *item, Tree *tree) {
    Node *parent = NULL;
    Node *current = tree->root;

    while (current != NULL) {
        if (*item < current->item) {
            parent = current;
            current = current->left;
            continue;
        }

        if (*item > current->item) {
            parent = current;
            current = current->right;
            continue;
        }

        return false;
    }

    Node *node = create_node(item);

    if (node == NULL) {
        return false;
    }

    if (parent == NULL) {
        tree->root = node;
    } else if (*item < parent->item) {
        parent->left = node;
    } else {
        parent->right = node;
    }

    node->parent = parent;
    insert_fixup(node, tree);
    return true;
}

bool insert_item(const Item *item, Tree *tree) {
    bool res = insert(item, tree);

    if (res) {
        tree->size++;
    }

    return res;
}

void pre_order(const Node *node, Func fun) {
    if (node != NULL) {
        fun(node->item, node);
        pre_order(node->left, fun);
        pre_order(node->right, fun);
    }
}

void in_order(const Node *node, Func fun) {
    if (node != NULL) {
        in_order(node->left, fun);
        fun(node->item, node);
        in_order(node->right, fun);
    }
}

void traverse(const Tree *tree, Func fun) {
    if (tree->root != NULL) {
        in_order(tree->root, fun);
    }
}
