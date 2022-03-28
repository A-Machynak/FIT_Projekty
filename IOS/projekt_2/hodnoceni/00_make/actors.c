#include "actors.h"

int msgC_id, \
    elfC_id, \
    reinC_id, \
    msgSem_id, \
    globMutex_id, \
    santaENSem_id, \
    hitchSem_id, \
    queueSem_id, \
    chstmsENSem_id, \
    elfHelpSem_id, \
    elfHelpedSem_id, \
    takeHoliday_id, \
    closedWS_id;

int *msgC = NULL, \
    *elfC = NULL, \
    *reinC = NULL, \
    *takeHoliday = NULL, \
    *closedWS = NULL;

sem_t *msgSem = NULL, \
    *globMutex = NULL, \
    *santaENSem = NULL, \
    *hitchSem = NULL, \
    *queueSem = NULL, \
    *chstmsENSem = NULL, \
    *elfHelpedSem = NULL, \
    *elfHelpSem = NULL;


int initActors() {
    // init shared memory for semaphores
    if(initSem(&msgSem, &msgSem_id, 1)) return 1;
    if(initSem(&hitchSem, &hitchSem_id, 0)) return 1;
    if(initSem(&globMutex, &globMutex_id, 1)) return 1;
    if(initSem(&santaENSem, &santaENSem_id, 0)) return 1;
    if(initSem(&queueSem, &queueSem_id, 1)) return 1;
    if(initSem(&chstmsENSem, &chstmsENSem_id, 0)) return 1;
    if(initSem(&elfHelpSem, &elfHelpSem_id, 0)) return 1;
    if(initSem(&elfHelpedSem, &elfHelpedSem_id, 0)) return 1;

    if(initInt(&msgC, &msgC_id, 1)) return 1;
    if(initInt(&elfC, &elfC_id, 0)) return 1;
    if(initInt(&reinC, &reinC_id, 0)) return 1;
    if(initInt(&takeHoliday, &takeHoliday_id, 0)) return 1;
    if(initInt(&closedWS, &closedWS_id, 0)) return 1;
    return 0;
}

void startSanta(FILE *f) {
    sendMsg(SANTA, 0, f, "going to sleep\n");
    while(1) {
        sem_wait(santaENSem);
        sem_wait(globMutex);
        if(*takeHoliday) {
            break;
        } else if ((*elfC) == 3) {
            sendMsg(SANTA, 0, f, "helping elves\n");
            sem_post(globMutex);
            // help 3 elves
            sem_post(elfHelpSem);
            sem_post(elfHelpSem);
            sem_post(elfHelpSem);
            // wait until they finish
            sem_wait(elfHelpedSem);
            sem_wait(globMutex);
            // reindeers could return while santa was helping
            if(*takeHoliday) {
                break;
            } /*else // - Should Santa go to sleep when he's about to close workshop ?*/
            sendMsg(SANTA, 0, f, "going to sleep\n");
        }
        sem_post(globMutex);
    }
    sendMsg(SANTA, 0, f, "closing workshop\n");
    *closedWS = 1;
    sem_post(hitchSem);
    sem_wait(chstmsENSem);
    sendMsg(SANTA, 0, f, "Christmas started\n");
    sem_post(elfHelpSem);
    sem_post(globMutex);
}

void startElf(FILE *f, int eId, int maxWorkTime) {
    srand(time(0));
    int workTime = (maxWorkTime == 0 ? 0 : (rand() % maxWorkTime));
    sendMsg(ELF, eId, f, "started\n");
    while(1) {
        actWork(workTime);
        sendMsg(ELF, eId, f, "need help\n");

        sem_wait(queueSem);
        sem_wait(globMutex);
        if(*closedWS) break;
        if((++(*elfC)) >= 3) {
            sem_post(santaENSem);
        } else {
            sem_post(queueSem);
        }
        sem_post(globMutex);

        sem_wait(elfHelpSem);

        sem_wait(globMutex);
        if(*closedWS) break;
        if((--(*elfC)) <= 0) {
            sendMsg(ELF, eId, f, "get help\n");
            sem_post(elfHelpedSem);
            sem_post(queueSem);
        } else {
            sendMsg(ELF, eId, f, "get help\n");
        }
        sem_post(globMutex);
    }
    sem_post(queueSem);
    sem_post(elfHelpSem);
    sem_post(globMutex);
    sendMsg(ELF, eId, f, "taking holidays\n");
}

void startReindeer(FILE *f, int rId, int maxWorkTime, int reindeerCount) {
    srand(time(0));
    int workTime = (maxWorkTime == 0 ? 0 : (rand() % (maxWorkTime / 2)));
    sendMsg(REINDEER, rId, f, "rstarted\n");
    actWork(workTime+ (maxWorkTime / 2));
    sendMsg(REINDEER, rId, f, "return home\n");

    sem_wait(globMutex);
    if(++(*reinC) == reindeerCount) {
        *takeHoliday = 1;
        sem_post(santaENSem);
    }
    sem_post(globMutex);

    sem_wait(hitchSem);
    sendMsg(REINDEER, rId, f, "get hitched\n");
    if(--(*reinC) == 0) {
        sem_post(chstmsENSem);
    }
    sem_post(hitchSem);
}


void actWork(uint16_t msWork) {
    usleep(1000 * msWork);
}

void sendMsg(Actors a, int actorId, FILE *f, const char *fmt, ... ) {
    sem_wait(msgSem);
    switch(a) {
        case SANTA: fprintf(f, "%d: Santa: ", (*msgC)++); break;
        case ELF: fprintf(f, "%d: Elf %d: ", (*msgC)++, actorId); break;
        case REINDEER: fprintf(f, "%d: RD %d: ", (*msgC)++, actorId); break;
        default: fprintf(stderr, "[WARNING]: Unknown state"); break;
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    fflush(f);
    va_end(args);
    sem_post(msgSem);
}

void destroyActors() {
    sem_destroy(msgSem);
    sem_destroy(globMutex);
    sem_destroy(santaENSem);
    sem_destroy(hitchSem);
    sem_destroy(queueSem);
    sem_destroy(chstmsENSem);
    sem_destroy(elfHelpSem);
    sem_destroy(elfHelpedSem);

    shmctl(msgC_id, IPC_RMID, NULL);
    shmctl(elfC_id, IPC_RMID, NULL);
    shmctl(msgSem_id, IPC_RMID, NULL);
    shmctl(hitchSem_id, IPC_RMID, NULL);
    shmctl(globMutex_id, IPC_RMID, NULL);
    shmctl(santaENSem_id, IPC_RMID, NULL);
    shmctl(reinC_id, IPC_RMID, NULL);
    shmctl(queueSem_id, IPC_RMID, NULL);
    shmctl(chstmsENSem_id, IPC_RMID, NULL);
    shmctl(elfHelpSem_id, IPC_RMID, NULL);
    shmctl(takeHoliday_id, IPC_RMID, NULL);
    shmctl(elfHelpedSem_id, IPC_RMID, NULL);
    shmctl(closedWS_id, IPC_RMID, NULL);
}

int initSem(sem_t **sem, int *semId, int val) {
    if((*semId = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0666)) == -1) {
        return -1;
    }
    if((*sem = (sem_t*)(shmat(*semId, NULL, 0))) == ((void *) -1)) {
        return -1;
    }
    if(sem_init(*sem, 1, val)) {
        return -1;
    }
    return 0;
}

int initInt(int **intP, int *intId, int val) {
    if((*intId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1) {
        return -1;
    }
    if((*intP = (int*)(shmat(*intId, NULL, 0))) == ((void *) -1)) {
        return -1;
    }
    **intP = val;
    return 0;
}