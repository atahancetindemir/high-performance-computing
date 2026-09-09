#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int size, rank, master = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int buff[size];
    if (rank == master) {
        for (int i = 0; i < size; i++) {
            buff[i] = i + 1;
        }
    }

    MPI_Bcast(buff, size, MPI_INT, master, MPI_COMM_WORLD);

    printf("Rank %d recieved: ", rank);
    for (int i = 0; i < size; i++) {
        printf("%d ", buff[i]);
    }
    printf("\n");

    MPI_Finalize();
}

/*
mpicxx broadcast.c -o broadcast
mpiexec -n 6 ./broadcast

Rank 2 recieved: 1 2 3 4 5 6
Rank 4 recieved: 1 2 3 4 5 6
Rank 5 recieved: 1 2 3 4 5 6
Rank 0 recieved: 1 2 3 4 5 6
Rank 3 recieved: 1 2 3 4 5 6
Rank 1 recieved: 1 2 3 4 5 6
*/