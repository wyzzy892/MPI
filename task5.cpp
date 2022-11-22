#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int N = 16;
    int receiveID = 0xE3DA;

    int blockSize = N / (size-1);
    if (rank == size - 1) {
        blockSize += (N % (size - 1));
    }

    int* bufm = 0;
    int* X = 0;
    int* diag = 0;

    if (rank == 0) {
        srand(time(0));

        int* matrix = new int[N*N];

        for (int i = 0; i < N*N; i++) {
            matrix[i] = rand() % 10;
        }

        printf("\nThis is process %d.\nmatrix = \n", rank);
        int j = N;
        for (int i = 0; i < N*N; i++) {
            if (i == j) {
                printf("\n");
                j += N;
            }
            printf("%d, ", matrix[i]);
        }
        printf("\n\n");

        int* tmp_matr = matrix;

        int step = N / (size - 1);
        for (int i = 1; i < size; i++) {
            if (i == size - 1) {
                step += (N % (size-1));
            }
            MPI_Send(tmp_matr, step*N, MPI_INT, i, i, MPI_COMM_WORLD);
            tmp_matr += step*N;
        }
    }
    else {
        bufm = new int[blockSize*N];
        diag = new int[blockSize];

        MPI_Recv(bufm, blockSize*N, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("This is process %d\n", rank);
        for (int i = 0; i < blockSize*N; i++)
            printf("%d, ", bufm[i]);
        printf("\n");

        int diag_idx = blockSize * rank - blockSize;
        if (rank == size - 1) {
            diag_idx = (blockSize-1) * rank - (blockSize-1);
        }
        for (int i = 0; i < blockSize; i++) 
        {
            diag[i] = bufm[diag_idx];
            diag_idx += (N + 1);
        }
            
        printf("\nDiag = \n");
        for (int i = 0; i < blockSize; i++)
            printf("%d, ", diag[i]);
        printf("\n\n");
    }

    //Gather data
    if (rank != 0) {
        MPI_Send(diag, blockSize, MPI_INT, 0, receiveID, MPI_COMM_WORLD);
    }
    else {
        X = new int[N];
        for (int i = 0; i < N; i++)
            X[i] = 0;

        int* tmpArr = X;

        int receivSize = N / (size - 1);
        for (int i = 1; i < size; i++)
        {
            if (i == size-1)
            {
                receivSize += (N % (size - 1));
            }

            MPI_Recv(tmpArr, receivSize, MPI_INT, i, receiveID, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            tmpArr+=receivSize;
        }

        printf("\nTHE END. This is process %d\n", rank);
        printf("Diag = ");
        for (int i = 0; i < N; i++)
            printf("%d, ", X[i]);
        printf("\n\n");

        delete[] X;
    }

    delete[] bufm;
    delete[] diag;

    MPI_Finalize();

    return EXIT_SUCCESS;
}