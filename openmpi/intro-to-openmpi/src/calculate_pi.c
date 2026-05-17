#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int size, rank;
    unsigned long long rect;
    float start_time, end_time;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        rect = 48000000000ULL;
        start_time = MPI_Wtime();
    }

    MPI_Bcast(&rect, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

    double start = 0.0;
    double stop = 1.0;
    double interval = (stop - start) / (double)rect;

    unsigned int local_n = rect / size; 
    unsigned long long local_start = (unsigned long long)rank * local_n; 
    double x_base = start + (local_start + 0.5) * interval;
    
    double local_sum = 0.0;

    #pragma omp simd reduction(+:local_sum)
    for (unsigned int i = 0; i < local_n; i++) {
        double x = x_base + i * interval;
        local_sum += 4.0 / (1.0 + x * x);  
    }
    local_sum *= interval;

    double global_sum;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        end_time = MPI_Wtime();
        printf("Area: %.15lf\n", global_sum);
        printf("Time: %.6f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}

/*
mpicc -O3 -march=native -ffast-math -fopenmp-simd calculate_pi.c -o calculate_pi
mpiexec --use-hwthread-cpus --bind-to hwthread -n 12 ./calculate_pi

Area: 3.141592653589789
Time: 4.230324 seconds
*/