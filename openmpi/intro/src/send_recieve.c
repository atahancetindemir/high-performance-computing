#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int count = 1, master = 0;
    int buff = 123, tag = 456; // arbitrary
    MPI_Status status;

    if (world_rank == master) {
        for (int i = 1; i < world_size; i++) {
            MPI_Send(&buff, count, MPI_INT, i, tag, MPI_COMM_WORLD);
            printf("%d sends '%d' to %d\n", world_rank, buff, i);
        }
    } else {
        MPI_Recv(&buff, count, MPI_INT, master, tag, MPI_COMM_WORLD, &status);
        printf("%d receives '%d' from %d\n", world_rank, buff, master);
    }

    MPI_Finalize();
    return 0;
}

/*
mpicxx send_recieve.c -o send_recieve
mpiexec -n 6 ./send_recieve

0 sends '123' to 1
0 sends '123' to 2
0 sends '123' to 3
0 sends '123' to 4
0 sends '123' to 5
1 receives '123' from 0
3 receives '123' from 0
5 receives '123' from 0
2 receives '123' from 0
4 receives '123' from 0
*/
