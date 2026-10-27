#include "shared.h"

void selection_sort(int *arr) {
    for (int i = 0; i < ARRAY_LEN - 1; i++) {
        int swap_index = i;

        for (int j = i + 1; j < ARRAY_LEN; j++) {
            if (arr[swap_index] > arr[j]) {
                swap_index = j;
            }
        }

        int t = arr[swap_index];
        arr[swap_index] = arr[i];
        arr[i] = t;
    }
}
