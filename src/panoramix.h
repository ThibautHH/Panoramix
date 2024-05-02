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
    pthread_mutex_t herbs;
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

    #define V_READY "Villager %lu: Going into battle!\n"

    #define _THIRST "I see %d servings left.\n"
    #define _THIRSTY "Villager %lu: I need a drink... " _THIRST

    #define _DRINKS "We need more potion.\n"
    #define _NO_DRINKS "Villager %lu: Hey Pano wake up! " _DRINKS

    #define _FIGHT "Only %lu left.\n"
    #define _FIGHTING "Villager %lu: Take that roman scum! " _FIGHT

    #define V_DONE "Villager %lu: I'm going to sleep now.\n"

    #define D_READY "Druid: I'm ready... but sleepy...\n"

    #define _LING "I can only make %lu more refills after this one.\n"
    #define _REFIL "Working on it! Beware " _LING
    #define _REFILLING "Druid: Ah! Yes, yes, I'm awake! " _REFIL

    #define D_DONE "Druid: I'm out of viscum. I'm going back to... zZz\n"

    #define _V_FORMATS fght[] = _FIGHTING, done[] = V_DONE
    #define V_FORMATS rdy[] = V_READY, thrst[] = _THIRSTY, _V_FORMATS

    #define D_FORMATS rdy[] = D_READY, rfl[] = _REFILLING, done[] = D_DONE

static inline void drink(villager_t *v)
{
    const char fmt[] = _NO_DRINKS;

    errno = 0;
    if (sem_trywait(&v->pot->drinks) && errno == EAGAIN) {
        printf(fmt, v->id);
        sem_post(&v->pot->emptiness);
        sem_wait(&v->pot->drinks);
    }
}

#endif /* !PANORAMIX_H_ */
