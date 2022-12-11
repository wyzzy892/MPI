#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#define N 8

typedef struct Mytype {
    int arr[N][N];
};

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size < 5)
    {
        printf("This application is meant to be run with 3 processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    int blocklenghts[2] = { N, N };
    MPI_Datatype types[2] = { MPI_INT, MPI_INT };
    MPI_Aint disps[2] = { offsetof(struct Mytype, arr[0]), offsetof(struct Mytype, arr[4]) };
    MPI_Datatype newtype;
    MPI_Type_create_struct(2, blocklenghts, disps, types, &newtype);
    MPI_Type_commit(&newtype);

    int d[2][N]; // buffer for receiving data

    Mytype mytype;

    if (rank == 4) {
        srand(time(0));
        printf("START. This is process %d\nMATRIX:\n", rank);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                mytype.arr[i][j] = rand() % 100;
                printf("%d\t", mytype.arr[i][j]);
            }
            printf("\n");
        }
        printf("----------------------------------------------------------\n\n");

        MPI_Send(&mytype.arr[0][0], 1, newtype, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&mytype.arr[1][0], 1, newtype, 1, 1, MPI_COMM_WORLD);
        MPI_Send(&mytype.arr[2][0], 1, newtype, 2, 2, MPI_COMM_WORLD);
        MPI_Send(&mytype.arr[3][0], 1, newtype, 3, 3, MPI_COMM_WORLD);
    }
    else {
        MPI_Recv(&d, 2 * N, MPI_INT, 4, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("END. This is process %d\nROWS:\n", rank);
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < N; j++) {
                mytype.arr[i][j] = rand() % 10;
                printf("%d \t", d[i][j]);
            }
            printf("\n");
        }
        printf("----------------------------------------------------------\n\n");
    }

    MPI_Type_free(&newtype);
    MPI_Finalize();

    return EXIT_SUCCESS;
}