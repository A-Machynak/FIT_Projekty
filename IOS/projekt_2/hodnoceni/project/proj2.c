/**
 * @file proj2.c
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IOS - project 2 (Synchronization)
 *        "Santa Claus problem"
 *        Compileg using gcc (Debian 10.2.1-6)
 * @date 2021-04-16
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/wait.h>

#include "actors.h"

/**
 * @brief Prints usage and exits with exit code 1
 * 
 */
void printUsage() {
    printf("Usage: ./proj2 NE NR TE TR\n\n\
NE:\tElf count (0 < NE < 1000)\n\
NR:\tReindeer count (0 < NR < 20)\n\
TE:\tMax elf work time in ms (0 <= TE <= 1000)\n\
TR:\tMax reindeer holiday time in ms (0 <= RE <= 1000)\n");
    exit(1);
}

/**
 * @brief Checks and saves the input numbers (NE, NR, TE, TR) to retNums
 * 
 * @param argc argc
 * @param argv argv
 * @param retNums array of size 4 to save the input into
 */
void parseInput(int argc, char **argv, uint16_t *retNums) {
    if(argc != 5) {
        fprintf(stderr, "[ERROR]: Expected %d arguments, received %d.\n", 4, argc-1);
        printUsage();
    }

    // Get input numbers
    int numRange[4][2] = { {1, 999}, {1, 19}, {0, 1000}, {0, 1000} };
    for(uint8_t i = 1; i < 5; i++) {
        char *ret = NULL;
        long int num = strtol(argv[i], &ret, 10);
        if(*ret != 0) {
            fprintf(stderr, "[ERROR]: Expected number, received \"%s\".\n", argv[i]);
            printUsage();
        }
        if((num < numRange[i-1][0]) || (num > numRange[i-1][1])) {
            fprintf(stderr, "[ERROR]: Number out of range. \
Expected number between %d - %d, received %ld\n", numRange[i-1][0], numRange[i-1][1], num);
            printUsage();
        }
        retNums[i-1] = (uint16_t)num;
    }
}

int main(int argc, char *argv[]) {
    enum { NE, NR, TE, TR };
    uint16_t inputNum[4];
    parseInput(argc, argv, inputNum);
    FILE *f = fopen("proj2.out", "w");

    if(f == NULL) {
        fprintf(stderr, "[ERROR]: Couldn't create/open file proj2.out.\n");
        return 1;
    }

    if(initActors()) {
        fprintf(stderr, "[ERROR]: Error while creating semaphores/shared memory.\n");
        destroyActors();
        fclose(f);
        return 1;
    }

    pid_t pid = fork();
    if(pid < 0) {
        perror("Fork error\n");
        return 1;
    } else if(pid == 0) { // Child process
        startSanta(f);
        fclose(f);
        return 0;
    }

    for(uint16_t i = 0; i < inputNum[NE]; i++) {
        pid = fork();
        if(pid < 0) {
            perror("Fork error");
        } else if (pid == 0) { // Child process
            startElf(f, i + 1, inputNum[TE]);
            fclose(f);
            return 0;
        }
    }

    for(uint16_t i = 0; i < inputNum[NR]; i++) {
        pid = fork();
        if(pid < 0) {
            perror("Fork error");
        } else if (pid == 0) { // Child process
            startReindeer(f, i + 1, inputNum[TR], inputNum[NR]);
            fclose(f);
            return 0;
        }
    }
    
    while(wait(NULL)) {
        if(errno == ECHILD) break;
    }

    fclose(f);
    destroyActors();
    return 0;
}