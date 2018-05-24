#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


#define ALIVE 1
#define DEAD 0

int main (int argc, char* argv[])
{
	int rank;      //rank processori
	int numP;	   //numero processori
	int nRows; //numero righe in input
	int nCols; //numero colonne in input
	int timesteps; //numero di timestep
	int **oldMatrix,**newMatrix,*oldMatrix1Dim,*newMatrix1Dim;
	MPI_Status status;

	//Inizializzazione MPI
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numP);

	if(rank == 0)
	{

		nRows = 200;
		nCols = 200;
		timesteps = 500;

	}

	//Faccio il broadcast di questi valori a tutti gli altri processori
	MPI_Bcast(&nCols,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&nRows,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&timesteps,1,MPI_INT,0,MPI_COMM_WORLD);

//	printf("finite le BCast\n");

	//Start partioning
	int nRowsLocal = nRows /numP; //Indica il numero di righe da inserire in ogni processore;
	//Essendo una divisione intera può avere un resto quindi mettiamo i restanti elementi nell'ultimo processore
	if(rank == numP -1)
	{
		nRowsLocal+=(nRows % numP); //abbiamo aggiunto il resto all'ultimo processore
	}
	//Esendo che abbiamo bisogno di due righe ghost una sotto e na sopra dobbiamo inserire anche quelle allora
	int nRowsLocalWithGhost = nRowsLocal+2;
	//Abbiamo bisogno di inserire anche le due collonne ghost
	int nColsWithGhost = nCols +2;
//	printf("Fatta la suddivisione delle righe\n");
	//Indico la dimensione delle sottomatrici
	int matrixSize = nRowsLocalWithGhost * nColsWithGhost;
	//Le alloco
	oldMatrix1Dim = malloc(matrixSize * sizeof(int));
	newMatrix1Dim = malloc(matrixSize * sizeof(int));

	oldMatrix = malloc(nRowsLocalWithGhost * sizeof(int*));
	newMatrix = malloc(nRowsLocalWithGhost * sizeof(int*));
	int i,j,z,f,iRow,jCol;
	for(i = 0; i < nRowsLocalWithGhost; i++)
	{
		oldMatrix[i] = &oldMatrix1Dim[i*nColsWithGhost];
		newMatrix[i] = &newMatrix1Dim[i*nColsWithGhost];
	}

//	printf("Fatta la allocazione delle righe\n");
	//ricordiamo cle la riga 0 è una ghost row e l'ultima anche
	//inizializziamo
	for(i = 1; i < nRowsLocalWithGhost; i++)
	{
		for(j = 1; j < nCols; j++)
		{
			oldMatrix[i][j] = rand() % 2;
		}
	}

//	printf("Fatta la inizializzazione\n");

	int upperNeighbor,lowerNeighbor;
	if(rank == 0)
	{
		upperNeighbor = numP-1; //la colonna ghost superiore sarà quella dell'ultimo processore
	}
	else
	{
		upperNeighbor = rank-1; //se il rank è 3 la colonna ghost superiore sarà quella del processore due e così via
	}

	//Per il lowerNeighbor facciamo il ragionamento inverso
	if(rank == numP-1)
	{
		lowerNeighbor = 0;
	}
	else
	{
		lowerNeighbor = rank+1;
	}
//	printf("sto quie\n");

	//Time loop
	for(i = 0; i < timesteps; i++)
	{
		//Dobbiamo inviare la prima riga al processo con rank precedente a quelllo corrente
		//Invio la riga 1 perche la 0 è ghost
		MPI_Send(&oldMatrix[1][0],nColsWithGhost,MPI_INT,upperNeighbor,0,MPI_COMM_WORLD);
//		printf("Fatta 1 send del proc %d\n",rank);
		//Dobbiamo inviare la riga ultima al processore con rank succesivo a quello corrente
		//L'iltima riga è data da nRowsLocal
		MPI_Send(&oldMatrix[nRowsLocal][0],nColsWithGhost,MPI_INT,lowerNeighbor,0,MPI_COMM_WORLD);
//		printf("Fatta 2 send del proc %d\n",rank);

		//Riceviamo la TopGhostRow
		MPI_Recv(&oldMatrix[0][0],nColsWithGhost,MPI_INT,upperNeighbor,0,MPI_COMM_WORLD,&status);
//		printf("Fatta 1 rec del proc %d\n",rank);
		//Riceviamo la bottomRow
		MPI_Recv(&oldMatrix[nRowsLocal+1][0],nColsWithGhost,MPI_INT,lowerNeighbor,0,MPI_COMM_WORLD,&status);
//		printf("Fatta 2 rec del proc %d\n",rank);
		//ghost colums
		for(j = 0 ;j < nRowsLocalWithGhost ; j++)
		{
			//Stiamo settando le colonne ghost elemeto per elemento
			oldMatrix[j][0] = oldMatrix[j][nCols];
			oldMatrix[j][nCols+1] = oldMatrix[j][1];
		}





		//Update della matrice
		//Parto da uno perchè non voglio le ghost rows

		for(iRow = 1; iRow <= nRowsLocal;iRow++)
		{
			for(jCol = 1; jCol <= nCols ; jCol++)
			{
				int nAliveNeigbor = 0;
				//Scorriamo i vicini di una cella ne sono 8
				//z indica le righe vicine e f indica le colonne vicine
				for(z = iRow-1; z <= iRow +1; z++)
					//Tale for va dalla riga precedente dell'elemeto considerato a quella successiva
				{
					for(f = jCol -1;f<=jCol+1;f++)
						//Lo stesso vale per le colonne
					{
						//Essendo che l'elemto di interesse è al centro non lo dobiiamo considerare quindi
						if(z != iRow || f!= jCol)
						{
							if (oldMatrix[z][f] == ALIVE)
							{
								//Ho trovato un eleento vivo allora
								nAliveNeigbor++;
							}
						}
					}
				}

//
//				//Verifico le condizioni e modifico la nuova matrice

				if(nAliveNeigbor < 2)
				{
					newMatrix[iRow][jCol] = DEAD;
					//printf("Dead\n");
				}
				else if(nAliveNeigbor >3)
				{
					newMatrix[iRow][jCol] = DEAD;
//					printf("Dead\n");
				}
				else if(nAliveNeigbor  == 3)
				{
					newMatrix[iRow][jCol] = ALIVE;
//					printf("Alive\n");
				}
				else if(nAliveNeigbor  == 2)
				{
					newMatrix[iRow][jCol] = oldMatrix[iRow][jCol];
//					printf("Normal\n");
				}
			}
		}
		for(iRow = 1; iRow <= nRowsLocal;iRow++)
		{
			for(jCol = 1; jCol <= nCols ; jCol++)
			{
				oldMatrix[iRow][jCol] = newMatrix[iRow][jCol];
			}
		}

	}//fine Time loop


	//Faccio la somma
	int cellSum = 0,tempCellSum,sender;
	for(i=0; i < nRowsLocal; i++)
	{
		for(j = 0; j < nCols; j++)
		{

			cellSum+=newMatrix[i][j];
		}
	}

	//printf("Celle vive %d\n nella sotto matrice del proc %d\n",cellSum,rank);

	if(rank == 0)
	{
		for (sender = 1; sender < numP; sender++) {
			MPI_Recv(&tempCellSum,1,MPI_INT,sender,0,MPI_COMM_WORLD,&status);
			cellSum+=tempCellSum;
		}

		printf("Celle vive %d\n",cellSum);

	}
	else
	{
		MPI_Send(&cellSum,1,MPI_INT,0,0,MPI_COMM_WORLD);
	}

	//Fare calcolo delle celle vive
	MPI_Finalize();

	return 0;
}
