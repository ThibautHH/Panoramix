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
"I see %lu servings left.\n";
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
        if (v->is_fighting)
            pthread_cancel(v->thread);
        if (pthread_join(v->thread, &status) || INVALID_STATUS(v->is_fighting))
            exit = 84;
        free(v);
    }
    if (d->is_brewing)
        pthread_cancel(d->thread);
    if (pthread_join(d->thread, &status) || INVALID_STATUS(d->is_brewing)
        || sem_destroy(&d->pot->awareness))
        exit = 84;
    return exit;
}

static void *villager_thread(void *arg)
{
    villager_t *v = arg;

    sem_wait(&v->pot->awareness);
    printf("Villager %lu: Going into battle!\n", v->id);
    for (; v->fights_left; v->fights_left--) {
        pthread_mutex_lock(&v->pot->laddle);
        printf(VILLAGER_THIRSTY, v->id, v->pot->drinks_left);
        if (!v->pot->drinks_left) {
            sem_post(&v->pot->awareness);
            printf(VILLAGER_NO_DRINKS, v->id);
            sem_wait(&v->pot->awareness);
        } else
            v->pot->drinks_left--;
        pthread_mutex_unlock(&v->pot->laddle);
        printf(VILLAGER_FIGHTING, v->id, v->fights_left - 1);
    }
    printf("Villager %lu: I'm going to sleep now.\n", v->id);
    v->is_fighting = 0;
    return NULL;
}

static void *druid_thread(void *arg)
{
    druid_t *d = arg;

    printf("Druid: I'm ready... but sleepy...\n");
    for (; d->refills_left; d->refills_left--) {
        sem_wait(&d->pot->awareness);
        printf(DRUID_REFILLING, d->refills_left);
        d->pot->drinks_left = d->pot->size;
        sem_post(&d->pot->awareness);
    }
    printf("Druid: I'm out of viscum. I'm going back to... zZz\n");
    d->is_brewing = 0;
    return NULL;
}

int panoramix(unsigned long villager_count, unsigned long pot_size,
    unsigned long fight_count, unsigned long refill_count)
{
    struct villagers_s villagers = SLIST_HEAD_INITIALIZER(villagers_s);
    villager_t *v;
    pot_t pot = {pot_size, pot_size, PTHREAD_MUTEX_INITIALIZER, {{0}}};
    druid_t druid = {refill_count, &pot, 0, 0};

    if (!create_villagers(&villagers, &pot, villager_count, fight_count))
        return 84;
    if (sem_init(&pot.awareness, 0, villager_count))
        return 84;
    if (pthread_create(&druid.thread, NULL, druid_thread, &druid))
        return free_villagers(84, &villagers, &druid);
    druid.is_brewing = 1;
    SLIST_FOREACH(v, &villagers, next) {
        if (pthread_create(&v->thread, NULL, villager_thread, v))
            return free_villagers(84, &villagers, &druid);
        v->is_fighting = 1;
    }
    return free_villagers(0, &villagers, &druid);
}
