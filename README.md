# Gaussian-Elimination

Gaussian Elimination without pivoting using Pthreads and MPI

Goal: To transform a system of linear equations into an upper-triangular matrix in order to solve the unknowns and derive a solution. A multiplier is used to make the entries below the diagonal element as zero and then back-substitution is applied to derive the solution.

Steps to run:

For gauss_mpi.c:

mpicc gauss_mpi.c
mpirun –np <no. of processes> ./a.out <matrix_dimension> <random seed> <output file name>

For gauss_thread.c:

gcc gauss_thread.c –pthread
./a.out <matrix_dimension> <random seed> <output_file_name>


