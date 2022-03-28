/**
 * @file htab_free.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 2)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-08
 * 
 */
#include "htab.h"
#include "htab_priv.h"

void htab_free(htab_t *t) {
    htab_clear(t);
    free(t);
}