#include <stdio.h>
#include <stdlib.h>

__global__ void vector_add(float* a, float* b, float* c, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        c[i] = a[i] + b[i];
    }
}

int main() {
    int n = 2 << 20;
    size_t bytes = n * sizeof(float);

    float* host_a = (float*)malloc(bytes);
    float* host_b = (float*)malloc(bytes);
    float* host_c = (float*)malloc(bytes);

    for (int i = 0; i < n; i++) {
        host_a[i] = 10;
        host_b[i] = 20;
    }

    float* device_a;
    float* device_b;
    float* device_c;

    cudaMalloc(&device_a, bytes);
    cudaMalloc(&device_b, bytes);
    cudaMalloc(&device_c, bytes);
    cudaMemcpy(device_a, host_a, bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(device_b, host_b, bytes, cudaMemcpyHostToDevice);

    int threads = 256;
    int blocks = (n + threads - 1) / threads;

    vector_add<<<blocks, threads>>>(device_a, device_b, device_c, n);
    cudaDeviceSynchronize();

    cudaMemcpy(host_c, device_c, bytes, cudaMemcpyDeviceToHost);

    // for (int i = 0; i < n; i++) {
    //     printf("%f ", host_c[i]);
    // }

    cudaFree(device_a);
    cudaFree(device_b);
    cudaFree(device_c);

    free(host_a);
    free(host_b);
    free(host_c);

    return 0;
}

