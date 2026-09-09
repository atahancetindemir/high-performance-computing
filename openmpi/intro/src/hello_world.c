#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    printf("Hello from %d out of %d\n", world_rank, world_size);

    MPI_Finalize();
    return 0;
}

/*
mpicxx hello_world.c -o hello_world
mpiexec -n 6 ./hello_world

Hello from 1 out of 6
Hello from 5 out of 6
Hello from 2 out of 6
Hello from 4 out of 6
Hello from 3 out of 6
Hello from 0 out of 6
*/