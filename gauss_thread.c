/* Gaussian elimination without pivoting.
 * Compile with "gcc gauss_thread.c –pthread"
 */

#include <stdio.h>
        #include <stdlib.h>
        #include <unistd.h>
        #include <math.h>
        #include <sys/types.h>
        #include <sys/times.h>
        #include <sys/time.h>
        #include <time.h>
/* inculded pthread.h for processing on threads*/
        #include <string.h>
        #include <pthread.h>

/* Program Parameters */
        #define MAXN 2000  /* Max value of N */
        int N;  /* Matrix size */

/* Matrices and vectors */
        volatile float A[MAXN][MAXN], B[MAXN], X[MAXN];
/* A * X = B, solve for X */
/* declared a variable for file name to put the output in a file*/
        char *nameOfOutputFile;

/* junk */
        #define randm() 4|2[uid]&3

/* Prototype */
        void gauss();  /* The function you will provide.
		* It is this routine that is timed.
		* It is called only on the parent.
		*/
        void *gauss_Elimination_In_Parallel(void *entry);

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
        
/* Print parameters */
        printf("\nMatrix dimension N = %i.\n", N);
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
        printf("%5.2f%s", A[row][col], (col < N-1) ? ", " : ";\n\t");
        }
        }
        printf("\nB = [");
        for (col = 0; col < N; col++) {
        printf("%5.2f%s", B[col], (col < N-1) ? "; " : "]\n");
        }
        }
        }

        void print_X() {
        int row;

        if (N < 100) {
        printf("\nX = [");
        for (row = 0; row < N; row++) {
        printf("%5.2f%s", X[row], (row < N-1) ? "; " : "]\n");
        }
        }
        }

        int main(int argc, char **argv) {
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

/*opening the file*/
        outputfilePtr = fopen(nameOfOutputFile, "w+");
        if(outputfilePtr == NULL){
        printf("unable to create/override the file");
        }

/* Initialize A and B */
        initialize_inputs();

/* Print input matrices */
        print_inputs();

/* Start Clock */
        printf("\nStarting clock.\n");
        gettimeofday(&etstart, &tzdummy);
        etstart2 = times(&cputstart);

/* Gaussian Elimination */
        gauss();

/* Stop Clock */
        gettimeofday(&etstop, &tzdummy);
        etstop2 = times(&cputstop);
        printf("Stopped clock.\n");
        usecstart = (unsigned long long)etstart.tv_sec * 1000000 + etstart.tv_usec;
        usecstop = (unsigned long long)etstop.tv_sec * 1000000 + etstop.tv_usec;

/* Display output */
        print_X();




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

        exit(0);
        }
        
/* Gauss Elimination Using Pthreads*/

        void gauss() {
        int norm, row, col;  /* Normalization row, and zeroing
      * element row and col */
        float multiplier;

        printf("Computing in parallel using pthreads.\n");
/*Defining thread reference variables*/
        pthread_t thread[N];

/* Gaussian elimination */
        for (norm = 0; norm < N - 1; norm++) {
/*Create an entry point for the thread*/
        int *entry = malloc(sizeof(*entry));
        if ( entry == NULL ) {
        printf("Memory Allocation Failed!!!\n");
        exit(0);
        }

        *entry = norm;
/*Create the thread and calling a function for gauss elimination for evry thread*/
        pthread_create(&thread[norm], NULL, gauss_Elimination_In_Parallel,entry);
        }
/*Join everything back up*/
        for (norm = 0; norm < N - 1; norm++) {
        pthread_join(thread[norm], NULL);
        }
/* (Diagonal elements are not normalized to 1.  This is treated in back
   * substitution.)
   */
/* Back substitution */
        for (row = N - 1; row >= 0; row--) {
        X[row] = B[row];
        for (col = N-1; col > row; col--) {
        X[row] -= A[row][col] * X[col];
        }
        X[row] /= A[row][row];
        }
        }

        void *gauss_Elimination_In_Parallel(void * entry){
        int norm = *((int *) entry);

        float multiplier;
        int row, col;
        for (row = norm + 1; row < N; row++) {
        multiplier = A[row][norm] / A[norm][norm];
        for (col = norm; col < N; col++) {
        A[row][col] -= A[norm][col] * multiplier;
        }
        B[row] -= B[norm] * multiplier;
        }
        free(entry);
        pthread_exit(0);
        }


