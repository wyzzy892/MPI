#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#define N 8

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size != 3)
    {
        printf("This application is meant to be run with 3 processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    enum rank_roles { SENDER, RECEIVER_B, RECEIVER_C };

    switch (rank)
    {
    case SENDER:
        MPI_Datatype even_odd_rows;
        MPI_Type_vector(N / 2, N, 2 * N, MPI_INT, &even_odd_rows);
        MPI_Type_commit(&even_odd_rows);

        int a[N][N];
        srand(time(0));
        printf("START. This is process %d\nMATRIX:\n", rank);
        for (size_t i = 0; i < N; i++) {
            for (size_t j = 0; j < N; j++) {
                a[i][j] = rand() % 10;
                printf("%d \t", a[i][j]);
            }
            printf("\n");
        }
        printf("----------------------------------------------------------\n\n");

        MPI_Send(a, 1, even_odd_rows, RECEIVER_B, 1, MPI_COMM_WORLD);
        MPI_Send(&a[1][0], 1, even_odd_rows, RECEIVER_C, 2, MPI_COMM_WORLD);
		MPI_Type_free(&even_odd_rows);
        break;
    case RECEIVER_B:
        int b[N / 2][N];

        MPI_Recv(b, N*N/2, MPI_INT, SENDER, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("This is process %d\nEVEN ROWS:\n", rank);
        for (size_t i = 0; i < N / 2; i++) {
            for (size_t j = 0; j < N; j++) {
                printf("%d\t", b[i][j]);
            }
            printf("\n");
        }
        printf("----------------------------------------------------------\n\n");

        break;
    case RECEIVER_C:
        int c[N / 2][N];

        MPI_Recv(c, N*N/2, MPI_INT, SENDER, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("This is process %d\nODD ROWS:\n", rank);
        for (size_t i = 0; i < N / 2; i++) {
            for (size_t j = 0; j < N; j++) {
                printf("%d\t", c[i][j]);
            }
            printf("\n");
        }
        printf("----------------------------------------------------------\n\n");

        break;
    default:
        break;
    }
    
    MPI_Finalize();

    return EXIT_SUCCESS;
}