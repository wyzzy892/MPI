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

    int N = 11;
    int receiveID = 0xE3DA;

    int step = N / (size-1);
    if (rank == size - 1) {
        step += (N % (size - 1));
    }

    int* bufX = 0;
    int* bufY = 0;
    int* bufZ = 0;

    if (rank == 0) {
        srand(time(0));

        int* x = new int[N];
        int* y = new int[N];

        for (int i = 0; i < N; i++) {
            x[i] = rand() % 10;
            y[i] = rand() % 10;
        }

        printf("\nThis is process %d.\nX = ", rank);
        for (int i = 0; i < N; i++)
            printf("%d, ", x[i]);
        printf("\nY = ");
        for (int i = 0; i < N; i++)
            printf("%d, ", y[i]);
        printf("\n");

        int* tmpX = x;
        int* tmpY = y;

        for (int i = 1; i < size; i++) {
            if (i == size - 1) {
                step += (N % (size-1));
            }

            MPI_Send(tmpX, step, MPI_INT, i, N+i, MPI_COMM_WORLD);
            MPI_Send(tmpY, step, MPI_INT, i, 2*N+i, MPI_COMM_WORLD);

            tmpX += step;
            tmpY += step;
        }

        delete[] x;
        delete[] y;
    }
    else {
        int step = N / (size - 1);
        step += (rank == size - 1 ? N % (size - 1) : 0);

        bufX = new int[step];
        bufY = new int[step];
        bufZ = new int[step];

        MPI_Recv(bufX, step, MPI_INT, 0, N+rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(bufY, step, MPI_INT, 0, 2*N+rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("This is process %d\nbufX = ", rank);
        for (int i = 0; i < step; i++)
            printf("%d, ", bufX[i]);
        printf("\nbufY = ");
        for (int i = 0; i < step; i++)
            printf("%d, ", bufY[i]);

        for (int i = 0; i < step; i++)
        {
            // Change operation +/* (a, b)
            bufZ[i] = bufX[i] + bufY[i];
            //bufZ[i] = bufX[i] * bufY[i];
        }

        printf("\nbufZ = ");
        for (int i = 0; i < step; i++)
            printf("%d, ", bufZ[i]);
        printf("\n\n");
    }

    //Gather data
    if (rank != 0) {
        MPI_Send(bufZ, step, MPI_INT, 0, receiveID, MPI_COMM_WORLD);
    }
    else {
        int* z = new int[N];
        for (int i = 0; i < N; i++)
            z[i] = 0;

        int receiveSize = N / (size - 1);
        int* tmpArr = z;

        printf("\n");
        for (int i = 1; i < size; i++)
        {
            if (i == size-1)
            {
                receiveSize += (N % (size - 1));
            }

            MPI_Recv(tmpArr, receiveSize, MPI_INT, i, receiveID, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            tmpArr += receiveSize;
        }

        printf("\nTHE END. This is process %d\n", rank);
        printf("Z = ");
        for (int i = 0; i < N; i++)
            printf("%d, ", z[i]);
        printf("\n\n");

        delete[] z;
    }

    delete[] bufX;
    delete[] bufY;
    delete[] bufZ;

    MPI_Finalize();

    return EXIT_SUCCESS;
}