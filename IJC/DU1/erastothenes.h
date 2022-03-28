/**
 * @file erastothenes.h
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU1, assignment A)
 *        Compiled using gcc (Debian 10.2.1-6) 10.2.1
 * @date 2021-02-27
 * 
 */

#include <math.h>
#include "bitset.h"

#ifndef _ERASTOTHENES_H_
#define _ERASTOTHENES_H_

/**
 * @brief Multiples of 2
 *  (0x55555555)16 = (0101 0101 0101 0101 0101 0101 0101 0101)2
 *  (0xBBBBBBBB)16 = (1010 1010 1010 1010 1010 1010 1010 1010)2
 * 
 */ 
#define _MUL2 0x55555555UL

/**
 * @brief Finds all the prime numbers from 0 to N-1,
 *        where N is the size of input bit array.
 *        Non-prime numbers will have their corresponding
 *        indexes set to 1, primes to 0.
 * 
 * @param arr bitset_t filled with 0s
 */
void Erastothenes(bitset_t arr);

#endif /*** End of _ERASTOTHENES_H_ ***/