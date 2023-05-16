#include <stdio.h>
#include <math.h>
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
void pre_marked_primes(long int *num_factors, long int *num_primes, long int *A, long int *a, long int *primes){
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

void marked_primes(long int *num_factors, long int *num_primes, long int *A, long int *a, long int *primes){
    int i, k, rest, num;
    for (num = S + 1; num <= N; num++) {
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

int main(int argc, char** argv) {


    long int *A = (long int*) malloc(sizeof(long int)*(S+1));
    long int *primes = (long int*) malloc(sizeof(long int)*M);
    long int *a = (long int*) malloc(sizeof(long int)*(S+1)); //temp vector for A
    
    int i; //temp var
    
    //counters for factors and primes
    long int num_factors = 0; 
    long int num_primes = 0;
    long int *ptr_num_factors = &num_factors; 
    long int *ptr_num_primes = &num_primes;

    set_factors(a);
    pre_marked_primes(ptr_num_factors, ptr_num_primes, A, a, primes);
    marked_primes(ptr_num_factors, ptr_num_primes, A, a, primes);

    //Only for small limit if you want to verify result
    // printf("Found primes ************\n");
    // for (i = 0; i < num_primes; i++) {
    //     printf(" %d ", primes[i]);
    //     if (i % 10 == 0){
    //         printf("\n");
    //     }
    // }
    // printf("\n*************\n");

    //dynamically de-allocate the memory
    free(A);
    free(primes);
    free(a);

    return 0;
}