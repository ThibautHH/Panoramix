/*
** EPITECH PROJECT, 2024
** Panoramix
** File description:
** Thread functions
*/

#include "panoramix.h"

void *villager_thread(void *arg)
{
    const char V_FORMATS;
    villager_t *v = arg;
    int drinks_left;

    pthread_mutex_lock(&v->weapon);
    printf(rdy, v->id);
    for (; v->fights_left; v->fights_left--) {
        pthread_mutex_lock(&v->pot->laddle);
        sem_getvalue(&v->pot->drinks, &drinks_left);
        printf(thrst, v->id, (drinks_left < 0) ? 0 : drinks_left);
        drink(v);
        printf(fght, v->id, v->fights_left - 1);
        pthread_mutex_unlock(&v->pot->laddle);
    }
    printf(done, v->id);
    v->is_running = 0;
    pthread_mutex_unlock(&v->weapon);
    return NULL;
}

void *druid_thread(void *arg)
{
    const char D_FORMATS;
    druid_t *d = arg;

    pthread_mutex_lock(&d->herbs);
    printf(rdy);
    for (; d->refills_left; d->refills_left--) {
        sem_wait(&d->pot->emptiness);
        printf(rfl, d->refills_left - 1);
        for (unsigned long i = d->pot->size; i; i--)
            sem_post(&d->pot->drinks);
    }
    printf(done);
    pthread_mutex_unlock(&d->herbs);
    return NULL;
}
