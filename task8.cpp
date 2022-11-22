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
    int* bufX = new int[procN];
    int* bufY = new int[procN];
    int* x = 0;
    int* y = 0;
    int result = 0;

    if (rank == 0) {
        x = new int[N];
        y = new int[N];
        srand(time(0));
        for (int i = 0; i < N; i++) {
            x[i] = rand() % 10;
            y[i] = rand() % 10;
        }

        printf("=====================================\n");
        printf("Start. This is process %d\nx = ", rank);
        for (int i = 0; i < N; i++) {
            printf("%d ", x[i]);
        }
        printf("\ny = ");
        for (int i = 0; i < N; i++) {
            printf("%d ", y[i]);
        }
        printf("\n=====================================\n\n");
    }

    MPI_Scatter(x, procN, MPI_INT, bufX, procN, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(y, procN, MPI_INT, bufY, procN, MPI_INT, 0, MPI_COMM_WORLD);


    printf("This is process %d\nbufX = ", rank);
    for (int i = 0; i < procN; i++)
        printf("%d ", bufX[i]);
    printf("\nbufY = ");
    for (int i = 0; i < procN; i++)
        printf("%d ", bufY[i]);
    printf("\n\n");

    int procSum = 0;
    for (int i = 0; i < procN; i++)
        procSum += bufX[i] * bufY[i];

    MPI_Reduce(&procSum, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("=====================================\n");
        printf("End. This is process %d\nDot product = %d\n", rank, result);
        printf("=====================================\n\n");
    }

    delete[] bufX;
    delete[] bufY;

    MPI_Finalize();

    return EXIT_SUCCESS;
}