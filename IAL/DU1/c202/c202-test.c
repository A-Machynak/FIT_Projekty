
/* **************************** c202-test.c ********************************* */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c202 - Zásobník znaků v poli                                        */
/*  Vytvořil: Václav Topinka, září 2005                                       */
/*  Úpravy: Kamil Jeřábek, září 2019                                          */
/*          Daniel Dolejška, září 2021                                        */
/* ************************************************************************** */

/* Základní testy pro c202.c */

#include "c202.h"

#include <stdio.h>
#include <stdlib.h>

Stack *stack;
int STACK_SIZE;
int error_flag;
int solved;

/****************************************************************************** 
 * Speciální ošetření testovaných funkcí.                                     *
 ******************************************************************************/

/** Vytiskne obsah zásobníku. */
void stackPrint( Stack *stack ) {
	if (solved)
	{
		int maxi = stack->topIndex;
		if (stack->topIndex > STACK_SIZE - 1)
		{
			printf("Warning: stack->topIndex (value: %i) is out of range STACK_SIZE (%i).\n", stack->topIndex,
			       STACK_SIZE);
			maxi = STACK_SIZE - 1;
		}
		printf("--- BOTTOM [ ");
		for (int i = 0; i <= maxi; i++)
			putchar(stack->array[i]);
		printf(" ] TOP ---\n");
	}
}

/** Zavolá funkci Stack_Init a v případě, že nebyla řešena, ohlásí to. */
void use_stack_init( Stack *stack ) {
	solved = TRUE;
	Stack_Init(stack);
	if (!solved)
		printf("[W] Funkce Stack_Init nebyla implementována.\n");
}

/** Zavolá funkci Stack_IsEmpty a vytiskne výsledek. */
void use_stack_empty( Stack *stack ) {
	solved = TRUE;
	int i = Stack_IsEmpty(stack);
	if (!solved)
		printf("[W] Funkce Stack_IsEmpty nebyla implementována.\n");
	else
		printf("Stack_IsEmpty returned '%s'\n", i ? "TRUE" : "FALSE");
}

/** Zavolá funkci Stack_IsFull a vytiskne výsledek. */
void use_stack_full( Stack *stack ) {
	solved = TRUE;
	int i = Stack_IsFull(stack);
	if (!solved)
		printf("[W] Funkce Stack_IsFull nebyla implementována.\n");
	else
		printf("Stack_IsFull returned '%s'\n", i ? "TRUE" : "FALSE");
}

/** Zavolá funkci Stack_Top a vytiskne výsledek. */
void use_stack_top( Stack *stack ) {
	solved = TRUE;
	error_flag = 0;
	char c;
	Stack_Top(stack, &c);
	if (!solved)
		printf("[W] Funkce Stack_Top nebyla implementována.\n");
	else if (!error_flag)
		printf("Stack_Top returned '%c'\n", c);
}

/** Zavolá funkci Stack_Pop. */
void use_stack_pop( Stack *stack ) {
	solved = TRUE;
	Stack_Pop(stack);
	if (!solved)
		printf("[W] Funkce Stack_Pop nebyla implementována.\n");
}

/** Zavolá funkci Stack_Push. */
void use_stack_push( Stack *stack, char c ) {
	solved = TRUE;
	Stack_Push(stack, c);
	if (!solved)
		printf("[W] Funkce Stack_Push nebyla implementována.\n");
}

/****************************************************************************** 
 * Vlastní testování                                                          *
 ******************************************************************************/

int main() {
	printf("C202 - Stack Implemented Using an Array - Basic Tests\n");
	printf("-----------------------------------------------------\n");

	STACK_SIZE = 8;
	stack = (Stack *) malloc(sizeof(Stack));

	printf("\n[TEST01] Stack initialization\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	use_stack_init(stack);
	stackPrint(stack);

	printf("\n[TEST02] Checking a state of the stack\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	use_stack_empty(stack);
	use_stack_full(stack);

	printf("\n[TEST03] Pushing a first item 'A'\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	use_stack_push(stack, 'A');
	stackPrint(stack);

	printf("\n[TEST04] Checking a state of the stack again\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	use_stack_empty(stack);
	use_stack_full(stack);

	printf("\n[TEST05] Reading and removing the top item\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	use_stack_top(stack);
	use_stack_pop(stack);
	stackPrint(stack);

	printf("\n[TEST06] Pushing items from '0' to '7'\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	for (int i = 0; i < 8; i++)
		use_stack_push(stack, '0' + i);
	stackPrint(stack);

	printf("\n[TEST07] Checking a state of the stack\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	use_stack_empty(stack);
	use_stack_full(stack);

	printf("\n[TEST08] Removing all items one by one\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	for (int i = 0; i < 8; i++)
	{
		use_stack_top(stack);
		use_stack_pop(stack);
	}
	stackPrint(stack);

	printf("\n[TEST09] Final check of the stack\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	use_stack_empty(stack);
	use_stack_full(stack);

	printf("\n\n----- C202 - The End of Basic Tests -----\n");

	free(stack);
	return (0);
}

/* Konec c202-test.c */
