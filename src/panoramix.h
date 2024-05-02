/*
** EPITECH PROJECT, 2024
** Panoramix
** File description:
** Main header
*/

#ifndef PANORAMIX_H_
    #define PANORAMIX_H_

    #include <pthread.h>
    #include <semaphore.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/queue.h>

typedef struct pot_s {
    unsigned long size;
    unsigned long drinks_left;
    pthread_mutex_t laddle;
    sem_t awareness;
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
    pthread_t thread;
    SLIST_ENTRY(villager_s) next;
} villager_t;

SLIST_HEAD(villagers_s, villager_s);

int panoramix(unsigned long villager_count, unsigned long pot_size,
    unsigned long fight_count, unsigned long refill_count);

#endif /* !PANORAMIX_H_ */
