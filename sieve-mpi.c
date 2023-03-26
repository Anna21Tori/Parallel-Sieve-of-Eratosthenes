#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>

#define N 100000000 /* given limit - size of computation!!!! 
                        For me minimu size for worth measuring time is 100000000 because algorithm is very fast! */
#define S (int)sqrt(N)  /* divide the range 2 through limit into segments of some size  <= sqrt(limit)*/
#define M N /*size of factors vector*/

//all factors are initially prime
void set_factors(long int *a){
    int i;
    for (i = 2; i <= S; i++){
        a[i] = 1;
    }
}
//Find the primes in the first (lowest) segment
void pre_marked_primes(int *num_factors, int *num_primes, long int *A, long int *a, long int *primes){
    int i, k;
    *num_factors = 0;
    *num_primes = 0;
    for (i = 2; i <= S; i++){
        if (a[i] == 1) {
            /* Marked it! It can be prime */
            primes[(*num_primes)++] = i;
            A[(*num_factors)++] = i; 

            /* Multiples of prime should not be marked*/
            for (k = i + i; k <= S; k += i){
                a[k] = 0;
            } 
        }
    }
}

void marked_primes(int *num_factors, int *num_primes, long int *A, long int *a, long int *primes, int p, int rank){
    int i, k, rest, num;
    int sub_length = (N - S) / p;

    if ((N - S) % p != 0) 
        sub_length++;

    if(rank > 0)
        *num_primes = 0; //reset - elimination of primes dumplicates found by pre_marked_primes function

    for (num = S + 1 + sub_length * rank; num <= S + 1 +sub_length * (rank + 1); num++) {
        if(num <= N){
            for (k = 0; k < (*num_factors); k++) {
                rest = (num % A[k]);
                if (rest == 0) 
                    break; /* composite number */
            }
            if (rest != 0) {
                primes[(*num_primes)++] = num; /* prime number */
            }
        }
        
    }
}

int main(int argc, char** argv) {
    
    long int *A = (long int*) malloc(sizeof(long int)*(S+1));
    long int *primes = (long int*) malloc(sizeof(long int)*M);
    long int *a = (long int*) malloc(sizeof(long int)*(S+1)); //temp vector for A

    long int *final_primes = NULL; //only important for root (rank = 0)
    int *displacements = NULL; //only important for root (rank = 0)
    int *num_found_primes = NULL; //only important for root (rank = 0)

    //counters for factors and primes
    int num_factors = 0; 
    int num_primes = 0;
    int *ptr_num_factors = &num_factors; 
    int *ptr_num_primes = &num_primes;
    
    int global_num_primes = 0; //only important for root (rank = 0)

    double time; 

    int rank, p, i;         

    MPI_Status status;
    MPI_Init(&argc, &argv);

    time = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    //parallel sieve algorithm - MPI version
    set_factors(a);
    pre_marked_primes(ptr_num_factors, ptr_num_primes, A, a, primes);
    marked_primes(ptr_num_factors, ptr_num_primes, A, a, primes, p, rank);

    //Gather result from each processes to root process

    if(rank == 0){
        num_found_primes = (int*) malloc(sizeof(int)*p);
    }

    //compute sum of number found primes by each proc
    MPI_Reduce(&num_primes, &global_num_primes, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    //gather number found primes form each process to root process to compute displacements - id is equvalent rank 
    MPI_Gather(&num_primes, 1, MPI_INT, num_found_primes, 1, MPI_INT, 0, MPI_COMM_WORLD);
    

    if(rank == 0){
        displacements = (int*) malloc(sizeof(int)*p);

        //compute vector displacements needed by Gatherv - each process found different number of primes
        displacements[0] = 0;

        for (i = 1; i < p; i++) {
            displacements[i] = displacements[i-1] + num_found_primes[i - 1] ;
        }
    
        final_primes = (long int*) malloc(sizeof(long int)*global_num_primes);
    } 
       
    MPI_Gatherv(primes, num_primes, MPI_LONG, final_primes, num_found_primes, displacements, MPI_LONG, 0, MPI_COMM_WORLD);

    //stop measuring time after ending parallel sieve algorithm
    time = MPI_Wtime() - time;

    if (rank == 0) {
    
        //sum up parallel exection
        printf("Parallel time execution: %f\n", time);
        printf("Number found primes from %d through %d: %d\n", 2, N, global_num_primes);

        //Only for small limit if you want to verify result
        // printf("Found primes ************\n");
        // for (i = 0; i < global_num_primes; i++) {
        //     printf(" %ld ", final_primes[i]);
        //     if (i % 10 == 0){
        //         printf("\n");
        //     }
        // }
        // printf("\n*************\n");

    }

    //dynamically de-allocate the memory
    free(A);
    free(primes);
    free(a); 
    free(final_primes);
    free(displacements);
    free(num_found_primes);

    MPI_Finalize();
    return 0;
}


