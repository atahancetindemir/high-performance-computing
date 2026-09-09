#include <stdio.h>

__global__ void hello_world() {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    printf("Hello world from block: %d thread: %d global: %d\n", blockIdx.x, threadIdx.x, i);
}

int main() {
    int block = 3;
    int thread = 4;

    hello_world<<<block, thread>>>();

    cudaDeviceSynchronize();
    return 0;
}

/*

nvcc hello_world.cu -o hello_world
./hello_world

Hello world from block: 0 thread: 0 global: 0
Hello world from block: 0 thread: 1 global: 1
Hello world from block: 0 thread: 2 global: 2
Hello world from block: 0 thread: 3 global: 3
Hello world from block: 1 thread: 0 global: 4
Hello world from block: 1 thread: 1 global: 5
Hello world from block: 1 thread: 2 global: 6
Hello world from block: 1 thread: 3 global: 7
Hello world from block: 2 thread: 0 global: 8
Hello world from block: 2 thread: 1 global: 9
Hello world from block: 2 thread: 2 global: 10
Hello world from block: 2 thread: 3 global: 11

*/