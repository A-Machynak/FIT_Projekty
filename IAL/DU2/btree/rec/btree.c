/*
 * Binárny vyhľadávací strom — rekurzívna varianta
 *
 * S využitím dátových typov zo súboru btree.h a pripravených kostier funkcií
 * implementujte binárny vyhľadávací strom pomocou rekurzie.
 */

#include "../btree.h"
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
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
  if(!tree) {
    return false;
  } else if(tree->key == key) {
    *value = tree->value;
    return true;
  } else if(tree->left && tree->key > key) {
    return bst_search(tree->left, key, value);
  } else if(tree->right && tree->key < key) {
    return bst_search(tree->right, key, value);
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
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
  if(!*tree) {
    bst_node_t *node = malloc(sizeof(bst_node_t));
    if(!node) {
      // err
      return;
    }
    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    *tree = node;
  } else if((*tree)->key == key) {
    (*tree)->value = value;
  } else if((*tree)->key > key) {
    if((*tree)->left) {
      bst_insert(&(*tree)->left, key, value);
    } else {
      bst_node_t *node = malloc(sizeof(bst_node_t));
      if(!node) {
        // err
        return;
      }
      node->key = key;
      node->value = value;
      node->left = NULL;
      node->right = NULL;
      (*tree)->left = node;
    }
  } else if((*tree)->key < key) {
    if((*tree)->right) {
      bst_insert(&(*tree)->right, key, value);
    } else {
      bst_node_t *node = malloc(sizeof(bst_node_t));
      if(!node) {
        // err
        return;
      }
      node->key = key;
      node->value = value;
      node->left = NULL;
      node->right = NULL;
      (*tree)->right = node;
    }
  }
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
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
  if((*tree)->right && *tree) {
    if((*tree)->right->right) {
      bst_replace_by_rightmost(target, &(*tree)->right);
    } else {
      target->key = (*tree)->right->key;
      target->value = (*tree)->right->value;
      free((*tree)->right);
      (*tree)->right = NULL;
    }
  }
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
 * Funkciu implementujte rekurzívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key) {
  if(!tree || !*tree) {
    return;
  }

  if((*tree)->key == key) {
    if((*tree)->right && (*tree)->left) {
      bst_replace_by_rightmost(*tree, &(*tree)->left);
    } else if((*tree)->right) {
      bst_node_t *temp = (*tree)->right;
      free(*tree);
      *tree = temp;
    } else if((*tree)->left) {
      bst_node_t *temp = (*tree)->left;
      free(*tree);
      *tree = temp;
    } else {
      free(*tree);
      *tree = NULL;
    }
  } else if((*tree)->right && (*tree)->right->key == key) {
    if((*tree)->right->right && (*tree)->right->left) {
      bst_replace_by_rightmost((*tree)->right, &(*tree)->right->left);
    } else if((*tree)->right->right) {
      bst_node_t *temp = (*tree)->right->right;
      free((*tree)->right);
      (*tree)->right = temp;
    } else if((*tree)->right->left) {
      bst_node_t *temp = (*tree)->right->left;
      free((*tree)->right);
      (*tree)->right = temp;
    } else {
      free((*tree)->right);
      (*tree)->right = NULL;
    }
  } else if((*tree)->left && (*tree)->left->key == key) {
    if((*tree)->left->right && (*tree)->left->left) {
      bst_replace_by_rightmost((*tree)->left, &(*tree)->left->left);
    } else if((*tree)->left->right) {
      bst_node_t *temp = (*tree)->left->right;
      free((*tree)->left);
      (*tree)->left = temp;
    } else if((*tree)->left->left) {
      bst_node_t *temp = (*tree)->left->left;
      free((*tree)->left);
      (*tree)->left = temp;
    } else {
      free((*tree)->left);
      (*tree)->left = NULL;
    }
  } else {
    if((*tree)->key > key) {
      bst_delete(&(*tree)->left, key);
    } else {
      bst_delete(&(*tree)->right, key);
    }
  }
}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree) {
  if(*tree) {
    bst_dispose(&(*tree)->left);
    bst_dispose(&(*tree)->right);
    free(*tree);
    *tree = NULL;
  }
}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
  if(tree) {
    bst_print_node(tree);
    bst_preorder(tree->left);
    bst_preorder(tree->right);
  }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) {
  if(tree) {
    bst_inorder(tree->left);
    bst_print_node(tree);
    bst_inorder(tree->right);
  }
}
/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
  if(tree) {
    bst_postorder(tree->left);
    bst_postorder(tree->right);
    bst_print_node(tree);
  }
}
