#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char* argv[]){

	int  my_rank; /* rank of process */
	int  p;       /* number of processes */
	int source=0;   /* rank of sender */
	int dest;     /* rank of receiver */
	int tag=0;    /* tag for messages */
	int message[10];        /* storage for message */
	MPI_Status status ;   /* return status for receive */
	int i;
	int* flag = 0;

	MPI_Init(&argc, &argv);
	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);


	if(my_rank == 0)
	{
		for(i = 0; i < 10; i++)
		{
			message[i]=i;
		}

		for(dest = 1; dest < p; dest++)
		{
			MPI_Send(message,10,MPI_INT,dest,tag,MPI_COMM_WORLD);
			MPI_Recv(&flag,1,MPI_INT,dest,tag,MPI_COMM_WORLD,&status); //aspetto che il procesore finisca bloccando la situazione
		}
		//printf("Master:All message sent\n");


	}
	else
	{
		MPI_Recv(message,10,MPI_INT,source,tag,MPI_COMM_WORLD,&status);

		printf("Message recived! I'am %d proces!\n",my_rank);
		printf("I print message now!\n");
		for(i=0; i< 10;i++)
		{
			printf("%d\t",i);
		}
		printf("\n");

		MPI_Send(&flag,1,MPI_INT,source,tag,MPI_COMM_WORLD);// mi serve a sbloccare la situazione sopra per fare in modo che i messaggi arrivino ordinati

	}

	MPI_Finalize();
	return 0;
}

