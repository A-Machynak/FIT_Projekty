/**
 * @file htab_priv.h
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 2)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-08
 * 
 */

#include "htab.h"

#ifndef __HTAB_PRIV__
#define __HTAB_PRIV__

// [(key, data), next]
struct htab_item {
    struct htab_item *next;
    struct htab_pair pair;
};

struct htab {
    size_t size;
    size_t arr_size;
    struct htab_item *pArr[];
};

#endif // __HTAB_PRIV__