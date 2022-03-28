
/* ******************************* c202.h *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c202 - Zásobník znaků v poli                                        */
/*  Hlavičkový soubor pro c202.c                                              */
/*  Vytvořil: Václav Topinka, září 2005                                       */
/*  Úpravy: Kamil Jeřábek, září 2019                                          */
/*          Daniel Dolejška, září 2021                                        */
/* ************************************************************************** */

/* TENTO SOUBOR, PROSÍME, NEUPRAVUJTE! */

#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>

#define TRUE 1
#define FALSE 0

/**
 * Hodnota MAX_STACK udává skutečnou velikost statického pole pro uložení
 * hodnot zásobníku.
 * Při implementaci hodnotu MAX_STACK vůbec nepoužívejte.
 */
#define MAX_STACK 20

/**
 * Při implementaci operací nad ADT zásobník předpokládejte, že velikost tohoto
 * pole je pouze STACK_SIZE.
 * Umožní to jednoduše měnit velikost zásobníku v průběhu testování.
 */
extern int STACK_SIZE;

/** Globální proměnná - indikuje, zda operace volala chybu. */
extern int error_flag;
/** Globální proměnná - indikuje, zda byla operace řešena. */
extern int solved;

/** Celkový počet možných chyb. */
#define MAX_SERR    3
/** Chyba při Stack_Init. */
#define SERR_INIT   1
/** Chyba při Stack_Push. */
#define SERR_PUSH   2
/** Chyba při Stack_Top. */
#define SERR_TOP    3

/** ADT zásobník implementovaný ve statickém poli. */
typedef struct {
	/** Pole pro uložení hodnot. */
	char array[MAX_STACK];
	/** Index prvku na vrcholu zásobníku. */
	int topIndex;
} Stack;

void Stack_Error( int );

void Stack_Init( Stack * );

int Stack_IsEmpty( const Stack * );

int Stack_IsFull( const Stack * );

void Stack_Top( const Stack *, char * );

void Stack_Pop( Stack * );

void Stack_Push( Stack *, char );

#endif

/* Konec hlavičkového souboru c202.h */
