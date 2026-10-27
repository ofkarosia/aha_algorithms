#include "shared.h"

typedef struct AVLNode {
    Item item;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

typedef struct {
    AVLNode *root;
    int size;
} AVLTree;

AVLTree new_tree(void) {
    return (AVLTree) {
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

static int get_height(AVLNode *node) {
    if (node == NULL) {
        return 0;
    }

    return node->height;
}

static void update_height(AVLNode *node) {
    if (node != NULL) {
        node->height = max(get_height(node->left), get_height(node->right)) + 1;
    }
}

static int get_balance_factor(AVLNode *node) {
    if (node == NULL) {
        return 0;
    }

    return get_height(node->left) - get_height(node->right);
}

static AVLNode *create_node(const Item *item) {
    AVLNode *node = (AVLNode *) malloc(sizeof(AVLNode));
    
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
AVLNode *rotate_right(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *subtree = x->right;
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
AVLNode *rotate_left(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *subtree = y->left;
    y->left = x;
    x->right = subtree;
    update_height(x);
    update_height(y);
    return y;
}

static bool insert(const Item *item, AVLNode **ppn) {
    if (*ppn == NULL) {
        *ppn = create_node(item);
        return *ppn != NULL;
    }

    AVLNode *node = *ppn;

    bool res;

    if (*item < node->item) {
        res = insert(item, &node->left);
    } else if (*item > node->item) {
        res = insert(item,&node->right);
    } else {
        return false;
    }

    if (res) {
        update_height(node);
    }

    int balance_factor = get_balance_factor(node);

    if (balance_factor > 1) {
        if (*item < node->left->item) {
            // LL
            node = rotate_right(node);
        } else {
            // LR
            node->left = rotate_left(node->left);
            node = rotate_right(node);
        }
    } else if (balance_factor < -1) {
        if (*item < node->right->item) {
            // RL
            node->right = rotate_right(node->right);
            node = rotate_left(node);
        } else {
            // RR
            node = rotate_left(node);
        }
    }

    *ppn = node;

    return res;
}

bool insert_item(const Item *item, AVLTree *tree) {
    bool res = insert(item, &tree->root);

    if (res) {
        tree->size++;
    }

    return res;
}

void in_order(AVLNode *node, Fn fun) {
    if (node != NULL) {
        in_order(node->left, fun);
        fun(node->item);
        in_order(node->right, fun);
    }
}

void pre_order(AVLNode *node, Fn fun) {
    if (node != NULL) {
        fun(node->item);
        pre_order(node->left, fun);
        pre_order(node->right, fun);
    }
}

void traverse(AVLTree *tree, Fn fun) {
    if (tree->root != NULL) {
        in_order(tree->root, fun);
    }
}
