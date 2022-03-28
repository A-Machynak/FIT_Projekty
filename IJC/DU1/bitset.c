/**
 * @file bitset.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU1, assignment A)
 *        Compiled using gcc (Debian 10.2.1-6) 10.2.1
 * @date 2021-02-27
 * 
 */
#include "bitset.h"

extern void bitset_free(bitset_t arrName);

extern unsigned long bitset_size(bitset_t arrName);

extern void bitset_setbit(bitset_t arrName, bitset_index_t index, unsigned long set);

extern unsigned long bitset_getbit(bitset_t arrName, bitset_index_t index);
