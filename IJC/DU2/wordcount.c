/**
 * @file wordcount.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IJC-DU2, assignment 2)
 *        Compiled using gcc (Debian 10.2.1-6)
 * @date 2021-04-07
 * 
 */
#include "htab.h"
#include "io.h"

/**
 * @brief Max string length
 * 
 */
#define WRDC_MAX_LEN 128

/**
 * "V programu zvolte vhodnou velikost pole a v komentari zduvodnete vase rozhodnuti."
 *
 * Po vyhledani "Average number of unique words in a book" a kliknuti na prvni odkaz 
 * (https://writing.stackexchange.com/questions/6988/how-many-different-words-are-in-the-average-novel)
 * jsem zjistil, ze pocet unikatnich slov v knihach se muze pohybovat mezi 4000-15000.
 * Proto bylo vybrano cislo 8000, ktere mi prislo vhodne.
 */
#define WRDC_ARR_SIZE 8000

#ifdef HASHTEST
// Slightly different htab_hash_function
size_t htab_hash_function(const char *str) {
    uint32_t h = 0;
    const unsigned char *p;
    for (p = (const unsigned char *)str; *p != '\0'; p++)
        h = 32650 * h + *p;
    return h;
}
#endif // HASHTEST

void printData(htab_pair_t *data) {
    printf("%s\t%d\n", data->key, data->value);
}

int main(void) {
    htab_t *tab = htab_init(WRDC_ARR_SIZE);
    if(tab == NULL) {
        fprintf(stderr, "[ERROR]: Couldn't create new hash table.\n");
        return 1;
    }

    char str[WRDC_MAX_LEN] = {0};
    FILE *fd = stdin;
    htab_pair_t *pair;
    void (*fp)(htab_pair_t *) = &printData;

    int ret;
    bool warned = false;
    while((ret = read_word(str, WRDC_MAX_LEN, fd)) != EOF) {
        if (ret == WRDC_MAX_LEN && !warned) {
            warned = true;
            fprintf(stderr, "[WARNING]: Found word(s) with more than %d (maximum word limit) characters. The word(s) will be trimmed.\n", WRDC_MAX_LEN-1);
        }
        
        pair = htab_lookup_add(tab, str);
        if(pair == NULL) {
            fprintf(stderr, "[ERROR]: Couldn't add new item to hash table.");
            htab_free(tab);
            return 1;
        }
        pair->value++;
    }
    htab_for_each(tab, fp);

    htab_free(tab);
    fclose(fd);
    return 0;
}