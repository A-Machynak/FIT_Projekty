/**
 * @file steg-decode.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU1, assignment B)
 *        Compiled using gcc (Debian 10.2.1-6) 10.2.1
 * @date 2021-03-05
 * 
 */
#include "ppm.h"
#include "erastothenes.h"
#include "bitset.h"

/**
 * @brief Specifies the amount of memory to be used
 *        for character buffer. Increases by x2
 *        whenever the buffer runs out of space.
 * 
 */
#define BUFF_ALLOC_SIZE 110

int main(void) {
    struct ppm *p = ppm_read("du1-obrazek.ppm");
    if(p == NULL) return 1;

    unsigned long bits = p->xsize * p->ysize * 3 * CHAR_BIT;
    bitset_alloc(bitArr, bits);
    Erastothenes(bitArr);

    // Character buffer
    char *str = malloc(BUFF_ALLOC_SIZE);
    if(str == NULL) {
        ppm_free(p);
        bitset_free(bitArr);
        error_exit("steg-decode: Chyba pri alokaci pameti");
    }

    // mul - realloc multiplier
    unsigned long mul = 1, bitCount = 0, charCount = 0;
    char c = 0;
    for(unsigned long i = 23; i < bits-1; i++) {
        if(!bitset_getbit(bitArr, i)) {
            c |= ((p->data[i] & 1) << bitCount++);
            if(bitCount == CHAR_BIT) {
                str[charCount++] = c;
                if(c == '\0')
                    break;
                c = 0;
                bitCount = 0;
            }
            // Resize buffer if needed
            if(charCount >= (mul * BUFF_ALLOC_SIZE)) {
                void *p = realloc(str, (mul *= 2, mul * BUFF_ALLOC_SIZE));
                if(p == NULL) {
                    free(str);
                    ppm_free(p);
                    bitset_free(bitArr);
                    error_exit("steg-decode: Chyba pri realokaci pameti");
                }
                str = p;
            }
        }
    }

    if(str[charCount-1] != '\0') {
        free(str);
        ppm_free(p);
        bitset_free(bitArr);
        error_exit("steg_decode: Zprava neni korektne ukoncena");
    }

    fprintf(stdout, "%s", str);
    free(str);
    ppm_free(p);
    bitset_free(bitArr);
    return 0;
}