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

    int procN = (N / size) + (N % size);
    int n = procN * size;
    float* x = 0;
    float* bufX = new float[procN];


    struct {
        float value;
        int index;
    } in, out;

    if (rank == 0) {
        x = new float[n];
        srand(time(0));

        printf("============================================\n");
        printf("Start. This is process %d\nx = ", rank);
        for (int i = 0; i < N; i++) {
            x[i] = ((float)(rand() % 1000)) / 100;
        }
        for (int i = N; i < n; i++) {
            x[i] = ((float)(rand() % 1000)) / 100;
        }
        for (int i = 0; i < n; i++) {
            printf("%.2f  ", x[i]);
        }

        printf("\n============================================\n\n");
    }

    MPI_Scatter(x, procN, MPI_FLOAT, bufX, procN, MPI_FLOAT, 0, MPI_COMM_WORLD);


    printf("This is process %d\nbufX = ", rank);
    for (int i = 0; i < procN; i++)
        printf("%.2f  ", bufX[i]);

    in.value = bufX[0];
    in.index = 0;
    for (int i = 1; i < procN; i++) {
        if (in.value > bufX[i]) {
            in.value = bufX[i];
            in.index = i;
        }
    }

    //set global index
    in.index = procN * rank + in.index;

    printf("\nLocal min value = %.2f\nGlobal index = %d\n\n", in.value, in.index);

    MPI_Reduce(&in, &out, 1, MPI_FLOAT_INT, MPI_MINLOC, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("============================================\n");
        printf("End.\nMin value = %.2f\nGlobal index = %d\n", out.value, out.index);
        printf("============================================\n\n");
    }


    delete[] bufX;
    delete[] x;

    MPI_Finalize();

    return EXIT_SUCCESS;
}