/*
** EPITECH PROJECT, 2024
** Panoramix
** File description:
** Main header
*/

#ifndef PANORAMIX_H_
    #define PANORAMIX_H_

    #include <errno.h>
    #include <pthread.h>
    #include <semaphore.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/queue.h>

    #define _CANCELLED(c) (c && status != PTHREAD_CANCELED)
    #define INVALID_STATUS(c) _CANCELLED(c) || (!c && status != NULL)
    #define _SEM_INIT(sem) (&d->pot->sem##_initialized)
    #define DESTORY_SEM(sem) (_SEM_INIT(sem) && sem_destroy(&d->pot->sem))

typedef struct pot_s {
    unsigned long size;
    pthread_mutex_t laddle;
    sem_t drinks;
    _Bool drinks_initialized;
    sem_t emptiness;
    _Bool emptiness_initialized;
} pot_t;

typedef struct druid_s {
    unsigned long refills_left;
    pot_t *pot;
    _Bool is_brewing;
    pthread_t thread;
} druid_t;

typedef struct villager_s {
    unsigned long id;
    unsigned long fights_left;
    pot_t *pot;
    _Bool is_fighting;
    _Bool is_running;
    pthread_t thread;
    pthread_mutex_t weapon;
    SLIST_ENTRY(villager_s) next;
} villager_t;

SLIST_HEAD(villagers_s, villager_s);

int panoramix(unsigned long villager_count, unsigned long pot_size,
    unsigned long fight_count, unsigned long refill_count);

void *druid_thread(void *arg);
void *villager_thread(void *arg);

#endif /* !PANORAMIX_H_ */
