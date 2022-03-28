/**
 * @file htab_move.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 2)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-08
 * 
 */
#include "htab.h"
#include "htab_priv.h"

htab_t *htab_move(size_t n, htab_t *from) {
    if(from == NULL) return NULL;
    htab_t *tab = htab_init(n);
    if(tab == NULL) return NULL;
    // Ineffective?
    for(size_t i = 0; i < from->arr_size; i++) {
        if(from->pArr[i] == NULL) continue;

        const char *tmpKey = from->pArr[i]->pair.key;
        // calculate index for new htab
        unsigned int newIndex = (htab_hash_function(tmpKey) % n);
        tab->pArr[newIndex] = malloc(sizeof(struct htab_item));
        if(tab->pArr[newIndex] == NULL) { // oh no..
            htab_free(tab);
            return NULL;
        }

        char *cpy = malloc(strlen(tmpKey) + 1);
        if(cpy == NULL) {
            free(tab->pArr[newIndex]);
            htab_free(tab);
            return NULL;
        }
        strcpy(cpy, tmpKey);
        tab->pArr[newIndex]->pair.key = cpy;
        tab->pArr[newIndex]->pair.value = tab->pArr[newIndex]->pair.value;
        tab->pArr[newIndex]->next = NULL;
    }
    htab_clear(from);
    return tab;
}