# Gaussian-Elimination

Gaussian Elimination without pivoting using Pthreads and MPI

Goal: To transform a system of linear equations into an upper-triangular matrix in order to solve the unknowns and derive a solution. A multiplier is used to make the entries below the diagonal element as zero and then back-substitution is applied to derive the solution.

Steps to compile and run:

/***For gauss_mpi.c***/

Compile: mpicc gauss_mpi.c

Run: mpirun –np <no. of processes> ./a.out <matrix_dimension> <random seed> <output file name>

/***For gauss_thread.c***/

Compile: gcc gauss_thread.c –pthread

Run: ./a.out <matrix_dimension> <random seed> <output_file_name>


