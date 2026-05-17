#include <mpi.h>

#include "sort.h"

// Hardware: Intel Core i5-12400F

// Parallel Sorting using MPI
// Hybrid Approach: Dynamic Scattering + Tree Reduction Merge

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) fprintf(stderr, "Usage: mpirun -np <procs> %s <number_of_elements>\n", argv[0]);
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    uint32_t n = atoi(argv[1]);

    int remainder = n % size;
    uint32_t local_n = n / size + (rank < remainder ? 1 : 0);
    
    uint32_t* arr = NULL;
    uint32_t* local_arr = arr_create(local_n);

    int* sendcounts = NULL;
    int* displs = NULL;

    if (rank == 0) {
        arr = arr_create(n);
        arr_init(arr, n);
#ifdef DBG
        printf("Original array:\n");
        arr_display(arr, n);
#endif
        sendcounts = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));
        
        int offset = 0;
        for (int i = 0; i < size; i++) {
            sendcounts[i] = n / size + (i < remainder ? 1 : 0);
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // Dynamic Scattering
    MPI_Scatterv(arr, sendcounts, displs, MPI_UINT32_T, local_arr, local_n, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        free(sendcounts);
        free(displs);
    }

    // Local Sorting
    SORT(local_arr, local_n);

    // Tree Reduction Merge (logP)
    uint32_t current_local_n = local_n;
    uint32_t* current_arr = local_arr;

    int step = 1;
    while (step < size) {
        if (rank % (2 * step) == 0) {
            if (rank + step < size) {
                MPI_Status status;
                int recv_count;
                MPI_Probe(rank + step, 0, MPI_COMM_WORLD, &status);
                MPI_Get_count(&status, MPI_UINT32_T, &recv_count);

                // Get Data
                uint32_t* recv_arr = arr_create(recv_count);
                MPI_Recv(recv_arr, recv_count, MPI_UINT32_T, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Merge
                uint32_t* merged_arr = merge_arrays(current_arr, current_local_n, recv_arr, recv_count);

                // Free Old Memory
                if (current_arr != local_arr) free(current_arr); 
                else if (step > 1) free(current_arr);
                if (current_arr == local_arr) free(local_arr);
                
                free(recv_arr);
                current_arr = merged_arr;
                current_local_n += recv_count;
            }
        } else {
            // Send Data
            MPI_Send(current_arr, current_local_n, MPI_UINT32_T, rank - step, 0, MPI_COMM_WORLD);
            if (current_arr != local_arr) free(current_arr);
            else free(local_arr);
            break;
        }
        step *= 2;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    // Display Results and Clear
    if (rank == 0) {
#ifdef DBG
        printf("Sorted array:\n");
        arr_display(current_arr, n);
#endif
        printf("Time taken: %f seconds\n", end_time - start_time);
        
        free(current_arr);
        if (arr) free(arr);
    }

    MPI_Finalize();
    return 0;
}

/*

mpicc -O3 -march=native -Wall -Wextra sort.c parallel.c -o parallel -DBUBBLE

mpiexec -np 4 ./parallel 1048576
Time taken: 105.836470 seconds

mpiexec -np 6 ./parallel 1048576
Time taken: 49.565675 seconds

mpiexec --use-hwthread-cpus -np 8 ./parallel 1048576
Time taken: 29.065768 seconds

mpiexec --use-hwthread-cpus -np 12 ./parallel 1048576
Time taken: 16.246353 seconds



mpicc -O3 -march=native -Wall -Wextra sort.c parallel.c -o parallel -DSELECTION

mpiexec -np 4 ./parallel 1048576
Time taken: 18.788378 seconds

mpiexec -np 6 ./parallel 1048576
Time taken: 9.256772 seconds

mpiexec --use-hwthread-cpus -np 8 ./parallel 1048576
Time taken: 5.674420 seconds

mpiexec --use-hwthread-cpus -np 12 ./parallel 1048576
Time taken: 3.110608 seconds



mpicc -O3 -march=native -Wall -Wextra sort.c parallel.c -o parallel -DQUICK

mpiexec -np 4 ./parallel 134217728
Time taken: 4.830536 seconds

mpiexec -np 6 ./parallel 134217728
Time taken: 4.366539 seconds

mpiexec --use-hwthread-cpus -np 8 ./parallel 134217728
Time taken: 3.623892 seconds

mpiexec --use-hwthread-cpus -np 12 ./parallel 134217728
Time taken: 3.807343 seconds



mpicc -O3 -march=native -Wall -Wextra sort.c parallel.c -o parallel -DMERGE

mpiexec -np 4 ./parallel 134217728
Time taken: 6.665847 seconds

mpiexec -np 6 ./parallel 134217728
Time taken: 5.469084 seconds

mpiexec --use-hwthread-cpus -np 8 ./parallel 134217728
Time taken: 3.761117 seconds

mpiexec --use-hwthread-cpus -np 12 ./parallel 134217728
Time taken: 4.143812 seconds



mpicc -O3 -march=native -Wall -Wextra sort.c parallel.c -o parallel -DBITONIC

mpiexec -np 4 ./parallel 134217728
Time taken: 15.953590 seconds

mpiexec --use-hwthread-cpus -np 8 ./parallel 134217728
Time taken: 9.701621 seconds

*/