/**
 * @file htab_find.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 2)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-08
 * 
 */
#include "htab.h"
#include "htab_priv.h"

htab_pair_t * htab_find(htab_t *t, htab_key_t key) {
    if(t == NULL) return NULL;
    unsigned int index = (htab_hash_function(key) % t->arr_size);
    if(t->pArr[index] == NULL) {
        return NULL;
    }
    struct htab_item *pItem = t->pArr[index];
    while(pItem != NULL) {
        if(!strcmp(pItem->pair.key, key)) {
            return &(pItem->pair);
        }
        pItem = pItem->next;
    }
    return NULL;
}