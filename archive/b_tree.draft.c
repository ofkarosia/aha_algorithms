#include "b_tree.draft.h"
#include <string.h>
#include <assert.h>

Tree new_tree(int m) {
    assert(m >= 2);

    return (Tree) {
        .root = NULL,
        .m = m,
        .size = 0
    };
}

static Node *create_node(int m) {
    Node *node = malloc(sizeof(Node));

    if (node == NULL) {
        return NULL;
    }

    node->key_count = 0;
    node->keys = malloc(sizeof(Item) * (m - 1));

    if (node->keys == NULL) {
        free(node);
        return NULL;
    }

    node->children = calloc(m, sizeof(Node *));

    if (node->children == NULL) {
        free(node->keys);
        free(node);
        return NULL;
    }

    node->leaf = false;
    return node;
}

static int find_insert_pos(const Item *item, const Item *keys, int key_count) {
    int i = 0;

    while (i < key_count) {
        if (*item < keys[i]) {
            return i;
        }

        if (*item == keys[i]) {
            return -1;
        }

        i++;
    }

    return i;
}

static bool insert(const Item *item, Tree *tree) {
    Node **ppn = &tree->root;

    while (*ppn != NULL) {
        Node *node = *ppn;

        assert(node->key_count <= tree->m - 1);
        int pos = find_insert_pos(item, node->keys, node->key_count);

        if (pos < 0) {
            return false;
        }

        if (node->key_count < tree->m - 1) {
            assert(pos <= node->key_count);

            if (pos < node->key_count) {
                memmove(&node->keys[pos + 1], &node->keys[pos], sizeof(Item) * (node->key_count - pos));
            }

            node->keys[pos] = *item;
            node->key_count++;
            return true;
        }

        ppn = &node->children[pos];
    }

    *ppn = create_node(tree->m);

    if (*ppn == NULL) {
        return false;
    }

    (*ppn)->keys[0] = *item;
    (*ppn)->key_count++;
    return true;
}

bool insert_item(const Item *item, Tree *tree) {
    bool res = insert(item, tree);

    if (res) {
        tree->size++;
    }

    return res;
}
