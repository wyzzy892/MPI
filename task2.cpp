#include <stdio.h>
#include <time.h>
#include <random>
#include "mpi.h"

//using namespace std;

int main(int argc, char** argv)
{
	int procSize, procRank, message_Item;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &procSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
	
	if (procRank == 0) {
		srand(time(0));
		printf("This is process %d. Starting to create an array.\n", procRank);
		int a[10]{};
		for (size_t i = 0; i < 10; ++i)
			a[i] = rand() % 20;
		printf("This is process %d. Array is created.\n", procRank);
		MPI_Send(a, 10, MPI_INT, 1, 2, MPI_COMM_WORLD);
		printf("This is process %d. Sending started.\n", procRank);
	}
	else {
		int* recv_buff = new int[10];
		MPI_Recv(recv_buff, 10, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		for (size_t i = 0; i < 10; ++i)
			printf("%d ", recv_buff[i]);
		delete[] recv_buff;
	}

	MPI_Finalize();
}