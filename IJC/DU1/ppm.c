/**
 * @file ppm.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU1, assignment B)
 *        Compiled using gcc (Debian 10.2.1-6) 10.2.1
 * @date 2021-03-04
 * 
 */
#include "ppm.h"

struct ppm * ppm_read(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) {
        warning_msg("ppm_read: Soubor se nepodarilo otevrit");
        return NULL;
    }

    char temp[PPM_MAX_SIZELEN];
    int count = 0;
    unsigned xsize, ysize;

    /*** FORMAT CHECK + xsize && ysize ***/

    // Expect "P6\n"
    if(fgets(temp, PPM_MAX_SIZELEN, fp) == NULL)
        goto err_ret;
    if((temp[0] != 'P') || (temp[1] != '6') || (temp[2] != '\n'))
        goto err_ret;

    // Expect "<xsize> "
    if(fgets(temp, PPM_MAX_SIZELEN, fp) == NULL)
        goto err_ret;
    for(; (count < PPM_MAX_SIZELEN) && (temp[count] != ' '); count++)
        if((temp[count] == '\n') || (!IS_DIGIT(temp[count])))
            goto err_ret;

    if(temp[count++] != ' ')
        goto err_ret;

    // Get xsize
    xsize = strtoul(temp, NULL, 10);

    // Expect "<ysize>\n"
    for(int i = count; (i < PPM_MAX_SIZELEN) && (temp[i] != '\n'); i++)
        if((temp[i] == EOF) || (!IS_DIGIT(temp[i])))
            goto err_ret;

    // Get ysize
    ysize = strtoul(&(temp[count]), NULL, 10);
    if((3 * xsize * ysize) > PPM_MAX_SIZE) {
        warning_msg("ppm_read: PPM soubor je prilis velky (%d > %d)", (3 * xsize * ysize), PPM_MAX_SIZE);
        fclose(fp);
        return NULL;
    }

    // Expect "255\n"
    if(fgets(temp, PPM_MAX_SIZELEN, fp) == NULL) 
        goto err_ret;
    if((temp[0] != '2') && (temp[1] != '5') && (temp[2] != '5') && (temp[3] != '\n'))
        goto err_ret;

    /*** END OF FORMAT CHECK ***/

    struct ppm *p = malloc((2 * sizeof(unsigned)) + (3 * xsize * ysize));
    if(p == NULL) {
        warning_msg("ppm_read: Chyba pri alokaci pameti.");
        fclose(fp);
        return NULL;
    }
    p->xsize = xsize;
    p->ysize = ysize;

    unsigned int read = fread(p->data, 1, (3 * xsize * ysize), fp);
    if(read != (3 * xsize * ysize)) {
        warning_msg("ppm_read: Spatna velikost souboru. Velikost: %d | Ocekavano: %d).", read, (3 * xsize * ysize));
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    return p;

    err_ret:
        warning_msg("ppm_read: PPM soubor neodpovida formatu P6");
        fclose(fp);
        return NULL;
}

void ppm_free(struct ppm *p) {
    free(p);
}