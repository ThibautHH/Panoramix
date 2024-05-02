/*
** EPITECH PROJECT, 2024
** Panoramix
** File description:
** Main program
*/

#include "panoramix.h"

static const char DRUID_REFILLING[] = "Druid: Ah! Yes, yes, I'm awake! "
"Working on it! Beware I can only make %lu more refills after this one.\n";
static const char VILLAGER_THIRSTY[] = "Villager %lu: I need a drink... "
"I see %d servings left.\n";
static const char VILLAGER_NO_DRINKS[] = "Villager %lu: Hey Pano wake up! "
"We need more potion.\n";
static const char VILLAGER_FIGHTING[] = "Villager %lu: Take that roman scum! "
"Only %lu left.\n";

static _Bool create_villagers(struct villagers_s *villagers, pot_t *pot,
    unsigned long villager_count, unsigned long fight_count)
{
    for (villager_t *villager; villager_count; villager_count--) {
        villager = malloc(sizeof(villager_t));
        if (!villager)
            return 0;
        villager->id = villager_count - 1;
        villager->fights_left = fight_count;
        villager->pot = pot;
        villager->is_fighting = 0;
        villager->weapon = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
        SLIST_INSERT_HEAD(villagers, villager, next);
    }
    return 1;
}

static int free_villagers(int exit, struct villagers_s *villagers,
    druid_t *d)
{
    villager_t *v;
    villager_t *tmp;
    void *status;

    for (v = SLIST_FIRST(villagers); v; v = tmp) {
        tmp = SLIST_NEXT(v, next);
        SLIST_REMOVE_HEAD(villagers, next);
        if (exit && v->is_running)
            pthread_cancel(v->thread);
        if (v->is_fighting
            && (pthread_join(v->thread, &status)
                || INVALID_STATUS(v->is_running)))
            exit = 84;
        free(v);
    }
    if ((d->is_brewing
            && (pthread_join(d->thread, &status) || status != NULL))
        || DESTORY_SEM(drinks) || DESTORY_SEM(emptiness))
        exit = 84;
    return exit;
}

static void *villager_thread(void *arg)
{
    villager_t *v = arg;
    int drinks_left;

    pthread_mutex_lock(&v->weapon);
    printf("Villager %lu: Going into battle!\n", v->id);
    for (; v->fights_left; v->fights_left--) {
        pthread_mutex_lock(&v->pot->laddle);
        sem_getvalue(&v->pot->drinks, &drinks_left);
        printf(VILLAGER_THIRSTY, v->id, (drinks_left < 0) ? 0 : drinks_left);
        errno = 0;
        if (sem_trywait(&v->pot->drinks) && errno == EAGAIN) {
            printf(VILLAGER_NO_DRINKS, v->id);
            sem_post(&v->pot->emptiness);
            sem_wait(&v->pot->drinks);
        }
        printf(VILLAGER_FIGHTING, v->id, v->fights_left - 1);
        pthread_mutex_unlock(&v->pot->laddle);
    }
    printf("Villager %lu: I'm going to sleep now.\n", v->id);
    v->is_running = 0;
    pthread_mutex_unlock(&v->weapon);
    return NULL;
}

static void *druid_thread(void *arg)
{
    druid_t *d = arg;

    printf("Druid: I'm ready... but sleepy...\n");
    for (; d->refills_left; d->refills_left--) {
        sem_wait(&d->pot->emptiness);
        printf(DRUID_REFILLING, d->refills_left - 1);
        for (unsigned long i = d->pot->size; i; i--)
            sem_post(&d->pot->drinks);
    }
    printf("Druid: I'm out of viscum. I'm going back to... zZz\n");
    return NULL;
}

int panoramix(unsigned long villager_count, unsigned long pot_size,
    unsigned long fight_count, unsigned long refill_count)
{
    struct villagers_s villagers = SLIST_HEAD_INITIALIZER(villagers_s);
    villager_t *v;
    pot_t pot = {pot_size, PTHREAD_MUTEX_INITIALIZER, {{0}}, 0, {{0}}, 0};
    druid_t druid = {refill_count, &pot, 0, 0};

    if (!create_villagers(&villagers, &pot, villager_count, fight_count))
        return 84;
    if (sem_init(&pot.drinks, 0, pot_size))
        return free_villagers(84, &villagers, &druid);
    pot.drinks_initialized = 1;
    if (sem_init(&pot.emptiness, 0, 1))
        return free_villagers(84, &villagers, &druid);
    pot.emptiness_initialized = 1;
    sem_wait(&pot.emptiness);
    SLIST_FOREACH(v, &villagers, next) {
        pthread_mutex_lock(&v->weapon);
        if (pthread_create(&v->thread, NULL, villager_thread, v))
            return free_villagers(84, &villagers, &druid);
        v->is_fighting = 1;
        v->is_running = 1;
        pthread_mutex_unlock(&v->weapon);
    }
    if (pthread_create(&druid.thread, NULL, druid_thread, &druid))
        return free_villagers(84, &villagers, &druid);
    druid.is_brewing = 1;
    return free_villagers(0, &villagers, &druid);
}
