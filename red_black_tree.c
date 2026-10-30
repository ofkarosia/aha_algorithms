#include "red_black_tree.h"

Tree *new_tree(void) {
    Tree *tree = malloc(sizeof(Tree));

    if (tree == NULL) {
        return tree;
    }

    tree->nil.item = 0;
    tree->nil.color = BLACK;
    tree->nil.left = &tree->nil;
    tree->nil.right = &tree->nil;
    tree->nil.parent = &tree->nil;
    tree->root = &tree->nil;

    return tree;
}

static Node *create_node(const Item *item, Tree *tree) {
    Node *node = malloc(sizeof(Node));

    if (node == NULL) {
        return &tree->nil;
    }

    node->item = *item;
    node->color = RED;
    node->left = &tree->nil;
    node->right = &tree->nil;
    node->parent = &tree->nil;

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
    
    if (subtree_B != &tree->nil) {
        subtree_B->parent = y;
    }

    x->parent = y->parent;
    
    if (y->parent == &tree->nil) {
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
    
    if (subtree_B != &tree->nil) {
        subtree_B->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == &tree->nil) {
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
    while (node->parent->color == RED) {
        if (node->parent->parent->left == node->parent) {
            Node *uncle = node->parent->parent->right;

            if (uncle->color == RED) {
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node->parent->color = BLACK;
                node = node->parent->parent;
                continue;
            }

            // LR
            if (node->parent->right == node) {
                node = node->parent;
                rotate_left(node, tree);
            }

            // LL
            node->parent->color = BLACK;
            node->parent->parent->color = RED;
            rotate_right(node->parent->parent, tree);
        } else {
            Node *uncle = node->parent->parent->left;

            if (uncle->color == RED) {
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node->parent->color = BLACK;
                node = node->parent->parent;
                continue;
            }

            // RL
            if (node->parent->left == node) {
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
    Node *parent = &tree->nil;
    Node *current = tree->root;

    while (current != &tree->nil) {
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

    Node *node = create_node(item, tree);

    if (node == &tree->nil) {
        return false;
    }

    if (parent == &tree->nil) {
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

void pre_order(const Node *node, const Tree *tree, Func fun) {
    if (node != &tree->nil) {
        fun(node->item, node);
        pre_order(node->left, tree, fun);
        pre_order(node->right, tree, fun);
    }
}

void in_order(const Node *node, const Tree *tree, Func fun) {
    if (node != &tree->nil) {
        in_order(node->left, tree, fun);
        fun(node->item, node);
        in_order(node->right, tree, fun);
    }
}

void traverse(const Tree *tree, Func fun) {
    if (tree->root != &tree->nil) {
        pre_order(tree->root, tree, fun);
    }
}

void iterate(const Tree *tree, Func fun) {
    if (tree->root != &tree->nil) {
        in_order(tree->root, tree, fun);
    }
}
