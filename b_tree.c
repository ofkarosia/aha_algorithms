#include "b_tree.h"
#include <string.h>

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

static void insert_child(Node *child, int pos, Node *node) {
    memmove(&node->children[pos + 1], &node->children[pos], sizeof(Node *) * (node->key_count - pos));
    node->children[pos] = child;
}

static Item remove_key(int pos, Node *node) {
    Item key = node->keys[pos];
    memmove(&node->keys[pos], &node->keys[pos + 1], sizeof(Item) * (node->key_count - pos - 1));
    node->key_count--;
    return key;
}

static Node *remove_child(int pos, Node *node) {
    Node *removed = node->children[pos];
    memmove(&node->children[pos], &node->children[pos + 1], sizeof(Node *) * (node->key_count - pos - 1));
    return removed;
}

static void free_node(Node *node) {
    free(node->children);
    free(node->keys);
    free(node);
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

    insert_key(&inner_res.split_result.promoted_key, search_promoted.pos, node);
    insert_child(inner_res.split_result.right, search_promoted.pos + 1, node);
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

static Item *seek_min_key(Node *node) {

}

typedef struct {
    bool ok;
    bool underflow;
} DeleteResult;

static DeleteResult delete(Item *item, Node *node, int m) {
    int min_keys = (m + 1) / 2 - 1;
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

        res.underflow = true;
        return res;
    }

    int child_pos = search.pos;

    if (search.found) {
        Item *successor = seek_min_key(node->children[search.pos + 1]);
        node->keys[search.pos] = *successor;
        child_pos = search.pos + 1;
    }

    Node *child = node->children[child_pos];
    DeleteResult inner_res = delete(item, child, m);

    if (!inner_res.ok) {
        return res;
    }

    if (!inner_res.underflow) {
        res.ok = true;
        return res;
    }

    Node *left_s = get_child_s(node, child_pos - 1);
    Node *right_s = get_child_s(node, child_pos + 1);

    if (left_s != NULL) {
        if (left_s->key_count > min_keys) {
            insert_key(node->keys[child_pos - 1], 0, child);
            insert_child(left_s->children[left_s->key_count], 0, child);
            node->keys[child_pos - 1] = remove_key(left_s->key_count - 1, left_s);
        } else {
            insert_key(node->keys[child_pos - 1], left_s->key_count, left_s);
            memcpy(&left_s->keys[left_s->key_count], child->keys, sizeof(Item) * child->key_count);
            memcpy(&left_s->children[left_s->key_count + 1], child->children, sizeof(Node *) * (child->key_count + 1));
            left_s->key_count += child->key_count;
            free_node(remove_child(child_pos, node));
            remove_key(child_pos - 1, node);
        }
    } else if (right_s != NULL) {
        if (right_s->key_count > min_keys) {
            insert_key(node->keys[child_pos], child->key_count, child);
            insert_child(right_s->children, child->key_count, child);
            node->keys[child_pos] = remove_key(0, right_s);
        } else {
            insert_key(node->keys[child_pos], child->key_count, child);
            memcpy(&child->keys[child->key_count], right_s->keys, sizeof(Item) * right_s->key_count);
            memcpy(&child->children[child->key_count + 1], right_s->children, sizeof(Node *) * (right_s->key_count + 1));
            child->key_count += right_s->key_count;
            free_node(remove_child(child_pos + 1, node));
            remove_key(child_pos, node);
        }
    }

    res.ok = true;
    res.underflow = node->key_count < min_keys;
    return res;
}

bool delete_item(Item *item, Tree *tree) {

}
