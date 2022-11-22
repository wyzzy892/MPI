#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <cmath>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int N = 10;

    if (N % size)
        N += (N % size);

    int procN = N / size;
    int* recv = new int[procN];
    int* x = 0;
    int result = 0;

    if (rank == 0) {
        x = new int[N];
        srand(time(0));
        for (int i = 0; i < N; i++) {
            x[i] = rand() % 10;
        }

        printf("=====================================\n");
        printf("Start. This is process %d\nx = ", rank);
        for (int i = 0; i < N; i++) {
            printf("%d ", x[i]);
        }
        printf("\n=====================================\n\n");
    }

    MPI_Scatter(x, procN, MPI_INT, recv, procN, MPI_INT, 0, MPI_COMM_WORLD);


    printf("This is process %d\nrecv = ", rank);
    for (int i = 0; i < procN; i++) {
        printf("%d ", recv[i]);
    }
    printf("\n\n");

    int procSum = 0;
    for (int i = 0; i < procN; i++)
        procSum += recv[i] * recv[i];

    MPI_Reduce(&procSum, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("=====================================\n");
        printf("End. This is process %d\nVector norm = %f\n", rank, sqrt(result));
        printf("=====================================\n\n");
    }

    delete[] recv;

    MPI_Finalize();

    return EXIT_SUCCESS;
}