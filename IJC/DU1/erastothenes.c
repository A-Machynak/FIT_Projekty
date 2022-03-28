/**
 * @file erastothenes.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU1, assignment A)
 *        Compiled using gcc (Debian 10.2.1-6) 10.2.1
 * @date 2021-02-27
 * 
 */

#include "erastothenes.h"
#include <string.h>

void Erastothenes(bitset_t arr) {
    unsigned long arrSize = bitset_size(arr), sqrtN = sqrt(arrSize);
    register unsigned long count;

    // Set multiples of two to 1
    unsigned long top = ((arrSize / CHAR_BIT) / sizeof(unsigned long));
    
    for(count = 1; count <= top; count++) {
        memset(&(arr[count]), _MUL2, sizeof(unsigned long));
    }

    bitset_setbit(arr, 0, 1);
    bitset_setbit(arr, 1, 1);
    bitset_setbit(arr, 2, 0);

    for(unsigned long i = 3; i < sqrtN; i+=2) {
        if(!bitset_getbit(arr, i)) {
            // Skip multiples of two
            count = 3*i;
            do {
                bitset_setbit(arr, count, 1);
            } while((count += i) < arrSize);
        }
    }
}
