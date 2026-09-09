#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int size, rank, master = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int send_count = 10; // arbitrary
    int recv_count = send_count;
    int send_size = size * send_count;

    int recv_buff[send_count];
    int send_buff[send_size];

    if (rank == master) {
        for (int i = 0; i < send_size; i++) {
            send_buff[i] = i + 1;
        }
    }

    MPI_Scatter(send_buff, send_count, MPI_INT, recv_buff, recv_count, MPI_INT, master, MPI_COMM_WORLD);

    printf("Rank %d recieved: ", rank);
    for (int i = 0; i < recv_count; i++) {
        printf("%d ", recv_buff[i]);
    }
    printf("\n");

    MPI_Finalize();
}

/*
mpicxx scatter.c -o scatter
mpiexec -n 6 ./scatter

Rank 4 recieved: 41 42 43 44 45 46 47 48 49 50
Rank 5 recieved: 51 52 53 54 55 56 57 58 59 60
Rank 0 recieved: 1 2 3 4 5 6 7 8 9 10
Rank 1 recieved: 11 12 13 14 15 16 17 18 19 20
Rank 2 recieved: 21 22 23 24 25 26 27 28 29 30
Rank 3 recieved: 31 32 33 34 35 36 37 38 39 40
*/