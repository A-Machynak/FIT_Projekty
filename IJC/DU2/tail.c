/**
 * @file tail.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 1)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-07
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Default number of lines to print
 * 
 */
#define DEFAULT_LC 10

/**
 * @brief Max number of characters per line
 * 
 */
#define MAX_CHARS 511

    // simple exit
void errExit(const char *str) {
    fprintf(stderr, "%s\n", str);
    exit(1);
}

int readAndPrint(int lines, FILE *fd) {
    int c, counter = 0, lineCount = 0, linesRead = 0;
    char overTheLimit = 0;
    char *outLines[MAX_CHARS+1];
    outLines[0] = malloc(MAX_CHARS+1);

    while((c = fgetc(fd)) != EOF) {
        if(c == '\n') {
            outLines[lineCount++][counter] = '\0';
            lineCount = (counter = 0, lineCount == lines ? 0 : lineCount);
            linesRead++;
            if(linesRead < lines) {
                outLines[lineCount] = malloc(MAX_CHARS+1);
                // Error while allocating memory ?
                if(outLines[lineCount] == NULL) {
                    goto _err_exit;
                }
            }
            continue;
        }
        outLines[lineCount][counter++] = c;
        if(counter >= MAX_CHARS) {
            overTheLimit = 1;
            outLines[lineCount++][counter] = '\0';
            if(linesRead < lines) {
                outLines[lineCount] = malloc(MAX_CHARS+1);
                // Error while allocating memory ?
                if(outLines[lineCount] == NULL) {
                    goto _err_exit;
                }
            }
            // Skip until newline/EOF
            while((c = getc(fd)) != EOF) if(c == '\n') break;
        }
    }

    if(linesRead < lines) {
        for(int i = 0; i < linesRead; i++) {
            printf("%s\n", outLines[i]);
            free(outLines[i]);
        }
        free(outLines[linesRead]);
        return 0;
    }

    if(overTheLimit) {
        fprintf(stderr, "[WARNING]: One of the lines is longer than maximum (%d) characters. Only the first %d characters of that line will be printed\n", MAX_CHARS, MAX_CHARS);
    }

    int startLine = lineCount;
    printf("%s\n", outLines[lineCount]);
    free(outLines[lineCount]);
    lineCount = (++lineCount >= lines ? 0 : lineCount);

    while(lineCount != startLine) {
        printf("%s\n", outLines[lineCount]);
        free(outLines[lineCount]);
        lineCount++;
        lineCount = (lineCount >= lines ? 0 : lineCount);
    }
    return 0;

    _err_exit:  // Allocation error
        // free what was allocated previously and exit
        for(int i = 0; i < lineCount; i++) {
            free(outLines[i]);
        }
        if(fd != stdin) {
            fclose(fd);
        }
        errExit("[ERROR]: Error while allocating memory");
        return 1;
}

int main(int argc, char **argv) {
    unsigned int printL = DEFAULT_LC;
    FILE *fd = stdin;

    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "-n")) {
            char *ret = NULL;
            if(++i >= argc) {
                errExit("[ERROR]: Missing number after -n");
            }
            long int lines = strtol(argv[i], &ret, 10);
            if(*ret != 0) {
                errExit("[ERROR]: Enter a number after -n");
            }
            if(lines <= 0) {
                errExit("[ERROR]: Enter a number greater than zero");
            }
            printL = lines;
        } else { // expect file
            fd = fopen(argv[i], "r");
            if(fd == NULL) {
                errExit("[ERROR]: Couldn't open file or it doesn't exist");
            }
        }
    }
    readAndPrint(printL, fd);
    if(fd != stdin) {
        fclose(fd);
    }
    return 0;
}
