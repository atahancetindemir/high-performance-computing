#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0) {
        printf("Hello from %d (master) out of %d\n", world_rank, world_size);
    } else {
        printf("Hello from %d (worker) out of %d\n", world_rank, world_size);
    }

    MPI_Finalize();
    return 0;
}

/*
mpicxx master_worker.c -o master_worker
mpiexec -n 6 ./master_worker

Hello from 1 (worker) out of 6
Hello from 2 (worker) out of 6
Hello from 3 (worker) out of 6
Hello from 4 (worker) out of 6
Hello from 5 (worker) out of 6
Hello from 0 (master) out of 6
*/