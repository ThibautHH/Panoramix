/*
** EPITECH PROJECT, 2024
** Panoramix
** File description:
** Thread functions
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

static void drink(villager_t *v)
{
    errno = 0;
    if (sem_trywait(&v->pot->drinks) && errno == EAGAIN) {
        printf(VILLAGER_NO_DRINKS, v->id);
        sem_post(&v->pot->emptiness);
        sem_wait(&v->pot->drinks);
    }
}

void *villager_thread(void *arg)
{
    villager_t *v = arg;
    int drinks_left;

    pthread_mutex_lock(&v->weapon);
    printf("Villager %lu: Going into battle!\n", v->id);
    for (; v->fights_left; v->fights_left--) {
        pthread_mutex_lock(&v->pot->laddle);
        sem_getvalue(&v->pot->drinks, &drinks_left);
        printf(VILLAGER_THIRSTY, v->id, (drinks_left < 0) ? 0 : drinks_left);
        drink(v);
        printf(VILLAGER_FIGHTING, v->id, v->fights_left - 1);
        pthread_mutex_unlock(&v->pot->laddle);
    }
    printf("Villager %lu: I'm going to sleep now.\n", v->id);
    v->is_running = 0;
    pthread_mutex_unlock(&v->weapon);
    return NULL;
}

void *druid_thread(void *arg)
{
    druid_t *d = arg;

    pthread_mutex_lock(&d->herbs);
    for (; d->refills_left; d->refills_left--) {
        sem_wait(&d->pot->emptiness);
        printf(DRUID_REFILLING, d->refills_left - 1);
        for (unsigned long i = d->pot->size; i; i--)
            sem_post(&d->pot->drinks);
    }
    pthread_mutex_unlock(&d->herbs);
    return NULL;
}
