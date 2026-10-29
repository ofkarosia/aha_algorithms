#include <stdio.h>
#include <stdint.h>
#include "shared.h"
#include "binary_search_tree.h"

void print_item(Item item) {
    printf("%d ", item);
}

int main(void) {
    Tree tree = new_tree();

    // int arr[] = {
    //     50, 30, 80, 60, 70
    // };

    // int arr[] = {
    //     79, 74, 86,
    //     3, 10, 14,
    //     95, 100
    // };

    int arr[] = {
        14, 9, 5,
        17, 11, 12,
        7, 19, 16,
        27, 18, 8, 23
    };

    size_t len = sizeof arr / sizeof(int);

    for (int i = 0; i < len; i++) {
        insert_item(&arr[i], &tree);
    }

    delete_item(&(int){16}, &tree);
    delete_item(&(int){5}, &tree);

    pre_order(tree.root, print_item);
}
