/*
 ============================================================================
 Name        : PIComputation.c
 Author      : Giovanni Leo
 Number      : 0522500538
 Description : Calculate Pi in MPI
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

//#define N 1E9 			//Significa 1*10^7
//#define d 1E-9 			//Significa 1*10^-7
//#define d2 1E-18 		//Significa 1*10^-14
#define SEED 35791246
#define PI 3.141592653589793238462643


void trapezoidPI(int rank,int numP,MPI_Status status,int start, int stop,long long int N,double d, double d2);
void monteCarloPI(int rank,int numP,MPI_Status status,int start, int stop,long long int N);

int main (int argc, char* argv[])
{
	int rank;      //processors rank
	int numP;	   //processors number
	MPI_Status status;

	//MPI Inizialization
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numP);

	double d,d2;
	long long int N;
	int command;
	int multiply;
	if(argc > 1)
	{
		command = atoi(argv[1]);
		multiply =  atoi(argv[2]);
		switch (command) {
		case 1:
			N = 1E2 * multiply;
			d = 1E-2 * multiply;
			d2 = 1E-4 * multiply;
			break;
		case 2:
			N = 1E3 * multiply;
			d = 1E-3 * multiply;
			d2 = 1E-6 * multiply;
			break;
		case 3:
			N = 1E4 * multiply;
			d = 1E-4 * multiply;
			d2 = 1E-8 * multiply;
			break;
		case 4:
			N = 1E5 * multiply;
			d = 1E-5 * multiply;
			d2 = 1E-10 * multiply;
			break;
		case 5:
			N = 1E6 * multiply;
			d = 1E-6 * multiply;
			d2 = 1E-12 * multiply;
			break;
		case 6:
			N = 1E7 * multiply;
			d = 1E-7 * multiply;
			d2 = 1E-14 * multiply;
			break;
		case 7:
			N = 1E8 * multiply;
			d = 1E-8 * multiply;
			d2 = 1E-16 * multiply;
			break;
		case 8:
			N = 1E9 * multiply;
			d = 1E-9 * multiply;
			d2 = 1E-18 * multiply;
			break;
		default:
			N = 1E7;
			d = 1E-7;
			d2 = 1E-14;
			break;
		}
	}



	int chunck = N / numP;
	int remainder = fmod(N,numP); //(long)N % numP;
	int start, stop;

	//Start work partitioning
	if (rank < remainder) {
		// The first 'remainder' ranks get 'count + 1' tasks each
		start = rank * (chunck + 1);
		stop = start + chunck;

	} else {
		// The remaining 'size - remainder' ranks get 'count' task each
		start = rank * chunck + remainder;
		stop = start + (chunck - 1);

	}
	//End work partitioning


	monteCarloPI(rank,numP,status,start,stop,N);

	trapezoidPI(rank,numP,status,start,stop,N,d,d2);

	MPI_Finalize();
	return 0;

}

void trapezoidPI(int rank,int numP,MPI_Status status,int start, int stop,long long int N,double d, double d2)
{

	long long int i;
	double pi=0.0, result=0.0,x2=0.0;
	double startTime,endTime;
	startTime = MPI_Wtime();
	double piSum;


	for (i=start; i<=stop; i+=1)
	{
		x2=d2*i*i;
		result+=1.0/(1.0+x2);

	}
	piSum=result;
	if(rank != 0)
	{
		MPI_Send(&piSum,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
	}
	else
	{
		double recivedPI;
		for(i = 1; i < numP; i++)
		{
			recivedPI = 0;
			MPI_Recv(&recivedPI,1,MPI_DOUBLE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
			/*
			 *Facendo dei test ho notato che utilizzando come sorgente MPI_ANY_SOURCE al posto di mettere come sorgente i
			 *il tempo di esecuzione migliora anche se di poco ~ 0,010 millisencodi in meno.
			 * */
			piSum+=recivedPI;
		}

		pi = 4*d*piSum;
		printf("PI Trapezoid is approximately=%lf, error is %.16f\n", pi,pi-PI);
		endTime = MPI_Wtime();
		printf("Elapsed time %lf\n",endTime-startTime);
		printf("===================================\n");
	}

}

void monteCarloPI(int rank,int numP,MPI_Status status,int start, int stop,long long int N)
{
	long long int i;
	double x,y;
	int count=0; /* # of points in the 1st quadrant of unit circle */
	double z;
	double pi,piSum;
	double startTime,endTime;
	startTime = MPI_Wtime();

	/* initialize random numbers */
	srand(SEED);
	count=0;


	for ( i=start; i<=stop; i++) {

		x = (double)rand()/RAND_MAX;
		y = (double)rand()/RAND_MAX;
		z = x*x+y*y;
		if (z<=1) count++;

	}

	piSum=count;



	if(rank != 0)
	{
		MPI_Send(&piSum,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
	}
	else
	{
		double recivedPI;

		for(i = 1; i < numP; i++)
		{
			recivedPI = 0;
			MPI_Recv(&recivedPI,1,MPI_DOUBLE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
			/*
			 *Facendo dei test ho notato che utilizzando come sorgente MPI_ANY_SOURCE al posto di mettere come sorgente i
			 *il tempo di esecuzione migliora anche se di poco ~ 0,010 millisencodi in meno.
			 * */
			piSum+=recivedPI;

		}
		pi = (piSum*4)/N;
		printf("PI MonteCarlo is approximately=%lf, error is %.16f\n", pi,pi-PI);
		endTime = MPI_Wtime();
		printf("Elapsed time %lf\n",endTime-startTime);
		printf("===================================\n");
	}

}
