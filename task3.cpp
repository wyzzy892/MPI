#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    // Size of the default communicator
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 1) {
        const int n = 5;
        int buf[n]{};
        for (int i = 0; i < n; i++)
            buf[i] = rand() % 20;

        printf("Process %d. Array is created\n", rank);

        for (int i = 0; i < size; i++)
            if (i != 1) {
                MPI_Send(buf, n, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
    }
    else {
        // Retrieve information about the incoming message
        MPI_Status status;
        MPI_Probe(1, 0, MPI_COMM_WORLD, &status);
        printf("Process %d. Probing.\n", rank);

        // Get the number of integers in the message probed
        int count;
        MPI_Get_count(&status, MPI_INT, &count);

        //int* buffer = new int[count];
        int* buffer = (int*)malloc(sizeof(int) * count);

        MPI_Recv(buffer, count, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        printf("Process %d received message\n", rank);

        for (int i = 0; i < count; i++)
            printf("%d, ", buffer[i]);
        printf("\n");

        free(buffer);
        //delete[] buffer;
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}