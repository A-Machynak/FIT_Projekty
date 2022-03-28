/**
 * @file htab_clear.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 2)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-08
 * 
 */
#include "htab.h"
#include "htab_priv.h"

void htab_clear(htab_t * t) {
    if(t == NULL) return;
    struct htab_item *pNextItem, *pItem;
    for(size_t i = 0; i < t->arr_size; i++) {
        pItem = t->pArr[i];
        while(pItem != NULL) {
            pNextItem = pItem->next;
            free((htab_key_t *)pItem->pair.key);
            free(pItem);
            pItem = pNextItem;
        }
        t->pArr[i] = NULL;
    }
    t->size = 0;
}