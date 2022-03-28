/**
 * @file io.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 2)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-08
 * 
 */
#include "io.h"

int read_word(char *s, int max, FILE *f) {
    int c, charCount = 0;
    while((c = fgetc(f)) != EOF) {
        if(isspace(c)) {
            s[charCount] = '\0';
            return charCount;
        }
        s[charCount++] = c;
        if(charCount == max) {
            s[charCount-1] = '\0';
            while((c = fgetc(f)) != EOF) {
                if(isspace(c)) return max;
            }
            return max;
        }
    }
    s[charCount] = '\0';
    return EOF;
}