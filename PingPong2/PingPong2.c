#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#include <assert.h>

int main(int argc, char* argv[]) {

	MPI_Status status;
	int rank,numP;


	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numP);
	int msgToSent,msgRecived,limit;
	double t1, t2;
	t1 = MPI_Wtime(); //considero l'istante temporale di quando è iniziata la esecuzione
	if(rank == 0)
	{

		msgToSent = 0;
		limit = atoi(argv[1]);
		MPI_Send(&msgToSent,1,MPI_INT,1,0,MPI_COMM_WORLD);
		printf("Il master invia allo slave e il numero:%d\n",msgToSent);
		MPI_Recv(&msgRecived,1,MPI_INT,1,0,MPI_COMM_WORLD,&status);
		printf("Il master riceve dallo slave e il numero:%d\n",msgRecived);

		while(msgRecived <= limit)
		{
			printf("Il master riceve dallo slave e il numero:%d\n",msgRecived);
			MPI_Send(&msgToSent,1,MPI_INT,1,0,MPI_COMM_WORLD);
			MPI_Recv(&msgRecived,1,MPI_INT,1,0,MPI_COMM_WORLD,&status);
		}
		printf("Raggiunto il limito esco!\n");


		t2 = MPI_Wtime();//considero l'istante temporale di quando è finita la esecuzione
		printf("Tempo di esecuzione: %f\n",t2-t1);
		MPI_Abort(MPI_COMM_WORLD,911); //Fa il kill di tutti i processi
		return 0;

	}
	else
	{
		int randomValue;
		srand((unsigned) time(NULL));
		while(1)
		{

			MPI_Recv(&msgRecived,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
			randomValue=(rand() % 1)+1;
			msgToSent+=msgRecived+randomValue;
			MPI_Send(&msgToSent,1,MPI_INT,0,0,MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
	return 0;
}
