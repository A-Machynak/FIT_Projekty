/**
 * @file error.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU1, assignment B)
 *        Compiled using gcc (Debian 10.2.1-6) 10.2.1
 * @date 2021-02-27
 * 
 */

#include "error.h"

void warning_msg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void error_exit(const char *fmt, ...) {
    fprintf(stderr, "CHYBA: ");

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
}