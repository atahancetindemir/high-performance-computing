#include <time.h>

#include "sort.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_elements>\n", argv[0]);
        return EXIT_FAILURE;
    }
    uint32_t n = atoi(argv[1]);
    uint32_t* arr = arr_create(n);
    arr_init(arr, n);

#ifdef DBG
    printf("Original array:\n");
    arr_display(arr, n);
#endif

    clock_t start = clock();
    SORT(arr, n);
    clock_t end = clock();

#ifdef DBG
    printf("Sorted array:\n");
    arr_display(arr, n);
#endif

    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", time_taken);

    arr_free(arr);
    return 0;
}

/*

gcc -O3 -march=native -Wall -Wextra sort.c serial.c -o serial -DBUBBLE

./serial 1048576
Time taken: 1517.855727 seconds

./serial 524288
Time taken: 380.528176 seconds

./serial 262144
Time taken: 94.307143 seconds



gcc -O3 -march=native -Wall -Wextra sort.c serial.c -o serial -DSELECTION

./serial 1048576
Time taken: 235.130490 seconds

./serial 524288
Time taken: 60.663000 seconds

./serial 262144
Time taken: 14.634935 seconds



gcc -O3 -march=native -Wall -Wextra sort.c serial.c -o serial -DQUICK

./serial 134217728
Time taken: 12.255832 seconds

./serial 16777216
Time taken: 1.356106 seconds

./serial 1048576
Time taken: 0.069462 seconds

./serial 524288
Time taken: 0.032576 seconds

./serial 262144
Time taken: 0.016156 seconds



gcc -O3 -march=native -Wall -Wextra sort.c serial.c -o serial -DMERGE

./serial 134217728
Time taken: 18.652145 seconds

./serial 16777216
Time taken: 2.091175 seconds

./serial 1048576
Time taken: 0.105812 seconds

./serial 524288
Time taken: 0.053425 seconds

./serial 262144
Time taken: 0.026305 seconds



gcc -O3 -march=native -Wall -Wextra sort.c serial.c -o serial -DBITONIC

./serial 134217728
Time taken: 51.994917 seconds

./serial 16777216
Time taken: 5.411257 seconds

./serial 1048576
Time taken: 0.259196 seconds

./serial 524288
Time taken: 0.123132 seconds

./serial 262144
Time taken: 0.060352 seconds

*/