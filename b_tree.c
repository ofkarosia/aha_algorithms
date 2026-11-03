#include "b_tree.h"
#include <string.h>
#include <assert.h>

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

SearchResult search_key(const Item *item, const Node *node) {
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

bool contains(const Item *item, const Tree *tree) {
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

static void insert_key(const Item *item, int pos, Node *node) {
    memmove(&node->keys[pos + 1], &node->keys[pos], sizeof(Item) * (node->key_count - pos));
    node->keys[pos] = *item;
    node->key_count++;
    return;
}

static void insert_child(const Node *child, int pos, Node *node) {
    memmove(&node->children[pos + 1], &node->children[pos], sizeof(Node *) * (node->key_count + 1 - pos));
    node->children[pos] = child;
    return;
}

static Item remove_key(Node *node, int pos) {
    Item val = node->keys[pos];
    memmove(&node->keys[pos], &node->keys[pos + 1], sizeof(Item) * (node->key_count - pos - 1));
    node->key_count--;
    return val;
}

typedef struct {
    Item promoted_key;
    Node *right;
} SplitResult;

SplitResult split(Node *node, const Item *item, int item_pos, int m) {
    SplitResult res = { 0 };
    Node *right = create_node(m, node->leaf);

    if (right == NULL) {
        return res;
    }

    int sep = node->key_count / 2;

    if (item_pos == sep) {
        res.promoted_key = *item;
    } else if (item_pos < sep) {
        res.promoted_key = remove_key(node, sep - 1);
        insert_key(item, item_pos, node);
    } else {
        res.promoted_key = remove_key(node, sep);
        insert_key(item, item_pos - 1, node);
    }

    memcpy(right->keys, &node->keys[sep], sizeof(Item) * (node->key_count - sep));
    right->key_count = node->key_count - sep;
    node->key_count = sep;
    res.right = right;
    return res; 
}

typedef struct {
    bool ok;
    bool split;
    SplitResult split_result;
} InsertResult;

InsertResult insert(const Item *item, Node *node, int m) {
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
            res.split_result = split(node, item, search.pos, m);
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

    SearchResult search_promoted = search_key(inner_res.split_result.promoted_key, node);

    if (node->key_count == m - 1) {
        res.split_result = split(node, inner_res.split_result.promoted_key, search_promoted.pos, m);
        res.ok = true;
        res.split = res.split_result.right != NULL;
        return res;
    }

    insert_key(inner_res.split_result.promoted_key, search_promoted.pos, node);
    insert_child(inner_res.split_result.right, search_promoted.pos + 1, node);
    res.ok = true;
    return res;
}
