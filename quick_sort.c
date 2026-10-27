#include "shared.h"

static void q_sort(int *arr, int left, int right) {
    int base = arr[left];
    int i = left;
    int j = right;

    if (left > right) {
        return;
    }

    while (i != j) {
        while (arr[j] >= base && i < j) {
            j--;
        }

        while (arr[i] <= base && i < j) {
            i++;
        }

        if (i < j) {
            int t = arr[i];
            arr[i] = arr[j];
            arr[j] = t;
        }
    }

    arr[left] = arr[i];
    arr[i] = base;

    q_sort(arr, left,  i - 1);
    q_sort(arr, i + 1, right);
}

void quick_sort(int *arr) {
    q_sort(arr, 0, ARRAY_LEN - 1);
}
