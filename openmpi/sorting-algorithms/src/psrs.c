#include <mpi.h>
#include <string.h>
#include "sort.h"

// Hardware: Intel Core i5-12400F

// Parallel Regular Sampling Sort (PSRS)
// Hybrid Approach: Dynamic Scattering + Local Quick Sort + All-to-All Exchange + K-way Merge


// Binary tree k-way merge: O(total * log n)
static uint32_t* kway_merge(uint32_t** ptrs, uint32_t* lens, int n) {
    if (n == 0) return arr_create(1);
    if (n == 1) {
        uint32_t* out = arr_create(lens[0]);
        memcpy(out, ptrs[0], lens[0] * sizeof(uint32_t));
        return out;
    }

    uint32_t* cur_ptrs[32];
    uint32_t cur_lens[32];
    int is_heap[32];

    for (int i = 0; i < n; i++) {
        cur_ptrs[i] = ptrs[i];
        cur_lens[i] = lens[i];
        is_heap[i] = 0;
    }

    while (n > 1) {
        uint32_t* nxt_ptrs[32];
        uint32_t nxt_lens[32];
        int nxt_heap[32];
        int nn = 0;

        for (int i = 0; i + 1 < n; i += 2) {
            nxt_ptrs[nn] = merge_arrays(cur_ptrs[i], cur_lens[i], cur_ptrs[i+1], cur_lens[i+1]);
            nxt_lens[nn] = cur_lens[i] + cur_lens[i+1];
            nxt_heap[nn] = 1;
            if (is_heap[i])   free(cur_ptrs[i]);
            if (is_heap[i+1]) free(cur_ptrs[i+1]);
            nn++;
        }
        if (n % 2 == 1) {
            nxt_ptrs[nn] = cur_ptrs[n-1];
            nxt_lens[nn] = cur_lens[n-1];
            nxt_heap[nn] = is_heap[n-1];
            nn++;
        }

        for (int i = 0; i < nn; i++) {
            cur_ptrs[i] = nxt_ptrs[i];
            cur_lens[i] = nxt_lens[i];
            is_heap[i] = nxt_heap[i];
        }
        n = nn;
    }

    return cur_ptrs[0];
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) fprintf(stderr, "Usage: mpirun -np <procs> %s <n>\n", argv[0]);
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    int rem = n % size;
    int local_n = n / size + (rank < rem ? 1 : 0);

    uint32_t* arr = NULL;
    uint32_t* local_arr = (uint32_t*)malloc(local_n * sizeof(uint32_t));

    int* sendcounts = NULL;
    int* displs = NULL;

    if (rank == 0) {
        arr = arr_create((uint32_t)n);
        arr_init(arr, (uint32_t)n);
#ifdef DBG
        printf("Original array:\n");
        arr_display(arr, (uint32_t)n);
#endif
        sendcounts = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));
        int offset = 0;
        for (int i = 0; i < size; i++) {
            sendcounts[i] = n / size + (i < rem ? 1 : 0);
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
        free(arr);
    }

    // Local Sorting
    if (local_n > 1)
        sort_quick(local_arr, 0, local_n - 1);

    // Regular Sampling: each process picks 'size' evenly spaced pivots
    uint32_t* local_samples = (uint32_t*)malloc(size * sizeof(uint32_t));
    for (int i = 0; i < size; i++)
        local_samples[i] = local_arr[(long long)i * local_n / size];

    uint32_t* all_samples = (uint32_t*)malloc(size * size * sizeof(uint32_t));
    MPI_Allgather(local_samples, size, MPI_UINT32_T, all_samples, size, MPI_UINT32_T, MPI_COMM_WORLD);
    free(local_samples);

    // Splitter Selection: sort p^2 samples, pick p-1 splitters at every p-th position
    sort_quick(all_samples, 0, size * size - 1);
    uint32_t* splitters = (uint32_t*)malloc((size - 1) * sizeof(uint32_t));
    for (int i = 1; i < size; i++) {
        splitters[i - 1] = all_samples[i * size];
    }
    free(all_samples);

    // Partitioning
    int* send_counts = (int*)calloc(size, sizeof(int));
    for (int i = 0; i < local_n; i++) {
        int lo = 0, hi = size - 2, b = size - 1;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (local_arr[i] <= splitters[mid]) {
                b = mid;
                hi = mid - 1;
            } else {
                lo = mid + 1;
            }  
        }
        send_counts[b]++;
    }
    free(splitters);

    int* send_displs = (int*)malloc(size * sizeof(int));
    send_displs[0] = 0;
    for (int i = 1; i < size; i++) {
        send_displs[i] = send_displs[i - 1] + send_counts[i - 1];
    }

    // All-to-All Exchange
    int* recv_counts = (int*)malloc(size * sizeof(int));
    MPI_Alltoall(send_counts, 1, MPI_INT, recv_counts, 1, MPI_INT, MPI_COMM_WORLD);

    int* recv_displs = (int*)malloc(size * sizeof(int));
    recv_displs[0] = 0;
    int total_recv  = recv_counts[0];
    for (int i = 1; i < size; i++) {
        recv_displs[i] = recv_displs[i - 1] + recv_counts[i - 1];
        total_recv += recv_counts[i];
    }

    uint32_t* recv_arr = arr_create((uint32_t)total_recv);
    MPI_Alltoallv(local_arr, send_counts, send_displs, MPI_UINT32_T, recv_arr,  recv_counts, recv_displs, MPI_UINT32_T, MPI_COMM_WORLD);

    free(local_arr);
    free(send_counts);
    free(send_displs);

    // K-way Merge (log p)
    uint32_t* chunk_ptrs[32];
    uint32_t chunk_lens[32];
    int nchunks = 0;
    for (int i = 0; i < size; i++) {
        if (recv_counts[i] > 0) {
            chunk_ptrs[nchunks] = recv_arr + recv_displs[i];
            chunk_lens[nchunks] = (uint32_t)recv_counts[i];
            nchunks++;
        }
    }
    free(recv_counts);
    free(recv_displs);

    uint32_t* sorted = kway_merge(chunk_ptrs, chunk_lens, nchunks);
    free(recv_arr);

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

#ifdef DBG
    if (rank == 0) {
        printf("Sorted array:\n");
        arr_display(sorted, (uint32_t)total_recv);
    }
#endif

    if (rank == 0) {
        printf("Time taken: %f seconds\n", end_time - start_time);
    }
        
    free(sorted);
    MPI_Finalize();
    return 0;
}

// mpicc -O3 -march=native -Wall -Wextra sort.c psrs.c -o psrs

// mpiexec -np 1 ./psrs 134217728
// Time taken: 12.647341 seconds

// mpiexec -np 4 ./psrs 134217728
// Time taken: 4.084884 seconds

// mpiexec -np 6 ./psrs 134217728
// Time taken: 3.034929 seconds

// mpiexec --use-hwthread-cpus -np 8 ./psrs 134217728
// Time taken: 2.283560 seconds

// mpiexec --use-hwthread-cpus -np 12 ./psrs 134217728
// Time taken: 1.847167 seconds