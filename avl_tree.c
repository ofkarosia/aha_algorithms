#include "shared.h"
#include "avl_tree.h"

Tree new_tree(void) {
    return (Tree) {
        .root = NULL,
        .size = 0
    };
}

static int max(int a, int b) {
    if (a < b) {
        return b;
    } else {
        return a;
    }
}

static int get_height(Node *node) {
    if (node == NULL) {
        return 0;
    }

    return node->height;
}

static void update_height(Node *node) {
    if (node != NULL) {
        node->height = max(get_height(node->left), get_height(node->right)) + 1;
    }
}

static int get_balance_factor(Node *node) {
    if (node == NULL) {
        return 0;
    }

    return get_height(node->left) - get_height(node->right);
}

static Node *create_node(const Item *item) {
    Node *node = (Node *) malloc(sizeof(Node));
    
    if (node != NULL) {
        node->height = 1;
        node->item = *item;
        node->left = NULL;
        node->right = NULL;
    }

    return node;
}

/*
 *       y                 x
 *      / \               / \
 *     x   C      ->     A   y
 *    / \                   / \
 *   A   B                 B   C
 */
Node *rotate_right(Node *y) {
    Node *x = y->left;
    Node *subtree = x->right;
    x->right = y;
    y->left = subtree;
    update_height(y);
    update_height(x);
    return x;
}

/*
 *     x                     y
 *    / \                   / \
 *   A   y        ->       x   C
 *      / \               / \
 *     B   C             A   B
 */
Node *rotate_left(Node *x) {
    Node *y = x->right;
    Node *subtree = y->left;
    y->left = x;
    x->right = subtree;
    update_height(x);
    update_height(y);
    return y;
}

static void rebalance(Node **ppn) {
    Node *node = *ppn;

    update_height(node);
    int balance_factor = get_balance_factor(node);

    if (balance_factor > 1) {
        if (get_balance_factor(node->left) >= 0) {
            // LL
            node = rotate_right(node);
        } else {
            // LR
            node->left = rotate_left(node->left);
            node = rotate_right(node);
        }
    } else if (balance_factor < -1) {
        if (get_balance_factor(node->right) <= 0) {
            // RR
            node = rotate_left(node);
        } else {
            // RL
            node->right = rotate_right(node->right);
            node = rotate_left(node);
        }
    }

    *ppn = node;
}

static bool insert(const Item *item, Node **ppn) {
    if (*ppn == NULL) {
        *ppn = create_node(item);
        return *ppn != NULL;
    }

    Node *node = *ppn;
    bool res;

    if (*item < node->item) {
        res = insert(item, &node->left);
    } else if (*item > node->item) {
        res = insert(item,&node->right);
    } else {
        return false;
    }

    rebalance(ppn);
    return res;
}

bool insert_item(const Item *item, Tree *tree) {
    bool res = insert(item, &tree->root);

    if (res) {
        tree->size++;
    }

    return res;
}

void in_order(Node *node, Fn fun) {
    if (node != NULL) {
        in_order(node->left, fun);
        fun(node->item);
        in_order(node->right, fun);
    }
}

void pre_order(Node *node, Fn fun) {
    if (node != NULL) {
        fun(node->item);
        pre_order(node->left, fun);
        pre_order(node->right, fun);
    }
}

void traverse(Tree *tree, Fn fun) {
    if (tree->root != NULL) {
        in_order(tree->root, fun);
    }
}

typedef struct {
    Node *parent;
    Node *child;
} Pair;

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

static void bst_delete_node(Node **ppn) {
    if (ppn == NULL || *ppn == NULL) {
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

static bool delete_node(const Item *item, Node **ppn) {
    if (ppn == NULL || *ppn == NULL) {
        return false;
    }

    Node *node = *ppn;
    bool res;

    if (*item < node->item) {
        res = delete_node(item, &node->left);
    } else if (*item > node->item) {
        res = delete_node(item, &node->right);
    } else {
        bst_delete_node(ppn);
        res = true;
    }

    rebalance(ppn);
    return res;
}

bool delete_item(const Item *item, Tree *tree) {
    bool res = delete_node(item, &tree->root);

    if (res) {
        tree->size--;
    }

    return res;
}
