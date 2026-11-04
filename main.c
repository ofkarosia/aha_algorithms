#include <stdio.h>
#include <stdint.h>
#include "shared.h"
#include "b_tree.h"

bool print_item(const Item *item, void *ctx) {
    (void) ctx;
    printf("%d ", *item);
    return true;
}

// void print_rb_item(Item item, const Node *node) {
//     const char *s;

//     if (node->color == RED) {
//         s = "R";
//     } else {
//         s = "B";
//     }

//     printf("%d(%s) ", node->item, s);
// }

int main(void) {
    Tree tree = new_tree(5);

    // int arr[] = {
    //     50, 30, 80, 60, 70
    // };

    // int arr[] = {
    //     79, 74, 86,
    //     3, 10, 14,
    //     95, 100
    // };

    // int arr[] = {
    //     14, 9, 5,
    //     17, 11, 12,
    //     7, 19, 16,
    //     27, 18, 8, 23
    // };

    int arr[] = {
        17, 18, 23, 34,
        27, 15, 9, 6, 8,
        5, 25
    };

    // int arr[] = {
    //     1, 4, 3, 2
    // };

    size_t len = sizeof arr / sizeof(int);

    for (int i = 0; i < len; i++) {
        insert_item(&arr[i], &tree);
    }

    printf("Size: %d\n", tree.size);
    traverse(&tree, print_item, NULL);
}
