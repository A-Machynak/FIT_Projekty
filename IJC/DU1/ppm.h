/**
 * @file ppm.h
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU1, assignment B)
 *        Compiled using gcc (Debian 10.2.1-6) 10.2.1
 * @date 2021-03-04
 * 
 */

#include <stdio.h>
#include <string.h>
#include "error.h"

#ifndef _PPM_H_
#define _PPM_H_

/**
 * @brief Maximum size of image
 * 
 */
#define PPM_MAX_SIZE (8000 * 8000 * 3)

/**
 * @brief Max length of the first 3 lines specifying format
 *        { P6
 *         <xsize> <ysize>
 *         255 }
 * 
 */
#define PPM_MAX_SIZELEN 20

/**
 * @brief Simple digit check (0 - 9)
 *        (0 - ASCII 48; 9 - ASCII 57)
 * 
 */
#define IS_DIGIT(x) (((x) >= 48) && ((x) <= 57))

/**
 * @brief Holds PPM file data
 * 
 */
struct ppm {
    unsigned xsize;
    unsigned ysize;
    char data[];      // RGB - 3 * xsize * ysize
};

/**
 * @brief Loads a PPM file into dynamically allocated
 *        struct and returns it.
 *        Returns NULL and prints error message
 *        if the file cannot be loaded.
 * 
 * @param filename Name of the PPM file ( *.ppm )
 * @return struct ppm* Struct filled with PPM file data
 */
struct ppm * ppm_read(const char *filename);

/**
 * @brief Frees ppm struct created by ppm_read()
 * 
 * @param p struct to be freed
 */
void ppm_free(struct ppm *p);

#endif /*** End of _PPM_H_ ***/