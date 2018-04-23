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



	if(rank == 0)
	{
		char* msg;
		char* recivedMsgSlave;
		int i,recivedMsgLen,destSlave,firstIteration;

		msg = (char*)malloc(3*sizeof(char));
		strcpy(msg,"Msg");

		i = 1;
		firstIteration = 1;
		while(1)
		{
			if(i == 1 && firstIteration == 1)
			{

				MPI_Send(msg,3+1,MPI_CHAR,i,0,MPI_COMM_WORLD);
				printf("Il Master ha fatto pong del messaggio:%s, verso lo slave %d\n",msg,i);

				MPI_Probe(i,0,MPI_COMM_WORLD,&status);
				MPI_Get_count(&status,MPI_CHAR,&recivedMsgLen);
				recivedMsgSlave = (char*)malloc(recivedMsgLen*sizeof(char));

				MPI_Recv(recivedMsgSlave,recivedMsgLen,MPI_CHAR,i,0,MPI_COMM_WORLD,&status);
				printf("il master ha ricevuto il ping dello slave %d  \n",i);

				destSlave = i+1;
				if(destSlave < numP)
				{
					MPI_Send(recivedMsgSlave,recivedMsgLen,MPI_CHAR,destSlave,0,MPI_COMM_WORLD);
					printf("Il Master ha fatto pong del messaggio:%s, verso lo slave %d\n",recivedMsgSlave,destSlave);
				}
				firstIteration = 0;
			}
			else if(i == 1 && firstIteration != 1)
			{

				MPI_Probe(i,0,MPI_COMM_WORLD,&status);
				MPI_Get_count(&status,MPI_CHAR,&recivedMsgLen);
				recivedMsgSlave = (char*)malloc(recivedMsgLen*sizeof(char));

				MPI_Recv(recivedMsgSlave,recivedMsgLen,MPI_CHAR,i,0,MPI_COMM_WORLD,&status);
				printf("il master ha ricevuto il ping dello slave %d  \n",i);


				destSlave = i+1;
				if(destSlave < numP)
				{
					MPI_Send(recivedMsgSlave,recivedMsgLen,MPI_CHAR,destSlave,0,MPI_COMM_WORLD);
					printf("Il Master ha fatto pong del messaggio:%s, verso lo slave %d\n",recivedMsgSlave,destSlave);
				}
			}
			else
			{

				MPI_Probe(i,0,MPI_COMM_WORLD,&status);
				MPI_Get_count(&status,MPI_CHAR,&recivedMsgLen);
				recivedMsgSlave = (char*)malloc(recivedMsgLen*sizeof(char));

				MPI_Recv(recivedMsgSlave,recivedMsgLen,MPI_CHAR,i,0,MPI_COMM_WORLD,&status);
				printf("il master ha ricevuto il ping dello slave %d  \n",i);


				destSlave = i+1;
				if(destSlave == numP)
				{
					destSlave = 1;

				}
				if(destSlave < numP)
				{
					MPI_Send(recivedMsgSlave,recivedMsgLen,MPI_CHAR,destSlave,0,MPI_COMM_WORLD);
					printf("Il Master ha fatto pong del messaggio:%s, verso lo slave %d\n",recivedMsgSlave,destSlave);
				}
			}

			i++;
			if(i == numP)
			{
				i = 1;

			}
		}
	}
	else
	{
		while(1)
		{
			int msgLen;
			char* recivedMsg;

			MPI_Probe(0,0,MPI_COMM_WORLD,&status);
			MPI_Get_count(&status,MPI_CHAR,&msgLen);
			recivedMsg = (char*)malloc(msgLen*sizeof(char));


			MPI_Recv(recivedMsg,msgLen,MPI_CHAR,0,0,MPI_COMM_WORLD,&status);
			printf("Lo slave %d ha ricevuto il pong del master\n",rank);

			MPI_Send(recivedMsg,msgLen,MPI_CHAR,0,0,MPI_COMM_WORLD);
			printf("Lo slave %d fa il ping del messaggio:%s\n",rank,recivedMsg);
		}

	}
	MPI_Finalize();
	return 0;
}
