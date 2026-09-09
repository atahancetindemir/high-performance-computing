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

    /*
    NOTE FOR FUTURE REFERENCE:
    For the sake of simplicity and keeping the learning curve gentle,
    we are using static arrays here for all processes.
    However, the proper/optimized way to handle large datasets in MPI
    is to use pointers and dynamically allocate memory (using malloc/free)
    ONLY on the master process. This prevents wasting RAM on worker processes.
    */

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

    float local_sum = 0, local_avg;
    for (int i = 0; i < recv_count; i++) {
        local_sum += recv_buff[i];
    }
    local_avg = local_sum / send_count;
    printf("Rank %d average: %f\n", rank, local_avg);

    float avgs[size];

    MPI_Gather(&local_avg, 1, MPI_FLOAT, avgs, 1, MPI_FLOAT, master, MPI_COMM_WORLD);

    // Reduce can also be used to compute the global average without gathering
    // all local averages to the master, but we are using gather here to
    // demonstrate the use of both scatter and gather in the same program.
    if (rank == master) {
        float global_sum = 0, global_avg;
        for (int i = 0; i < size; i++) {
            global_sum += avgs[i];
        }
        global_avg = global_sum / size;

        printf("Global Average: %f\n", global_avg);
    }

    MPI_Finalize();
}

/*
mpicxx gather.c -o gather
mpiexec -n 6 ./gather

Rank 0 recieved: 1 2 3 4 5 6 7 8 9 10
Rank 0 average: 5.500000
Rank 1 recieved: 11 12 13 14 15 16 17 18 19 20
Rank 1 average: 15.500000
Rank 3 recieved: 31 32 33 34 35 36 37 38 39 40
Rank 3 average: 35.500000
Rank 4 recieved: 41 42 43 44 45 46 47 48 49 50
Rank 4 average: 45.500000
Rank 5 recieved: 51 52 53 54 55 56 57 58 59 60
Rank 5 average: 55.500000
Rank 2 recieved: 21 22 23 24 25 26 27 28 29 30
Rank 2 average: 25.500000
Global Average: 30.500000
*/