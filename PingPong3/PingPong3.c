#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#include <assert.h>

int main(int argc, char* argv[])
{
	int rank,numP;
	MPI_Status status;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numP);

	int msgToSent = 0,msgRecived = 0,limit;
	double t1, t2;
	t1 = MPI_Wtime(); 					//considero l'istante temporale di quando è iniziata la esecuzione

	if(rank == 0)
	{
		int lastP;
		limit = atoi(argv[1]);
		msgToSent = 0;
		lastP = numP -1;
		MPI_Send(&msgToSent,1,MPI_INT,1,0,MPI_COMM_WORLD);
		MPI_Recv(&msgRecived,1,MPI_INT,lastP,0,MPI_COMM_WORLD,&status);

		while(msgRecived <= limit)
		{
			printf("Il master riceve dallo slave %d e il numero:%d\n",msgRecived,numP-1);
			MPI_Send(&msgToSent,1,MPI_INT,1,0,MPI_COMM_WORLD);
			MPI_Recv(&msgRecived,1,MPI_INT,lastP,0,MPI_COMM_WORLD,&status);
		}
		printf("Raggiunto il limito esco!\n");
		t2 = MPI_Wtime();							//considero l'istante temporale di quando è finita la esecuzione
		printf("Tempo di esecuzione: %f\n",t2-t1);
		MPI_Abort(MPI_COMM_WORLD,911); 				//Fa il kill di tutti i processi
		return 0;
	}
	else
	{

		int randomValue,destinationSlave;
		srand((unsigned) time(NULL));

		while(1)
		{
			if(rank == 1)
			{
				MPI_Recv(&msgRecived,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
				printf("Sono lo slave 1 ho appena ricevuto da master\n");
				randomValue=(rand() % 2)+1;
				msgToSent+=msgRecived+randomValue;
				destinationSlave = rank+1;
				if(destinationSlave < numP)
				{
					MPI_Send(&msgToSent,1,MPI_INT,destinationSlave,0,MPI_COMM_WORLD);
				}
				else
				{

					MPI_Send(&msgToSent,1,MPI_INT,0,0,MPI_COMM_WORLD);
				}

			}
			else if(rank == (numP -1))
			{
				MPI_Recv(&msgRecived,1,MPI_INT,rank-1,0,MPI_COMM_WORLD,&status);
				randomValue=(rand() % 2)+1;
				msgToSent+=msgRecived+randomValue;
				MPI_Send(&msgToSent,1,MPI_INT,0,0,MPI_COMM_WORLD);
			}
			else
			{

				MPI_Recv(&msgRecived,1,MPI_INT,rank-1,0,MPI_COMM_WORLD,&status);
				randomValue=(rand() % 2)+1;
				msgToSent+=msgRecived+randomValue;
				destinationSlave = rank+1;
				if(destinationSlave < numP)
				{
					MPI_Send(&msgToSent,1,MPI_INT,destinationSlave,0,MPI_COMM_WORLD);
				}
				else
				{
					MPI_Send(&msgToSent,1,MPI_INT,0,0,MPI_COMM_WORLD);
				}


			}
		}


	}
	MPI_Finalize();
	return 0;
}
