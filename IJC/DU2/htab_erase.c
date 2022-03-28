/**
 * @file htab_erase.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 2)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-08
 * 
 */
#include "htab.h"
#include "htab_priv.h"

bool htab_erase(htab_t * t, htab_key_t key) {
    if(t == NULL) return false;
    // Can't use htab_find - gotta find the previous key
    unsigned int index = (htab_hash_function(key) % t->arr_size);
    if(t->pArr[index] == NULL) {
        return false;
    }
    struct htab_item *pItem = t->pArr[index], *pItemPrev = t->pArr[index];
    while(pItem != NULL) {
        if(!strcmp(pItem->pair.key, key)) {
            free(((htab_pair_t *)pItem->pair.key));
            if(pItem != pItemPrev) {
                if(pItemPrev->next != NULL) {
                    pItemPrev->next->next = pItemPrev->next;
                }
                pItemPrev->next = NULL;
            } else {
                t->pArr[index] = NULL;
            }
            free(pItem);
            t->size--;
            return true;
        }
        pItemPrev = pItem;
        pItem = pItem->next;
    }
    return false;
}