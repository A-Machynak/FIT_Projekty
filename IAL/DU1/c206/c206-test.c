
/*
 *  Předmět: Algoritmy (IAL) - FIT VUT v Brně
 *  Základní testy pro příklad c206.c (Dvousměrně vázaný lineární seznam)
 *  Vytvořil: Martin Tuček, září 2005
 *  Úprava: Kamil Jeřábek, září 2019
 *          Daniel Dolejška, září 2021
 */

#include "c206.h"

DLList TEMPLIST;
int ElemValue = 1;
/* Handles wrongly linked lists. */
int MaxListLength = 100;

/*******************************************************************************
 * Pomocné funkce usnadňující testování vlastní implementace.
 ******************************************************************************/

void print_elements_of_list( DLList TL ) {
	DLList TempList = TL;
	int CurrListLength = 0;
	printf("-----------------");
	while ((TempList.firstElement != NULL) && (CurrListLength < MaxListLength))
	{
		printf("\n \t%d", TempList.firstElement->data);
		if ((TempList.firstElement == TL.activeElement) && (TL.activeElement != NULL))
			printf("\t <= toto je aktivní prvek ");
		TempList.firstElement = TempList.firstElement->nextElement;
		CurrListLength++;
	}
	if (CurrListLength >= MaxListLength)
		printf("\nList exceeded maximum length!");
	printf("\n-----------------\n");
}

int test_DLL_Init() {
	solved = TRUE;
	DLL_Init(&TEMPLIST);
	if (!solved)
	{
		printf("Operace List_Init nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_Dispose() {
	solved = TRUE;
	DLL_Dispose(&TEMPLIST);
	if (!solved)
	{
		printf("Operace DLL_Dispose nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_InsertFirst() {
	solved = TRUE;
	DLL_InsertFirst(&TEMPLIST, ElemValue);
	if (!solved)
	{
		printf("Operace DLL_InsertFirst nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_InsertLast() {
	solved = TRUE;
	DLL_InsertLast(&TEMPLIST, ElemValue);
	if (!solved)
	{
		printf("Operace DLL_InsertLast nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_First() {
	solved = TRUE;
	DLL_First(&TEMPLIST);
	if (!solved)
	{
		printf("Operace DLL_First nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_Last() {
	solved = TRUE;
	DLL_Last(&TEMPLIST);
	if (!solved)
	{
		printf("Operace DLL_Last nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_GetFirst() {
	solved = TRUE;
	int temp;
	DLL_GetFirst(&TEMPLIST, &temp);
	if (!solved)
	{
		printf("Operace DLL_GetFirst nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		if (error_flag)
		{
			printf("Operace DLL_GetFirst volala funkci DLL_Error.\n");
			error_flag = FALSE;
			return (FALSE);
		}
		else
		{
			printf("Operace DLL_GetFirst vrací obsah %d.\n", temp);
			return (TRUE);
		}
	}
}

int test_DLL_GetLast() {
	solved = TRUE;
	int temp;
	DLL_GetLast(&TEMPLIST, &temp);
	if (!solved)
	{
		printf("Operace DLL_GetLast nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		if (error_flag)
		{
			printf("Operace DLL_GetLast volala funkci DLL_Error.\n");
			error_flag = FALSE;
			return (FALSE);
		}
		else
		{
			printf("Operace DLL_GetLast vrací obsah %d.\n", temp);
			return (TRUE);
		}
	}
}

int test_DLL_DeleteFirst() {
	solved = TRUE;
	DLL_DeleteFirst(&TEMPLIST);
	if (!solved)
	{
		printf("Operace DLL_DeleteFirst nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_DeleteLast() {
	solved = TRUE;
	DLL_DeleteLast(&TEMPLIST);
	if (!solved)
	{
		printf("Operace DLL_DeleteLast nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_DeleteAfter() {
	solved = TRUE;
	DLL_DeleteAfter(&TEMPLIST);
	if (!solved)
	{
		printf("Operace DLL_DeleteAfter nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_DeleteBefore() {
	solved = TRUE;
	DLL_DeleteBefore(&TEMPLIST);
	if (!solved)
	{
		printf("Operace DLL_DeleteBefore nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_InsertAfter() {
	solved = TRUE;
	DLL_InsertAfter(&TEMPLIST, ElemValue);
	if (!solved)
	{
		printf("Operace DLL_InsertAfter nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_InsertBefore() {
	solved = TRUE;
	DLL_InsertBefore(&TEMPLIST, ElemValue);
	if (!solved)
	{
		printf("Operace DLL_InsertBefore nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_GetValue() {
	solved = TRUE;
	int temp;
	DLL_GetValue(&TEMPLIST, &temp);

	if (!solved)
	{
		printf("Operace DLL_GetValue nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		if (error_flag)
		{
			printf("Operace DLL_GetValue volala funkci DLL_Error.\n");
			error_flag = FALSE;
			return (FALSE);
		}
		else
		{
			printf("Operace DLL_GetValue vrací obsah %d.\n", temp);
			return (TRUE);
		}
	}
}

int test_DLL_SetValue() {
	solved = TRUE;
	DLL_SetValue(&TEMPLIST, ElemValue);
	if (!solved)
	{
		printf("Operace DLL_SetValue nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_Next() {
	solved = TRUE;
	DLL_Next(&TEMPLIST);
	if (!solved)
	{
		printf("Operace DLL_Next nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_Previous() {
	solved = TRUE;
	DLL_Previous(&TEMPLIST);
	if (!solved)
	{
		printf("Operace DLL_Previous nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		print_elements_of_list(TEMPLIST);
		return (TRUE);
	}
}

int test_DLL_IsActive() {
	solved = TRUE;
	int tmp = DLL_IsActive(&TEMPLIST);
	if (!solved)
	{
		printf("Operace DLL_IsActive nebyla implementována!\n");
		return (FALSE);
	}
	else
	{
		if (tmp)
			printf("Operace DLL_IsActive vrací hodnotu true.\n");
		else
			printf("Operace DLL_IsActive vrací hodnotu false.\n");
		return (TRUE);
	}
}

/*******************************************************************************
 * ZÁKLADNÍ TESTY
 * -------------- 
 * Za jejich úspěšné projítí získá student první část bodů za příklad.
 * Při hodnocení však budou použity rozšířené testy, které se zaměří
 * i na okrajové situace. Doporučujeme proto, aby si každý student
 * zkusitl tyto základní testy rozšířit o další zajímavé situace.
 *  
 ******************************************************************************/

int main() {
	printf("C206 - Doubly Linked List - Basic Tests\n");
	printf("---------------------------------------\n");

	printf("\n[TEST01]\n");
	printf("Inicializace seznamu\n");
	printf("~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_Init();

	printf("\n[TEST02]\n");
	printf("Zavoláme 2x operaci DLL_InsertFirst a 2x operaci DLL_InsertLast.\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	ElemValue = 1;
	test_DLL_InsertFirst();
	ElemValue = 2;
	test_DLL_InsertFirst();
	ElemValue = 3;
	test_DLL_InsertLast();
	ElemValue = 4;
	test_DLL_InsertLast();

	printf("\n[TEST03]\n");
	printf("Otestujeme funkci DLL_First při neaktivním seznamu a funkci DLL_IsActive.\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_First();
	test_DLL_IsActive();

	printf("\n[TEST04]\n");
	printf("Otestujeme funkci DLL_Last.\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_Last();
	test_DLL_IsActive();

	printf("\n[TEST05]\n");
	printf("Test funkce DLL_GetValue při aktivitě na posledním prvku\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_GetValue();

	printf("\n[TEST06]\n");
	printf("Test funkce DLL_Previous -- voláme 3x, aktivita zůstane na prvním prvku.\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_Previous();
	test_DLL_Previous();
	test_DLL_Previous();
	test_DLL_IsActive();

	printf("\n[TEST07]\n");
	printf("Aktualizujeme obsah aktivního prvku.\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	ElemValue = 5;
	test_DLL_SetValue();
	test_DLL_IsActive();
	test_DLL_GetValue();

	printf("\n[TEST08]\n");
	printf("Test funkce DLL_Next -- voláme 3x, aktivita zůstane na posledním prvku.\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_Next();
	test_DLL_Next();
	test_DLL_Next();
	test_DLL_IsActive();

	printf("\n[TEST09]\n");
	printf("Provedeme ještě jednou DLL_Next - aktivita se ztratí.\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_Next();
	test_DLL_IsActive();

	printf("\n[TEST10]\n");
	printf("Nastavíme aktivitu na začátek a pak ji zrušíme operací DLL_Previous.\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_First();
	test_DLL_Previous();
	test_DLL_IsActive();

	printf("\n[TEST11]\n");
	printf("Pokus o DLL_GetValue při neaktivním seznamu => ošetřená chyba.\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_GetValue();

	printf("\n[TEST12]\n");
	printf("Použití operace DLL_GetFirst při neaktivním seznamu\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_GetFirst();

	printf("\n[TEST13]\n");
	printf("Použití operace DLL_GetLast při neaktivním seznamu\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_GetLast();

	printf("\n[TEST14]\n");
	printf("Použití operace DLL_DeleteFirst při neaktivním seznamu\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_DeleteFirst();
	test_DLL_IsActive();

	printf("\n[TEST15]\n");
	printf("Použití operace DLL_DeleteLast při neaktivním seznamu\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_DeleteLast();
	test_DLL_IsActive();

	printf("\n[TEST16]\n");
	printf("Zkusíme DLL_DeleteAfter při aktivitě na posledním prvku.\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_Last();
	test_DLL_DeleteAfter();
	test_DLL_IsActive();

	printf("\n[TEST17]\n");
	printf("Operace DLL_DeleteBefore nyní smaže první prvek.\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_DeleteBefore();
	test_DLL_IsActive();

	printf("\n[TEST18]\n");
	printf("Seznam nakonec zrušíme.\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_DLL_Dispose();
	test_DLL_IsActive();

	printf("\n----- C206 - The End of Basic Tests -----\n");

	return (0);
}
