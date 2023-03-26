# Set up
    sudo apt install mipch

# sequential 
    run OpenMP version (set only 1 thread)

# OpenMP
    gcc -fopenmp sieve-openmp -o sieve-openmp
    ./sieve-openmp    

# MPI
    mpicc sieve-mpi.c -o sieve-mpi
    mpirun -np 4 ./sieve-mpi

# OpenMP + MPI 
    mpicc -fopenmp sieve-hybrid -o sieve-hybrid
    mpirun -np 4 ./sieve-hybrid