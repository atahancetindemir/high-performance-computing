#include "sort.h"

int is_power_of_two(uint32_t n) {
    return n && (!(n & (n - 1)));
}

void swap(uint32_t* a, uint32_t* b) {
    uint32_t temp = *a;
    *a = *b;
    *b = temp;
}

uint32_t xorshift32(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

void arr_display(uint32_t* arr, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        printf("%u ", arr[i]);
    }
    printf("\n");
}

uint32_t* arr_create(uint32_t n) {
    uint32_t* arr = (uint32_t*)malloc(n * sizeof(uint32_t));
    if (!arr) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return arr;
}

void arr_free(uint32_t* arr) {
    free(arr);
}

void arr_init(uint32_t* arr, uint32_t n) {
    uint32_t seed = 54;
    for (uint32_t i = 0; i < n; i++) {
        arr[i] = xorshift32(&seed);
    }
}

void bitonic_merge(uint32_t* arr, uint32_t low, uint32_t cnt, int dir) {
    if (cnt > 1) {
        uint32_t k = cnt / 2;
        for (uint32_t i = low; i < low + k; i++) {
            if (dir == (arr[i] > arr[i + k])) {
                swap(&arr[i], &arr[i + k]);
            }
        }
        bitonic_merge(arr, low, k, dir);
        bitonic_merge(arr, low + k, k, dir);
    }
} 

uint32_t* merge_arrays(uint32_t* arr1, uint32_t n1, uint32_t* arr2, uint32_t n2) {
    uint32_t* result = arr_create(n1 + n2);
    uint32_t i = 0, j = 0, k = 0;
    while (i < n1 && j < n2) {
        if (arr1[i] <= arr2[j]) result[k++] = arr1[i++];
        else result[k++] = arr2[j++];
    }
    while (i < n1) result[k++] = arr1[i++];
    while (j < n2) result[k++] = arr2[j++];
    return result;
}

void merge(uint32_t* arr, uint32_t l, uint32_t m, uint32_t r) {
    uint32_t i, j, k;
    uint32_t n1 = m - l + 1;
    uint32_t n2 = r - m;

    uint32_t* L = (uint32_t*)malloc(n1 * sizeof(uint32_t));
    uint32_t* R = (uint32_t*)malloc(n2 * sizeof(uint32_t));

    for (i = 0; i < n1; i++) L[i] = arr[l + i];
    for (j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    i = 0; j = 0; k = l;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void sort_bubble(uint32_t* arr, uint32_t n) {
    for (uint32_t i = 0; i < n - 1; i++) {
        for (uint32_t j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

void sort_selection(uint32_t* arr, uint32_t n) {
    for (uint32_t i = 0; i < n - 1; i++) {
        uint32_t min_idx = i;
        for (uint32_t j = i + 1; j < n; j++) {
            min_idx = (arr[j] < arr[min_idx]) ? j : min_idx;
        }
        if (min_idx != i) {
            swap(&arr[i], &arr[min_idx]);
        }
    }
}

void sort_quick(uint32_t* arr, int low, int high) {
    if (low < high) {
        uint32_t pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (arr[j] < pivot) {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
        swap(&arr[i + 1], &arr[high]);
        int pi = i + 1;
        sort_quick(arr, low, pi - 1);
        sort_quick(arr, pi + 1, high);
    }
}

void sort_merge(uint32_t* arr, uint32_t l, uint32_t r) {
    if (l < r) {
        uint32_t m = l + (r - l) / 2; 
        sort_merge(arr, l, m);
        sort_merge(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void sort_bitonic(uint32_t* arr, uint32_t low, uint32_t cnt, int dir) {
    if (!is_power_of_two(cnt)) {
        fprintf(stderr, "Error: Bitonic sort requires the number of elements to be a power of two.\n");
        exit(EXIT_FAILURE);
    }
    if (cnt > 1) {
        uint32_t k = cnt / 2;
        sort_bitonic(arr, low, k, 1);
        sort_bitonic(arr, low + k, k, 0);
        bitonic_merge(arr, low, cnt, dir);
    }
}
