/*
** EPITECH PROJECT, 2024
** Panoramix
** File description:
** Main program
*/

#include "panoramix.h"

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
        villager->is_running = 0;
        villager->is_fighting = 0;
        villager->weapon = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
        SLIST_INSERT_HEAD(villagers, villager, next);
    }
    return 1;
}

static int free_villagers(int exit, struct villagers_s *villagers, druid_t *d)
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

static _Bool send_out_villager(villager_t *v)
{
    pthread_mutex_lock(&v->weapon);
    if (pthread_create(&v->thread, NULL, villager_thread, v))
        return 0;
    v->is_fighting = 1;
    v->is_running = 1;
    return 1;
}

static int ready_villagers(struct villagers_s *villagers, druid_t *druid)
{
    villager_t *v;

    SLIST_FOREACH(v, villagers, next)
        if (!send_out_villager(v))
            return free_villagers(84, villagers, druid);
    SLIST_FOREACH(v, villagers, next)
        pthread_mutex_unlock(&v->weapon);
    pthread_mutex_lock(&druid->herbs);
    if (pthread_create(&druid->thread, NULL, druid_thread, druid))
        return free_villagers(84, villagers, druid);
    druid->is_brewing = 1;
    pthread_mutex_unlock(&druid->herbs);
    return free_villagers(0, villagers, druid);
}

int panoramix(unsigned long villager_count, unsigned long pot_size,
    unsigned long fight_count, unsigned long refill_count)
{
    struct villagers_s villagers = SLIST_HEAD_INITIALIZER(villagers_s);
    pot_t pot = {pot_size, PTHREAD_MUTEX_INITIALIZER, {{0}}, 0, {{0}}, 0};
    druid_t druid = {refill_count, &pot, 0, 0, PTHREAD_MUTEX_INITIALIZER};

    if (!create_villagers(&villagers, &pot, villager_count, fight_count))
        return 84;
    if (sem_init(&pot.drinks, 0, pot_size))
        return free_villagers(84, &villagers, &druid);
    pot.drinks_initialized = 1;
    if (sem_init(&pot.emptiness, 0, 1))
        return free_villagers(84, &villagers, &druid);
    pot.emptiness_initialized = 1;
    sem_wait(&pot.emptiness);
    return ready_villagers(&villagers, &druid);
}
