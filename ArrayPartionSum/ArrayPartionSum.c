#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#include <assert.h>

int* arrayInizialization(int arrayLen);

int main(int argc, char* argv[]) {

	MPI_Status status;
	int rank; //Rango dei processori
	int numP; //Numero processori
	int arrayLen; //Lughezza array
	int* array;

	//Inizializzo l'ambiente
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numP);

	if(rank == 0)
	{
		arrayLen = atoi(argv[1]);
		array = arrayInizialization(arrayLen);

		int quoziente,resto,i,subStar,subLen;
		quoziente = arrayLen / numP;
		resto = arrayLen % numP;

		if(resto == 0)
		{
			for(i = 1; i < numP; i++)
			{
				subStar = (i*quoziente);
				MPI_Send(&(array[subStar]),quoziente,MPI_INT,i,0,MPI_COMM_WORLD);
			}

		}
		else
		{
			subLen=0;
			for(i = 1; i < numP; i++)
			{
				if(resto >= i)
				{
					if(subLen == 0)
					{
						subStar = (i*quoziente);
						subLen = (quoziente)+1;
					}
					else
					{
						subStar = (i*quoziente)+1;
					}

				}
				else
				{
					subStar = (i*quoziente)+resto;
					subLen=quoziente;
				}
//				printf("subStart:%d\n",subStar);
//				printf("subLen:%d\n",subLen);

				MPI_Send(&(array[subStar]),subLen,MPI_INT,i,0,MPI_COMM_WORLD);
			}
		}

		int globalSum = 0,partialSum,masterSum=0;

		for(i = 0; i < quoziente; i++)
		{
			masterSum+=array[i];
		}
//		printf("Master somma è:%d\n",masterSum);
		globalSum +=masterSum;

//		printf("Mater sta per ricevere\n");
		for(i = 1; i < numP; i++)
		{
			MPI_Recv(&partialSum,1,MPI_INT,i,0,MPI_COMM_WORLD,&status);
			globalSum+=partialSum;
//			printf("Slave riceve\n");

		}
		//printf("Slave riceve\n");

		printf("La somma dell array è:%d\n",globalSum);

	}
	else
	{
//		printf("Slave riceve\n");
		MPI_Probe(0,0,MPI_COMM_WORLD,&status);

		int subArrayLen,sum,i;
		MPI_Get_count(&status,MPI_INT,&subArrayLen);
		int* subArray;
		subArray = (int*)malloc(sizeof(int)*subArrayLen);

		MPI_Recv(subArray,subArrayLen,MPI_INT,0,0,MPI_COMM_WORLD,&status);

		sum = 0;
		for(i = 0; i < subArrayLen; i++)
		{
//			printf("%d ",subArray[i]);
			sum+=subArray[i];
		}
//		fflush(stdout);
//		printf("Sum:%d\n",sum);

		MPI_Send(&sum,1,MPI_INT,0,0,MPI_COMM_WORLD);
		//printf("Slave %d invia\n",);
	}




	MPI_Finalize();
	return 0;
}

int* arrayInizialization(int arrayLen)
{
	int* array;
	int i;
	array =(int*)malloc(sizeof(int)*arrayLen);
	assert(array != NULL);

	for(i = 0; i < arrayLen; i++)
	{
		array[i] = i;
	}
	return array;
}
