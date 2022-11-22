#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <cmath>
#define N 4

void initVectorData(int* arr, int n);
void initMatrixData(int* arr, int n);
void printVectorData(int* arr, int n);
void printMatrixData(int* arr, int n, int m);

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size > N) {
        printf("Number of processes is more than size of matrix.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // root rank
    int root = 0;

    // gather rank
    int gather = 1;

    // calculate block size
    int block = (N / size) + (N % size);
    int n = block * size;
    //int straid = n * block;

    // define x and A
    int* x = new int[n];
    int* A = 0;
    int* bufA = new int[n * block];
    int* z = new int[block];
    int* res = new int[n];

    if (rank == root) {
        // init x data
        A = new int[n * n];
        initVectorData(x, n);
        // init A data
        initMatrixData(A, n);

        // print x
        printf("START. This is process %d\nx = \t", rank);
        printVectorData(x, n);

        //print A
        printf("START. This is process %d\nA = \t", rank);
        printMatrixData(A, n, n);

        //delete[] A;
    }

    // Sending x  data
    MPI_Bcast(x, n, MPI_INT, root, MPI_COMM_WORLD);

    printf("This is process %d\nx = \t", rank);
    printVectorData(x,n);

    // Sending A data
    MPI_Scatter(A, n * block, MPI_INT, bufA, n * block, MPI_INT, root, MPI_COMM_WORLD);
    printf("This is process %d\nbufA = \n", rank);
    printMatrixData(bufA, block, n);
    
    // Calculating data
    for (int i = 0; i < block; i++) {
        z[i] = 0;
        for (int j = 0; j < n; j++) {
            z[i] += bufA[i * n + j] * x[j];
        }
    }
    printf("This is process %d\nLocal z = \t", rank);
    printVectorData(z, block);

    MPI_Gather(z, block, MPI_INT, res, block, MPI_INT, gather, MPI_COMM_WORLD);
    if (rank == gather) {
        printf("END. z = \t");
        for (int i = 0; i < N; i++) {
            printf("%d \t", res[i]);
        }
        printf("\n===============================================\n\n");
    }

    delete[] x;
    delete[] bufA;
    delete[] z;
    delete[] res;

    MPI_Finalize();

    return EXIT_SUCCESS;
}


void initVectorData(int* arr, int n) {
    srand(time(0));
    for (int i = 0; i < N; i++)
        arr[i] = rand() % 10;
    for (int i = N; i < n; i++)
        arr[i] = 0;
}

void initMatrixData(int* arr, int n) {
    srand(time(NULL));
    int innerborder = 0;
    int outerborder = 0;

    for (int i = 0; i < n; i++) {
        if (i < N) {
            for (int j = 0; j < N; j++) {
                arr[i * n + j] = rand() % 10 + 1;
            }
            for (int j = N; j < n; j++) {
                arr[i * n + j] = 0;
            }
            //printf("%d ", arr[i]);
        }
        else {
            for (int j = 0; j < n; j++)
                arr[i * n + j] = 0;
        }
    }
}

void printVectorData(int* arr, int n) {
    for (int i = 0; i < n; i++)
        printf("%d \t", arr[i]);
    printf("\n========================================\n\n");
}

void printMatrixData(int* arr, int n, int m) {
    printf("\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%d \t", arr[i * m + j]);
        }
        printf("\n");
    }
    printf("\n========================================\n\n");
}