/**
 * @file htab_for_each.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 2)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-08
 * 
 */

#include "htab.h"
#include "htab_priv.h"

void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data)) {
    if(t == NULL) return;
    for(size_t i = 0; i < t->arr_size; i++) {
        if(t->pArr[i] != NULL) {
            struct htab_item *pItem = t->pArr[i];
            while(pItem != NULL) {
                f(&(pItem->pair));
                if(pItem == NULL) break; // Table was changed... What to do..?
                                         // Could create copy of 't' with htab_move() (?)
                                         // TODO idk...
                pItem = pItem->next;
            }
        }
    }
}