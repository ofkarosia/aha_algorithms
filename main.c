#include <stdio.h>
#include <stdint.h>
#include "shared.h"
#include "binary_search_tree.h"

void print_item(Item item) {
    printf("%d ", item);
}

int main(void) {
    Tree tree = new_tree();

    int arr[] = {
        50, 30, 80, 60, 70
    };

    size_t len = sizeof arr / sizeof(int);

    for (int i = 0; i < len; i++) {
        insert_item(&arr[i], &tree);
    }

    delete_item(&arr[0], &tree);

    pre_order(tree.root, print_item);
}
