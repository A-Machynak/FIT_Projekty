/**
 * @file primes.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU1, assignment A)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-02-27
 * 
 */

#include "erastothenes.h"
#include "bitset.h"
#include <stdio.h>
#include <time.h>

/**
 * @brief Size of the bit array
 *      = Find prime numbers from 0 to specified size
 * 
 */
#define ARR_SIZE 200000000UL

/**
 * @brief Print the highest N numbers
 * 
 */
#define PRINT_HIGHEST 10

int main(void) {
    unsigned long nums[PRINT_HIGHEST], counter = 0;
    // Start timer
    clock_t start = clock();

    bitset_create(p, ARR_SIZE);
    //bitset_alloc(p, ARR_SIZE);
    Erastothenes(p);
    
    // Find the 10 highest numbers
    for(int i = ARR_SIZE-1; i > 0; i--) {
        if(!bitset_getbit(p, i)) {
            nums[(PRINT_HIGHEST-1)-counter] = i;
            counter++;
        }
        if(counter == PRINT_HIGHEST) break;
    }

    // Print the numbers
    for(int i = 0; i < PRINT_HIGHEST; i++) {
        printf("%lu\n", nums[i]);
    }
    //bitset_free(p);
    
    // End timer
    fprintf(stderr, "Time=%.3g\n", (double)(clock()-start) / CLOCKS_PER_SEC);
    return 0;
}