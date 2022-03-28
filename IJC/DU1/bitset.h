/**
 * @file bitset.h
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU1, assignment A)
 *        Compiled using gcc (Debian 10.2.1-6) 10.2.1
 * @date 2021-02-27
 * 
 */

#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include "error.h"

#ifndef _BITSET_H_
#define _BITSET_H_

typedef unsigned long bitset_t[];

typedef unsigned long bitset_index_t;

/**
 * @brief Create array of N(size) bits
 * 
 */
#define bitset_create(arrName, size) \
        static_assert((size) > 0, "bitset_create: Pole musi mit alespon 1 bit."); \
        unsigned long arrName[(((size) / CHAR_BIT) / sizeof(unsigned long)) + ((size) % sizeof(unsigned long) == 0UL ? 1UL : 2UL)] = { (size), 0 }

/**
 * @brief Create array of N(size) bits (on heap)
 * 
 */
#define bitset_alloc(arrName, size) \
        unsigned long *arrName = calloc((((size) / CHAR_BIT) / sizeof(unsigned long)) + ((size) % sizeof(unsigned long) == 0UL ? 1UL : 2UL), sizeof(unsigned long)); \
        if(arrName == NULL) error_exit("bitset_alloc: Chyba alokace pameti."); \
        arrName[0] = size


#ifndef USE_INLINE /*** USE_INLINE NOT DEFINED ***/

/**
 * @brief Free array allocated by bitset_alloc()
 * 
 */
#define bitset_free(arrName) free(arrName)

/**
 * @brief Get size of an array created by bitset_create or bitset_alloc
 * 
 */
#define bitset_size(arrName) arrName[0]

/**
 * @brief Set bit in an array on specific index to a value (0 / 1)
 *        Exits if the index is out of bounds.
 * 
 */
#define bitset_setbit(arrName, index, set) \
        if((bitset_index_t)(index) >= (arrName[0])) \
                error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu", (unsigned long)(index), (unsigned long)arrName[0]-1); \
        if(set) arrName[((index) / (sizeof(unsigned long) * CHAR_BIT)) + 1UL] |= 1UL << ((index) & ((sizeof(unsigned long) * CHAR_BIT)-1)); \
        else arrName[((index) / (sizeof(unsigned long) * CHAR_BIT)) + 1UL] &= ~(1UL << ((index) & ((sizeof(unsigned long) * CHAR_BIT)-1))); \

/**
 * @brief Get bit value on specific index
 * 
 */
#define bitset_getbit(arrName, index) \
        (((bitset_index_t)(index) < arrName[0]) ? \
        ((arrName[((index) / (sizeof(unsigned long) * CHAR_BIT)) + 1UL] & (1UL << ((index) & ((sizeof(unsigned long) * CHAR_BIT)-1)))) > 0) : \
        (error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu", (unsigned long)(index), (unsigned long)arrName[0]-1), 0))


#else /*** USE_INLINE DEFINED ***/

/**
 * @brief Free array allocated by bitset_alloc()
 * 
 */
inline void bitset_free(bitset_t arrName) {
        free(arrName);
}

/**
 * @brief Get size of an array created by bitset_create or bitset_alloc
 * 
 */
inline unsigned long bitset_size(bitset_t arrName) {
        return arrName[0];
}

/**
 * @brief Set bit in an array on specific index to a value (0 / 1)
 *        Exits if the index is out of bounds.
 * 
 */
inline void bitset_setbit(bitset_t arrName, bitset_index_t index, unsigned long set) {
        if(index > (arrName[0] - 1))
                error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu", (unsigned long)(index), (unsigned long)arrName[0]-1);
        if(set)
                arrName[((index) / (sizeof(unsigned long) * CHAR_BIT)) + 1UL] |= 1UL << ((index) & ((sizeof(unsigned long) * CHAR_BIT)-1));
        else
                arrName[((index) / (sizeof(unsigned long) * CHAR_BIT)) + 1UL] &= ~(1UL << ((index) & ((sizeof(unsigned long) * CHAR_BIT)-1)));
}

/**
 * @brief Get bit value on specific index
 * 
 */
inline unsigned long bitset_getbit(bitset_t arrName, bitset_index_t index) {
        return (((bitset_index_t)(index) < arrName[0]) ?
                ((arrName[((index) / (sizeof(unsigned long) * CHAR_BIT)) + 1UL] & (1UL << ((index) & ((sizeof(unsigned long) * CHAR_BIT)-1)))) > 0) :
                (error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu", (unsigned long)(index), (unsigned long)arrName[0]-1), 0));
}

#endif /*** End of USE_INLINE ***/
#endif /*** End of _BITSET_H_ ***/
