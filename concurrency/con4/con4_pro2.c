#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

pthread_mutex_t agentLckMtx;
pthread_mutex_t pusherLockMtx;
sem_t smokerSem[3];
sem_t pusherSem[3];

char *smokeType[3] = {"matches & tobacco", "matches & paper", "tobacco & paper"};
/*matches, tobacco, paper*/
int itemStates[3] = {0, 0, 0};

int randNumber(int min, int max)
{
    return rand() % (max + 1 - min) + min;
}

void *smoker(void *arg)
{
    int smokerId = *(int *)arg;
    int idx = smokerId % 3;

    while (1)
    {
        printf("Smoker %d >> Waiting for %s\n", smokerId, smokeType[idx]);

        sem_wait(&smokerSem[idx]);

        printf("Smoker %d << Now making the a cigarette\n", smokerId);
        sleep(randNumber(2, 5));
        pthread_mutex_unlock(&agentLckMtx);

        printf("Smoker %d -- Now smoking\n", smokerId);
        sleep(randNumber(2, 5));
    }

    return NULL;
}

void *pusher(void *arg)
{
    int puserId = *(int *)arg;

    while (1)
    {
        sem_wait(&pusherSem[puserId]);
        pthread_mutex_lock(&pusherLockMtx);

        if (itemStates[(puserId + 1) % 3])
        {
            itemStates[(puserId + 1) % 3] = 0;
            sem_post(&smokerSem[puserId]);
        }
        else if (itemStates[(puserId + 2) % 3])
        {
            itemStates[(puserId + 2) % 3] = 0;
            sem_post(&smokerSem[(puserId + 2) % 3]);
        }
        else
        {
            itemStates[puserId] = 1;
        }

        pthread_mutex_unlock(&pusherLockMtx);
    }

    return NULL;
}

void *agent(void *arg)
{
    int agent_id = *(int *)arg;

    while (1)
    {
        sleep(randNumber(0, 2));

        pthread_mutex_lock(&agentLckMtx);

        sem_post(&pusherSem[agent_id]);
        sem_post(&pusherSem[(agent_id + 1) % 3]);

        printf("Agent %d giving out %s\n", agent_id, smokeType[agent_id]);
    }

    return NULL;
}

int main(int argc, char *arvg[])
{
    int i;
    srand(time(NULL));
    pthread_mutex_init(&agentLckMtx, NULL);
    pthread_mutex_init(&pusherLockMtx, NULL);

    for (i = 0; i < 3; ++i)
    {
        sem_init(&smokerSem[i], 0, 0);
        sem_init(&pusherSem[i], 0, 0);
    }

    int args[3];
    pthread_t pusherThd[3];
    pthread_t agentThd[3];
    pthread_t smokerThd[3];

    for (i = 0; i < 3; ++i)
    {
        args[i] = i;
        pthread_create(&smokerThd[i], NULL, smoker, &args[i]);
        pthread_create(&pusherThd[i], NULL, pusher, &args[i]);
        pthread_create(&agentThd[i], NULL, agent, &args[i]);
    }

    for (i = 0; i < 3; ++i)
        pthread_join(smokerThd[i], NULL);

    return 0;
}