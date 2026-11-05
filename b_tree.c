#include "b_tree.h"
#include <string.h>
#include <stdio.h>

Tree new_tree(int m) {
    return (Tree) {
        .root = NULL,
        .m = m,
        .size = 0,
    };
}

static Node *create_node(int m, bool leaf) {
    Node *node = malloc(sizeof(Node));

    if (node == NULL) {
        return NULL;
    }

    node->keys = malloc(sizeof(Item) * (m - 1));
    node->children = calloc(m, sizeof(Node *));

    if (node->keys == NULL || node->children == NULL) {
        free(node->keys);
        free(node->children);
        free(node);
        return NULL;
    }

    node->key_count = 0;
    node->leaf = leaf;
    return node;
}

typedef struct {
    bool found;
    int pos;
} SearchResult;

static SearchResult search_key(const Item *item, const Node *node) {
    SearchResult res = { 0 };
    int i = 0;

    while (i < node->key_count) {
        if (*item < node->keys[i]) {
            break;
        }

        if (*item == node->keys[i]) {
            res.found = true;
            res.pos = i;
            return res;
        }

        i++;
    }

    res.found = false;
    res.pos = i;
    return res;
}

static void insert_key(const Item *key, int pos, Node *node) {
    memmove(&node->keys[pos + 1], &node->keys[pos], sizeof(Item) * (node->key_count - pos));
    node->keys[pos] = *key;
    node->key_count++;
}

static void insert_key_and_child(const Item *key, Node *child, int pos, Node *node) {
    int child_pos = pos + 1;
    insert_key(key, pos, node);
    memmove(&node->children[child_pos + 1], &node->children[child_pos], sizeof(Node *) * (node->key_count - pos));
    node->children[child_pos] = child;
}

typedef struct {
    Item key;
    Node *child;
} Pair;

static Item remove_key(int pos, Node *node) {
    memmove(&node->keys[pos], &node->keys[pos + 1], sizeof(Item) * (node->key_count - pos - 1));
    node->key_count--;
}

static Pair remove_key_and_child(int pos, Node *node) {
    int child_pos = pos + 1;
    Pair pair = { 0 };
    pair.key = remove_key(pos, node);
    pair.child = node->children[child_pos];
    memmove(&node->children[child_pos], &node->children[child_pos + 1], sizeof(Node *) * (node->key_count - pos - 1));
    return pair;
}

static Item logical_key(Node *node, int insert_pos, const Item *inserted, int logical_pos) {
    if (logical_pos < insert_pos) {
        return node->keys[logical_pos];
    }

    if (logical_pos == insert_pos) {
        return *inserted;
    }

    return node->keys[logical_pos - 1];
}

static Node *logical_child(Node *node, int insert_pos, Node *inserted, int logical_pos) {
    if (logical_pos < insert_pos) {
        return node->children[logical_pos];
    }

    if (logical_pos == insert_pos) {
        return inserted;
    }

    return node->children[logical_pos - 1];
}

static Node *get_child_s(Node *node, int pos) {
    if (pos < 0 || pos > node->key_count) {
        return NULL;
    }

    return node->children[pos];
}

static bool contains(const Item *item, const Tree *tree) {
    const Node *node = tree->root;

    while (node != NULL) {
        SearchResult res = search_key(item, node);

        if (res.found) {
            return true;
        }

        if (node->leaf) {
            return false;
        }

        node = node->children[res.pos];
    }

    return false;
}

typedef struct {
    Item promoted_key;
    Node *right;
} SplitResult;

static SplitResult split(Node *node, const Item *item, int item_pos, Node *right_child, int m) {
    SplitResult res = { 0 };
    Node *right = create_node(m, node->leaf);

    if (right == NULL) {
        return res;
    }

    int sep = m / 2;
    res.promoted_key = logical_key(node, item_pos, item, sep);
    res.right = right;
    
    for (int i = sep + 1; i < m; i++) {
        right->keys[i - sep - 1] = logical_key(node, item_pos, item, i);
    }

    if (item_pos < sep) {
        for (int i = sep - 1; i > item_pos; i--) {
            node->keys[i] = logical_key(node, item_pos, item, i);
        }

        node->keys[item_pos] = *item;
    }

    node->key_count = sep;
    right->key_count = m - sep - 1;

    if (right_child == NULL) {
        return res;
    }

    int child_pos = item_pos + 1;

    for (int i = sep + 1; i < m + 1; i++) {
        right->children[i - sep - 1] = logical_child(node, child_pos, right_child, i);
    }

    if (child_pos <= sep) {
        for (int i = sep ; i > child_pos; i--) {
            node->children[i] = logical_child(node, child_pos, right_child, i);
        }

        node->children[child_pos] = right_child;
    }

    return res;
}

typedef struct {
    bool ok;
    bool split;
    SplitResult split_result;
} InsertResult;

static InsertResult insert(const Item *item, Node *node, int m) {
    InsertResult res = { 0 };

    if (node == NULL) {
        return res;
    }

    SearchResult search = search_key(item, node);

    if (search.found) {
        return res;
    }

    if (node->leaf) {
        if (node->key_count == m - 1) {
            res.split_result = split(node, item, search.pos, NULL, m);
            res.ok = true;
            res.split = res.split_result.right != NULL;
            return res;
        }

        insert_key(item, search.pos, node);
        res.ok = true;
        return res;
    }

    InsertResult inner_res = insert(item, node->children[search.pos], m);

    if (!inner_res.ok) {
        return res;
    }

    if (!inner_res.split) {
        res.ok = true;
        return res;
    }

    SearchResult search_promoted = search_key(&inner_res.split_result.promoted_key, node);

    if (node->key_count == m - 1) {
        res.split_result = split(node, &inner_res.split_result.promoted_key, search_promoted.pos, inner_res.split_result.right, m);
        res.ok = true;
        res.split = res.split_result.right != NULL;
        return res;
    }

    insert_key_and_child(&inner_res.split_result.promoted_key, inner_res.split_result.right, search_promoted.pos, node);
    res.ok = true;
    return res;
}

bool insert_item(const Item *item, Tree *tree) {
    if (tree->root == NULL) {
        tree->root = create_node(tree->m, true);
    }

    InsertResult res = insert(item, tree->root, tree->m);
    
    if (!res.ok) {
        return false;
    }

    if (!res.split) {
        tree->size++;
        return true;
    }

    Node *old_root = tree->root;
    tree->root = create_node(tree->m, true);

    if (tree->root == NULL) {
        return false;
    }

    tree->root->keys[0] = res.split_result.promoted_key;
    tree->root->key_count = 1;
    tree->root->children[0] = old_root;
    tree->root->children[1] = res.split_result.right;
    tree->root->leaf = false;
    tree->size++;
    return true;
}

static bool in_order(const Node *node, VisitFn visit, void *ctx) {
    if (node == NULL) {
        return true;
    }

    for (int i = 0; i < node->key_count; i++) {
        if (!node->leaf) {
            if (!in_order(node->children[i], visit, ctx)) {
                return false;
            }
        }

        if (!visit(&node->keys[i], ctx)) {
            return false;
        }
    }

    if (!node->leaf) {
        return in_order(node->children[node->key_count], visit, ctx);
    }

    return true;
}

void traverse(const Tree *tree, VisitFn visit, void *ctx) {
    in_order(tree->root, visit, ctx);
}

typedef struct {
    bool ok;
    bool overflow;
} DeleteResult;

static DeleteResult delete(Item *item, Node *node, int m) {
    int min_keys = (m + 1) / 2 + 1;
    DeleteResult res = { 0 };

    if (node == NULL) {
        return res;
    }

    SearchResult search = search_key(item, node);

    if (node->leaf) {
        if (!search.found) {
            return res;
        }

        remove_key(search.pos, node);
        res.ok = true;

        if (node->key_count >= min_keys) {
            return res;
        }

        res.overflow = true;
        return res;
    }

    if (search.found) {
        // ..
    }

    int child_pos = search.pos;
    Node *child = node->children[child_pos];
    DeleteResult inner_res = delete(item, child, m);

    if (!inner_res.ok) {
        return res;
    }

    if (!inner_res.overflow) {
        res.ok = true;
        return res;
    }

    Node *left_s = get_child_s(node, child_pos - 1);
    Node *right_s = get_child_s(node, child_pos + 1);

    if (left_s != NULL) {
        if (left_s->key_count > min_keys) {
            insert_key(node->keys[child_pos - 1], 0, child);
            node->keys[child_pos - 1] = remove_key(left_s->key_count - 1, left_s);
        } else {
            insert_key(node->keys[child_pos - 1], left_s->key_count, left_s);
            memcpy(&left_s->keys[left_s->key_count], child->keys, sizeof(Item) * child->key_count);
            left_s->key_count += child->key_count;
            remove_key_and_child(child_pos - 1, node);
        }
    } else if (right_s != NULL) {
        if (right_s->key_count > min_keys) {
            insert_key(node->keys[child_pos], child->key_count, child);
            node->keys[child_pos] = remove_key(0, right_s);
        } else {
            insert_key(node->keys[child_pos], child->key_count, child);
            memcpy(&child->keys[child->key_count], right_s->keys, sizeof(Item) * right_s->key_count);
            child->key_count += right_s->key_count;
            remove_key_and_child(child_pos, node);
        }
    }

}

bool delete_item(Item *item, Tree *tree) {

}
