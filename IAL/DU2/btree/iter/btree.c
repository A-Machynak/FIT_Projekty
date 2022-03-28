/*
 * Binárny vyhľadávací strom — iteratívna varianta
 *
 * S využitím dátových typov zo súboru btree.h, zásobníkov zo súborov stack.h a
 * stack.c a pripravených kostier funkcií implementujte binárny vyhľadávací
 * strom bez použitia rekurzie.
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializácia stromu.
 *
 * Užívateľ musí zaistiť, že incializácia sa nebude opakovane volať nad
 * inicializovaným stromom. V opačnom prípade môže dôjsť k úniku pamäte (memory
 * leak). Keďže neinicializovaný ukazovateľ má nedefinovanú hodnotu, nie je
 * možné toto detegovať vo funkcii.
 */
void bst_init(bst_node_t **tree) {
  *tree = NULL;
}

/*
 * Nájdenie uzlu v strome.
 *
 * V prípade úspechu vráti funkcia hodnotu true a do premennej value zapíše
 * hodnotu daného uzlu. V opačnom prípade funckia vráti hodnotu false a premenná
 * value ostáva nezmenená.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
  if(!tree) {
    return false;
  }

  bst_node_t *top = tree;
  if(top->key == key) {
    *value = top->value;
    return true;
  }

  while(top) {
    if(top->left && top->left->key == key) {
      *value = top->left->value;
      return true;
    }

    if(top->right && top->right->key == key) {
      *value = top->right->value;
      return true;
    }

    top = (top->key > key ? top->left : top->right);
  }

  return false;
}

/*
 * Vloženie uzlu do stromu.
 *
 * Pokiaľ uzol so zadaným kľúčom v strome už existuje, nahraďte jeho hodnotu.
 * Inak vložte nový listový uzol.
 *
 * Výsledný strom musí spĺňať podmienku vyhľadávacieho stromu — ľavý podstrom
 * uzlu obsahuje iba menšie kľúče, pravý väčšie.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
   if(!tree) {
    return;
  }
  bst_node_t *top = *tree;

  // Empty tree, insert top
  if(!top) {
    *tree = malloc(sizeof(bst_node_t));
    if(!*tree) {
        // err
        return;
    }
    (*tree)->left = NULL;
    (*tree)->right = NULL;
    (*tree)->key = key;
    (*tree)->value = value;
    return;
  }

  if(top->key == key) {
    top->value = value;
  }

  while(top) {
    if(!top->left && top->key > key) {
      bst_node_t *new_node = malloc(sizeof(bst_node_t));
      if(!new_node) {
        // err
        return;
      }
      new_node->left = NULL;
      new_node->right = NULL;
      new_node->key = key;
      new_node->value = value;

      top->left = new_node;
      return;
    }

    if(!top->right && top->key < key) {
      bst_node_t *new_node = malloc(sizeof(bst_node_t));
      if(!new_node) {
        // err
        return;
      }
      new_node->left = NULL;
      new_node->right = NULL;
      new_node->key = key;
      new_node->value = value;

      top->right = new_node;
      return;
    }

    if(top->left && top->left->key == key) {
      top->left->value = value;
      return;
    }

    if(top->right && top->right->key == key) {
      top->right->value = value;
      return;
    }

    top = (top->key > key ? top->left : top->right);
  }

  // err
}

/*
 * Pomocná funkcia ktorá nahradí uzol najpravejším potomkom.
 *
 * Kľúč a hodnota uzlu target budú nahradené kľúčom a hodnotou najpravejšieho
 * uzlu podstromu tree. Najpravejší potomok bude odstránený. Funkcia korektne
 * uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkcia predpokladá že hodnota tree nie je NULL.
 *
 * Táto pomocná funkcia bude využitá pri implementácii funkcie bst_delete.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
  if(!tree || !target) {
    return;
  }

  bst_node_t *top = *tree, *prevTop = NULL;

  if(!top->right) {
    return;
  }

  while(top->right) {
    prevTop = top;
    top = top->right;
  }

  target->key = top->key;
  target->value = top->value;

  prevTop->right = NULL;
  free(top);
}

/*
 * Odstránenie uzlu v strome.
 *
 * Pokiaľ uzol so zadaným kľúčom neexistuje, funkcia nič nerobí.
 * Pokiaľ má odstránený uzol jeden podstrom, zdedí ho otec odstráneného uzla.
 * Pokiaľ má odstránený uzol oba podstromy, je nahradený najpravejším uzlom
 * ľavého podstromu. Najpravejší uzol nemusí byť listom!
 * Funkcia korektne uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkciu implementujte iteratívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key) {
  if(!tree || !*tree) {
    return;
  }

  bst_node_t *top = *tree, *prevTop = NULL;

  while(top) {
    if(top->key == key) {
      if(top->left && top->right) {
        // 2 children
        bst_replace_by_rightmost(top, &top->left);
      } else if(top->left) {
        // 1 child
        if(prevTop) {
          if(prevTop->right == top) {
            prevTop->right = top->left;
          } else {
            prevTop->left = top->left;
          }
        } else {
          *tree = top->left;
        }
        free(top);
      } else if(top->right) {
        // 1 child
        if(prevTop) {
          if(prevTop->right == top) {
            prevTop->right = top->right;
          } else {
            prevTop->left = top->right;
          }
        } else {
          *tree = top->right;
        }
        free(top);
      } else {
        // no children
        if(prevTop) {
          if(prevTop->right == top) {
            prevTop->right = NULL;
          } else {
            prevTop->left = NULL;
          }
        } else {
          // delete from a tree with a single node
          *tree = NULL;
        }
        free(top);
      }
      return; // found
    }

    prevTop = top;
    top = (top->key > key ? top->left : top->right);
  }

  // not found
}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree) {
  if(!tree || !*tree) {
    return;
  }
  
  bst_node_t *top = *tree, *temp;
  stack_bst_t *stack = malloc(sizeof(stack_bst_t));
  if(!stack) {
    // err
    return;
  }
  stack_bst_init(stack);
  stack_bst_push(stack, top);

  while(!stack_bst_empty(stack)) {
    if(top->left) {
      stack_bst_push(stack, top->left);
      top = top->left;
      continue;
    }
    if(top->right) {
      stack_bst_push(stack, top->right);
      top = top->right;
      continue;
    }
    stack_bst_pop(stack);
    temp = top;
    top = stack_bst_top(stack);
    if(!stack_bst_empty(stack)) {
      if(temp == top->left) {
        top->left = NULL;
      } else {
        top->right = NULL;
      }
    }
    free(temp);
  }
  *tree = NULL;
  free(stack);
}

/*
 * Pomocná funkcia pre iteratívny preorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu.
 * Nad spracovanými uzlami zavola bst_print_node a uloží ich do zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit) {
  if(!tree || !to_visit) {
    return;
  }
  while(tree) {
    stack_bst_push(to_visit, tree);
    bst_print_node(tree);
    tree = tree->left;
  }
}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_preorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
  if(!tree) {
    return;
  }

  bst_node_t *top = tree;
  stack_bst_t *stack = malloc(sizeof(stack_bst_t));
  if(!stack) {
    // err
    return;
  }
  stack_bst_init(stack);

  bst_leftmost_preorder(top, stack);

  while(!stack_bst_empty(stack)) {
    top = stack_bst_pop(stack);
    bst_leftmost_preorder(top->right, stack);
  }
  free(stack);
}

/*
 * Pomocná funkcia pre iteratívny inorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
  if(!tree || !to_visit) {
    return;
  }
  while(tree) {
    stack_bst_push(to_visit, tree);
    tree = tree->left;
  }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_inorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) {
  if(!tree) {
    return;
  }

  bst_node_t *top = tree;
  stack_bst_t *stack = malloc(sizeof(stack_bst_t));
  if(!stack) {
    // err
    return;
  }
  stack_bst_init(stack);
  bst_leftmost_inorder(top, stack);
  while(!stack_bst_empty(stack)) {
    top = stack_bst_pop(stack);
    bst_print_node(top);
    bst_leftmost_inorder(top->right, stack);
  }
  free(stack);
}

/*
 * Pomocná funkcia pre iteratívny postorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov. Do zásobníku bool hodnôt ukladá informáciu že uzol
 * bol navštívený prvý krát.
 *
 * Funkciu implementujte iteratívne pomocou zásobníkov uzlov a bool hodnôt a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit) {
  if(!tree || !to_visit || !first_visit) {
    return;
  }

  while(tree) {
    stack_bst_push(to_visit, tree);
    stack_bool_push(first_visit, true);
    tree = tree->left;
  }
}

/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_postorder a
 * zásobníkov uzlov a bool hodnôt bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
  if(!tree) {
    return;
  }

  bst_node_t *top = tree;
  bool fromLeft;
  stack_bst_t *stack = malloc(sizeof(stack_bst_t));
  if(!stack) {
    // err
    return;
  }
  stack_bool_t *bStack = malloc(sizeof(stack_bool_t));
  if(!bStack) {
    // err
    return;
  }
  stack_bst_init(stack);
  stack_bool_init(bStack);
  bst_leftmost_postorder(top, stack, bStack);

  while(!stack_bst_empty(stack)) {
    top = stack_bst_top(stack);
    fromLeft = stack_bool_pop(bStack);
    if(fromLeft) {
      stack_bool_push(bStack, false);
      bst_leftmost_postorder(top->right, stack, bStack);
    } else {
      stack_bst_pop(stack);
      bst_print_node(top);
    }
  }
  free(stack);
  free(bStack);
}
