/**/
/*
 *  Course: Algorithms (IAL) - FIT BUT
 *  Advanced tests for c206.c (Double linked list)
 *  Created: Martin Tuček, September 2005
 *  Modification: Bohuslav Křena, October 2016
 */

#include "c206.h"
                                                         /* working variables */
DLList TEMPLIST;		                                         /* list for test */ 
int content=0;                               /* content for inserted elements */
int MaxListLength = 100;                 /* for hangling wrongly linked lists */

/*******************************************************************************
 * Wrapping functions to make tests scripts shorter: 
 ******************************************************************************/

void print_elements_of_list(DLList TL)	{
/* Prints elements and activity of list TL. */
	
	DLList TempList=TL;
	                               /* We count elements in order to avoid infinite
                                    loops when the list is wrongly linked. */
	int CurrListLength = 0;
	printf("-----------------");
	while ((TempList.firstElement!=NULL) && (CurrListLength<MaxListLength))	{
		printf("\n \t%d",TempList.firstElement->data);
		if ((TempList.firstElement==TL.activeElement) && (TL.activeElement!=NULL))
			printf("\t <= activity ");
		TempList.firstElement=TempList.firstElement->nextElement;
		CurrListLength++;
	}
    if (CurrListLength>=MaxListLength)
      printf("\nList exceeded the maximum length!");
	
  printf("\n-----------------\n");     
}

int test_DLL_Init()	{
	solved=TRUE;	
	DLL_Init(&TEMPLIST);
	if (!solved)	{
		printf("Function InitList is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}	
}

int test_DLL_Dispose()	{
	solved=TRUE;
	DLL_Dispose(&TEMPLIST);
	if (!solved)	{
		printf("Function DLL_Dispose is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}	
}

int test_DLL_InsertFirst()	{
	solved=TRUE;
	content++;
	DLL_InsertFirst(&TEMPLIST,content);
	if (!solved)	{
		printf("Function DLL_InsertFirst is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}	

int test_DLL_InsertLast()	{
	solved=TRUE;
	content++;
	DLL_InsertLast(&TEMPLIST,content);
	if (!solved)	{
		printf("Function DLL_InsertLast is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}	

int test_DLL_First()	{
	solved=TRUE;
	DLL_First(&TEMPLIST);
	if (!solved)	{
		printf("Function DLL_First is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}

int test_DLL_Last()	{
	solved=TRUE;
	DLL_Last(&TEMPLIST);
	if (!solved)	{
		printf("Function DLL_Last is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}

int test_DLL_GetFirst()	{
	solved=TRUE;	
    int temp;
    DLL_GetFirst(&TEMPLIST,&temp);	
	if (!solved)	{
		printf("Function DLL_GetFirst is not implemented!\n");
		return(FALSE);
	}
	else {
		if (error_flag)	{
			printf("Function DLL_GetFirst has called function DLL_Error.\n");
			error_flag=FALSE;
			return(FALSE);
		}	
		else	{	 			
			printf("Return value of function DLL_GetFirst is %d.\n",temp);
			return(TRUE);
		}	
	}
}	

int test_DLL_GetLast()	{
	solved=TRUE;
    int temp;
    DLL_GetLast(&TEMPLIST,&temp);	
	if (!solved)	{
		printf("Function DLL_GetLast is not implemented!\n");
		return(FALSE);
	}
	else {
		if (error_flag)	{
			printf("Function DLL_GetLast has called function DLL_Error.\n");
			error_flag=FALSE;
			return(FALSE);
		}	
		else	{	 			
			printf("Return value of function DLL_GetLast is %d.\n",temp);
			return(TRUE);
		}	
	}
}	

int test_DLL_DeleteFirst ()	{
	solved=TRUE;	
	DLL_DeleteFirst(&TEMPLIST);
	if (!solved)	{
		printf("Function DLL_DeleteFirst is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}	

int test_DLL_DeleteLast ()	{
	solved=TRUE;	
	DLL_DeleteLast(&TEMPLIST);
	if (!solved)	{
		printf("Function DLL_DeleteLast is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}	

int test_DLL_DeleteAfter()	{
	solved=TRUE;	
	DLL_DeleteAfter(&TEMPLIST);
	if (!solved)	{
		printf("Function DLL_DeleteAfter is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}	

int test_DLL_DeleteBefore()	{
	solved=TRUE;	
	DLL_DeleteBefore(&TEMPLIST);
	if (!solved)	{
		printf("Function DLL_DeleteBefore is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}

int test_DLL_InsertAfter()	{
	solved=TRUE;
	content++;
	DLL_InsertAfter(&TEMPLIST,content);
	if (!solved)	{
		printf("Function DLL_InsertAfter is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}

int test_DLL_InsertBefore()	{
	solved=TRUE;
	content++;
	DLL_InsertBefore(&TEMPLIST,content);
	if (!solved)	{
		printf("Function DLL_InsertBefore is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}

int test_DLL_GetValue()	{
	solved=TRUE;
	int temp;
  DLL_GetValue(&TEMPLIST,&temp);	
	
	if (!solved)	{
		printf("Function DLL_GetValue is not implemented!\n");
		return(FALSE);
	}
	else {
		if (error_flag)	{
			printf("Function DLL_GetValue has called function DLL_Error.\n");
			error_flag=FALSE;
			return(FALSE);
		}	
		else	{	 			
			printf("Return value of function DLL_GetValue is %d.\n",temp);
			return(TRUE);
		}	
	}	
}

int test_DLL_SetValue()	{
	solved=TRUE;
	content++;
	DLL_SetValue(&TEMPLIST,content);
	if (!solved)	{
		printf("Function DLL_SetValue is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}

int test_DLL_Next()	{
	solved=TRUE;
	DLL_Next(&TEMPLIST);
	if (!solved)	{
		printf("Function DLL_Next is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}

int test_DLL_Previous()	{
	solved=TRUE;
	DLL_Previous(&TEMPLIST);
	if (!solved)	{
		printf("Function DLL_Previous is not implemented!\n");
		return(FALSE);
	}	
	else	{
		print_elements_of_list(TEMPLIST);
		return(TRUE);
	}
}

int test_DLL_IsActive()	{
	solved=TRUE;
	int tmp=DLL_IsActive(&TEMPLIST);
	if (!solved)	{
		printf("Function DLL_IsActive is not implemented!\n");
		return(FALSE);
	}	
	else	{
		if (tmp)
			printf("Return value of function DLL_IsActive is TRUE.\n");
		else
			printf("Return value of function DLL_IsActive is FALSE.\n");	
		return(TRUE);			
	}
}

/*******************************************************************************
 * Advanced Tests
 ******************************************************************************/

int main()	{
    printf("Double Linked List - Advanced Tests\n");
    printf("===================================\n");

    printf("\n[TEST01]\n");
    printf("List initialization\n");
    printf("~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_Init();

                       /* We test behaviour of some functions for empty list. */	
    printf("\n[TEST02]\n");
    printf("Calling DLL_GetFirst for empty list should cause en error.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_GetFirst();
	
    printf("\n[TEST03]\n");
    printf("Calling DLL_GetLast for empty list should cause en error.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_GetLast();

    printf("\n[TEST04]\n");
    printf("Calling DLL_GetValue for empty list should cause en error.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_GetValue();

    printf("\n[TEST05]\n");
    printf("Calling DLL_First for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_First();

    printf("\n[TEST06]\n");
    printf("Calling DLL_Last for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_Last();

    printf("\n[TEST07]\n");
    printf("Calling DLL_DeleteFirst for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_DeleteFirst();

    printf("\n[TEST08]\n");
    printf("Calling DLL_DeleteLast for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_DeleteLast();

    printf("\n[TEST09]\n");
    printf("Calling DLL_DeleteAfter for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_DeleteAfter();

    printf("\n[TEST10]\n");
    printf("Calling DLL_DeleteBefore for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_DeleteBefore();

    printf("\n[TEST11]\n");
    printf("Calling DLL_InsertAfter for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_InsertAfter();

    printf("\n[TEST12]\n");
    printf("Calling DLL_InsertBefore for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_InsertBefore();

    printf("\n[TEST13]\n");
    printf("Calling DLL_SetValue for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_SetValue();

    printf("\n[TEST14]\n");
    printf("Calling DLL_Next for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_Next();

    printf("\n[TEST15]\n");
    printf("Calling DLL_Previous for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_Previous();

    printf("\n[TEST16]\n");
    printf("Function DLL_IsActive for empty list should return FALSE.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_IsActive();

    printf("\n[TEST17]\n");
    printf("Calling DLL_Dispose for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_Dispose();
   
                                 /* We concentrate on DLL_InsertFirst function. */	
    printf("\n[TEST18]\n");
    printf("3x DLL_InsertFirst\n");
    printf("~~~~~~~~~~~~~~~~\n");	
    test_DLL_InsertFirst();	
    test_DLL_InsertFirst();	
    test_DLL_InsertFirst();	

    printf("\n[TEST19]\n");
    printf("The list should not be active now.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_IsActive();

    printf("\n[TEST20]\n");
    printf("We start at the begin of the list and go to the end.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_First();
    test_DLL_IsActive();
    test_DLL_Next();
    test_DLL_IsActive();
    test_DLL_Next();
    test_DLL_IsActive();
    test_DLL_Next();
    test_DLL_IsActive();

    printf("\n[TEST21]\n");
    printf("We start at the end of the list and go to its begin.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_Last();
    test_DLL_IsActive();
    test_DLL_Previous();
    test_DLL_IsActive();
    test_DLL_Previous();
    test_DLL_IsActive();
    test_DLL_Previous();
    test_DLL_IsActive();

                                /* We read and destroy the list from the end. */	
    printf("\n[TEST22]\n");
    printf("We read and destroy the list by DLL_GetLast and DLL_DeleteLast functions.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_GetLast();
    test_DLL_DeleteLast();
    test_DLL_IsActive();
    test_DLL_First();
    test_DLL_GetLast();
    test_DLL_DeleteLast();
    test_DLL_IsActive();
    test_DLL_Last();
    test_DLL_GetLast();
    test_DLL_DeleteLast();
    test_DLL_IsActive();

                                  /* We concentrate on DLL_InsertLast function. */	
    printf("\n[TEST23]\n");
    printf("3x DLL_InsertLast\n");
    printf("~~~~~~~~~~~~~~~\n");	
    test_DLL_InsertLast();	
    test_DLL_InsertLast();	
    test_DLL_InsertLast();	

    printf("\n[TEST24]\n");
    printf("The list should not be active now.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_IsActive();

    printf("\n[TEST25]\n");
    printf("We start at the begin of the list and go to the end.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_First();
    test_DLL_IsActive();
    test_DLL_Next();
    test_DLL_IsActive();
    test_DLL_Next();
    test_DLL_IsActive();
    test_DLL_Next();
    test_DLL_IsActive();

    printf("\n[TEST26]\n");
    printf("We start at the end of the list and go to its begin.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_Last();
    test_DLL_IsActive();
    test_DLL_Previous();
    test_DLL_IsActive();
    test_DLL_Previous();
    test_DLL_IsActive();
    test_DLL_Previous();
    test_DLL_IsActive();

                              /* We read and destroy the list from the begin. */	
    printf("\n[TEST27]\n");
    printf("We read and destroy the list by DLL_GetFirst and DLL_DeleteFirst functions.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_GetFirst();
    test_DLL_DeleteFirst();
    test_DLL_IsActive();
    test_DLL_First();
    test_DLL_GetFirst();
    test_DLL_DeleteFirst();
    test_DLL_IsActive();
    test_DLL_Last();
    test_DLL_GetFirst();
    test_DLL_DeleteFirst();
    test_DLL_IsActive();

                       /* We concentrate on some functions for inactive list. */	
    printf("\n[TEST28]\n");
    printf("We start with one element in inactive list. Nothing should happen.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_IsActive();
    test_DLL_InsertLast();
    test_DLL_DeleteAfter();
    test_DLL_DeleteBefore();
    test_DLL_InsertAfter();
    test_DLL_InsertBefore();
    test_DLL_Next();
    test_DLL_Previous();
    test_DLL_SetValue();
    test_DLL_IsActive();

    printf("\n[TEST29]\n");
    printf("Function DLL_GetValue for inactive list should cause an error.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_IsActive();
    test_DLL_GetValue();
    test_DLL_IsActive();

    printf("\n[TEST30]\n");
    printf("We continue with additional element in inactive list. Again, nothing should happen.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_IsActive();
    test_DLL_InsertFirst();
    test_DLL_DeleteAfter();
    test_DLL_DeleteBefore();
    test_DLL_InsertAfter();
    test_DLL_InsertBefore();
    test_DLL_Next();
    test_DLL_Previous();
    test_DLL_SetValue();
    test_DLL_IsActive();

    printf("\n[TEST31]\n");
    printf("Function DLL_GetValue for inactive list should cause an error again.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_IsActive();
    test_DLL_GetValue();
    test_DLL_IsActive();

                                             /* We change values of elements. */	
    printf("\n[TEST32]\n");
    printf("We change values of elements from the begin.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_First();
    test_DLL_SetValue();
    test_DLL_Next();
    test_DLL_SetValue();
    test_DLL_IsActive();

    printf("\n[TEST33]\n");
    printf("We change values of elements from the end.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
    test_DLL_Last();
    test_DLL_SetValue();
    test_DLL_Previous();
    test_DLL_SetValue();
    test_DLL_IsActive();

                       /* We try to lose activity by deleting active element. */	
    printf("\n[TEST34]\n");
    printf("Function DLL_DeleteFirst can cause loss of activity.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_InsertLast();
    test_DLL_IsActive();
    test_DLL_First();
    test_DLL_DeleteFirst();
    test_DLL_IsActive();
   
    printf("\n[TEST35]\n");
    printf("Function DLL_DeleteLast can cause loss of activity.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_InsertFirst();
    test_DLL_IsActive();
    test_DLL_Last();
    test_DLL_DeleteLast();
    test_DLL_IsActive();

  /* We try to delete first and last element by DLL_DeleteBefore and DLL_DeleteAfter. */	
    printf("\n[TEST36]\n");
    printf("Function DLL_DeleteBefore deletes first element.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_InsertFirst();
    test_DLL_First();
    test_DLL_Next();
    test_DLL_DeleteBefore();
    test_DLL_GetFirst();

    printf("\n[TEST37]\n");
    printf("Function DLL_DeleteAfter deletes last element.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_InsertLast();
    test_DLL_Last();
    test_DLL_Previous();
    test_DLL_DeleteAfter();
    test_DLL_GetLast();

               /* We try some functions at the begin and the end of the list. */	
    printf("\n[TEST38]\n");
    printf("Function DLL_DeleteBefore at the begin of the list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_First();
    test_DLL_DeleteBefore();
    test_DLL_IsActive();

    printf("\n[TEST39]\n");
    printf("We add a new element at the begin by DLL_InsertBefore.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_InsertBefore();
    test_DLL_GetFirst();
    test_DLL_GetValue();
    test_DLL_IsActive();

    printf("\n[TEST40]\n");
    printf("Function DLL_DeleteAfter at the end of the list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_Last();
    test_DLL_DeleteAfter();
    test_DLL_IsActive();

    printf("\n[TEST41]\n");
    printf("We add a new element at the end by DLL_InsertAfter.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_InsertAfter();
    test_DLL_GetLast();
    test_DLL_GetValue();
    test_DLL_IsActive();

                                                          /* We dispose list. */	
    printf("\n[TEST42]\n");
    printf("We dispose the list.\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_Dispose();

    printf("\n[TEST43]\n");
    printf("Twice.\n");
    printf("~~~~~~~~~~\n");
    test_DLL_Dispose();
 
  
                            /* We dispose list and test some functions again. */	
    printf("\n[TEST44]\n");
    printf("Calling DLL_GetFirst for empty list should cause en error.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_GetFirst();
	
    printf("\n[TEST45]\n");
    printf("Calling DLL_GetLast for empty list should cause en error.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_GetLast();

    printf("\n[TEST46]\n");
    printf("Calling DLL_GetValue for empty list should cause en error.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_GetValue();

    printf("\n[TEST47]\n");
    printf("Calling DLL_First for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_First();

    printf("\n[TEST48]\n");
    printf("Calling DLL_Last for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_Last();

    printf("\n[TEST49]\n");
    printf("Calling DLL_DeleteFirst for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_DeleteFirst();

    printf("\n[TEST50]\n");
    printf("Calling DLL_DeleteLast for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_DeleteLast();

    printf("\n[TEST51]\n");
    printf("Calling DLL_DeleteAfter for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_DeleteAfter();

    printf("\n[TEST52]\n");
    printf("Calling DLL_DeleteBefore for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_DeleteBefore();

    printf("\n[TEST53]\n");
    printf("Calling DLL_InsertAfter for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_InsertAfter();

    printf("\n[TEST54]\n");
    printf("Calling DLL_InsertBefore for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_InsertBefore();

    printf("\n[TEST55]\n");
    printf("Calling DLL_SetValue for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_SetValue();

    printf("\n[TEST56]\n");
    printf("Calling DLL_Next for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_Next();

    printf("\n[TEST57]\n");
    printf("Calling DLL_Previous for empty list should do nothing.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_Previous();

    printf("\n[TEST58]\n");
    printf("Function DLL_IsActive for empty list should return FALSE.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    test_DLL_IsActive();

    printf("\n----------------------- End of advanced test for c206 ----------------------\n");
		
	return(0);
} 
/**/

