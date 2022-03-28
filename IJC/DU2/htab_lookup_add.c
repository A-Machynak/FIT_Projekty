/**
 * @file htab_lookup_add.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 2)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-08
 * 
 */
#include "htab.h"
#include "htab_priv.h"

htab_pair_t * htab_lookup_add(htab_t *t, htab_key_t key) {
    if(t == NULL) return NULL;
    unsigned int index = (htab_hash_function(key) % t->arr_size);
    struct htab_item *pItem = t->pArr[index];
    if(pItem != NULL) {
        struct htab_item *pPrevItem = pItem;
        while(pItem != NULL) {
            if(!strcmp(pItem->pair.key, key)) {
                // Found index and key, return it
                return &(pItem->pair);
            }
            pPrevItem = pItem;
            pItem = pItem->next;
        }
        // Index found, but not the key
        pPrevItem->next = malloc(sizeof(struct htab_item));
        pItem = pPrevItem->next;
    } else {
        // Index not found
        t->pArr[index] = malloc(sizeof(struct htab_item));
        pItem = t->pArr[index];
    }

    if(pItem == NULL) {
        return NULL;
    }

    char *cpy = malloc(strlen(key) + 1);
    if(cpy == NULL) {
        free(pItem);
        return NULL;
    }
    strcpy(cpy, key);
    pItem->pair.key = cpy;
    pItem->pair.value = 0;
    pItem->next = NULL;
    t->size++;
    return &(pItem->pair);
}