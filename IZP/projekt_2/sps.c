#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define _THROW_ERR(msg) { \
    fprintf(stderr, msg); \
    return 1; \
}
#define _THROW_ERR_NULL(msg) { \
    fprintf(stderr, msg); \
    return NULL; \
}

#define _USAGE "[?] Usage: ./sps [-d DELIM] CMD_SEQUENCE FILE\n"
#define _MISSING_ARGS "[ERROR]: Not enough arguments.\n"
#define _NONEXISTING_FILE "[ERROR]: File doesn't exist.\n"
#define _INVALID_DELIM "[ERROR]: Inserted invalid delimiter\n"
#define _ALLOC_FAIL "[ERROR]: Failed allocating memory.\n"
#define _BAD_SELECTOR "[ERROR]: Unknown selector command (Too many arguments).\n"
#define _INVALID_SELECTOR "[ERROR]: Invalid selector command.\n"
#define _DELETED_TABLE "[ERROR]: Deleted whole table.\n"
#define _COUNT_ERR "[ERROR]: Can't insert the value into multiple cells.\n"
#define _SYNTAX_ERR "[ERROR]: Incorrect syntax.\n"
#define _NO_VALUE_SAVED "[ERROR]: No value saved.\n"

typedef enum {
    SEL_CELL,
    SEL_ROW,
    SEL_COL,
    SEL_RANGE,
    SEL_ALL
} selectors;

typedef struct {
    selectors selector;
    int selArgs[4];     // 4 = maximalni pocet argumentu u selektoru (to je u [R1,C1,R2,C2])
} selectorStruct;

typedef struct {
    char *str;
    int len;
} cellStruct;

typedef struct {
    cellStruct *cells;
    int cellCount;
} rowStruct;

typedef struct {
    rowStruct *rows;
    int rowCount;
    char *delim;
    selectorStruct tempSelect; // pro praci s docasnymi selektory
    cellStruct tempDef[10]; // pro praci s docasnymi promennymi
} tableStruct;

// Fce pro upravu struktury tabulky aj.
typedef int(*structFunc)(tableStruct*, selectorStruct*);
// Fce pro upravu obsahu bunek aj.
typedef int(*editFunc)(tableStruct*, selectorStruct*, char*);

struct structMapArgs {
    char *str;
    structFunc pFunc;
};

struct editMapArgs {
    char *str;
    editFunc pFunc;
};

/********************************************/
/*********** INICIALIZACE STRUCTU ***********/
/********************************************/

    /* Inicializace hodnot rowStructu */
void initRow(rowStruct *row_t) {
    row_t->cells = NULL;
    row_t->cellCount = 0;
}

    /* Inicializace hodnot cellStructu */
void initCell(cellStruct *cell_t) {
    cell_t->str = NULL;
    cell_t->len = 0;
}

void initSelector(selectorStruct *select_t) {
    for(int i = 0; i < 3; i++) {
        select_t->selArgs[i] = 0;
    }
    select_t->selector = SEL_CELL;
}

/********************************************/

/********************************************/
/************* PRACE SE STRUCTY *************/
/********************************************/

    /* Prida novy char 'c' na pozici 'pos' */
int addCellChar(cellStruct *cell_t, char c, int pos) {
    char *temp = malloc((cell_t->len + 2) * sizeof(char));
    if(temp == NULL) _THROW_ERR(_ALLOC_FAIL);
    for(int i = 0; i < pos; i++) {
        temp[i] = cell_t->str[i];
    }
    temp[pos] = c;
    for(int i = pos; i < cell_t->len; i++) {
        temp[i+1] = cell_t->str[i];
    }
    temp[cell_t->len+1] = '\0';
    free(cell_t->str);
    cell_t->str = temp;
    cell_t->len++;
    return 0;
}

    /* Odebere char na pozici 'pos' */
int removeCellChar(cellStruct *cell_t, int pos) {
    char *temp = malloc(cell_t->len * sizeof(char));
    if(temp == NULL) _THROW_ERR(_ALLOC_FAIL);
    for(int i = 0; i < pos; i++) {
        temp[i] = cell_t->str[i];
    }
    for(int i = pos+1; i < cell_t->len; i++) {
        temp[i-1] = cell_t->str[i];
    }
    temp[cell_t->len-1] = '\0';
    free(cell_t->str);
    cell_t->str = temp;
    cell_t->len--;
    return 0;
}

    /* Naplneni noveho (inicalizovaneho) cellStructu */
int setCell(cellStruct *cell_t, char *str, int len) {
    cell_t->str = malloc(len * sizeof(char));
    if(cell_t->str == NULL) _THROW_ERR(_ALLOC_FAIL);
    memmove(cell_t->str, str, len);
    cell_t->len = len;
    return 0;
}

    /* Vytvoreni noveho cellStructu */
cellStruct *newCell(char *str, int len) {
    cellStruct *cell_t = malloc(sizeof(cellStruct));
    if(cell_t == NULL) _THROW_ERR_NULL(_ALLOC_FAIL);
    cell_t->str = malloc(len * sizeof(char));
    if(cell_t->str == NULL) _THROW_ERR_NULL(_ALLOC_FAIL);
    memmove(cell_t->str, str, len);
    cell_t->len = len;
    return cell_t;
}

    /* Pridani cellStructu do rowStructu */
int addToRow(rowStruct *row_t, cellStruct *cell_t) {
    void *p = realloc(row_t->cells, (row_t->cellCount + 1) * sizeof(cellStruct));
    if(p == NULL) _THROW_ERR(_ALLOC_FAIL);
    row_t->cells = p;
    row_t->cells[row_t->cellCount] = *cell_t;
    row_t->cellCount++;
    return 0;
}

    /* Pridani rowStructu do tableStructu */
int addRow(tableStruct *table_t, rowStruct *row_t) {
    void *p = realloc(table_t->rows, (table_t->rowCount + 1) * sizeof(rowStruct));
    if(p == NULL) _THROW_ERR(_ALLOC_FAIL);
    table_t->rows = p;
    table_t->rows[table_t->rowCount] = *row_t;
    table_t->rowCount++;
    return 0;
}

    /* Inicializace hodnot tableStructu */
int initTable(tableStruct *table_t) {
    table_t->rows = NULL;
    table_t->delim = " ";
    table_t->rowCount = 0;
    for(int i = 0; i < 10; i++) {
        cellStruct *cell_t = newCell("\0", 2);
        if(cell_t == NULL) {
            for(int j = i; j >= 0; j--) {
                free(table_t->tempDef[j].str);
            }
            _THROW_ERR(_ALLOC_FAIL);
        }
        table_t->tempDef[i] = *cell_t;
        free(cell_t);
    }
    return 0;
}

/********************************************/

    /* Uvolni misto na rowPos pro rowStruct (zvetsi ho o 1) */
int spaceForRow(tableStruct *table_t, int rowPos) {
    void *p = realloc(table_t->rows, (table_t->rowCount + 1) * sizeof(rowStruct));
    if(p == NULL) _THROW_ERR(_ALLOC_FAIL);
    table_t->rows = p;
    table_t->rowCount++;
    for(int i = table_t->rowCount-2; i >= rowPos; i--) {
        table_t->rows[i+1] = table_t->rows[i];
    }
    return 0;
}

    /* Uvolni misto na colPos pro cellStruct (zvetsi ho o 1) */
int spaceForCol(rowStruct *row_t, int colPos) {
    void *p = realloc(row_t->cells, (row_t->cellCount + 1) * sizeof(cellStruct));
    if(p == NULL) _THROW_ERR(_ALLOC_FAIL);
    row_t->cells = p;
    row_t->cellCount++;
    for(int i = row_t->cellCount-2; i >= colPos; i--) {
        row_t->cells[i+1] = row_t->cells[i];
    }
    return 0;
}

    /* Vytvori prazdny rowStruct s cellCount bunky */
int makeEmptyRow(rowStruct *row_t, int cellCount) {
    for(int i = 0; i < cellCount; i++) {
        cellStruct *cell_t = newCell("\0", 2);
        if(cell_t == NULL) return 1;
        addToRow(row_t, cell_t);
        free(cell_t);
    }
    return 0;
}

    /* Uvolni pamet alokovanou bunkami v row_t */
void freeRowCells(rowStruct *row_t) {
    for(int i = 0; i < row_t->cellCount; i++) {
        free(row_t->cells[i].str);
    }
    free(row_t->cells);
}

    /* Uvolni pamet alokovanou bunkami v row_t + row_t */
void freeRow(rowStruct *row_t) {
    freeRowCells(row_t);
    free(row_t);
}

void freeTempDefs(tableStruct *table_t) {
    for(int i = 0; i < 10; i++) {
        free(table_t->tempDef[i].str);
    }
}

    /* Uvolneni veskerou pamet alokovanou tableStructem */
void freeEverything(tableStruct *table_t) {
    freeTempDefs(table_t);
    for(int i = 0; i < table_t->rowCount; i++) {
        freeRowCells(&(table_t->rows[i]));
    }
    free(table_t->rows);
    free(table_t);
}

/********************************************/
/*************** POMOCNE FCE ****************/
/********************************************/

    /* Kontrola delimiteru */
int isDelim(tableStruct table_t, char c) {
    for(size_t i = 0; i < strlen(table_t.delim); i++) {
        if(table_t.delim[i] == c) return 1;
    }
    return 0;
}

    /* Kontrola delimiteru a uvozovky v bunce */
int shouldQuote(tableStruct table_t, cellStruct *cell_t) {
    bool addQuotes = false;
    if(cell_t->len < 2) return 0;
    for(int i = 0; i < cell_t->len; i++) {
        if(isDelim(table_t, cell_t->str[i])) {
            addQuotes = true;
        } else if (cell_t->str[i] == '\"') {
            if(addCellChar(cell_t, '\\', i)) return 1;
            i++;
        }
    }
    if(addQuotes) {
        if(addCellChar(cell_t, '\"', 0)) return 1;
        if(addCellChar(cell_t, '\"', cell_t->len-1)) return 1;
    }
    return 0;
}

    /* Prevod ze stringu na cislo */
int toNumber(char *argv, int *saveTo) {
    char *ptr;
    int temp;
    temp = strtol(argv, &ptr, 10);
    if(ptr[0] == '\0') {
        *saveTo = temp;
    } else {
        *saveTo = 1;
        return 1;
    }
    return 0;
}

    /* Vraci, jestli se string rovna aspon 1 charu ze stringu */
bool equalToAtLeastOne(char *str, char c) {
    for(int i = 0; str[i]; i++) {
        if(str[i] == c) return true;
    }
    return false;
}

    /* Zjisti, kolik sloupcu je na konci (prave strane tabulky) prazdnych */
int lastEmptyCols(tableStruct table_t) {
    int emptyCols = 0;
    for(int i = table_t.rows[0].cellCount-1; i >= 0; i--) {
        for(int j = 0; j < table_t.rowCount; j++) {
            if(table_t.rows[j].cells[i].len > 1) {
                return emptyCols;
            }
        }
        emptyCols++;
    }
    return emptyCols;
}

    /* Zjisti, kolik radku je na konci prazdnych */
int lastEmptyRows(tableStruct table_t) {
    int emptyRows = 0;
    for(int i = table_t.rowCount; i >= 0; i--) {
        for(int j = 0; j < table_t.rows[i].cellCount; j++) {
            if(table_t.rows[j].cells[i].len > 1) {
                return emptyRows;
            }
        }
        emptyRows++;
    }
    return emptyRows;
}

    /* Zapis do souboru */
void writeTable(tableStruct *table_t, FILE *fp) {
    int emptyCols = lastEmptyCols(*table_t);
    int emptyRows = 0; //lastEmptyRows(*table_t);
    for(int i = 0; i < table_t->rowCount - emptyRows; i++) {
        for(int j = 0; j < table_t->rows[i].cellCount - emptyCols; j++) {
            if(shouldQuote(*table_t, &(table_t->rows[i].cells[j]))) {
                return;
            }
            fputs(table_t->rows[i].cells[j].str, fp);
            if(j < table_t->rows[i].cellCount-emptyCols-1) {
                fputc(table_t->delim[0], fp);
            }
        }
        fputs("\n", fp);
    }
}

    /* Odstrani escapenute znaky bunek */
int formatCell(cellStruct *cell_t) {
    if(cell_t->len > 1) {
        if(cell_t->str[0] == '\"') {
            if(removeCellChar(cell_t, 0)) return 1;
        }
        if(cell_t->str[cell_t->len-2] == '\"') {
            if(removeCellChar(cell_t, cell_t->len-2)) return 1;
        }
    }
    for(int i = 0; i < cell_t->len; i++) {
        if(cell_t->str[i] == '\\') {
            if(removeCellChar(cell_t, i)) return 1;
        }
    }
    return 0; 
}

/********************************************/

    /* Precteni bunky ze souboru */
int readCell(tableStruct *table_t, cellStruct *cell_t, FILE *fp) {
    // temp array pro ukladani charu - at neni potreba reallocovat po 1 charu
    char *tempCell = malloc(100 * sizeof(char));
    if(tempCell == NULL) _THROW_ERR(_ALLOC_FAIL);

    int c, cellLen = 0, reallocCounter = 1;
    // isBounding urcuje, jestli jsme zrovna uprostred stringu uvozeneho uvozovkami
    bool isBounding = false, isEscaped = false;
    do {
        c = fgetc(fp);
        if(c == '\r') continue;
        if(c == EOF) {
            free(tempCell);
            return -2;
        }
        if(cellLen >= (100 * reallocCounter)) {
            reallocCounter *= 2;
            void *p = realloc(tempCell, 100 * reallocCounter * sizeof(char));
            if(p == NULL) {
                free(tempCell);
                _THROW_ERR(_ALLOC_FAIL);
            }
            tempCell = p;
        }
        if((c == '\\') && !isEscaped) {
            isEscaped = true;
            continue;
        }
        tempCell[cellLen] = c;
        cellLen++;
        if((c == '\"') && !isEscaped) {
            if(cellLen == 1) {
                isBounding = !isBounding;
                continue;
            } else if(!isBounding) {
                free(tempCell);

                return 1;
            }
            c = fgetc(fp);
            cellLen++;
            break;
        }
        if(isDelim(*table_t, c) && !isBounding && !isEscaped) {
            break;
        }
        isEscaped = false;
    } while(c != '\n');
    tempCell[cellLen-1] = '\0';
    if(setCell(cell_t, tempCell, cellLen)) {
        free(tempCell);
        return 1;
    }
    free(tempCell);
    return (c == '\n' ? -1 : 0);
}

    /* Precteni radku ze souboru (vola readCell a bunky vklada do rowStructu) */
int readRow(tableStruct *table_t, rowStruct *row_t, FILE *fp) {
    cellStruct *cell_t = malloc(sizeof(cellStruct));
    if(cell_t == NULL) return 1;
    initCell(cell_t);

    int result;
    while((result = readCell(table_t, cell_t, fp)) != -1) {
        // doslo k chybe / jsme na konci tabulky
        if((result == 1) || (result == -2)) {
            free(cell_t);
            return result;
        }
        if(formatCell(cell_t)) {
            free(cell_t);
            return 1;
        }
        // pridame bunku do rowStructu
        if(addToRow(row_t, cell_t)) {
            free(cell_t);
            return 1;
        }
        initCell(cell_t);
    }
    if(formatCell(cell_t)) {
        free(cell_t);
        return 1;
    }
    if(addToRow(row_t, cell_t)) {
        free(cell_t);
        return 1;
    }
    free(cell_t);
    return 0;
}

    /* Precteni souboru (vola readRow a radky uklada do tableStructu) */
int readFile(tableStruct *table_t, FILE *fp) {
    int result;
    rowStruct *row_t = malloc(sizeof(rowStruct));
    if(row_t == NULL) return 1;
    initRow(row_t);

    while((result = readRow(table_t, row_t, fp)) != -2) {
        if(result == 1) {
            free(row_t);
            return 1;
        }
        if(addRow(table_t, row_t)) {
            free(row_t);
            return 1;
        }
        initRow(row_t);
    }
    free(row_t);
    return 0;
}

    /*  Rozdeli string pomoci delim jako delimiteru a ulozi do rowStructu. 
        rowStruct je pouzit pouze proto, ze se chova jako pole stringu */
int splitStr(char *argv, char delim, char *ignore, rowStruct *saveTo) {
    char *tempArr = malloc(10 * sizeof(char));
    if(tempArr == NULL) _THROW_ERR(_ALLOC_FAIL);

    int charCounter = 0, reallocCounter = 1;
    for(size_t i = 0; i <= strlen(argv); i++) {
        if(equalToAtLeastOne(ignore, argv[i])) continue;
        tempArr[charCounter] = argv[i];
        charCounter++;
        if(charCounter >= (10 * reallocCounter)) {
            reallocCounter *= 2;
            void *p = realloc(tempArr, 10 * sizeof(char) * reallocCounter);
            if(p == NULL) {
                free(tempArr);
                _THROW_ERR(_ALLOC_FAIL);
            }
            tempArr = p;
        }
        if((argv[i] == delim) || (argv[i] == '\0')) {
            tempArr[charCounter-1] = '\0';
            cellStruct *cell_t = newCell(tempArr, charCounter);
            if(cell_t == NULL) {
                free(tempArr);
                return 1;
            }
            charCounter = 0;
            if(addToRow(saveTo, cell_t)) {
                free(tempArr);
                free(cell_t->str);
                free(cell_t);
                return 1;
            }
            free(cell_t);
            if(argv[i] == '\0') break;
        }
    }
    free(tempArr);
    return 0;
}

    /* Pro doplneni bunek do radku s mensim poctem bunek */
int fixTable(tableStruct *table_t) {
    int mostCells = 0;
    for(int i = 0; i < table_t->rowCount; i++) {
        if(table_t->rows[i].cellCount > mostCells) mostCells = table_t->rows[i].cellCount;
    }
    for(int i = 0; i < table_t->rowCount; i++) {
        if(table_t->rows[i].cellCount >= mostCells) continue;
        while(table_t->rows[i].cellCount != mostCells) {
            cellStruct *cell_t = newCell("\0", 2);
            if(cell_t == NULL) return 1;
            if(addToRow(&(table_t->rows[i]), cell_t)) {
                free(cell_t);
                return 1;
            }
            free(cell_t);
        }
    }
    return 0;
}

    /* Zvetseni tableStructu - prida/ponecha pocet sloupcu/radku */
int expandTable(tableStruct *table_t, int newRows, int newCols) {
    int oldRows = table_t->rowCount, oldCols = table_t->rows[0].cellCount;
    rowStruct *row_t = malloc(sizeof(rowStruct));
    if(row_t == NULL) _THROW_ERR(_ALLOC_FAIL);
    initRow(row_t);
    for(int i = 0; i < newRows; i++) {
        // mame udelat novy radek - udelame ho a skipnem pridavani bunek
        if(i >= oldRows) {
            if(makeEmptyRow(row_t, newCols)) {
                free(row_t);
                return 1;
            }
            if(addRow(table_t, row_t)) {
                free(row_t);
                return 1;
            }
            initRow(row_t);
        }
        // pridavame x bunek
        for(int j = 0; j < newCols; j++) {
            if((i < oldRows) && (j < oldCols)) continue;
            if(j >= oldCols) {
                cellStruct *cell_t = newCell("\0", 2);
                if(cell_t == NULL) {
                    free(row_t);
                    return 1;
                }
                if(addToRow(&(table_t->rows[i]), cell_t)) {
                    free(cell_t);
                    free(row_t);
                    return 1;
                }
                free(cell_t);
            }
        }
    }
    free(row_t);
    return 0;
}

/* **************************************** */

    /*  Plni rowCell souradnicemi vybrane bunky, dokud muze, jinak vrati -1 
        iterVal je pouze promenna slouzici pro tuto fci, at vi, kde pokracovat; 1. hodnota je 0 
        rowCell[0] = row; rowCell[1] = cell; */
int iterThroughCells(tableStruct *table_t, selectorStruct *select_t, int iterVal, int *rowCell) {
    switch(select_t->selector) {
        case SEL_CELL:
            if(iterVal == 1) return -1;
            rowCell[0] = select_t->selArgs[0];
            rowCell[1] = select_t->selArgs[1];
            return 1;
        case SEL_ROW:
            if(iterVal == table_t->rows[0].cellCount) return -1;
            rowCell[0] = select_t->selArgs[0];
            rowCell[1] = iterVal;
            return iterVal+1;
        case SEL_COL:
            if(iterVal == table_t->rowCount) return -1;
            rowCell[0] = iterVal;
            rowCell[1] = select_t->selArgs[0];
            return iterVal+1;
        case SEL_ALL:
            if(iterVal == 0) {
                rowCell[0] = 0;
                rowCell[1] = 0;
            } else {
                rowCell[0] = ((int)((iterVal / table_t->rows[0].cellCount) + 0.5));
                rowCell[1] = iterVal % table_t->rows[0].cellCount;
            }
            if((table_t->rowCount * table_t->rows[0].cellCount) == iterVal) return -1;
            return iterVal+1;
        case SEL_RANGE:
            if(iterVal == (((select_t->selArgs[2] - select_t->selArgs[0] + 1) * (select_t->selArgs[3] - select_t->selArgs[1] + 1)))) {
                return -1;
            }
            int tempIter = 0;
            for(int i = select_t->selArgs[0]; i <= select_t->selArgs[2]; i++) {
                rowCell[0] = i;
                for(int j = select_t->selArgs[1]; j <= select_t->selArgs[3]; j++) {
                    rowCell[1] = j;
                    if(tempIter == iterVal) {
                        return iterVal+1;
                    }
                    tempIter++;
                }
                rowCell[1] = select_t->selArgs[1] - 1;
            }
            return -1;
    }
    return -1;
}

int selMinMax(tableStruct *table_t, selectorStruct *select_t, bool min) {
    int iterVal = 0, rowCell[2], currNumCords[2], currentNum = 0;
    bool firstNum = true;
    while((iterVal = iterThroughCells(table_t, select_t, iterVal, rowCell)) != -1) {
        int num;
        if(!toNumber(table_t->rows[rowCell[0]].cells[rowCell[1]].str, &num)) {
            if(min) {
                if((firstNum) || (num < currentNum)) {
                    firstNum = false;
                    currentNum = num;
                    currNumCords[0] = rowCell[0];
                    currNumCords[1] = rowCell[1];
                }
            } else {
                if((firstNum) || (num > currentNum)) {
                    firstNum = false;
                    currentNum = num;
                    currNumCords[0] = rowCell[0];
                    currNumCords[1] = rowCell[1];
                }
            }
        }
    }
    if(!firstNum) {
        select_t->selector = SEL_CELL;
        select_t->selArgs[0] = currNumCords[0];
        select_t->selArgs[1] = currNumCords[1];
    }
    return 0;
}

int selFind(tableStruct *table_t, selectorStruct *select_t, char *argv) {
    int iterVal = 0, rowCell[2];
    char *str = (strstr(argv, " ")) + 1;    // uprava argv pro strstr - ' [find STR] -> STR] '
    str[strlen(str)-2] = '\0';    // ' STR] -> STR '
    while((iterVal = iterThroughCells(table_t, select_t, iterVal, rowCell)) != -1) {
        if(strstr(table_t->rows[rowCell[0]].cells[rowCell[1]].str, str) != NULL) {
            select_t->selector = SEL_CELL;
            select_t->selArgs[0] = rowCell[0];
            select_t->selArgs[1] = rowCell[1];
            break;
        }
    }
    return 0;
}

    /* Pomocna fce pro setSelectors - kontroluje, jestli je zadano cislo > 0, a < max cell/row */
int errorCheckSel(tableStruct *table_t, selectorStruct select_t) {
    switch(select_t.selector) {
        case SEL_RANGE:
            for(int i = 0; i < 4; i++) {
                if(select_t.selArgs[i] < 0) return 1;
            }
            int R1 = select_t.selArgs[0]+1, R2 = select_t.selArgs[2]+1, C1 = select_t.selArgs[1]+1, C2 = select_t.selArgs[3]+1;
            if((R1 > R2) || (C1 > C2)) {
                return 1;
            }
            if(R2 > table_t->rowCount) {
                if(expandTable(table_t, R2, table_t->rows[0].cellCount)) return 1;
            }
            if(C2 > table_t->rows[0].cellCount) {
                if(expandTable(table_t, table_t->rowCount, C2)) return 1;
            }
            return 0;
        case SEL_CELL:
            if((select_t.selArgs[0] < 0) || (select_t.selArgs[1] < 0)) {
                return 1;
            }
            if(select_t.selArgs[0]+1 > table_t->rowCount) {
                if(expandTable(table_t, select_t.selArgs[0]+1, table_t->rows[0].cellCount)) return 1;
            }
            if(select_t.selArgs[1]+1 > table_t->rows[0].cellCount) {
                if(expandTable(table_t, table_t->rowCount, select_t.selArgs[1]+1)) return 1;
            }
            return 0;
        case SEL_COL:
            if(select_t.selArgs[0] < 0) {
                return 1;
            }
            if(select_t.selArgs[0]+1 > table_t->rows[0].cellCount) {
                if(expandTable(table_t, table_t->rowCount, select_t.selArgs[0]+1)) return 1;
            }
            return 0;
        case SEL_ROW:
            if(select_t.selArgs[0] < 0) {
                return 1;
            }
            if(select_t.selArgs[0]+1 > table_t->rowCount) {
                if(expandTable(table_t, select_t.selArgs[0]+1, table_t->rows[0].cellCount)) return 1;
            }
            return 0;
        default:
            return 1;
    }
}

    /* Pomocna fce pro selektory - stara se o prevod na int a naplneni selectorStructu */
int setSelectors(tableStruct *table_t, rowStruct *row_t, selectorStruct *select_t) {
    int num[4];
    if(!strcmp(row_t->cells[0].str, "_")) {
        if(!strcmp(row_t->cells[1].str, "_")) {
            select_t->selector = SEL_ALL;
        } else {
            if(toNumber(row_t->cells[1].str, &num[0])) {
                if(strcmp(row_t->cells[1].str, "-") != 0) _THROW_ERR(_INVALID_SELECTOR);
                num[0] = table_t->rows[0].cellCount;
            }
            select_t->selector = SEL_COL;
            select_t->selArgs[0] = num[0] - 1;
            if(errorCheckSel(table_t, *select_t)) _THROW_ERR(_INVALID_SELECTOR);
        }
    } else {
        if(!strcmp(row_t->cells[1].str, "_")) {
            select_t->selector = SEL_ROW;
            if(toNumber(row_t->cells[0].str, &num[0])) {
                if(strcmp(row_t->cells[0].str, "-") != 0) _THROW_ERR(_INVALID_SELECTOR);
                num[0] = table_t->rowCount;
            }
            select_t->selArgs[0] = num[0] - 1;
            if(errorCheckSel(table_t, *select_t)) _THROW_ERR(_INVALID_SELECTOR);
        } else {
            if(row_t->cellCount == 4) select_t->selector = SEL_RANGE;
                else select_t->selector = SEL_CELL;
            for(int i = 0; i < row_t->cellCount; i++) {
                if(toNumber(row_t->cells[i].str, &num[i])) {
                    if(strcmp(row_t->cells[i].str, "-") != 0) _THROW_ERR(_INVALID_SELECTOR);
                    if((i == 0) || (i == 2)) {
                        num[i] = table_t->rowCount;
                    } else {
                        num[i] = table_t->rows[0].cellCount;
                    }
                }
                select_t->selArgs[i] = num[i] - 1;
            }
            if(errorCheckSel(table_t, *select_t)) _THROW_ERR(_INVALID_SELECTOR);
        }
    }
    return 0;
}

int editSelectors(tableStruct *table_t, selectorStruct *select_t, char *argv) {
    // provedem kontrolu [min], [max] a [find STR]
    if(!strcmp(argv, "[max]")) {
        return selMinMax(table_t, select_t, false);
    } else if(!strcmp(argv, "[min]")) {
        return selMinMax(table_t, select_t, true);
    } else if(!strncmp(argv, "[find ", 6)) { // 6 = delka "[find "
        return selFind(table_t, select_t, argv);
    } else if(!strcmp(argv, "[_]")) {
        *select_t = table_t->tempSelect;
        return 0;
    }
    // do rowStructu budou ukladany hodnoty z prikazu vyberu
    rowStruct *row_t = malloc(sizeof(rowStruct));
    if(row_t == NULL) _THROW_ERR(_ALLOC_FAIL);
    initRow(row_t);
    // Ocekavane prikazy: [R,C] [R,_] [_,C] [R1,C1,R2,C2] [_,_] [_]
    if(splitStr(argv, ',', "[]", row_t)) {
        freeRow(row_t);
        return 1;
    }
    if((row_t->cellCount > 4) || (row_t->cellCount < 2)) {
        freeRow(row_t);
        _THROW_ERR(_BAD_SELECTOR);
    }
    // Ocekavane prikazy: [R,C] [R,_] [_,C] [R1,C1,R2,C2] [_,_]
    if(setSelectors(table_t, row_t, select_t)) {
        freeRow(row_t);
        return 1;
    }
    freeRow(row_t);
    return 0;
}

int getSelectedRow(tableStruct table_t, selectorStruct select_t, bool first) {
    switch(select_t.selector) {
        case SEL_ROW:
        case SEL_CELL:
            return select_t.selArgs[0];
        case SEL_RANGE:
            return first ? select_t.selArgs[0] : select_t.selArgs[2];
        case SEL_ALL:
        case SEL_COL:
            return first ? 0 : table_t.rowCount-1;
    }
    return 1;
}

int getSelectedCol(tableStruct table_t, selectorStruct select_t, bool first) {
    switch(select_t.selector) {
        case SEL_ROW:
        case SEL_CELL:
            return select_t.selArgs[1];
        case SEL_RANGE:
            return first ? select_t.selArgs[1] : select_t.selArgs[3];
        case SEL_ALL:
            return first ? 0 : table_t.rows[0].cellCount-1;
        case SEL_COL:
            return select_t.selArgs[0];
    }
    return 0;
}

/* **************************************** */

int iarow(tableStruct *table_t, selectorStruct *select_t, bool arow) {
    rowStruct *row_t = malloc(sizeof(rowStruct));
    if(row_t == NULL) _THROW_ERR(_ALLOC_FAIL);
    initRow(row_t);
    makeEmptyRow(row_t, table_t->rows[0].cellCount);
    int rowPos = getSelectedRow(*table_t, *select_t, false);
    if(arow) rowPos++;
    if(spaceForRow(table_t, rowPos)) {
        freeRow(row_t);
        return 1;
    }
    table_t->rows[rowPos] = *row_t;
    free(row_t);
    return 0;
}

int irow(tableStruct *table_t, selectorStruct *select_t) {
    if(iarow(table_t, select_t, false)) return 1;
    return 0;
}
int arow(tableStruct *table_t, selectorStruct *select_t) {
    if(iarow(table_t, select_t, true)) return 1;
    return 0;
}

int drow(tableStruct *table_t, selectorStruct *select_t) {
    void *p;
    if((select_t->selector == SEL_ALL) || (select_t->selector == SEL_COL)) _THROW_ERR(_DELETED_TABLE);
    int shiftVal;
    if((select_t->selector == SEL_ROW) || (select_t->selector == SEL_CELL)) {
        shiftVal = 1;
        freeRowCells(&(table_t->rows[select_t->selArgs[0]]));
    } else {
        shiftVal = select_t->selArgs[2] - select_t->selArgs[0] + 1;
        if(shiftVal >= table_t->rowCount) _THROW_ERR(_DELETED_TABLE);
        for(int i = select_t->selArgs[0]; i <= select_t->selArgs[2]; i++) {
            freeRowCells(&(table_t->rows[i]));
        }
    }
    table_t->rowCount -= shiftVal;
    for(int i = select_t->selArgs[0]; i < table_t->rowCount; i++) {
        table_t->rows[i] = table_t->rows[i+shiftVal];
    }
    p = realloc(table_t->rows, table_t->rowCount * sizeof(rowStruct));
    if(p == NULL) _THROW_ERR(_ALLOC_FAIL);
    table_t->rows = p;
    return 0;
}

int iacol(tableStruct *table_t, selectorStruct *select_t, bool acol) {
    int colPos = getSelectedCol(*table_t, *select_t, true);
    if(acol) colPos++;
    for(int i = 0; i < table_t->rowCount; i++) {
        if(spaceForCol(&(table_t->rows[i]), colPos)) return 1;
        cellStruct *cell_t = newCell("\0", 2);
        if(cell_t == NULL) return 1;
        table_t->rows[i].cells[colPos] = *cell_t;
        free(cell_t);
    }
    return 0;
}

int icol(tableStruct *table_t, selectorStruct *select_t) {
    if(iacol(table_t, select_t, false)) return 1;
    return 0;
}

int acol(tableStruct *table_t, selectorStruct *select_t) {
    if(iacol(table_t, select_t, true)) return 1;
    return 0;
}

int dcol(tableStruct *table_t, selectorStruct *select_t) {
    void *p;
    if((select_t->selector == SEL_ALL) || (select_t->selector == SEL_ROW)) _THROW_ERR(_DELETED_TABLE);
    int shiftVal;
    if((select_t->selector == SEL_COL) || (select_t->selector == SEL_CELL)) {
        shiftVal = 1;
    } else {
        shiftVal = select_t->selArgs[3] - select_t->selArgs[1] + 1;
        if(shiftVal >= table_t->rows[0].cellCount) _THROW_ERR(_DELETED_TABLE);
    }
    for(int i = 0; i < table_t->rowCount; i++) {
        for(int j = select_t->selArgs[0]; j < (select_t->selArgs[0]+shiftVal); j++) {
            free(table_t->rows[i].cells[j].str);
        }
        table_t->rows[i].cellCount -= shiftVal;
    }
    for(int i = 0; i < table_t->rowCount; i++) {
        for(int j = select_t->selArgs[0]; j < table_t->rows[i].cellCount; j++) {
            table_t->rows[i].cells[j] = table_t->rows[i].cells[j+shiftVal];
        }
        p = realloc(table_t->rows[i].cells, table_t->rows[i].cellCount * sizeof(cellStruct));
        if(p == NULL) _THROW_ERR(_ALLOC_FAIL);
        table_t->rows[i].cells = p;
    }
    return 0;
}

/* **************************************** */

int clearStr(tableStruct *table_t, selectorStruct *select_t) {
    int iterVal = 0, rowCell[2];
    while((iterVal = iterThroughCells(table_t, select_t, iterVal, rowCell)) != -1) {
        void *p = realloc(table_t->rows[rowCell[0]].cells[rowCell[1]].str, 2 * sizeof(char));
        if(p == NULL) _THROW_ERR(_ALLOC_FAIL);
        table_t->rows[rowCell[0]].cells[rowCell[1]].str = p;
        memmove(table_t->rows[rowCell[0]].cells[rowCell[1]].str, "", 1);
        table_t->rows[rowCell[0]].cells[rowCell[1]].len = 1;
    }
    return 0;
}

int setStr(tableStruct *table_t, selectorStruct *select_t, char *argv) {
    char *spaceAt = (strstr(argv, " ")) + 1;
    int iterVal = 0, rowCell[2];
    int len = strlen(spaceAt)+1;
    spaceAt[len-1] = '\0';
    while((iterVal = iterThroughCells(table_t, select_t, iterVal, rowCell)) != -1) {
        free(table_t->rows[rowCell[0]].cells[rowCell[1]].str);
        cellStruct *cell_t = newCell(spaceAt, len);
        if(cell_t == NULL) return 1;
        if(formatCell(cell_t)) return 1;
        table_t->rows[rowCell[0]].cells[rowCell[1]] = *cell_t;
        free(cell_t);
    }
    return 0;
}

int *initSelect(tableStruct *table_t, char *argv) {
    rowStruct *row_t = malloc(sizeof(rowStruct));
    if(row_t == NULL) _THROW_ERR_NULL(_ALLOC_FAIL);
    initRow(row_t);
    char *spaceAt = strstr(argv, " ") + 1;
    int *saveNums = malloc(2 * sizeof(int));
    splitStr(spaceAt, ',', "[]", row_t);
    for(int i = 0; i < 2; i++) {
        if(toNumber(row_t->cells[i].str, &saveNums[i])) {
            freeRow(row_t);
            _THROW_ERR_NULL(_SYNTAX_ERR);
        }
        saveNums[i]--;
        if(saveNums[i] < 0) {
            freeRow(row_t);
            _THROW_ERR_NULL(_SYNTAX_ERR);
        }
    }
    if(saveNums[0] >= table_t->rowCount) {
        expandTable(table_t, saveNums[0]+1, table_t->rows[0].cellCount);
    }
    if(saveNums[1] >= table_t->rows[0].cellCount) {
        expandTable(table_t, table_t->rowCount, saveNums[1]+1);
    }
    freeRow(row_t);
    return saveNums;
}

int swapCells(tableStruct *table_t, selectorStruct *select_t, char *argv) {
    int *nums = initSelect(table_t, argv);
    int iterVal = 0, rowCell[2];
    if(nums == NULL) return 1;
    cellStruct *cell_t = malloc(sizeof(cellStruct));
    if(cell_t == NULL) {
        free(nums);
        _THROW_ERR(_ALLOC_FAIL);
    }
    while((iterVal = iterThroughCells(table_t, select_t, iterVal, rowCell)) != -1) {
        initCell(cell_t);
        *cell_t = table_t->rows[nums[0]].cells[nums[1]];
        table_t->rows[nums[0]].cells[nums[1]] = table_t->rows[rowCell[0]].cells[rowCell[1]];
        table_t->rows[rowCell[0]].cells[rowCell[1]] = *cell_t;
    }
    free(cell_t);
    free(nums);
    return 0;
}

int avgSumCount(tableStruct *table_t, selectorStruct *select_t, char *argv, int sumAvgCount) {
    int iterVal = 0, rowCell[2], cellCount = 0;
    char *p;
    double sum = 0.0, ret;
    int *nums = initSelect(table_t, argv);
    if(nums == NULL) return 1;
    while((iterVal = iterThroughCells(table_t, select_t, iterVal, rowCell)) != -1) {
        if((sumAvgCount == 0) || (sumAvgCount == 1)) {
            ret = strtod(table_t->rows[rowCell[0]].cells[rowCell[1]].str, &p);
            if((p[0] == '\0') && (table_t->rows[rowCell[0]].cells[rowCell[1]].len > 1)) {
                sum += ret;
                cellCount++;
            }
        } else {
            if(table_t->rows[rowCell[0]].cells[rowCell[1]].len > 1) {
                sum += 1;
            }
        }
    }
    if(sumAvgCount == 1) {
        sum = sum / cellCount;
    }
    int size = snprintf(NULL, 0, "%g", sum);
    char *res = malloc(size + 1);
    if(res == NULL) {
        free(nums);
        _THROW_ERR(_ALLOC_FAIL);
    }
    snprintf(res, size+1, "%g", sum);
    free(table_t->rows[nums[0]].cells[nums[1]].str);

    cellStruct *cell_t = malloc(sizeof(cellStruct));
    if(cell_t == NULL) {
        free(nums);
        free(res);
        _THROW_ERR(_ALLOC_FAIL);
    }
    cell_t->str = res;
    cell_t->len = size + 1;

    table_t->rows[nums[0]].cells[nums[1]] = *cell_t;
    free(cell_t);
    free(nums);
    return 0;
}

int sumCells(tableStruct *table_t, selectorStruct *select_t, char *argv) {
    if(avgSumCount(table_t, select_t, argv, 0)) return 1;
    return 0;
}

int avgCells(tableStruct *table_t, selectorStruct *select_t, char *argv) {
    if(avgSumCount(table_t, select_t, argv, 1)) return 1;
    return 0;
}

int countCells(tableStruct *table_t, selectorStruct *select_t, char *argv) {
    if(avgSumCount(table_t, select_t, argv, 2)) return 1;
    return 0;
}

int lenCell(tableStruct *table_t, selectorStruct *select_t, char *argv) {
    if(select_t->selector != SEL_CELL) _THROW_ERR(_COUNT_ERR);
    int *nums = initSelect(table_t, argv);
    if(nums == NULL) return 1;
    int len = table_t->rows[select_t->selArgs[0]].cells[select_t->selArgs[1]].len - 1;

    int size = snprintf(NULL, 0, "%d", len);
    char *res = malloc(size + 1);
    if(res == NULL) {
        free(nums);
        _THROW_ERR(_ALLOC_FAIL);
    }
    snprintf(res, size+1, "%d", len);
    free(table_t->rows[nums[0]].cells[nums[1]].str);

    cellStruct *cell_t = malloc(sizeof(cellStruct));
    if(cell_t == NULL) {
        free(nums);
        free(res);
        _THROW_ERR(_ALLOC_FAIL);
    }
    cell_t->str = res;
    cell_t->len = size;

    table_t->rows[nums[0]].cells[nums[1]] = *cell_t;

    free(cell_t);
    free(nums);
    return 0;
}

int convertAndCheckNum(char *argv, int *saveTo) {
    char *startAt = strstr(argv, "_");
    if((startAt == NULL) || (strlen(startAt) < 2)) _THROW_ERR(_SYNTAX_ERR);
    startAt+=1;
    if(toNumber(startAt, saveTo)) _THROW_ERR(_SYNTAX_ERR);
    bool isValid = false;
    for(int i = 0; i < 10; i++) {
        if(*saveTo == i) {
            isValid = true;
            break;
        }
    }
    if(!isValid) _THROW_ERR(_SYNTAX_ERR);
    return 0;
}

int useDefCell(tableStruct *table_t, selectorStruct *select_t, char *argv, bool use) {
    int selRow = getSelectedRow(*table_t, *select_t, true);
    int selCol = getSelectedCol(*table_t, *select_t, true);
    int num = 0;
    if(convertAndCheckNum(argv, &num)) return 1;
    if(use) {
        int iterVal = 0, rowCell[2];
        while((iterVal = iterThroughCells(table_t, select_t, iterVal, rowCell)) != -1) {
            free(table_t->rows[rowCell[0]].cells[rowCell[1]].str);
            cellStruct *cell_t = newCell(table_t->tempDef[num].str, table_t->tempDef[num].len);
            if(cell_t == NULL) return 1;
            table_t->rows[rowCell[0]].cells[rowCell[1]] = *cell_t;
            free(cell_t);
        }
    } else {
        free(table_t->tempDef[num].str);
        cellStruct *cell_t = newCell(table_t->rows[selRow].cells[selCol].str, table_t->rows[selRow].cells[selCol].len);
        if(cell_t == NULL) return 1;
        table_t->tempDef[num] = *cell_t;
        free(cell_t);
    }
    return 0;
}

int defCell(tableStruct *table_t, selectorStruct *select_t, char *argv) {
    if(useDefCell(table_t, select_t, argv, false)) return 1;
    return 0;
}

int useCell(tableStruct *table_t, selectorStruct *select_t, char *argv) {
    if(useDefCell(table_t, select_t, argv, true)) return 1;
    return 0;
}

int incCell(tableStruct *table_t, char *argv) {
    int num, cellNum;
    if(convertAndCheckNum(argv, &num)) return 1;
    if(toNumber(table_t->tempDef[num].str, &cellNum)) {
        free(table_t->tempDef[num].str);
        cellStruct *cell_t = newCell("1", 2);
        if(cell_t == NULL) return 1;
        table_t->tempDef[num] = *cell_t;
        free(cell_t);
    } else {
        cellNum++;
        int size = snprintf(NULL, 0, "%d", cellNum);
        char *res = malloc(size + 1);
        if(res == NULL) _THROW_ERR(_ALLOC_FAIL);
        snprintf(res, size+1, "%d", cellNum);
        free(table_t->tempDef[num].str);
        table_t->tempDef[num].str = res;
        table_t->tempDef[num].len = size+1;
    }
    return 0;
}

/* **************************************** */

int isInvalidDelim(char *argv) {
    for(int i = 0; argv[i]; i++) {
        if((argv[i] == '\\') || (argv[i] == '\"')) return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    tableStruct *table_t = malloc(sizeof(tableStruct));
    selectorStruct select_t;
    initSelector(&select_t);
    initSelector(&(table_t->tempSelect));
    if(initTable(table_t)) {
        free(table_t);
        return 1;
    }
    int cmdSeqPos = 1;
    if(argc < 2) {
        freeTempDefs(table_t);
        free(table_t);
        _THROW_ERR(_USAGE);
    }
    if(!strcmp(argv[1], "-d")) {
        if(argc < 5) {
            freeTempDefs(table_t);
            free(table_t);
            _THROW_ERR(_MISSING_ARGS);
        }
        if(isInvalidDelim(argv[2])) {
            freeTempDefs(table_t);
            free(table_t);
            _THROW_ERR(_INVALID_DELIM);
        }
        table_t->delim = argv[2];
        cmdSeqPos = 3;
    } else {
        if(argc < 3) {
            freeTempDefs(table_t);
            free(table_t);
            _THROW_ERR(_MISSING_ARGS);
        }
    }
    FILE *fp = fopen(argv[cmdSeqPos+1], "r");
    if(fp == NULL) {
        freeTempDefs(table_t);
        free(table_t);
        _THROW_ERR(_NONEXISTING_FILE);
    }
    // readFile vrati 1 pokud dojde k chybe
    if(readFile(table_t, fp)) {
        freeEverything(table_t);
        fclose(fp);
        return 1;
    }

    if(fixTable(table_t)) {
        freeEverything(table_t);
        fclose(fp);
        return 1;
    }

    rowStruct *row_t = malloc(sizeof(rowStruct));
    if(row_t == NULL) {
        freeEverything(table_t);
        fclose(fp);
        _THROW_ERR(_ALLOC_FAIL);
    }
    initRow(row_t);
    // projeti CMD_SEQUENCE
    splitStr(argv[cmdSeqPos], ';', "", row_t);

    struct structMapArgs structDict[] = {
        {"irow", &irow},
        {"arow", &arow},
        {"drow", &drow},
        {"icol", &icol},
        {"acol", &acol},
        {"dcol", &dcol},
        {"clear", &clearStr}    // nepatri sice do fci pro upravu struktury, ale parametry ma stejne
    };
    struct editMapArgs editDict[] = {
        {"set ", &setStr},
        {"swap ", &swapCells},
        {"sum ", &sumCells},
        {"avg ", &avgCells},
        {"count ", &countCells},
        {"len ", &lenCell},
        {"use ", &useCell},
        {"def ", &defCell}
    };

    int editMapSize = (sizeof(editDict) / sizeof(struct editMapArgs));
    int structMapSize = (sizeof(structDict) / sizeof(struct structMapArgs));

    for(int i = 0; i < row_t->cellCount; i++) {
        char *cmd = row_t->cells[i].str;
        if(!strcmp(cmd, "[set]")) {
            table_t->tempSelect = select_t;
        } else if(cmd[0] == '[') {
            if(editSelectors(table_t, &select_t, row_t->cells[i].str)) {
                freeEverything(table_t);
                freeRow(row_t);
                fclose(fp);
                return 1;
            }
        } else if(!strncmp("inc _", cmd, 5)) {
            if(incCell(table_t, row_t->cells[i].str)) {
                freeEverything(table_t);
                freeRow(row_t);
                fclose(fp);
                return 1;
            }
        } else {
            for(int i = 0; i < structMapSize; i++) {
                if(!strncmp(structDict[i].str, cmd, strlen(structDict[i].str))) {
                    if(structDict[i].pFunc(table_t, &select_t)) {
                        freeEverything(table_t);
                        freeRow(row_t);
                        fclose(fp);
                        return 1;
                    }
                }
            }
            for(int i = 0; i < editMapSize; i++) {
                if(!strncmp(editDict[i].str, cmd, strlen(editDict[i].str))) {
                    if(editDict[i].pFunc(table_t, &select_t, cmd)) {
                        freeEverything(table_t);
                        freeRow(row_t);
                        fclose(fp);
                        return 1;
                    }
                }
            }
        }
    }
    fclose(fp);
    // Vytvoreni noveho souboru (se stejnym nazvem) a zapis
    fp = fopen(argv[cmdSeqPos+1], "w+");
    if(fp == NULL) {
        freeEverything(table_t);
        freeRow(row_t);
        return 1;
    }
    writeTable(table_t, fp);
    freeEverything(table_t);
    freeRow(row_t);
    fclose(fp);
    return 0;
}
