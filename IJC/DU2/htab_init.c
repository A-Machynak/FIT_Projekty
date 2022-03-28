/**
 * @file htab_init.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 2)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-08
 * 
 */

#include "htab.h"
#include "htab_priv.h"

htab_t *htab_init(size_t n) {
    if(n == 0) return NULL;
    htab_t *tab = malloc(sizeof(htab_t) + n * sizeof(struct htab_item *));

    if(tab == NULL) {
        return NULL;
    }

    tab->arr_size = n;
    tab->size = 0;
    for(size_t i = 0; i < n; i++) {
        tab->pArr[i] = NULL;
    }
    
    return tab;
}