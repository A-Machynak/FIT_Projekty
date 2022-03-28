/**
 * @file actors.h
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 1BIT
 * @brief Solution for IOS - project 2 (Synchronization)
 *        "Santa Claus problem"
 *        Defines functions for actors (santa, elf and reindeer processes)
 *        and shared memory identificators
 *        Compileg using gcc (Debian 10.2.1-6)
 * @date 2021-04-17
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <sys/shm.h>

#ifndef _PROJ2_ACTORS_
#define _PROJ2_ACTORS_


typedef enum Actors {
    SANTA,
    ELF,
    REINDEER
} Actors;

/**
 * @brief Initializes shared memory and semaphores
 * 
 */
int initActors();

/**
 * @brief Initialize semaphore to be shared between processes
 * 
 * @param sem semaphore to be initialized
 * @param semId shared memory id
 * @param val default value
 */
int initSem(sem_t **sem, int *semId, int val);

/**
 * @brief Initialize integer to be shared between processes
 * 
 * @param intP integer to be initialize
 * @param intId shared memory id
 * @param val default value
 */
int initInt(int **intP, int *intId, int val);

/**
 * @brief Removes shared memory and semaphores
 * 
 */
void destroyActors();

/**
 * @brief Santa process
 * 
 * @param pid process id
 * @param f File descriptor to write the actions into
 */
void startSanta(FILE *f);

/**
 * @brief Elf process
 * 
 * @param pid process id
 * @param f File descriptor to write the actions into
 */
void startElf(FILE *f, int eId, int maxWorkTime);

/**
 * @brief Reindeer process
 * 
 * @param pid process id
 * @param f File descriptor to write the actions into
 */
void startReindeer(FILE *f, int rId, int maxWorkTime, int reindeerCount);

/**
 * @brief Sleeps for the specified time
 * 
 * @param msWork Time in miliseconds
 */
void actWork(uint16_t msWork);

/**
 * @brief Prints a message and increases message counter
 * Format: "[Msg count]: [Santa/Elf/Reindeer] [Actor ID (none for Santa)]: [Message]"
 * 
 * @param a Santa/Elf/Reindeer
 * @param actorId Actor's id
 * @param fmt printf format
 * @param ... printf format
 */
void sendMsg(Actors a, int actorId, FILE *f, const char *fmt, ... );

/* SMEM IDS */
extern int msgC_id;
extern int elfC_id;
extern int reinC_id;
extern int msgSem_id;
extern int globMutex_id;
extern int santaENSem_id;
extern int hitchSem_id;
extern int queueSem_id;
extern int chstmsENSem_id;
extern int elfHelpSem_id;
extern int elfHelpedSem_id;
extern int takeHoliday_id;
extern int closedWS_id;

/* INT SMEMS */
extern int *msgC;
extern int *elfC;
extern int *reinC;
extern int *takeHoliday;
extern int *closedWS;

/* SEMAPHORE SMEMS */
extern sem_t *msgSem;
extern sem_t *globMutex;
extern sem_t *santaENSem;
extern sem_t *hitchSem;
extern sem_t *queueSem;
extern sem_t *chstmsENSem;
extern sem_t *elfHelpedSem;
extern sem_t *elfHelpSem;

#endif