#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 4

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    // Get the number of processes and check only 2 processes are used
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 2)
    {
        printf("This application is meant to be run with 2 processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int a[N][N];
    printf("BEFORE. Process %d\na = \n", rank);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            a[i][j] = 0;
            printf("%d ", a[i][j]);
        }
        printf("\n");
    }
    printf("\n--------------------------------------------------\n\n");

    MPI_Datatype triangle_type;
    int lengths[N];
    int displacements[N];
    for (int i = 0; i < N; i++) {
        lengths[i] = N - i;
        displacements[i] = i * (N + 1);
    }
    MPI_Type_indexed(N, lengths, displacements, MPI_INT, &triangle_type);
    MPI_Type_commit(&triangle_type);

    if (rank == 0) {
        printf("FILL MATRIX. Process %d\na = \n", rank);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (j >= i)
                    a[i][j] = rand() % 10;
                printf("%d ", a[i][j]);
            }
            printf("\n");
        }
        printf("\n--------------------------------------------------\n\n");
    }

    MPI_Bcast(a, 1, triangle_type, 0, MPI_COMM_WORLD);

    printf("END. Process %d\na = \n", rank);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("%d ", a[i][j]);
        printf("\n");
    }
    printf("\n--------------------------------------------------\n\n");

    MPI_Finalize();

    return EXIT_SUCCESS;
}