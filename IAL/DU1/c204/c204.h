
/* ******************************* c204.h *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, září 2019                                         */
/*           Daniel Dolejška, září 2021                                       */
/* ************************************************************************** */

/* TENTO SOUBOR, PROSÍME, NEUPRAVUJTE! */

#ifndef _INFIX2POSTFIX_H_
#define _INFIX2POSTFIX_H_

#define FALSE 0
#define TRUE 1

#include <stdio.h>
#include <stdlib.h>

/** Využívá příklad c202 - zásobník. */
#include "c202.h"

/** Maximální délka řetězce výrazu. */
#define MAX_LEN 64

/** Globální proměnná - indikuje, zda byla operace řešena. */
extern int solved;

char *infix2postfix( const char *infixExpression );

#endif

/* Konec hlavičkového souboru c204.h */
