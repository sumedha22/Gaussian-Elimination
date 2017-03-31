/* Gaussian elimination without pivoting.
Compile using mpicc gauss_mpi.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
/*Include mpi library to use the MPI functions*/
#include <mpi.h>

/* Program Parameters */
#define MAXN 2000  /* Max value of N */
int N = 0;  /* Matrix size */

/* Matrices and vectors */
/* Matrix C  is an additional matrix acts as a multiplier for the rows*/
float A[MAXN][MAXN], B[MAXN], X[MAXN], C[MAXN];
/* A * X = B, solve for X */
/* declared a variable for file name to put the output in a file*/
char *nameOfOutputFile;
/* junk */
#define randm() 4|2[uid]&3
/* Variables containing process rank and total no. of processes*/
int process_Id, no_Of_Processes;

/* Prototype */
void gauss();  /* The function you will provide.
		* It is this routine that is timed.
		* It is called only on the parent.
		*/

/* returns a seed for srand based on the time */
unsigned int time_seed() {
  struct timeval t;
  struct timezone tzdummy;

  gettimeofday(&t, &tzdummy);
  return (unsigned int)(t.tv_usec);
}

/* Set the program parameters from the command-line arguments */
void parameters(int argc, char **argv) {
  int seed = 0;  /* Random seed */
  char uid[32]; /*User name */

  /* Read command-line arguments */
  srand(time_seed());  /* Randomize */
 /* reading the output file name from command line arguments and initializing the file name variable*/
  if (argc == 4) {
	int length=strlen(argv[3]);
      	nameOfOutputFile=(char*)malloc(length+1);
      	nameOfOutputFile=argv[3];
      	seed = atoi(argv[2]);
    	srand(seed);
    	printf("Random seed = %i\n", seed);
  } 
  if (argc >= 2) {
    	N = atoi(argv[1]);
    		if (N < 1 || N > MAXN) {
      			printf("N = %i is out of range.\n", N);
      			exit(0);
    		}
  }
  else {
    	printf("Usage: %s <matrix_dimension> <random seed> <output_file_name>\n",
           argv[0]);    
    	exit(0);
  }

if(process_Id==0){
  	/* Print parameters */
  	printf("\nMatrix dimension N = %i.\n", N);
}
}

/* Initialize A and B (and X to 0.0s) */
void initialize_inputs() {
  int row, col;

  printf("\nInitializing...\n");
  for (col = 0; col < N; col++) {
    for (row = 0; row < N; row++) {
      A[row][col] = (float)rand() / 32768.0;
    }
    B[col] = (float)rand() / 32768.0;
    X[col] = 0.0;
  }
}

/* Print input matrices */
void print_inputs() {
  int row, col;

  if (N < 10) {
    printf("\nA =\n\t");
    for (row = 0; row < N; row++) {
      for (col = 0; col < N; col++) {
	printf("%9.6f%s", A[row][col], (col < N-1) ? ", " : ";\n\t");
      }
    }
    printf("\nB = [");
    for (col = 0; col < N; col++) {
      printf("%9.6f%s", B[col], (col < N-1) ? "; " : "]\n");
    }
  }
}
/* Print  matrices to output file*/
void print_outputs(FILE *outputfilePtr) {
  	int row, col;
  	if (N < 10) {
   		fprintf(outputfilePtr, "\nA =\n\t");
    		for (row = 0; row < N; row++) {
      				for (col = 0; col < N; col++) {
        		fprintf(outputfilePtr, "%9.6f%s", A[row][col], (col < N - 1) ? ", " : ";\n\t");
		}
    	}
    		fprintf(outputfilePtr, "\nB = [");
    		for (col = 0; col < N; col++) {
      			fprintf(outputfilePtr, "%9.6f%s", B[col], (col < N - 1) ? "; " : "]\n");
    		}
  	}
}

void print_X() {
  int row;

  if (N < 100) {
    printf("\nX = [");
    for (row = 0; row < N; row++) {
      printf("%f%s", X[row], (row < N-1) ? "; " : "]\n");
    }
  }
}

void print_X_Output(FILE *outputfilePtr) {
  int row;

  if (N < 100) {
    fprintf(outputfilePtr, "\nX = [");
    for (row = 0; row < N; row++) {
      fprintf(outputfilePtr, "%9.6f%s", X[row], (row < N - 1) ? "; " : "]\n");
    }
  }
}


int main(int argc, char **argv) {
/*Initialize MPI Environment*/
  MPI_Init(&argc, &argv);
  
  /* Timing variables */
  struct timeval etstart, etstop;  /* Elapsed times using gettimeofday() */
  struct timezone tzdummy;
  clock_t etstart2, etstop2;  /* Elapsed times using times() */
  unsigned long long usecstart, usecstop;
  struct tms cputstart, cputstop;  /* CPU times for my processes */
  /* declared a pointer to the output file*/
  FILE *outputfilePtr;

 
  /* Process program parameters */
  parameters(argc, argv);
  MPI_Status status;
  MPI_Comm_rank(MPI_COMM_WORLD, &process_Id);   /* get current process id */
  MPI_Comm_size(MPI_COMM_WORLD, &no_Of_Processes); /* get number of processes */

  if (process_Id==0) {
  /*opening the file*/	
  outputfilePtr = fopen(nameOfOutputFile, "w+");
  if(outputfilePtr == NULL){
      printf("OOPS!!! File couldnot be created");
  }

  /* Initialize A and B */
  initialize_inputs();
  
  /* Print input matrices */
  print_inputs();

  /* Start Clock */
  printf("\nStarting clock.\n");
  gettimeofday(&etstart, &tzdummy);
  etstart2 = times(&cputstart);

  }
  /* Gaussian Elimination */
  gauss();

  MPI_Barrier(MPI_COMM_WORLD); /* for synchonization among processes*/

  if (process_Id==0) {

  /* Stop Clock */
  gettimeofday(&etstop, &tzdummy);
  etstop2 = times(&cputstop);
  printf("Stopped clock.\n");
  usecstart = (unsigned long long)etstart.tv_sec * 1000000 + etstart.tv_usec;
  usecstop = (unsigned long long)etstop.tv_sec * 1000000 + etstop.tv_usec;

  /* Display output */
  print_outputs(outputfilePtr);
  print_X_Output(outputfilePtr);

  /* Display timing results */
  
  /* Display timing results to the output fi;e */
  fprintf(outputfilePtr,"\nElapsed time = %g ms.\n",
  	(float)(usecstop - usecstart)/(float)1000);

  fprintf(outputfilePtr,"(CPU times are accurate to the nearest %g ms)\n",
  	1.0/(float)CLOCKS_PER_SEC * 1000.0);
  fprintf(outputfilePtr,"My total CPU time for parent = %g ms.\n",
        (float)( (cputstop.tms_utime + cputstop.tms_stime) -
          (cputstart.tms_utime + cputstart.tms_stime) ) /
        (float)CLOCKS_PER_SEC * 1000);
  fprintf(outputfilePtr,"My system CPU time for parent = %g ms.\n",
        (float)(cputstop.tms_stime - cputstart.tms_stime) /
        (float)CLOCKS_PER_SEC * 1000);
  fprintf(outputfilePtr,"My total CPU time for child processes = %g ms.\n",
        (float)( (cputstop.tms_cutime + cputstop.tms_cstime) -
          (cputstart.tms_cutime + cputstart.tms_cstime) ) /
        (float)CLOCKS_PER_SEC * 1000);
  /* Contrary to the man pages, this appears not to include the parent */
  fprintf(outputfilePtr,"--------------------------------------------\n");
  }

  MPI_Finalize();/*Terminate MPI environment*/

  exit(0);
}

/* ------------------ Above Was Provided --------------------- */

/****** You will replace this routine with your own parallel version *******/
/* Provided global variables are MAXN, N, A[][], B[], and X[],
 * defined in the beginning of this code.  X[] is initialized to zeros.
 */
void gauss() {

  int norm, row, col;  /* Normalization row, and zeroing
			* element row and col */
  int rows_of_Process[N];

  printf("********Computing Parallely using MPI*********\n");
  /* Process_Id 0 is Broadcasting the row A[0] and vector B to all the processes*/

  MPI_Bcast(&A[0][0],MAXN*MAXN,MPI_FLOAT,0,MPI_COMM_WORLD);
  MPI_Bcast(B,N,MPI_FLOAT,0,MPI_COMM_WORLD);    
      for(row=0; row<N; row++)
      {
          rows_of_Process[row]= row % no_Of_Processes;
      } 

      for(norm=0;norm<N;norm++)
      {

          MPI_Bcast (&A[norm][norm],N-norm,MPI_FLOAT,rows_of_Process[norm],MPI_COMM_WORLD);
          MPI_Bcast (&B[norm],1,MPI_FLOAT,rows_of_Process[norm],MPI_COMM_WORLD);         
          for(row= norm+1; row<N; row++) 
          {
              if(rows_of_Process[row] == process_Id)
              {
                C[row]=A[row][norm]/A[norm][norm];
				  
              }
          }               
          for(row= norm+1; row<N; row++) 
          {       
              if(rows_of_Process[row] == process_Id)
              {
                  for(col=0;col<N;col++)
                  {
                      A[row][col] -= A[norm][col] * C[row];
                }
                  B[row] -= B[norm]* C[row] ;
              }
          }
      }
  if (process_Id==0){
 /* Back substitution */
  for (row = N - 1; row >= 0; row--) {
    X[row] = B[row];
    for (col = N-1; col > row; col--) {
      X[row] -= A[row][col] * X[col];
      
    }   
    X[row] /= A[row][row];  
  }
}
}
