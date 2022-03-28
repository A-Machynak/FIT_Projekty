/**
 * @file htab_bucket_count.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 2)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-08
 * 
 */
#include "htab.h"
#include "htab_priv.h"

size_t htab_bucket_count(const htab_t *t) {
    return (t == NULL) ? 0 : t->arr_size;
}