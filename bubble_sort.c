#include "shared.h"

void bubble_sort(int *arr) {
    for (int i = 1; i <= ARRAY_LEN - 1; i++) {
        for (int j = 0; j < ARRAY_LEN - i; j++) {
            int t = 0;

            if (arr[j] > arr[j+1]) {
                t = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = t;
            }
        }
    }
}
