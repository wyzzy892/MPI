#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char buffer[100]{};
    int position = 0;

    int c[10]{};
    float d[8]{};

    if (rank == 0) {
        int a[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        float b[8] = { 2.3, 0.5, 8.9, 1.1, 5.5, 4.3, 2.2, 8.7 };

        printf("Process %d\na = ", rank);
        for (size_t i = 0; i < 10; i++) {
            printf("%d  ", a[i]);
        }
        printf("\nb = ");
        for (size_t i = 0; i < 8; i++) {
            printf("%.1f  ", b[i]);
        }
        printf("\n------------------------------------------\n\n");

        MPI_Pack(&a, 10, MPI_INT, buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Pack(&b, 8, MPI_FLOAT, buffer, 100, &position, MPI_COMM_WORLD);        
    }

    MPI_Bcast(&buffer, 100, MPI_PACKED, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        MPI_Unpack(buffer, 100, &position, &c, 10, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(buffer, 100, &position, &d, 8, MPI_FLOAT, MPI_COMM_WORLD);

        printf("Process %d\na = ", rank);
        for (size_t i = 0; i < 10; i++) {
            printf("%d  ", c[i]);
        }
        printf("\nb = ");
        for (size_t i = 0; i < 8; i++) {
            printf("%.1f  ", d[i]);
        }
        printf("\n------------------------------------------\n\n");
    }
    
    MPI_Finalize();

    return EXIT_SUCCESS;
}