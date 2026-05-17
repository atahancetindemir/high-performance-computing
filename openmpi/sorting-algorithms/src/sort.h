#ifndef SORT_H
#define SORT_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#if defined(BUBBLE)
    #define SORT(arr, n)       sort_bubble(arr, n)
#elif defined(SELECTION)
    #define SORT(arr, n)       sort_selection(arr, n)
#elif defined(QUICK)
    #define SORT(arr, n)       sort_quick(arr, 0, n - 1)
#elif defined(MERGE)
    #define SORT(arr, n)       sort_merge(arr, 0, n - 1)
#elif defined(BITONIC)
    #define SORT(arr, n)       sort_bitonic(arr, 0, n, 1)
#else
    #define SORT(arr, n)       sort_bubble(arr, n)
#endif

int is_power_of_two(uint32_t n);
void swap(uint32_t* a, uint32_t* b);
uint32_t xorshift32(uint32_t *state);

void arr_display(uint32_t* arr, uint32_t n);
uint32_t* arr_create(uint32_t n);
void arr_free(uint32_t* arr);
void arr_init(uint32_t* arr, uint32_t n);

void bitonic_merge(uint32_t* arr, uint32_t low, uint32_t cnt, int dir);
uint32_t* merge_arrays(uint32_t* arr1, uint32_t n1, uint32_t* arr2, uint32_t n2);
void merge(uint32_t* arr, uint32_t l, uint32_t m, uint32_t r);

void sort_bubble(uint32_t* arr, uint32_t n);
void sort_selection(uint32_t* arr, uint32_t n);
void sort_quick(uint32_t* arr, int low, int high);
void sort_merge(uint32_t* arr, uint32_t l, uint32_t r);
void sort_bitonic(uint32_t* arr, uint32_t low, uint32_t cnt, int dir);

#endif // SORT_H