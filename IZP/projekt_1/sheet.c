/**
 * Projekt do predmetu IZP (2020/2021)  
 *
 * 1. Projekt - Prace s textem
 * 1BIT FIT 2020
 * 
 * Autor:   Augustin Machynak
 *          (xmachy02)
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_ROW_SIZE 10240
#define MAX_CELL_LEN 100
#define MAX_INSERT_ARGS 100

#define NO_ARG_EDIT_COUNT 2
#define ONE_ARG_EDIT_COUNT 4
#define TWO_ARG_EDIT_COUNT 2

#define ONE_ARG_PROC_COUNT 4
#define TWO_ARG_PROC_COUNT 4

#define ARG_SEL_COUNT 3

typedef struct {
    char *delimiter;
    char rowChars[MAX_ROW_SIZE];
    int rowSize;
    int colCount;
} tableStruct;

typedef enum {
    ROW,
    COL,
    DCOL,
    DROW,
    DCOLS,
    DROWS
} insertEnum;

// pomocny struct pro dict ( v checkInsertArgs() )
struct mapArgs {
    char *str;
    insertEnum insArg;
    int num;
};

typedef struct {
    int insert[6][MAX_INSERT_ARGS];
    int count[6]; // 6 = pocet druhu prikazu - (a/i)row, (a/i)col, dcol, drow, dcols, drows
} insertStruct;

typedef enum {
    NONE,
    ROWS,
    ROWSLAST,
    BEGINSWITH,
    CONTAINS
} rowSelector;

typedef struct {
    rowSelector rowSelect;
    int rowsNum[2];                 // pro prikaz ROWS, BEGINSWITH a CONTAINS (BEGINSWITH a CONTAINS pouzivaji pouze rowsNum[0])
    char selectStr[MAX_CELL_LEN];   // pro prikazy BEGINSWITH a CONTAINS
    int strLen;                     // pro prikazy BEGINSWITH a CONTAINS
} selectorStruct;

    /*  Deklarace funkci, jejich implementace je nize pro vetsi prehlednost 
        (at je main na zacatku) */

int checkInputValid(int argc, char **argv, int startArg);
int validityHelpFunc(int *returnSet, int *returnCode, int *selector, int desiredCode);
int atLeastOneEqual(char **strings, int stringsCount, char *str);
int editCommands(int argc, char **argv, int startArg, tableStruct inTable);
int processingCommands(int argc, char **argv, int startArg, tableStruct inTable);

int editRowCol(tableStruct inTable, insertStruct rowCol);
int addSelector(selectorStruct *selector, int *nums, char *str);
int isSelected(tableStruct *inTable, selectorStruct selector, int row);
int colSet(tableStruct inTable, selectorStruct selector, int col, char *str);
void colSetOne(tableStruct inTable, int col, char *str);
int toLowerUpper(tableStruct inTable, selectorStruct selector, int col, int lowUp);
int roundIntCol(tableStruct inTable, selectorStruct selector, int col, int roundInt);
int copyCol(tableStruct inTable, selectorStruct selector, int *nums);
int swapCol(tableStruct inTable, selectorStruct selector, int *nums);
int moveCol(tableStruct inTable, selectorStruct selector, int *nums);

void printMulCells(tableStruct inTable, insertStruct rowCol, int count);
int isntLastEdit(insertStruct rowCol, int maxCol, int count);
int shouldEdit(insertStruct rowCol, insertEnum select, int num);
int checkInsertArgs(insertStruct *rowCol, int argc, char *argv[], int startArg);
int fillNumsBetween(insertStruct *rowCol, insertEnum insArg, int *nums);

int readRow(tableStruct *inTable);
void getCell(tableStruct inTable, int colPos, char *copyTo);
int printCell(tableStruct inTable, int colPos, int addDelim);
void printRow(tableStruct inTable);
void printFromToCell(tableStruct inTable, int col, int fromTo);
int printTable(tableStruct inTable);
void printXDelims(tableStruct inTable, int count);
int checkBetween(insertStruct rowCol, insertEnum select, int num);

int isNumber(const char *c);
int hasDelim(char c, tableStruct inTable);
int checkIn(char **num, int *saveTo, int numLen, const char *arg, int argc, int argcPos);
int checkStringCase(const char *arg);
int roundToInt(double num);
void strToLowUp(char *str, int lowUp);

int main(int argc, char *argv[]) {
    tableStruct inTable = { .colCount = 0, .rowSize = 0, .delimiter = " \0" };
    int startArg = 1;
    if(argc > 2) {
        if(strcmp(argv[1], "-d") == 0) {
            inTable.delimiter = argv[2];
            startArg = 3;
        }
    }
    // Kontrola validity zadanych argumentu
    int validity = checkInputValid(argc, argv, startArg);
    if(validity == 0) { // Nebyl nalezen prikaz
        return printTable(inTable);
    } else if(validity == 1) { // Byl nalezen prikaz pro upravu tabulky
        return editCommands(argc, argv, startArg, inTable);
    } else if(validity == 2) { // Byl nalezen prikaz pro selekci/zpracovani tabulky
        return processingCommands(argc, argv, startArg, inTable);
    } else { // Nebyl nalezen prikaz nebo byl nalezen spatny prikaz
        return 1;
    }
}

/* Fce pro kontrolu validity zadanych argumentu */
int checkInputValid(int argc, char **argv, int startArg) {
    char *editCmd0[] = { "arow", "acol" }, *editCmd1[] = { "irow", "drow", "icol", "dcol" }, *editCmd2[] = { "drows", "dcols" };
    char *procCmd1[] = { "tolower", "toupper", "round", "int" }, *procCmd2[] = { "cset", "copy", "swap", "move" };
    char *selCmd[] = { "rows", "beginswith", "contains" };
    int returnCode = 0, returnSet = 0, selector = 0;
    for(int i = startArg; i < argc; i++) {
        if(atLeastOneEqual(editCmd0, NO_ARG_EDIT_COUNT, argv[i])) {
            if(validityHelpFunc(&returnSet, &returnCode, &selector, 1) == -2) {
                return -2;
            }
            continue;
        }
        if(atLeastOneEqual(editCmd1, ONE_ARG_EDIT_COUNT, argv[i])) {
            i += 1;
            if(validityHelpFunc(&returnSet, &returnCode, &selector, 1) == -2) {
                return -2;
            }
            continue;
        }
        if(atLeastOneEqual(editCmd2, TWO_ARG_EDIT_COUNT, argv[i])) {
            i += 2;
            if(validityHelpFunc(&returnSet, &returnCode, &selector, 1) == -2) {
                return -2;
            }
            continue;
        }
        if(atLeastOneEqual(procCmd1, ONE_ARG_PROC_COUNT, argv[i])) {
            i += 1;
            if(validityHelpFunc(&returnSet, &returnCode, &selector, 2) == -2) {
                return -2;
            }
            continue;
        }
        if(atLeastOneEqual(procCmd2, TWO_ARG_PROC_COUNT, argv[i])) {
            i += 2;
            if(validityHelpFunc(&returnSet, &returnCode, &selector, 2) == -2) {
                return -2;
            }
            continue;
        }
        if(atLeastOneEqual(selCmd, ARG_SEL_COUNT, argv[i])) {
            i += 2;
            if(!selector) {
                selector = 1;
            } else {
                fprintf(stderr, "[ERROR]: Too many selector commands.\n");
                return -2;
            }
            continue;
        }
        fprintf(stderr, "[ERROR]: Invalid command - %s\n", argv[i]);
        return -2;
    }
    if(selector && (returnCode != 2)) {
        fprintf(stderr, "[ERROR]: Missing processing command.\n");
        return -2;
    }
    return returnCode;
}

/* Pomocna funkce pro fci checkInputValid - pro zkraceni */
int validityHelpFunc(int *returnSet, int *returnCode, int *selector, int desiredCode) {
    if(!(*returnSet)) {
        *returnCode = desiredCode;
        *returnSet = 1;
    } else if((*returnCode != desiredCode) || ((*returnCode == 1) && (*selector == 1))) {
        fprintf(stderr, "[ERROR]: Combining different types of commands. (Edit commands with processing/selection commands)\n");
        return -2;
    } else if((*returnCode == desiredCode) && (desiredCode == 2)) {
        fprintf(stderr, "[ERROR]: Too many processing commands\n");
        return -2;
    }
    return 0;
}

/* Pomocna fce - jestli se alespon 1 string z "strings" rovna "str" */
int atLeastOneEqual(char **strings, int stringsCount, char *str) {
    for(int i = 0; i < stringsCount; i++) {
        if(!strcmp(strings[i], str)) {
            return 1;
        }
    }
    return 0;
}

/* Fce pro upravy tabulky */
int editCommands(int argc, char **argv, int startArg, tableStruct inTable) {
    insertStruct rowCol = { .count[0] = 0, .count[1] = 0, .count[2] = 0, .count[3] = 0 };
    int foundEdit = checkInsertArgs(&rowCol, argc, argv, startArg);
    if(foundEdit == 1) {
        // Alespon 1 prikaz pro upravu tabulky nalezen (prikazy ulozeny v "rowCol")
        return (editRowCol(inTable, rowCol) == -2);
    } else {
        return 1;
    }
}

/* Fce pro selekci radku a zpracovani dat */
int processingCommands(int argc, char **argv, int startArg, tableStruct inTable) {
    char *toCheck[2];
    int nums[2];
    selectorStruct selector = { .rowSelect = NONE, .strLen = 0 };
    for(int i = startArg; i < argc; i++) {
        toCheck[0] = ((argc > i+1) ? argv[i+1] : "\0");
        toCheck[1] = ((argc > i+2) ? argv[i+2] : "\0");
        if(strcmp(argv[i], "rows") == 0) {
            selector.rowSelect = ROWS;
            if((strcmp(toCheck[0], "-") == 0) && (strcmp(toCheck[1], "-") == 0)) {
                // Neni potreba nic resit - zadano zvoleni posledniho radku (rows - -)
                selector.rowSelect = ROWSLAST;
            } else if(strcmp(toCheck[1], "-") == 0) {
                // Zadano rows N -
                if(checkIn(toCheck, nums, 1, "rows", argc, i) == -2) {
                    return 1;
                }
                if(addSelector(&selector, nums, "-") == -2) {
                    return 1;
                }
            } else {
                // Zadano rows N M
                if(checkIn(toCheck, nums, 2, "rows", argc, i) == -2) {
                    return 1;
                }
                if(addSelector(&selector, nums, "") == -2) {
                    return 1;
                }
            }
            break;
        } else if(strcmp(argv[i], "beginswith") == 0) {
            if(checkIn(toCheck, nums, 1, "beginswith", argc, i) == -2) {
                return 1;
            }
            selector.rowSelect = BEGINSWITH;
            if(addSelector(&selector, nums, toCheck[1]) == -2) {
                return 1;
            }
            break;
        } else if(strcmp(argv[i], "contains") == 0) {
            if(checkIn(toCheck, nums, 1, "contains", argc, i) == -2) {
                return 1;
            }
            selector.rowSelect = CONTAINS;
            if(addSelector(&selector, nums, toCheck[1]) == -2) {
                return 1;
            }
            break;
        }
    }
    for(int i = startArg; i < argc; i++) {
        toCheck[0] = ((argc > i+1) ? argv[i+1] : "\0");
        toCheck[1] = ((argc > i+2) ? argv[i+2] : "\0");
        if(strcmp(argv[i], "cset") == 0) {
            if(checkIn(toCheck, nums, 1, "cset", argc, i) == -2) {
                return 1;
            }
            if(argc <= (i+2)) {
                fprintf(stderr, "[ERROR]: Missing string (cset N STR).\n");
                return 1;
            }
            return (colSet(inTable, selector, nums[0], toCheck[1]) == -2);
        } else if(strcmp(argv[i], "tolower") == 0) {
            if(checkIn(toCheck, nums, 1, "tolower", argc, i) == -2) {
                return 1;
            }
            return (toLowerUpper(inTable, selector, nums[0], 0) == -2);
        } else if(strcmp(argv[i], "toupper") == 0) {
            if(checkIn(toCheck, nums, 1, "toupper", argc, i) == -2) {
                return 1;
            }
            return (toLowerUpper(inTable, selector, nums[0], 1) == -2);
        } else if(strcmp(argv[i], "round") == 0) {
            if(checkIn(toCheck, nums, 1, "round", argc, i) == -2) {
                return 1;
            }
            return (roundIntCol(inTable, selector, nums[0], 0) == -2);
        } else if(strcmp(argv[i], "int") == 0) {
            if(checkIn(toCheck, nums, 1, "int", argc, i) == -2) {
                return 1;
            }
            return (roundIntCol(inTable, selector, nums[0], 1) == -2);
        } else if(strcmp(argv[i], "copy") == 0) {
            if(checkIn(toCheck, nums, 2, "copy", argc, i) == -2) {
                return 1;
            }
            return (copyCol(inTable, selector, nums) == -2);
        } else if(strcmp(argv[i], "swap") == 0) {
            if(checkIn(toCheck, nums, 2, "swap", argc, i) == -2) {
                return 1;
            }
            return (swapCol(inTable, selector, nums) == -2);
        } else if(strcmp(argv[i], "move") == 0) {
            if(checkIn(toCheck, nums, 2, "move", argc, i) == -2) {
                return 1;
            }
            return (moveCol(inTable, selector, nums) == -2);
        }
    }
    // (Zde by se ani nemelo dostat)
    return 0;
}

/* Pokud je zadan prikaz pro upravu tabulky, tak naplni insertStruct (- parsovani argumentu pro upravu tabulky) */
int checkInsertArgs(insertStruct *rowCol, int argc, char *argv[], int startArg) {
    struct mapArgs dict[8] = {
        { "irow", ROW, 1, },
        { "arow", ROW, 0, },
        { "drow", DROW, 1, },
        { "icol", COL, 1, },
        { "acol", COL, 0, },
        { "dcol", DCOL, 1, },
        { "drows", DROWS, 2, },
        { "dcols", DCOLS, 2, }
    };
    int dictSize = sizeof(dict) / sizeof(*dict);
    int nums[2];
    char *toCheck[2];
    int foundArg = 0;
    for(int i = startArg; i < argc; i++) {
        if(isNumber(argv[i]) == 1) {
            continue;
        }
        toCheck[0] = ((argc >= i+1) ? argv[i+1] : "\0");
        toCheck[1] = ((argc >= i+2) ? argv[i+2] : "\0");
        for(int j = 0; j < dictSize; j++) {
            if(strcmp(argv[i], dict[j].str) == 0) {
                if(checkIn(toCheck, nums, dict[j].num, dict[j].str, argc, i) == -2) {
                    return -2;
                }
                foundArg = 1;
                insertEnum iArg = dict[j].insArg;
                int argCount = rowCol->count[iArg];
                // Maximalne 100 prikazu pro 1 druh upravy tabulky
                if(rowCol->count[iArg] >= (MAX_INSERT_ARGS - 1)) {
                    fprintf(stderr, "[ERROR]: Too many table edits.\n");
                    return -2;
                }
                if(dict[j].num == 2) {
                    if(nums[0] > nums[1]) {
                        fprintf(stderr, "[ERROR]: First number is larger than the second one (%s N M).\n", dict[j].str);
                        return -2;
                    }
                    rowCol->insert[iArg][argCount] = nums[0];
                    rowCol->insert[iArg][argCount+1] = nums[1];
                    rowCol->count[iArg] += 2;
                } else if(dict[j].num == 1) {
                    rowCol->insert[iArg][argCount] = nums[0];
                    rowCol->count[iArg]++;
                } else {
                    rowCol->insert[iArg][argCount] = __INT_MAX__;
                    rowCol->count[iArg]++;
                }
            }
        }
    }
    return foundArg;
}

/* Fce pro prikazy pro upravu tabulky (pridani/odebrani radku/sloupcu) */
int editRowCol(tableStruct inTable, insertStruct rowCol) {
    int rowPos = 1, addXrows = 0, tempCols = 0, result = readRow(&inTable);
    int newCols = inTable.colCount + rowCol.count[COL];
    // Spocitani noveho poctu sloupcu
    for(int i = 1; i <= newCols; i++) {
        if(!(shouldEdit(rowCol, DCOL, i) || shouldEdit(rowCol, DCOLS, i))) {
            tempCols++;
        }
    }
    newCols = tempCols;
    do {
        if(result == -2) {
            return result;
        }
        // zjistime, kolikrat je volan irow na tomto(rowPos) radku
        addXrows = shouldEdit(rowCol, ROW, rowPos);
        if(addXrows != 0) {
            for(int i = 0; i < addXrows; i++) {
                // pridame x radku s novym poctem sloupcu
                printXDelims(inTable, newCols);
                printf("\n");
            }
            // pokud neni zadano, at je tento radek smazan, tak jej printnem
            if((shouldEdit(rowCol, DROW, rowPos) == 0) && (shouldEdit(rowCol, DROWS, rowPos) == 0)) {
                printMulCells(inTable, rowCol, newCols);
            }
            rowPos++;
            continue;
        } else if((shouldEdit(rowCol, DROW, rowPos) != 0) || (shouldEdit(rowCol, DROWS, rowPos) != 0)) {
            // pokud mame smazat radek a nebyl zadan irow, tak pouze skipnem
            rowPos++;
            continue;
        }
        // nebyl zadan irow, drow, takze zbyva jen kontrola icol/dcol, o coz se stara fce
        printMulCells(inTable, rowCol, inTable.colCount);
        rowPos++;
    } while((result = readRow(&inTable)) != 1);
    int lastRows = shouldEdit(rowCol, ROW, __INT_MAX__);
    // posledni kontrola arow
    if(lastRows != 0) {
        for(int i = 0; i < lastRows; i++) {
            printXDelims(inTable, newCols);
            printf("\n");
        }
    }
    return 0;
}

/*  Fce pro print radku po bunkach, ktera se stara o kombinace icol, acol, dcol, dcols. */
void printMulCells(tableStruct inTable, insertStruct rowCol, int count) {
    int result;
    for(int i = 1; i <= count+1; i++) {
        result = shouldEdit(rowCol, COL, i);
        if(result != 0) {
            printXDelims(inTable, result);
            printCell(inTable, i, 0);
            if((i < count+1) && isntLastEdit(rowCol, count, i+1)) {
                printf("%c", inTable.delimiter[0]);
            }
        } else if (shouldEdit(rowCol, DCOL, i) || shouldEdit(rowCol, DCOLS, i)){
            continue;
        } else {
            printCell(inTable, i, 0);
            if((i < count+1) && isntLastEdit(rowCol, count, i+1)) {
                printf("%c", inTable.delimiter[0]);
            }
        }
    }
    printXDelims(inTable, shouldEdit(rowCol, COL, __INT_MAX__));
    printf("\n");
}

/* Pomocna fce pro printMulCells (osetreni toho, at se neprida delimiter, kdyz mazeme od x. sloupce do posledniho apod.) */
int isntLastEdit(insertStruct rowCol, int maxCol, int count) {
    for(int i = count; i <= maxCol+1; i++) {
        if(!(shouldEdit(rowCol, DCOL, i) || shouldEdit(rowCol, DCOLS, i))) {
            return 1;
        }
    }
    return 0;
}

/* Vraci cislo, ktere je rovne poctu editu na konkretnim miste u drows a dcols (projizdi vsechny cisla mezi N a M) */
int checkBetween(insertStruct rowCol, insertEnum select, int num) {
    int counter = 0;
    for(int i = 0; i < rowCol.count[select]; i+=2) {
        for(int j = rowCol.insert[select][i]; j <= rowCol.insert[select][i+1]; j++) {
            if(j == num) {
                counter++;
            }
        }
    }
    return counter;
}

/* Vraci cislo, ktere je rovne poctu editu na konkretnim miste (arow arow - 2x pridani noveho radku na konec apod.) */
int shouldEdit(insertStruct rowCol, insertEnum select, int num) {
    if((select == DROWS) || (select == DCOLS)) {
        return checkBetween(rowCol, select, num);
    }
    int counter = 0;
    for(int i = 0; i < rowCol.count[select]; i++) {
        if(rowCol.insert[select][i] == num) {
            counter++;
        }
    }
    return counter;
}

/* Pridani prikazu selekce */
int addSelector(selectorStruct *selector, int *nums, char *str) {
    switch(selector->rowSelect) {
        case ROWS:
            selector->rowsNum[0] = nums[0];
            if(strcmp(str, "-") == 0) {
                selector->rowsNum[1] = __INT_MAX__;
            } else {
                if(nums[0] > nums[1]) {
                    fprintf(stderr, "[ERROR]: First selector number is larger than the second one. [rows N M (N > M)]\n");
                    return -2;
                }
                selector->rowsNum[1] = nums[1];
            }
            break;
        // beginswith a contains se chovaji jinak pouze pri kontrole
        case BEGINSWITH:
        case CONTAINS:
            selector->rowsNum[0] = nums[0];
            for(size_t i = 0; i < strlen(str); i++) {
                if(i >= MAX_CELL_LEN-1) {
                    fprintf(stderr, "[ERROR]: The string is too long [beginswith N STR (STR length > %d)]\n", MAX_CELL_LEN);
                    return -2;
                }
                selector->selectStr[selector->strLen] = str[i];
                selector->strLen++;
            }
            break;
        default:
            return 0;
    }
    return 0;
}
/* cset - vyuziva colSetOne pro print po radku */
int colSet(tableStruct inTable, selectorStruct selector, int col, char *str) {
    int result, rowCount = 1;
    if(strlen(str) >= MAX_CELL_LEN) {
        fprintf(stderr, "[ERROR]: The string is too long. [cset N STR (STR length > %d)]\n", MAX_CELL_LEN);
        return -2;
    }
    while((result = isSelected(&inTable, selector, rowCount)) != -1) {
        if(result == -2) {
            return result;
        } else if(result != 1) {
            printRow(inTable);
        } else {
            colSetOne(inTable, col, str);
        }
        rowCount++;
    }
    return 0;
}

/* Printne radek s tim, ze do bunky "col" nastavi string "str" */
void colSetOne(tableStruct inTable, int col, char *str) {
    printFromToCell(inTable, col, 0);
    if(col <= inTable.colCount+1) {
        if(col < inTable.colCount+1) {
            printf("%s%c", str, inTable.delimiter[0]);
        } else {
            printf("%s", str);
        }
        printFromToCell(inTable, col+1, 1);
    } else {
        printFromToCell(inTable, col, 1);
    }
    printf("\n");
}

/* lowUp - 0 = tolower; 1 = toupper */
int toLowerUpper(tableStruct inTable, selectorStruct selector, int col, int lowUp) {
    int result, rowCount = 1;
    while((result = isSelected(&inTable, selector, rowCount)) != -1) {
        if(result == -2) {
            return result;
        } else if(result != 1) {
            printRow(inTable);
        } else {
            char temp[100] = "\0";
            getCell(inTable, col, temp);
            strToLowUp(temp, lowUp);
            colSetOne(inTable, col, temp);
        }
        rowCount++;
    }
    return 0;
}

/* roundInt = 0 - prikaz round; roundInt = 1 - prikaz int */
int roundIntCol(tableStruct inTable, selectorStruct selector, int col, int roundInt) {
    int result, rowCount = 1;
    while((result = isSelected(&inTable, selector, rowCount)) != -1) {
        if(result == -2) {
            return result;
        } else if(result != 1) {
            printRow(inTable);
            rowCount++;
            continue;
        }
        char *endPtr, temp[100] = "\0";
        getCell(inTable, col, temp);
        double num = strtod(temp, &endPtr);
        if(endPtr[0] == '\0') {
            char newStr[100];
            // Prevedem zpet na string (pro kompatibilitu s colSetOne)
            sprintf(newStr, "%d", roundInt ? (int)num : roundToInt(num));
            colSetOne(inTable, col, newStr);
        } else {
            colSetOne(inTable, col, temp);
        }
        rowCount++;
    }
    return 0;
}

/*  */
int copyCol(tableStruct inTable, selectorStruct selector, int *nums) {
    int rowCount = 1, result = isSelected(&inTable, selector, rowCount);
    bool canCopy = (nums[0] != nums[1]) && (nums[0] < inTable.colCount+2) && (nums[1] < inTable.colCount+2);
    do {
        if(result == -2) {
            return result;
        } else if((result != 1) || !canCopy) {
            printRow(inTable);
        } else {
            char fromCell[100] = "\0";
            getCell(inTable, nums[0], fromCell);
            colSetOne(inTable, nums[1], fromCell);
        }
        rowCount++;
    } while ((result = isSelected(&inTable, selector, rowCount)) != -1);
    return 1;
}

int swapCol(tableStruct inTable, selectorStruct selector, int *nums) {
    int rowCount = 1, result = isSelected(&inTable, selector, rowCount);
    bool canSwap = ((nums[0] != nums[1]) && (nums[0] < inTable.colCount+2) && (nums[1] < inTable.colCount+2));
    do {
        if(result == -2) {
            return result;
        } else if((result != 1) || !canSwap) {
            printRow(inTable);
            rowCount++;
            continue;
        }
        for(int i = 1; i <= inTable.colCount+1; i++) {
            if(i == nums[0]) {
                printCell(inTable, nums[1], 0);
                if(i != inTable.colCount+1) {
                    printf("%c", inTable.delimiter[0]);
                }
            } else if(i == nums[1]) {
                printCell(inTable, nums[0], 0);
                if(i != inTable.colCount+1) {
                    printf("%c", inTable.delimiter[0]);
                }
            } else {
                printCell(inTable, i, 1);
            }
        }
        printf("\n");
        rowCount++;
    } while ((result = isSelected(&inTable, selector, rowCount)) != -1);
    return 1;
}

int moveCol(tableStruct inTable, selectorStruct selector, int *nums) {
    int rowCount = 1, result = isSelected(&inTable, selector, rowCount);
    bool canMove = ((nums[0] != nums[1]) && (nums[0] < inTable.colCount+2) && (nums[1] < inTable.colCount+2));
    do {
        if(result == -2) {
            return result;
        } else if((result != 1) || !canMove) {
            printRow(inTable);
            rowCount++;
            continue;
        }
        if(nums[0] < nums[1]) {
            printFromToCell(inTable, nums[0], 0);
            for(int i = nums[0]+1; i < nums[1]; i++) {
                printCell(inTable, i, 1);
            }
            printCell(inTable, nums[0], 1);
            printFromToCell(inTable, nums[1], 1);
        } else {
            for(int i = 1; i < nums[1]; i++) {
                printCell(inTable, i, 1);
            }
            printCell(inTable, nums[0], 0);
            printf("%c", inTable.delimiter[0]);
            for(int i = nums[1]; i < nums[0]; i++) {
                printCell(inTable, i, 0);
                if(i < inTable.colCount) {
                    printf("%c", inTable.delimiter[0]);
                }
            }
            printFromToCell(inTable, nums[0]+1, 1);
        }
        printf("\n");
        rowCount++;
    } while((result = isSelected(&inTable, selector, rowCount)) != -1);
    return 1;
}

/* Kontrola selekce - VZDY PRECTE ALESPON 1 RADEK - pro konzistenci */
int isSelected(tableStruct *inTable, selectorStruct selector, int row) {
    if(readRow(inTable) == -2) {
        return -2;
    } else if(inTable->rowSize == 0) {
        return -1;
    }
    char temp[100] = "\0";
    switch(selector.rowSelect) {
        case ROWSLAST:
            // Neresime nic - precteme vsechno a nechame na programu, co si udela s poslednim radkem
            printRow(*inTable);
            while(readRow(inTable) != 2) {
                printRow(*inTable);
            }
            return 1;
        case ROWS:
            return ((row >= selector.rowsNum[0]) && (row <= selector.rowsNum[1]));
        case BEGINSWITH:
            getCell(*inTable, selector.rowsNum[0], temp);
            return (strncmp(temp, selector.selectStr, selector.strLen) == 0);
        case CONTAINS:
            getCell(*inTable, selector.rowsNum[0], temp);
            char *found = strstr(temp, selector.selectStr);
            return (found != NULL);
        case NONE:
            return 1;
    }
    return 1;
}

/*  Pomocna fce pro print od/do x-teho sloupce
    fromTo = 0 - from 0 to col 
    fromTo = 1 - from col to max
*/
void printFromToCell(tableStruct inTable, int col, int fromTo) {
    int condition1 = (fromTo == 0 ? 1 : col);
    int condition2 = (fromTo == 0 ? col : inTable.colCount+2);
    for(int i = condition1; i < condition2; i++) {
        printCell(inTable, i, 0);
        if(i < inTable.colCount+1) {
            printf("%c", inTable.delimiter[0]);
        }
    }
}

/* Precteni 1 radku a ulozeni do inTable structu */
int readRow(tableStruct *inTable) {
    int c, cellCharCount = 0, tempCol = 0;
    bool firstRowBool = (inTable->colCount != 0);
    inTable->rowSize = 0;
    while((c = getchar()) != EOF) {
        if(inTable->rowSize < MAX_ROW_SIZE) {
            if(c == '\n') {
                if((c = getchar()) == EOF) {
                    inTable->rowSize++;
                    if((inTable->colCount != tempCol) && firstRowBool) {
                        fprintf(stderr, "[ERROR]: Different amount of cells in a row.\n");
                        return -2;
                    }
                    inTable->colCount = tempCol;
                    return 2;
                } else {
                    ungetc(c, stdin);
                }
                inTable->rowSize++;
                if((inTable->colCount != tempCol) && firstRowBool) {
                    fprintf(stderr, "[ERROR]: Different amount of cells in a row.\n");
                    return -2;
                }
                inTable->colCount = tempCol;
                return 0;
            }
            if(hasDelim(c, *inTable) == 1) {
                tempCol++;
                cellCharCount = 0;
            }
            cellCharCount++;
            if(cellCharCount > MAX_CELL_LEN) {
                fprintf(stderr, "[ERROR]: Cell is too big (> %d)\n", MAX_CELL_LEN);
                return -2;
            }
            inTable->rowChars[inTable->rowSize] = c;
            inTable->rowSize++;
        } else {
            fprintf(stderr, "[ERROR]: Row is too long (> %d)\n", MAX_ROW_SIZE);
            return -2;
        }
    }
    if(!firstRowBool) {
        fprintf(stderr, "[ERROR]: Invalid table...\n");
        return -2;
    }
    return 1;
}

/* Fce pro ziskani stringu z bunky ve sloupci, ktery je zadan colPos. Ulozen je do *copyTo */
void getCell(tableStruct inTable, int colPos, char *copyTo) {
    int delimCount = 1;
    for(int i = 0; i < inTable.rowSize; i++) {
        if(delimCount == colPos) {
            int counter = 0;
            for(int j = i; j < inTable.rowSize; j++) {
                if(hasDelim(inTable.rowChars[j], inTable)) {
                    copyTo[j - i + 1] = '\0';
                    return;
                }
                copyTo[j - i] = inTable.rowChars[j];
                counter++;
            }
            copyTo[counter-1] = '\0';
            return;
        }
        if(hasDelim(inTable.rowChars[i], inTable)) {
            delimCount++;
        }
    }
}

/*  Fce pro print 1 bunky
    addDelim - 0 = nepridavat delim; 1 = vyhodnoti samo */
int printCell(tableStruct inTable, int colPos, int addDelim) {
    char cell[100] = "\0";
    getCell(inTable, colPos, cell);
    if(addDelim == 1) {
        if(colPos < inTable.colCount+1) {
            printf("%s%c", cell, inTable.delimiter[0]);
        } else {
            printf("%s", cell);
        }
    } else {
        printf("%s", cell);
    }
    return 0;
}

/* Fce pro print radku */
void printRow(tableStruct inTable) {
    printFromToCell(inTable, inTable.colCount+2, 0);
    printf("\n");
}

/* Fce pro print poctu delimiteru*/
void printXDelims(tableStruct inTable, int count) {
    for(int i = 0; i < count; i++) {
        printf("%c", inTable.delimiter[0]);
    }
}

/* Fce pro print cele tabulky */
int printTable(tableStruct inTable) {
    int result;
    while((result = readRow(&inTable)) != 1) {
        if(result == -2) {
            return result;
        }
        printRow(inTable);
    }
    return 0;
}

/* Fce pro kontrolu argumentu */
int checkIn(char **num, int *saveTo, int numLen, const char *arg, int argc, int argcPos) {
    // Je vubec nejaky argument na argv[i+x] ?
    if((argcPos+numLen+1) > argc) {
        fprintf(stderr, "[ERROR]: Insufficient amount of arguments (%s %s).\n", arg, (numLen == 1 ? "N" : "N M"));
        return -2;
    }
    for(int i = 0; i < numLen; i++) {
        // Jsou zadany cisla ?
        if(isNumber(num[i]) == 0) {
            // Kontrola prikazu 'beginswith', 'contains' a 'cset'
            if(checkStringCase(arg) == 1) {
                fprintf(stderr, "[ERROR]: Enter a number [%s N STR].\n", arg);
                return -2;
            }
            fprintf(stderr, "[ERROR]: Enter a number [%s %s].\n", arg, (numLen == 1 ? "N" : "N M"));
            return -2;
        }
        // Prevedeme na int
        saveTo[i] = strtol(num[i], NULL, 10);
        // Jsou zadany cisla > 0 a < max int ?
        if((saveTo[i] <= 0) || (saveTo[i] >= __INT_MAX__)) {
            // Kontrola prikazu 'beginswith', 'contains' a 'cset'
            if(checkStringCase(arg) == 1) {
                fprintf(stderr, "[ERROR]: Enter a number > 0 and < %d [%s N STR].\n", __INT_MAX__, arg);
                return -2;
            }
            fprintf(stderr, "[ERROR]: Enter a number > 0 and < %d [%s %s].\n", __INT_MAX__, arg, (numLen == 1 ? "N" : "N M"));
            return -2;
        }
    }
    return 0;
}

/*  Fce pro spravny vypis error zpravy. 
    V zadani jsou pouze 3(4 muzou byt, ale rows kontrolujeme jinak) pripady, ve kterych je nutne zadat po cisle i string */
int checkStringCase(const char *arg) {
    char *specialCase[] = { "beginswith", "contains", "cset" };
    for(int i = 0; i < 3; i++) {
        if(strcmp(arg, specialCase[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Pomocna fce pro kontrolu cisla */
int isNumber(const char *c) {
    char *ptr;
    strtol(c, &ptr, 10);
    return *ptr == '\0';
}

/* Pomocna fce pro kontrolu delimiteru */
int hasDelim(char c, tableStruct inTable) {
    for(size_t i = 0; i < strlen(inTable.delimiter); i++) {
        if(inTable.delimiter[i] == c) {
            return 1;
        }
    }
    return 0;
}

/* Pomocna fce pro prevod na int */
int roundToInt(double num) {
    return num < 0 ? ((int) (num - 0.5)) : ((int) (num + 0.5));
}

/* String to lower/upper - 0 = tolower; 1 = toupper */
void strToLowUp(char *str, int lowUp) {
    for(int i = 0; str[i]; i++) {
        str[i] = lowUp ? toupper(str[i]) : tolower(str[i]);
    }
}
