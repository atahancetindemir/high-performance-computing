#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int count = 1, master = 0, num = 0, sum = 0;
    int tag = 456; // arbitrary
    MPI_Status status;

    srand(time(NULL) + world_rank);

    if (world_rank != master) { // workers
        num = rand() % 101;     // 0 to 100
        MPI_Send(&num, count, MPI_INT, master, tag, MPI_COMM_WORLD);
        printf("%d sends '%d' to %d\n", world_rank, num, master);
    } else { // master
        num = rand() % 101;
        sum = num;
        printf("%d creates '%d'\n", master, num);

        for (int i = 1; i < world_size; i++) {
            MPI_Recv(&num, count, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
            sum += num;
            printf("%d receives '%d' from %d\n", master, num, i);
        }
        printf("total sum: %d\n", sum);
    }

    MPI_Finalize();
    return 0;
}

/*
mpicxx send_recieve_master.c -o send_recieve_master
mpiexec -n 6 ./send_recieve_master

0 creates '65'
0 receives '67' from 1
0 receives '18' from 2
0 receives '82' from 3
0 receives '73' from 4
0 receives '69' from 5
total sum: 374
1 sends '67' to 0
2 sends '18' to 0
3 sends '82' to 0
4 sends '73' to 0
5 sends '69' to 0
*/