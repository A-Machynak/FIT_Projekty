/**
 * @file error.h
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU1, assignment B)
 *        Compiled using gcc (Debian 10.2.1-6) 10.2.1
 * @date 2021-02-27
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef _ERROR_H_
#define _ERROR_H_

/**
 * @brief Prints warning message on stderr
 * 
 * @param fmt Message format
 * @param ... Additional (formatting) arguments
 */
void warning_msg(const char *fmt, ...);

/**
 * @brief Prints error message on stderr and exits
 * 
 * @param fmt Message format
 * @param ... Additional (formatting) arguments
 */
void error_exit(const char *fmt, ...);

#endif /*** End of _ERROR_H_ ***/